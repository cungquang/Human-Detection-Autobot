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
    //int count = 0;
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
    intmax_t distanceToTarget = getDistance();
    printf("distance to target in main: %lld",distanceToTarget);
   // drive_set_both_wheels(true);
    while(distanceToTarget > 40){
        drive_set_both_wheels(true);
        sleepForMs(1000);
        //distanceToTarget = getDistance();
        printf("distance to target in main: %lld",distanceToTarget);
        drive_set_both_wheels(false);
        sleepForMs(3000);
        distanceToTarget = getDistance();
       // count++;
    }
    
    //play_sound();

    //ultrasonicShutdown();
    drive_cleanup();
    return 0;
}