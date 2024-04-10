import threading
import socket
import traceback
import struct
import boto3
import cv2
import os
import numpy as np

#termination flag
isTerminated = 0

#server information
SERVER_IP = '192.168.26.128'
SERVER_PORT = 6666

#for sending termination signal
CLIENT_IP = '192.168.7.2'
CLIENT_PORT = 7777

PIXEL_CONVERSION_RATE = 0.02645
IMG_BUFFER = 8024         

AWS_ID = os.environ.get('AWS_ID')
AWS_ACCESS = os.environ.get('AWS_ACCESS')
IMG_PATH = "received_image.JPG"

#########################
#                       #
#   Human Recognition   #
#                       #
#########################

def detect_humans(image_path):    
    # Initialize the Rekognition client
    client = boto3.client(
        'rekognition',
        aws_access_key_id=AWS_ID,
        aws_secret_access_key=AWS_ACCESS,
        region_name='us-east-1'
    )

    # Open the image file
    with open(image_path, 'rb') as image_file:
        image_bytes = image_file.read()

    # Detect labels in the image
    response = client.detect_labels(
        Image={
            'Bytes': image_bytes
        },
        MaxLabels=10,
        MinConfidence=50
    )

    # Extract bounding box coordinates of detected humans
    human = {}
    for label in response['Labels']:
        if label['Name'] == 'Person':
            for instance in label['Instances']:
                bounding_box = instance['BoundingBox']
                
                #first valid human
                if bounding_box:
                    human['left'] = bounding_box['Left']
                    human['top'] = bounding_box['Top'] 
                    human['width'] = bounding_box['Width'] 
                    human['height'] = bounding_box['Height'] 
                
                    return human
    return None


def calculate_distance_to_center(image_path, bounding_box):
    image = cv2.imread(image_path)
    image_height, image_width, _ = image.shape

    # Calculate center of the image
    image_center_x = image_width / 2
    image_center_y = image_height / 2

    # Calculate center of the bounding box
    left = image_width*bounding_box['left']
    top = image_height*bounding_box['top']
    width = image_width*bounding_box['width']
    height = image_height*bounding_box['height']

    human_center_x = left + width/2
    human_center_y = top + height/2

    # gap on x_axis
    distance = image_center_x - human_center_x
    return distance

#identify position of human in the picture
def identify_human_position(image_path):
    human = detect_humans(image_path)
    if human:
        distance = calculate_distance_to_center(image_path, human)
        return distance
    else:
        return None

#processing image
def process_image(image_path):
    human_position_pixel = identify_human_position(image_path)

    return human_position_pixel 

#########################
#                       #
#      TCP Server       #
#                       #
#########################

#save image (in byte) to the file
def save_image_from_bytes_with_opencv(image_data, filename):
    image_np = np.frombuffer(image_data, dtype=np.uint8)
    image = cv2.imdecode(image_np, cv2.IMREAD_COLOR)
    cv2.imwrite(filename, image)

#thread to read image
def handle_client_image(client_connection):
    #Receive data in chunk - then combine
    image_data = b""
    bytes_received = 0
    
    try:
        # Read the bytes representing the image size
        image_size_bytes = client_connection.recv(20)
        image_size_str = image_size_bytes.decode('utf-8')
        image_size = int(image_size_str)

        #send message trigger sending data
        client_connection.send(b"okay")

        #keep read file until == image_size
        while bytes_received < image_size:
            chunk = client_connection.recv(min(IMG_BUFFER, image_size - bytes_received))
            if not chunk:
                break
            image_data += chunk
            bytes_received += len(chunk)

            #slow down the transferring data - send confirmation
            client_connection.send(b"okay")
        
        print("Total bytes received:", len(image_data))

        #Save iamge data into the file
        save_image_from_bytes_with_opencv(image_data, IMG_PATH)

        #call AI read image
        distance_to_human = process_image(IMG_PATH)
        if distance_to_human is None:
            print("No human detected within the image!")
            distance_bytes = struct.pack("!i", 0)
            client_connection.send(distance_bytes)
        else:
            print("Distance to nearest human: ", int(distance_to_human))
            distance_bytes = struct.pack("!i", int(distance_to_human))
            client_connection.send(distance_bytes)
    except Exception as e:
        print("An error occurred:", e)
        traceback.print_exc()


def start_tcp_server():
    # Create a TCP/IP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((SERVER_IP, SERVER_PORT))
    server_socket.listen(1) 
    
    print(f"Server listening on {SERVER_IP}:{SERVER_PORT}")

    try:
        while not isTerminated:
            #accept connection
            client_connection, client_address = server_socket.accept()

            #Start a new thread to handle the client
            client_thread = threading.Thread(target=handle_client_image, args=(client_connection,))
            client_thread.start()

            #Wait for both threads to finish
            client_thread.join()

            #close connection
            client_connection.close()

    except KeyboardInterrupt:
        print("Server shutting down.")
    finally:
        # Close the server socket
        server_socket.close()

def main():
    start_tcp_server()

if __name__ == "__main__":
    main()