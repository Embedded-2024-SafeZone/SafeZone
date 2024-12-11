#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>
#include <stdlib.h>
#include "global.h"
#include "twilio.h"

bool BtnState;
int prevState = 0;
int currState = 0;

int main(){
    BtnState = FALSE;
    wiringPiSetupGpio();
    pinMode(17, INPUT);
    pthread_t sirenPthread;
    pthread_t cameraPthread;
    pthread_t ServoOnPthread;
    pthread_t ServoOffPthread;
    pthread_t mailPthread;
    int r_val;
    while (TRUE)
    {
        currState = digitalRead(17);

        if(currState == 0 && prevState == 1){
            BtnState = !BtnState;

            if(BtnState){
                pthread_create(&sirenPthread, NULL, StartSiren, NULL);
                pthread_create(&cameraPthread, NULL, handle_button_press, NULL);
                pthread_create(&ServoOnPthread, NULL, rotate_Servo_on, NULL);
                pthread_create(&mailPthread, NULL, consumerFunc, NULL);
                if(r_val < 0){
                    perror("pthread_create() error\n");
                    exit(0);
                }
            }
            else if(!BtnState){
                pthread_create(&ServoOffPthread, NULL, rotate_Servo_off, NULL);
            }
            delay(200);
        }
        prevState = currState;
    }
    
    return 0;
}
