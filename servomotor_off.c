#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

void* rotate_Servo_off(void* arg){
    pinMode(18, PWM_OUTPUT);
    float duty = (0.5 + ((0 + 90.0) / 90)) * 100;
    pwmSetMode(PWM_MODE_MS);
    pwmSetRange(2000);
    pwmSetClock(192);
    pwmWrite(18, duty);
}