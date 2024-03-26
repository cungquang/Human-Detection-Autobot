#ifndef _APP_HELPER_H_
#define _APP_HELPER_H_

//Application level
// This module provide a list of helper functions
// - get time (in ms)
// - sleep for an amount of time (in ms)

long long getTimeInMs(void);
void sleepForMs(long long delayInMs);
unsigned char* read_image_inByte(const char *input_path, long *filesize);

#endif