#include <stdio.h>
#include <stdlib.h>
#include "../include/app_tcp_udp.h"

int main() 
{
    //printf("Hello World");
    char *imagePath = "./test1.JPG";
    Tcp_init();
    int result_fromAI = Tcp_sendImage(imagePath);
    Tcp_cleanUp();

    printf("Result from AI: %dcm\n", result_fromAI);
    return 0;
}