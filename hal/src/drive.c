#include "drive.h"
#include "helpers.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define LEFT_WHEEL_DIRECTION_FILE_PATH "/sys/class/gpio/gpio68/direction"
#define LEFT_WHEEL_VALUE_FILE_PATH "/sys/class/gpio/gpio68/value"

#define RIGHT_WHEEL_DIRECTION_FILE_PATH "/sys/class/gpio/gpio69/direction"
#define RIGHT_WHEEL_VALUE_FILE_PATH "/sys/class/gpio/gpio69/value"


int pixels = 1920;


// Allow module to ensure it has been initialized (once!)
static bool is_initialized = false;
double rampUpTime = 0.04;
double secondPerRotation = 1.1;
double secondPerDegree;

static void SleepForTurn(int degree);


void drive_init(void) 
{
    assert(!is_initialized);
    is_initialized = true;
    run_command("config-pin p8.09 gpio");
    run_command("config-pin p8.10 gpio");
    set_char_value_in_file(LEFT_WHEEL_DIRECTION_FILE_PATH, "out");
    set_char_value_in_file(RIGHT_WHEEL_DIRECTION_FILE_PATH, "out");
    drive_set_left_wheel(false);
    drive_set_right_wheel(false);
    secondPerDegree = secondPerRotation/360.0;
}

static void SleepForTurn(int degree)
{   
    if (degree == 0)
    {
        return;
    }
    usleep(abs((degree * secondPerDegree * 1000000)+1000*(80.0/(double)degree)));
    printf("standard turn time: %f\n",degree * secondPerDegree * 1000000);
    printf("added turn time: %d\n",1000*80.0/(double)degree);
}

void turn_left(int degree)
{
    drive_set_left_wheel(false);
    drive_set_right_wheel(true);
    SleepForTurn(degree);
    drive_set_left_wheel(false);
    drive_set_right_wheel(false);
}

void turn_right(int degree)
{
    drive_set_left_wheel(true);
    drive_set_right_wheel(false);
    SleepForTurn(degree);
    drive_set_left_wheel(false);
    drive_set_right_wheel(false);
}

void drive_set_left_wheel(bool forward)
{
    assert(is_initialized);
    set_int_value_in_file(LEFT_WHEEL_VALUE_FILE_PATH, !forward ? 1 : 0);
}

void drive_set_right_wheel(bool forward)
{
    assert(is_initialized);
    set_int_value_in_file(RIGHT_WHEEL_VALUE_FILE_PATH, !forward ? 1 : 0);
}

void drive_set_both_wheels(bool forward)
{
    assert(is_initialized);
    drive_set_left_wheel(forward);
    drive_set_right_wheel(forward);
}

void drive_cleanup(void)
{
    // Free any memory, close files, ..
    assert(is_initialized);
    is_initialized = false;
}