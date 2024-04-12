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


    int humanPos = camera_operation();
    printf("humanPos: %d\n",humanPos);
    while(true){

        humanPos = camera_operation();
        //printf("humanPos: %d\n",humanPos);
        if (humanPos >= 99999)
        {
            printf("no human found\n");
            turn_right(20);
            sleepForMs(1000);
            humanPos = camera_operation();
            printf("humanPos: %d\n",humanPos);
        }
        if(humanPos > 100){
            turn_right(humanPos/100);
            sleepForMs(1000);
            continue;
        }
        else if(humanPos < 100){
            turn_left(humanPos/100);
            sleepForMs(1000);
            continue;
        }
        printf("final humanPos: %d\n",humanPos);
        break;

         // printf("humanPos: %d\n",humanPos);
        intmax_t distanceToTarget = getDistance();
        printf("distance initial: %lld\n",distanceToTarget);
        // drive_set_both_wheels(true);
        if(distanceToTarget > 180){
            drive_set_both_wheels(true);
            sleepForMs(2000); // drives for 60 cm per second
            //distanceToTarget = getDistance();
            drive_set_both_wheels(false);
            sleepForMs(3000);
            distanceToTarget = getDistance();
            printf("avg distance to target in main: %lld\n",distanceToTarget);
            // count++;
        }
    }
    
    intmax_t distanceToTarget = getDistance();
    while (distanceToTarget > 80)
    {
        break;
        drive_set_both_wheels(true);
        sleepForMs(((distanceToTarget/2)/60)*1000);
        drive_set_both_wheels(false);
        sleepForMs(3000);
        distanceToTarget = getDistance();
        printf("avg distance to target in main: %lld\n",distanceToTarget);
    }
   
    
    
    //play_sound();

    //ultrasonicShutdown();
    drive_cleanup();
    return 0;
}