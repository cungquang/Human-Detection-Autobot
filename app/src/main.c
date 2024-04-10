#include <stdio.h>
#include <stdlib.h>
#include "../include/app_tcp_udp.h"
#include "camera.h"
#include "app_helper.h"
#include "buzzer.h"

void camera_operation(void)
{
    Tcp_init();
    char *imagePath = captureImage();
    int result_fromAI = Tcp_sendImage(imagePath);

    if (!result_fromAI)
    {
        printf("No human detected within the image!\n");
    } else {
        printf("Result from AI: %d pixel\n", result_fromAI);
    }
    Tcp_cleanUp();
}

int main() 
{
    Pwm_init();
    for(int i = 0; i < 4; i++)
    {
        camera_operation();
    }
    return 0;
}