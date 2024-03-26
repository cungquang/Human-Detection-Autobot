#include <stdio.h>
#include <stdlib.h>
#include "../include/app_tcp_udp.h"

int main() 
{
    //printf("Hello World");
    char *imagePath = "ai_server/pictures/test1.JPG";
    Tcp_sendImage(imagePath);

    return 0;
}