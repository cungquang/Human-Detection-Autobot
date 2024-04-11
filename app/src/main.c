#include <stdio.h>
#include <stdlib.h>
#include "../include/app_tcp_udp.h"
#include "camera.h"
#include "app_helper.h"
#include "buzzer.h"
#include "drive.h"
#include "ultrasonic.h"

void camera_operation(void)
{
    Tcp_init();
    char *imagePath = captureImage();
    int result_fromAI = Tcp_sendImage(imagePath);

    if (result_fromAI >= 99999)
    {
        printf("No human detected within the image!\n");
    } else {
        printf("Result from AI: %d pixel\n", result_fromAI);
    }
    Tcp_cleanUp();
}

int main_operation(void)
{
    Pwm_init();
    drive_init();  
    initializeUltrasonic();
    double distance = 0;
    for (int i=0;i<10;i++)
    {
        distance = getDistance();
        printf("Distance: %f cm\n", distance);
        sleepForMs(1000);
    }

    drive_cleanup();
    return 0;

}

void tcp_testing(void)
{
    Pwm_init();
    drive_init();  

    for(int i = 0; i < 10; i++)
    {

    }
}

int main() 
{
    main_operation();
    return 0;
}