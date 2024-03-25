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

#define ECHO_PATH_DIRECTION       "/sys/class/gpio/gpio66/direction" // p8.07
#define ECHO_PATH_VALUE       "/sys/class/gpio/gpio66/value" // p8.07
#define TRIGGER_PATH_DIRECTION    "/sys/class/gpio/gpio67/direction" // p8.08
#define TRIGGER_PATH_VALUE        "/sys/class/gpio/gpio67/value" // p8.08

#define SPEED_OF_SOUND_CM_PER_SEC 34300 // Speed of sound in cm/s
#define SPEED_OF_SOUND_CM_PER_MS 34.3 // Speed of sound in cm/ms

void initializeSensor() {
    runCommand("config-pin p8.07 gpio");
    runCommand("config-pin p8.08 gpio");
    writeToFile(ECHO_PATH_DIRECTION, "in");
    writeToFile(TRIGGER_PATH_DIRECTION, "out");
}

int main() {
    initializeSensor();
    double distance;
    int loop_count = 500;  // Number of measurements to take

    while (loop_count--) {
        // Measure the length of the pulse on the echo pin
        long long startTime;
        long long initialTime;
        long long stopTime;
        long long elapsed_time_ms = 0;
        long long timeout_ms = 3000;
        // Send a 10us pulse to trigger pin
        writeToFile(TRIGGER_PATH_VALUE, "1");
        sleepForMs(0.1);  // Wait 0.1ms
        writeToFile(TRIGGER_PATH_VALUE, "0");
        //printf("did the trigger\n");
        initialTime = getTimeInMs();
        while (valueReader(ECHO_PATH_VALUE) == 0 && elapsed_time_ms < timeout_ms) {  // Wait for echo to go high
            startTime = getTimeInMs();
            elapsed_time_ms = startTime - initialTime;
        }
        elapsed_time_ms = 0;
        while (valueReader(ECHO_PATH_VALUE) == 1 && elapsed_time_ms < timeout_ms) {
            //printf("Waiting for echo to return\n");
            stopTime = getTimeInMs();
            elapsed_time_ms = stopTime - startTime;
        }

        if (elapsed_time_ms >= timeout_ms) {
            printf("Timeout! Object is too far.\n");
        } else {
            long long timeElapsed = stopTime - startTime;
            //double timeElapsedInSec = (double)timeElapsed/1000;
            // Calculate distance in centimeters (assumes speed of sound is 343m/s)
            distance = (double)timeElapsed * SPEED_OF_SOUND_CM_PER_MS / 2.0;  // in cm
            if (distance < 0)
            {
                printf("Distance returned as negative!\n");
            } else {
                printf("Distance: %lf cm\n", distance);
            }
        }
        
        sleepForMs(100);  // Wait before next measurement
    }
    return 0;
}
