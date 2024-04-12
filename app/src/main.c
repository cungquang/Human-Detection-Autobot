#include <stdio.h>
#include <stdlib.h>
#include "../include/app_tcp_udp.h"
#include "camera.h"
#include "app_helper.h"
#include "buzzer.h"
#include "drive.h"
#include "ultrasonic.h"

int camera_operation(void)
{
    Tcp_init();
    char *imagePath = captureImage();
    int result_fromAI = Tcp_sendImage(imagePath);

    Tcp_cleanUp();
    return result_fromAI;
}

int main() 
{
    Pwm_init();
    drive_init();  
    initializeUltrasonic();

    // int humanPos;
    int humanPos = camera_operation();
    while (humanPos >= 99999)
    {
        sleepForMs(1000);
        humanPos = camera_operation();
    }
    
    // printf("humanPos: %d\n",humanPos);
    double distanceToTarget = getDistance();

    drive_set_both_wheels(true);
    while(distanceToTarget>40){
        distanceToTarget = getDistance();
        sleepForMs(100);
    }
    drive_set_both_wheels(false);
    play_sound();

    ultrasonicShutdown();
    drive_cleanup();
    return 0;
}