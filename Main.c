#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>
#include <stdlib.h>
#include "global.h"

bool BtnState;
int prevState = 0;
int currState = 0;

pthread_mutex_t mid;
int main(){
    BtnState = FALSE;
    wiringPiSetupGpio();
    pinMode(17, INPUT);
    pthread_t sirenPthread;
    int r_val;
    while (TRUE)
    {
        currState = digitalRead(17);

        if(currState == 1 && prevState == 0){
            BtnState = !BtnState;

            if(BtnState){
                pthread_create(&sirenPthread, NULL, StartSiren, NULL);

                if(r_val < 0){
                    perror("pthread_create() error\n");
                    exit(0);
                }
            }

            delay(200);
        }

        prevState = currState;
        
        // else{
        //     // 서보모터 내려가는 거
        //     // 사이렌 종료
        //     // 문자 종료
        // }
    }
    
    return 0;
}