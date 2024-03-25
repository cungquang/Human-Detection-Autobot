#include <stdio.h>
#include <stdlib.h>
#include "../include/app_tcpServer.h"


int main() 
{
    //printf("Hello World");
    TcpServer_Init();
    TcpServer_cleanUp();

    return 0;
}