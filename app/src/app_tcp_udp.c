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

void Tcp_init()
{
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
        perror("Connection failed\n");
        exit(EXIT_FAILURE);
    }

    printf("Successfully connected to server %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
}

void Tcp_cleanUp()
{
    // Clean up
    close(python_server_socket);
}

void Tcp_sendImage(char *imagePath)
{
    // Read image
    FILE *image_file = fopen(imagePath, "rb");
    if (image_file == NULL) {
        perror("Failed to open image file\n");
        return;
    }

    //Read size of file
    fseek(image_file, 0, SEEK_END);
    long image_size = ftell(image_file);
    rewind(image_file);


    // Allocate memory to store image data
    unsigned char *image_inByte = (unsigned char *)malloc(image_size);
    if (image_inByte == NULL) {
        perror("Memory allocation failed\n");
        fclose(image_file);
        return;
    }

    // Step 4: Send metadata (image size)
    if (send(python_server_socket, &image_size, sizeof(image_size), 0) != sizeof(image_size)) {
        perror("Metadata send failed");
        fclose(image_file);
        exit(EXIT_FAILURE);
    }

    // Step 5: Send the image data
    char buffer[IMG_BUFFER];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), image_file)) > 0) {
        if ((ssize_t)send(python_server_socket, buffer, bytes_read, 0) != (ssize_t)bytes_read) {
            perror("Image data send failed");
            fclose(image_file);
            exit(EXIT_FAILURE);
        }
    }


    // Step 6: Close resources and connection
    fclose(image_file);
}


/*
*****************************
*          PRIVATE          *
*****************************
*/
