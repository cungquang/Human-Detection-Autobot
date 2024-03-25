#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024

//#define SERVER_IP '192.168.7.2'
#define SERVER_PORT 7777
#define CLIENT_IP '192.168.142.129'                    
#define CLIENT_IP_TEST '127.0.0.1'
#define CLIENT_PORT 3001

//terminate_flag
static int isTerminated;

//Socket
static int server_socket;
static int client_socket;
static int is_connected = 0;

//Thread
static pthread_t listenTo_thread;
static pthread_t sendTo_thread;

// Private function initiate
void* TcpServer_listenTo();
void* TcpServer_send();


/*
*****************************
*           PUBLIC          *
*****************************
*/


void TcpServer_Init()
{
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
    //server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(SERVER_PORT);

    //Bind socket to Address & PORT
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }
    
    //Listen for clients to connect
    if (listen(server_socket, 1) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    
    printf("Server listening on port  %d...\n", SERVER_PORT);

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
        if(strcmp(client_ip, CLIENT_IP_TEST) != 0) 
        {
            printf("Client IP invalid");
            close(client_socket);
            continue;
        }
        else{
            is_connected = 1;
            printf("Client IP %s connected to server successfully", CLIENT_IP);
        }

        //Initiate thread - TcpServer_listenTo
        if(pthread_create(&listenTo_thread, NULL, TcpServer_listenTo, NULL)) 
        {
            perror("Failed to create listenTo_thread");
            exit(EXIT_FAILURE);
        }

        //Initiate thread - TcpServer_send
        if(pthread_create(&sendTo_thread, NULL, TcpServer_send, NULL))
        {
            perror("Failed to create sendTo_thread");
            exit(EXIT_FAILURE);
        }
    }
}


void TcpServer_cleanUp()
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

void TcpServer_setTerminate()
{
    isTerminated = 1;
}

/*
*****************************
*          PRIVATE          *
*****************************
*/

//Thread to receive message
void* TcpServer_listenTo()
{
    char buffer[BUFFER_SIZE];
    ssize_t bytes_received;

    //While isTerminated == 0 -> keep waiting
    while(!isTerminated)
    {
        //Parse data into buffer
        bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);

        //Handle message
        if(bytes_received > 0)
        {
            buffer[bytes_received] = '\0';

            //Command to terminate proram
            if(strcmp(buffer, "terminated") == 0)
            {
                TcpServer_setTerminate();
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
    return NULL;
}

//Thread to send message
void* TcpServer_send()
{
    char buffer[BUFFER_SIZE];

    while(!isTerminated)
    {
        //Replace below code with actual process
        fgets(buffer, BUFFER_SIZE, stdin);

        if(send(client_socket, buffer, strlen(buffer), 0) < 0)
        {
            //Fail to send message - server error -> terminate
            perror("Failed to send");
            TcpServer_setTerminate();
        }
    }

    //No clean up => will be handle outside
    return NULL;
}