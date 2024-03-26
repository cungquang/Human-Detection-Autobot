#include <stdio.h>
#include <stdlib.h>
#include "../include/app_tcp_udp.h"

int main() 
{
    //printf("Hello World");
    char *imagePath = "./test2.JPG";
    Tcp_init();
    Tcp_sendImage(imagePath);
    Tcp_cleanUp();

    return 0;
}