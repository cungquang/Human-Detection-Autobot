#include <stdio.h>
#include <stdlib.h>
#include "../include/app_tcp_udp.h"
#include "camera.h"
#include "app_helper.h"

void operation(void)
{
    //printf("Hello World");
    configureCamera();
    char *imagePath = "./grabber.jpg";
    sleepForMs(5000);
    Tcp_init();
    //char *imagePath = captureImage();
    int result_fromAI = Tcp_sendImage(imagePath);
    Tcp_cleanUp();
    //free(imagePath);

    if (result_fromAI == 0)
    {
        printf("No human detected within the image!\n");
    } else {
        printf("Result from AI: %d pixel\n", result_fromAI);
    }
    cameraShutdown();
}

int main() 
{
    Tcp_init();
    char *imagePath = "../grabber.jpg";
    int result_fromAI = Tcp_sendImage(imagePath);
    Tcp_cleanUp();

    if (!result_fromAI)
    {
        printf("No human detected within the image!\n");
    } else {
        printf("Result from AI: %d pixel\n", result_fromAI);
    }
    return 0;
}