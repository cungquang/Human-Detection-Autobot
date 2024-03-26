#include <time.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>		// Errors
#include <string.h>
#include <sys/epoll.h>  // for epoll()
#include <fcntl.h>      // for open()
#include <unistd.h>     // for close()

//Get time (in ms)
long long getTimeInMs(void)
{
    struct timespec spec;
    clock_gettime(CLOCK_REALTIME, &spec);
    long long seconds = spec.tv_sec;
    long long nanoSeconds = spec.tv_nsec;
    long long milliSeconds = seconds * 1000
    + nanoSeconds / 1000000;
    return milliSeconds;
}

//Sleep for a certain amount of time (in ms)
void sleepForMs(long long delayInMs)
{
    const long long NS_PER_MS = 1000 * 1000;
    const long long NS_PER_SECOND = 1000000000;
    long long delayNs = delayInMs * NS_PER_MS;
    int seconds = delayNs / NS_PER_SECOND;
    int nanoseconds = delayNs % NS_PER_SECOND;
    struct timespec reqDelay = {seconds, nanoseconds};
    nanosleep(&reqDelay, (struct timespec *) NULL);
}

unsigned char* read_image_inByte(char *input_path, long *filesize) {
    FILE *fp = fopen(input_path, "rb");
    if (fp == NULL) {
        perror("Error opening file");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    *filesize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    unsigned char *image_data = (unsigned char *)malloc(*filesize);
    if (image_data == NULL) {
        fclose(fp);
        perror("Memory allocation failed");
        return NULL;
    }

    fread(image_data, 1, *filesize, fp);
    fclose(fp);

    return image_data;
}