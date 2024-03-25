#include <stdio.h>
/*
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 3000
#define BUFFER_SIZE 1024
#define CLIENT_PORT 3001
#define CLIENT_IP "192.168.7.2"

//terminate_flag
static int *isTerminated;

//Socket
static int server_socket;
static int client_socket;
static int is_connected = 0;

//Thread
static pthread_t listenTo_thread;
static pthread_t sendTo_thread;

// Private function initiate
void* tcpServer_receive();
void* tcpServer_send();



*****************************
*           PUBLIC          *
*****************************

/// @brief constructor to initiate TCP Server
void tcpServer_Init(int *terminate_flag)
{
    isTerminated = terminate_flag;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    //Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    //Initialize server address structure
    memset(&server_addr, '0', sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    //Bind socket to Address & PORT
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    //Listen for clients to connect
    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port %d...\n", PORT);

    //Wait for connection from Python Server
    while(!is_connected)
    {
        if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_len)) < 0) {
            perror("Accept failed");
            exit(EXIT_FAILURE);
        }

        // Convert client IP address to string
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);

        //verify IP address
        if(strcmp(client_ip, CLIENT_IP) != 0) 
        {
            printf("Client IP invalid");
            close(client_socket);
            continue;
        }
        else{
            is_connected = 1;
            printf("Client IP %s connected to server successfully", CLIENT_IP);
        }

        //Initiate thread - listenTo_thread & sendTo_thread
        if(pthread_create(&listenTo_thread, NULL, tcpServer_receive, NULL)) 
        {
            perror("Failed to create listenTo_thread");
            exit(EXIT_FAILURE);
        }

        if(pthread_create(&sendTo_thread, NULL, tcpServer_send, NULL))
        {
            perror("Failed to create sendTo_thread");
            exit(EXIT_FAILURE);
        }
    }
}

/// @brief function to clean up TCP Server
void tcpServer_cleanUp()
{
    // Join listenTo_thread & sendTo_thread
    pthread_join(listenTo_thread, NULL);
    pthread_join(sendTo_thread, NULL);

    //Close server socket
    if(client_socket)
    {
        close(client_socket);   
    }

    close(server_socket);
}


*****************************
*          PRIVATE          *
*****************************

/// @brief The thread handle listening to client
/// @return void
void* tcpServer_receive()
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    //While isTerminated == 0 -> keep waiting
    while(!*isTerminated)
    {
        //Parse data into buffer
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

        //Handle message
        if(bytes_received > 0)
        {
            buffer[bytes_received] = "\0";

            //Command to terminate proram
            if(strcmp(buffer, "terminated") == 0)
            {
                *isTerminated = 1;
            }

            //Do something here
            printf("Client said: %s", buffer);
        } 
        //Not receive data -> disconnect
        else if(bytes_received == 0)
        {
            //Do something here
            printf("Client has been disconnected");
        }
        else
        {
            //Do something here
            printf("Message error");
        }
    }

    //No clean up => will be handle outside
}

/// @brief The thread handles sending data to client
/// @return void
void* tcpServer_send()
{
    char buffer[BUFFER_SIZE];

    while(!*isTerminated)
    {
        //Replace below code with actual process
        fgets(buffer, BUFFER_SIZE, stdin);

        if(send(client_socket, buffer, strlen(buffer), 0) < 0)
        {
            //Fail to send message - server error -> terminate
            perror("Failed to send");
            *isTerminated = 1;
        }
    }

    //No clean up => will be handle outside
}*/