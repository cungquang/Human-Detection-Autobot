// Joystick Direction module
// Part of the Hardware Abstraction Layer (HAL) 
// Controls a direction of the joystick

#include "joystick_direction.h"
#include "hal_helper.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define directionName     "/direction"
#define valueName    "/value"

void initializeJoystickDirection(struct JoystickDirection *joystick, const char *filePath) {
    // Copy filePath to directionPath and valuePath
    snprintf(joystick->directionPath, sizeof(joystick->directionPath), "%s", filePath);
    snprintf(joystick->valuePath, sizeof(joystick->valuePath), "%s", filePath);

    // Concatenate directionName to directionPath and valuePath
    snprintf(joystick->directionPath + strlen(joystick->directionPath),
             sizeof(joystick->directionPath) - strlen(joystick->directionPath),
             "%s", directionName);

    snprintf(joystick->valuePath + strlen(joystick->valuePath),
             sizeof(joystick->valuePath) - strlen(joystick->valuePath),
             "%s", valueName);

    fileWriter(joystick->directionPath, "in");
}

bool isPressed(struct JoystickDirection *joystick) {
    for (size_t i = 0; i < 10; i++)
    {
        int count = 0;
        if (!valueReader(joystick->valuePath))
        {
            count++;
        }
        if (count == 10)
        {
            return !valueReader(joystick->valuePath);
        }
    }
    return false;
}
