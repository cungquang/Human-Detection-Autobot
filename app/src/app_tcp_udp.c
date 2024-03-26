#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "../include/app_helper.h"

#define IMG_BUFFER 1024

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
    int image_size = ftell(image_file);
    fseek(image_file, 0, SEEK_SET);                 //move back to the head of file

    //Send metadata - image size
    char image_size_str[20];
    memset(image_size_str, 0, sizeof(image_size_str));
    snprintf(image_size_str, sizeof(image_size_str), "%d", image_size);

    printf("Sending image size: %s\n", image_size_str);
    if (send(python_server_socket, image_size_str, strlen(image_size_str), 0) < 0) {
        perror("Failed to send image size\n");
        fclose(image_file);
        return;
    }

    //Receive confirmation from server
    char confirmation[5]; // Assuming maximum length of confirmation message is 5 characters
    ssize_t bytes_received = recv(python_server_socket, confirmation, sizeof(confirmation) - 1, 0);
    if (bytes_received == -1) {
        perror("Failed to receive confirmation from server\n");
        fclose(image_file);
        return;
    }
    confirmation[bytes_received] = '\0'; // Null-terminate the received message

    // Check if confirmation message is "okay"
    if (strcmp(confirmation, "okay") != 0) {
        fprintf(stderr, "Unexpected confirmation message from server: %s\n", confirmation);
        fclose(image_file);
        return;
    }


    //Send the image data - use fread -> read into memory
    char buffer[IMG_BUFFER];
    memset(buffer, 0, sizeof(buffer));
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), image_file)) > 0) {
        
        //Send data
        if ((ssize_t)send(python_server_socket, buffer, bytes_read, 0) != (ssize_t)bytes_read) {
            perror("Image data send failed");
            fclose(image_file);
            exit(EXIT_FAILURE);
        }

        // Clear the buffer after each read
        memset(buffer, 0, sizeof(buffer));
    }

    // Step 6: Close resources and connection
    fclose(image_file);
}


/*
*****************************
*          PRIVATE          *
*****************************
*/
