#ifndef _HAL_HELPER_H_
#define _HAL_HELPER_H_

// BeagleBone hardware level
// This module provides helper functions for read/write into GPIO files
// - Support Write operation to GPIO files
// - Support Read operation to GPIO files

void writeToFile(const char* fileToWriteGpio, const char* content);
void readFromFile(const char* fileToReadGpio, char* buff, unsigned int maxLength);

#endif