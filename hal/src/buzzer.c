#include "buzzer.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "app_helper.h"
#include "hal_helper.h"


#define buzzer_period "/dev/bone/pwm/0/a/period"
#define buzzer_dutyCycle "/dev/bone/pwm/0/a/duty_cycle"
#define buzzer_enable "/dev/bone/pwm/0/a/enable"

//static bool endProgram = false;
//static bool play = false;

void play_sound() {
    //printf("buzzer miss");

    write_int_to_file(buzzer_dutyCycle, 0);
    write_int_to_file(buzzer_period, 1000000);
    write_int_to_file(buzzer_dutyCycle, 20000);  
    write_int_to_file(buzzer_enable, 1);
    sleepForMs(250);

    write_int_to_file(buzzer_dutyCycle, 0);
    write_int_to_file(buzzer_period, 1000000);
    write_int_to_file(buzzer_dutyCycle, 5000); 
    write_int_to_file(buzzer_enable, 1);
    sleepForMs(250);

    write_int_to_file(buzzer_dutyCycle, 0);
    write_int_to_file(buzzer_period, 500000);  
    write_int_to_file(buzzer_dutyCycle, 20000);
    write_int_to_file(buzzer_enable, 1);
    sleepForMs(250);

    write_int_to_file(buzzer_dutyCycle, 0);
    write_int_to_file(buzzer_period, 1000000);
    write_int_to_file(buzzer_dutyCycle, 20000);
    write_int_to_file(buzzer_enable, 1);
    sleepForMs(250);

    write_int_to_file(buzzer_enable, 0);
}

// static void* buzzer_loop() {
    
// 	while (!endProgram)
// 	{
//         if(play){
//             play = false;
//             play_sound();
//         } 
//     }
// 	return NULL;
// }

void Pwm_init() {
    runCommand("sudo config-pin p9_22 pwm");
	//pthread_create(&buzzerThread, NULL, buzzer_loop, NULL);
}

void Pwm_cleanup() {
    //endProgram = true;
    //pthread_join(buzzerThread, NULL);
}