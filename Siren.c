#include <wiringPi.h>
#include <softPwm.h>
#include <softTone.h>
#include <stdio.h>
#include <stdlib.h>
#include "global.h"

void* StartSiren(void* args) {
    int gpio1 = 18, gpio2 = 16;
    pinMode(gpio1, OUTPUT);
    softToneCreate(gpio2);
    softPwmCreate(gpio1, 0, 100);
    int val = 300;
    while(BtnState){
        int i;
        for(i = 0; i < 100; i++){
            val = val + 4;
            softPwmWrite(gpio1, i);
            softToneWrite(gpio2, val);
            delay(5);
        }
        for(; i > 0; i--){
            val = val - 4;
            softPwmWrite(gpio1, i);
            softToneWrite(gpio2, val);
            delay(5);
        }
    }
    EndSiren(gpio1, gpio2);
    return NULL;
}

void EndSiren(int gpio1, int gpio2){
    softPwmWrite(gpio1, 0);
    softToneWrite(gpio2, 0);
}