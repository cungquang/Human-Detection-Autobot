#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "hal_helper.h"
#include "app_helper.h"
#include "ultrasonic.h"
#include <stdint.h>
#include <time.h>
#include <pthread.h>

#define ECHO_PATH_DIRECTION       "/sys/class/gpio/gpio60/direction" // p9.12
#define ECHO_PATH_VALUE       "/sys/class/gpio/gpio60/value" // p9.12
#define TRIGGER_PATH_DIRECTION    "/sys/class/gpio/gpio50/direction" // p9.14
#define TRIGGER_PATH_VALUE        "/sys/class/gpio/gpio50/value" // p9.14

#define SPEED_OF_SOUND_CM_PER_SEC 34300 // Speed of sound in cm/s
#define SPEED_OF_SOUND_CM_PER_MS 34.3 // Speed of sound in cm/ms
#define SPEED_OF_SOUND_CM_PER_US 0.0343 // Speed of sound in cm/us
#define SPEED_OF_SOUND_CM_PER_NS  0.0000343 // Speed of sound in cm/ns

static bool endProgram = false;

void initializeUltrasonic() {
    runCommand("config-pin p9.12 gpio");
    runCommand("config-pin p9.14 gpio");
    writeToFile(ECHO_PATH_DIRECTION, "in");
    writeToFile(TRIGGER_PATH_DIRECTION, "out");
}

double getDistance(){
    intmax_t  distance;
    // Measure the length of the pulse on the echo pin
    intmax_t startTime = 0;
    intmax_t initialTime = 0;
    intmax_t stopTime = 0;
    intmax_t elapsed_time_ns = 0;
    intmax_t timeout_ns= 80000000; // 80 ms

    writeToFile(TRIGGER_PATH_VALUE, "1");
    sleepForMs(0.001);  // Wait 0.001ms
    writeToFile(TRIGGER_PATH_VALUE, "0");
    initialTime = getCurrentTimeNanoseconds();

     while (valueReader(ECHO_PATH_VALUE) == 0 && elapsed_time_ns < timeout_ns) {  // Wait for echo to go high
        startTime = getCurrentTimeNanoseconds();
        elapsed_time_ns = startTime - initialTime;
    }
    elapsed_time_ns = 0;
    while (valueReader(ECHO_PATH_VALUE) == 1 && elapsed_time_ns < timeout_ns) {
        //printf("Waiting for echo to return\n");
        stopTime = getCurrentTimeNanoseconds();
        elapsed_time_ns = stopTime - startTime;
    }

    if (elapsed_time_ns >= timeout_ns) {
        return -1;
    } else {
        intmax_t timeElapsed = stopTime - startTime;
        //double timeElapsedInSec = (double)timeElapsed/1000;
        // Calculate distance in centimeters (assumes speed of sound is 343m/s)
        distance = timeElapsed * SPEED_OF_SOUND_CM_PER_NS / 2.0;  // in cm
        //printf("Time elapsed is: %lld ms\n", timeElapsed);
        if (distance < 0)
        {
            return -1;
        } else {
            return distance;
        }
    }


}

void* ultrasonicLoop() {
    intmax_t  distance;

    while (!endProgram) {
        // Measure the length of the pulse on the echo pin
        intmax_t startTime = 0;
        intmax_t initialTime = 0;
        intmax_t stopTime = 0;
        intmax_t elapsed_time_ns = 0;
        intmax_t timeout_ns= 1000000000; // 1 second
        // Send a 10us pulse to trigger pin
        writeToFile(TRIGGER_PATH_VALUE, "1");
        sleepForMs(0.001);  // Wait 0.001ms
        writeToFile(TRIGGER_PATH_VALUE, "0");
        //printf("did the trigger\n");
        initialTime = getCurrentTimeNanoseconds();
        while (valueReader(ECHO_PATH_VALUE) == 0 && elapsed_time_ns < timeout_ns) {  // Wait for echo to go high
            startTime = getCurrentTimeNanoseconds();
            elapsed_time_ns = startTime - initialTime;
        }
        elapsed_time_ns = 0;
        while (valueReader(ECHO_PATH_VALUE) == 1 && elapsed_time_ns < timeout_ns) {
            //printf("Waiting for echo to return\n");
            stopTime = getCurrentTimeNanoseconds();
            elapsed_time_ns = stopTime - startTime;
        }

        if (elapsed_time_ns >= timeout_ns) {
            printf("Timeout! Object is too far.\n");
        } else {
            intmax_t timeElapsed = stopTime - startTime;
            //double timeElapsedInSec = (double)timeElapsed/1000;
            // Calculate distance in centimeters (assumes speed of sound is 343m/s)
            distance = timeElapsed * SPEED_OF_SOUND_CM_PER_NS / 2.0;  // in cm
            //printf("Time elapsed is: %lld ms\n", timeElapsed);
            if (distance < 0)
            {
                printf("Distance returned as negative!\n");
            } else {
                printf("Distance: %jd cm\n", distance);
            }
        }
        
        sleepForMs(100);  // Wait before next measurement
    }
    return 0;
}

