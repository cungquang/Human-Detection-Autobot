#ifndef _APP_TCPSERVER_H_
#define _APP_TCPSERVER_H_

/*
* Name: app_tcpServer
* Layer: application
* Main functionality:
*   - Initiate server
*   - Create 2 threads: receive & send 
*   - Clean up server 
*/

void tcpServer_Init(int *terminate_flag);
void tcpServer_cleanUp();

#endif