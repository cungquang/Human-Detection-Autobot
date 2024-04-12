// Joystick module
// Part of the Hardware Abstraction Layer (HAL) 
// Controls the entire joystick

#include "joystick_direction.h"
#include "joystick.h"
#include "hal_helper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include <pthread.h>

// Joystick File Paths
#define JOYSTICK_UP_PATH    "/sys/class/gpio/gpio26"

#define JOYSTICK_MAX_NUMBER_DIRECTIONS  1

static struct JoystickDirection joystickDirectionArray[JOYSTICK_MAX_NUMBER_DIRECTIONS];
static bool initialized = false;
//static pthread_t joystickThread;
//bool endProgram = false;

//void* startJoystickThread();

// Run the linux commands to set the pins as GPIO
static void configureJoystickPins()  {
    runCommand("config-pin p8.14 gpio");
}


// Initializes all Joystick directions
void initializeJoystick() {
    configureJoystickPins();
    initializeJoystickDirection(&joystickDirectionArray[UP_DIRECTION], JOYSTICK_UP_PATH);

    initialized = true;
    //pthread_create(&joystickThread, NULL, startJoystickThread, NULL);
}

// Check if the joystick is pressed in any direction and return the direction its pressed in as an int
int checkJoystick() {
    assert(initialized);
    
    for (size_t i = 0; i < JOYSTICK_MAX_NUMBER_DIRECTIONS; i++)
    {
        if (isPressed(&joystickDirectionArray[i]))
        {
            // Return the direction the joystick is pressed in
            printf("Joystick is pressed!\n");
            return i;
        }
    }
    // Return -1 if the joystick is not pressed in any direction
    return -1;
}

// void* workJoystickThread() {
//     if (checkJoystick != -1)
//     {
        
//     }
    
// }

// void endJoystickThread() {
//     endProgram = true;
//     pthread_join(joystickThread, NULL);
// }