// Joystick module
// Part of the Hardware Abstraction Layer (HAL) 
// Controls the entire joystick

#ifndef JOYSTICK_H
#define JOYSTICK_H

#define UP_DIRECTION    0
#define DOWN_DIRECTION  1
#define RIGHT_DIRECTION 2
#define LEFT_DIRECTION  3

// Initializes all Joystick directions
void initializeJoystick();
// Check if the joystick is pressed in any direction and return the direction its pressed in as an int
int checkJoystick();

#endif