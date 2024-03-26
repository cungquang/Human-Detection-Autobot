import threading
import socket
import cv2
from PIL import Image

#termination flag
isTerminated = 0

#server information
SERVER_IP = '192.168.148.129'
SERVER_PORT = 6666

#for sending termination signal
CLIENT_IP = '192.168.7.2'
CLIENT_PORT = 7777

PIXEL_CONVERSION_RATE = 0.02645
IMG_BUFFER = 4096           



#########################
#                       #
#   Human Recognition   #
#                       #
#########################

#Source: ChatGPT
def detect_human(image_path):
    human_cascade = cv2.CascadeClassifier(cv2.data.haarcascades + 'haarcascade_fullbody.xml')
    image = cv2.imread(image_path)
    gray = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    humans = human_cascade.detectMultiScale(gray, 1.1, 4)

    if len(humans) > 0:
        x, y, w, h = humans[0]
        human_center_x = x + w // 2
        return human_center_x
    else:
        #print("No human detected in the image.")
        return None

def get_gap_from_center(human_center_x, image_path):
    image = cv2.imread(image_path)
    image_width = image.shape[1]
    center = image_width / 2
    gap = human_center_x - center

    return gap

def identify_human_position(image_path):
    human_center_x = detect_human(image_path)

    #If found human
    if human_center_x is not None:
        #Return position: left -> negative; center -> 0; right -> positive
        return get_gap_from_center(human_center_x, image_path)
    
    else:
        #No human found OR unable to identify human
        return None

#on average 1 pixel = 0.02645 cm
def convert_pixel_cm(pixel):
    return round(pixel*PIXEL_CONVERSION_RATE)


def process_image(image_path):
    human_position_pixel = identify_human_position(image_path)
    human_position_cm = convert_pixel_cm(human_position_pixel)
    print(human_position_cm)
    return 

#########################
#                       #
#      TCP Server       #
#                       #
#########################


def handle_client_image(client_socket):

    #Read Metadata - size of the image => convert into integer
    image_size_bytes = client_socket.recv(IMG_BUFFER)
    image_size = int.from_bytes(image_size_bytes, byteorder='big')

    #Send confirm => trigger receive image

    #Receive data in chunk - then combine
    image_data = b""
    bytes_received = 0

    #keep read file until == image_size
    while bytes_received < image_size:
        chunk = client_socket.recv(min(IMG_BUFFER, image_size - bytes_received))
        if not chunk:
            break
        image_data += chunk
        bytes_received += len(chunk)
    
    print("Image received from client.")

    with open("received_image.JPEG", "wb") as file:
        file.write(image_data)

    # Open image using Pillow
    image = Image.open("received_image.JPG")
    image.save("received_image.JPEG", "JPEG")

    #use AI to read the file 


    #send back the result


def start_tcp_server(host, port):
    # Create a TCP/IP socket
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((SERVER_IP, SERVER_PORT))
    server_socket.listen(1) 
    
    print(f"Server listening on {SERVER_IP}:{SERVER_PORT}")

    try:
        while not isTerminated:
            #accept connection
            client_socket, client_address = server_socket.accept();

            #initiate threads
            # Start a new thread to handle the client
            client_thread = threading.Thread(target=handle_client_image, args=(client_socket,))
            client_thread.start()

            # Wait for both threads to finish
            client_thread.join()
            
            #close connection
            client_socket.close()

    except KeyboardInterrupt:
        print("Server shutting down.")
    finally:
        # Close the server socket
        server_socket.close()

def main():
    start_tcp_server(SERVER_IP, SERVER_PORT)

if __name__ == "__main__":
    main()