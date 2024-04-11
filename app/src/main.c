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


    int humanPos;
    humanPos = camera_operation();

    if (humanPos<99999){
        drive_set_both_wheels(true);
        sleepForMs(1000);
        drive_set_both_wheels(false);
    }


    drive_cleanup();
    return 0;
}