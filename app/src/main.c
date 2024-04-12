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
    buzzer_init();

    int humanPos;
    humanPos = camera_operation();
    printf("humanPos: %d\n",humanPos);
    if (humanPos<99999){
        double distanceToTarget = getDistance();
        while(distanceToTarget > 150){
            drive_set_both_wheels(true);
            sleepForMs(1000);
            drive_set_both_wheels(false);
            sleepForMs(1000);
            distanceToTarget = getDistance();
            printf("distanceToTarget %lf\n",distanceToTarget);
        }
        play_sound();
    }

    ultrasonicShutdown();
    drive_cleanup();
    return 0;
}