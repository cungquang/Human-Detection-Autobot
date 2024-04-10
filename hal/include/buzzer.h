#ifndef _BUZZER_H_
#define _BUZZER_H_
// Module that controls the buzzer

// Start the buzzer
void Pwm_init(void);
// Clean up the buzzer
void Pwm_cleanup(void);

void play_sound();

#endif