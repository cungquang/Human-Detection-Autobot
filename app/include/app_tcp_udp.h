#ifndef _APP_TCP_UDP_H_
#define _APP_TCP_UDP_H_

/*
* Name: app_tcpServer
* Layer: application
* Main functionality:
*   - Initiate server
*   - Create 2 threads: receive & send 
*   - Clean up server 
*/

void Tcp_sendImage(char *imagePath);

#endif