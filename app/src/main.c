#include <stdio.h>
#include <stdlib.h>
#include "../include/app_tcp_udp.h"
#include "camera.h"
#include "app_helper.h"
#include "buzzer.h"
#include "drive.h"
#include "ultrasonic.h"
#include "hal_helper.h"
#include "joystick.h"
#include "display.h"

static int pixelToDegree = 50;

void endAll();

int camera_operation(void)
{
    Tcp_init();
    char *imagePath = captureImage();
    sleepForMs(1000);
    int result_fromAI = Tcp_sendImage(imagePath);

    Tcp_cleanUp();
    char command[200];
    sprintf(command, "rm %s", imagePath);
    runCommand(command);
    free(imagePath);
    return result_fromAI;
}

bool checkSensor() {
    intmax_t distanceToTarget = getDistance();
    if (distanceToTarget < 180)
    {
        return true;
    } else {
        return false;
    }
}

int hasSeenPerson = 0;
int hasNotSeenPerson = 0;

int main() 
{
    //int count = 0;
    //initSegDisplay();
    Pwm_init();
    drive_init();  
    initializeUltrasonic();
    initializeJoystick();
    
    int humanPos;
    intmax_t distanceToTarget;
    while(true){

        humanPos = camera_operation();
        if (checkJoystick() != -1)
        {
            endAll();
            return 1;
        }
        
        printf("humanPos: %d\n",humanPos);
        if (humanPos >= 99999)
        {
            printf("no human found\n");
            turn_right(12);
            sleepForMs(1000);
            if (hasSeenPerson == 1)
            {
                hasNotSeenPerson++;
            }
            if (hasNotSeenPerson == 2 && hasSeenPerson == 1)
            {
                printf("Found the person! Returning");
                endAll();
                return 1;
            }
            continue;
            //humanPos = camera_operation();
            //printf("humanPos: %d\n",humanPos);
        }
        if(humanPos > 150){
            printf("Turning left\n");
            turn_left(humanPos/pixelToDegree);
            sleepForMs(1000);
            continue;
        }
        else if(humanPos < -150){
            printf("Turning right\n");
            turn_right(humanPos/pixelToDegree);
            sleepForMs(1000);
            continue;
        }
        hasSeenPerson = 1;
        printf("final humanPos: %d\n",humanPos);
        //break;
        if (checkJoystick() != -1)
        {
            endAll();
            return 1;
        }
         // printf("humanPos: %d\n",humanPos);
         //sleepForMs(1000);
        distanceToTarget = getDistance();
        printf("distance initial: %lld\n",distanceToTarget);
        // drive_set_both_wheels(true);
        if(distanceToTarget > 180){
            drive_set_both_wheels(true);
            sleepForMs(1000);
            //distanceToTarget = getDistance();
            drive_set_both_wheels(false);
            if (checkJoystick() != -1)
            {
                endAll();
                return 1;
            }
            sleepForMs(3000);
            distanceToTarget = getDistance();
            printf("avg distance to target in main: %lld\n",distanceToTarget);
            // count++;
        }
        if (distanceToTarget < 180)
        {
            printf("engaging fine tune mode.\n");
            break;
        }
    }
    if (checkJoystick() != -1)
    {
        endAll();
        return 1;
    }
    while (distanceToTarget > 150)
    {
        drive_set_both_wheels(true);
        sleepForMs((((double)distanceToTarget/2.0)/60.0)*1000.0);
        drive_set_both_wheels(false);
        if (checkJoystick() != -1)
        {
            endAll();
            return 1;
        }
        sleepForMs(3000);
        distanceToTarget = getDistance();
        printf("avg distance to target in fine tune: %lld\n",distanceToTarget);
    }
    endAll();
    return 0;
}

void endAll() {
    printf("Quitting Program!\n");
    //ultrasonicShutdown();
    drive_cleanup();
    //Display_cleanup();
    // pthread_t id= cleanupSegDisplay();
    // pthread_join(id, NULL);
}

//    int main() {
//         initializeUltrasonic();
//         while (true)
//         {
//             intmax_t distanceToTarget = getDistance();
//             printf("Average distance: is %lld", distanceToTarget);
//             sleepForMs(100);
//         }
//         ultrasonicShutdown();
//     return 0;
//    }