// Joystick Direction module
// Part of the Hardware Abstraction Layer (HAL) 
// Controls a direction of the joystick

#ifndef JOYSTICK_DIRECTION_H
#define JOYSTICK_DIRECTION_H

#include <stdbool.h>

#define JOYSTICK_PATH_BUFF  100

struct JoystickDirection {
    // Holds the path of the direction file
    char directionPath[JOYSTICK_PATH_BUFF];
    // Holds the path of the value file
    char valuePath[JOYSTICK_PATH_BUFF];
};

void initializeJoystickDirection(struct JoystickDirection *joystick, const char *filePath);
bool isPressed(struct JoystickDirection *joystick);

#endif