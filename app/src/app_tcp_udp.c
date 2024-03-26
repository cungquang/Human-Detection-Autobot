#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/app_helper.h"

#define MSG_BUFFER 1024
#define IMG_BUFFER 4096

//UDP Server to receive command
#define UDP_SERVER_IP "192.168.7.2"
#define UDP_SERVER_PORT 12345

//Python Server
#define PYTHON_SERVER_IP "192.168.148.129"
#define PYTHON_SERVER_PORT 6666

//Socket
static int python_server_socket;


/*
*****************************
*           PUBLIC          *
*****************************
*/

void Tcp_sendImage(char *imagePath)
{
    //Read image
    long image_size = 0;
    unsigned char *image_inByte = read_image_inByte(imagePath, &image_size);

    //Connect to TCP to send image
    struct sockaddr_in server_addr;

    //Create socket
    python_server_socket = socket(AF_INET, SOCK_STREAM, 0);
    
    // Initialize server address struct
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, PYTHON_SERVER_IP, &server_addr.sin_addr);
    server_addr.sin_port = htons(PYTHON_SERVER_PORT);

    // Connect to server
    if (connect(python_server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
        perror("Connection failed");
        free(image_inByte);
        exit(EXIT_FAILURE);
    }

    // Send image data to server
    size_t bytes_sent = 0;
    size_t remaining_bytes = image_size;
    while (bytes_sent < (size_t)image_size) {
        // remain < buffer => send all OTHERWISE send == IMG_BUFFER
        size_t chunk_size = remaining_bytes < IMG_BUFFER ? remaining_bytes : IMG_BUFFER;
        if (send(python_server_socket, image_inByte + bytes_sent, chunk_size, 0) == -1) {
            perror("Send failed");
            free(image_inByte);
            close(python_server_socket);
            return;
        }

        //Update the next byte to send
        bytes_sent += chunk_size;
        remaining_bytes -= chunk_size;
    }

    // Send "end" -> end the process
    const char *end_message = "end";
    if (send(python_server_socket, end_message, strlen(end_message), 0) == -1) {
        perror("Send failed");
        free(image_inByte);
        close(python_server_socket);
        return;
    }


    printf("Image sent to server\n");

    // Clean up
    close(python_server_socket);
    free(image_inByte);
}


/*
*****************************
*          PRIVATE          *
*****************************
*/
