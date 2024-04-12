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
    int count = 0;
    Pwm_init();
    drive_init();  
    initializeUltrasonic();

    // int humanPos;
    // int humanPos = camera_operation();
    // while (humanPos >= 99999)
    // {
    //     sleepForMs(1000);
    //     humanPos = camera_operation();
    // }
    
    // printf("humanPos: %d\n",humanPos);
    double distanceToTarget = getDistance();
    printf("distance to target in main: %lf",distanceToTarget);
   // drive_set_both_wheels(true);
    while(count<5){
        drive_set_left_wheel(true);
        sleepForMs(1000);
        //distanceToTarget = getDistance();
        //printf("distance to target in main: %lf",distanceToTarget);
        drive_set_both_wheels(false);
        sleepForMs(5000);
        count++;
    }
    
    //play_sound();

    ultrasonicShutdown();
    drive_cleanup();
    return 0;
}