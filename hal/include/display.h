//hardware abstraction layer for controlling 14-segment display
#ifndef _SEG_H_
#define _SEG_H_

#include <stdbool.h>
#include <stdatomic.h>
#include <pthread.h>

void initSegDisplay();
void setSegDisplay(int digit);
void* segMain();
void startCleanupSegDisplay();
pthread_t cleanupSegDisplay();

#endif