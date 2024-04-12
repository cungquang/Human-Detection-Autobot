
#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_

#include <stdint.h>

void initializeUltrasonic();
void* ultrasonicLoop();
intmax_t getDistance();
void ultrasonicShutdown();

#endif