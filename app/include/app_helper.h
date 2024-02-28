#ifndef _APP_HELPER_H_
#define _APP_HELPER_H_

#include <stdbool.h>

//Application level
// This module provide a list of helper functions
// - get time (in ms)
// - sleep for an amount of time (in ms)

long long getTimeInMs(void);
void sleepForMs(long long delayInMs);

#endif