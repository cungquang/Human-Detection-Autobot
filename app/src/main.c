#include <stdio.h>
#include <stdlib.h>
#include "../include/app_tcp_udp.h"

int main() 
{
    //printf("Hello World");
    char *imagePath = "./test1.JPG";
    Tcp_Init();
    Tcp_sendImage(imagePath);
    Tcp_cleanUp();
    
    return 0;
}