#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>

extern bool BtnState;
void* StartSiren(void* args);
void EndSiren(int gpio1, int gpio2);
void* handle_button_press(void* arg);
void* rotate_Servo_on(void* arg);
void* rotate_Servo_off(void* arg);
void* consumerFunc(void* arg);
#endif