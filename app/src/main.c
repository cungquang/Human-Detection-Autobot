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

   
    for (int i =0;i<5;i++){
        double distance = getDistance();
        printf("distance %lf\n",distance);
        sleepForMs(1000);
    }

    //int humanPos;
    //humanPos = camera_operation();
    //printf("humanPos: %d\n",humanPos);
    //if (humanPos<99999){
    drive_set_both_wheels(true);
    int count = 0;
    while(count<5){
        count++;
        // double distance = getDistance();
        // printf("distance %lf\n",distance);
        // if(distance < 5 && distance > 0){
        //     printf("stopping after detection\n");
        //     drive_set_both_wheels(false);
        //     play_sound();
        //     break;
        // }
        sleepForMs(1000);
    }
    drive_set_both_wheels(false);
    //}
    sleepForMs(1000);
    for (int i =0;i<5;i++){
        // double distance = getDistance();
        // printf("distance %lf\n",distance);
        sleepForMs(1000);
    }

    ultrasonicShutdown();
    drive_cleanup();
    return 0;
}