// Part of the Hardware Abstraction Layer (HAL) 

#ifndef _DRIVE_H_
#define _DRIVE_H_

#include <stdbool.h>

void drive_init(void);
void turn_left(int degree);
void turn_right(int degree);
void drive_set_left_wheel(bool forward);
void drive_set_right_wheel(bool forward);
void drive_set_both_wheels(bool forward);
void drive_cleanup(void);

#endif