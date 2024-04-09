import threading
import socket
import cv2
import traceback
import struct
import numpy as np

#termination flag
isTerminated = 0

#server information
SERVER_IP = '192.168.148.129'
SERVER_PORT = 6666

#for sending termination signal
CLIENT_IP = '192.168.7.2'
CLIENT_PORT = 7777

PIXEL_CONVERSION_RATE = 0.02645
IMG_BUFFER = 8024         

IMG_PATH = "received_image.JPG"

#########################
#                       #
#   Human Recognition   #
#                       #
#########################


def test_human():
    import cv2

    # Load the pre-trained Haar Cascade classifier for human detection
    human_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_fullbody.xml')

    # Load the image
    image_path = 'temp2.jpg'
    image = cv2.imread(image_path)

    # # Convert the image to grayscale
    # gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)

    # # Detect humans in the image
    # humans = human_cascade.detectMultiScale(gray, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

    # Detect humans in the color image
    humans = human_cascade.detectMultiScale(image, scaleFactor=1.1, minNeighbors=5, minSize=(30, 30))

    # Draw rectangles around the detected humans
    for (x, y, w, h) in humans:
        cv2.rectangle(image, (x, y), (x + w, y + h), (255, 0, 0), 2)

    # Display the image with detected humans
    cv2.imshow('Humans Detected', image)
    cv2.waitKey(0)
    cv2.destroyAllWindows()


#Source: ChatGPT
def detect_human(image):
    #load pre-trained data
    human_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_fullbody.xml')
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    humans = human_cascade.detectMultiScale(gray, 1.1, 4)
    print("num humans: "+str(len(humans)))

    if len(humans) > 0:
        print("human[0]:"+str(humans[0]))
        x, y, w, h = humans[0]
        human_center_x = x + w // 2
        return human_center_x
    else:
        #print("No human detected in the image.")
        return None

#get the position of the center line in the picture
def get_gap_from_center(human_center_x, image):
    #image = cv2.imread(image_path)
    image_width = image.shape[1]
    center = image_width / 2
    print("centre: "+str(center)+"/n")
    gap = human_center_x - center

    return gap

#identify position of human in the picture
def identify_human_position(image):
    human_center_x = detect_human(image)

    #If found human
    if human_center_x is not None:
        #Return position: left -> negative; center -> 0; right -> positive
        return get_gap_from_center(human_center_x, image)
    
    else:
        #No human found OR unable to identify human
        return None

#convert pixel to cm: average 1 pixel = 0.02645 cm
def convert_pixel_cm(pixel):
    return round(pixel*PIXEL_CONVERSION_RATE)

#processing image
def process_image(image_path):
    image = cv2.imread(image_path)
    human_position_pixel = identify_human_position(image)
    #human_position_cm = convert_pixel_cm(human_position_pixel)

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
    #start_tcp_server()
    test_human()

if __name__ == "__main__":
    main()