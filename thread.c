#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>
#include "twilio.h"

int share_var = 1;
void *producerFunc(void *); //생산자 쓰레드 실행 함수
void *consumerFunc(void *); //소비자 쓰레드 실행 함수

void* consumerFunc(void* arg){
    for(int i = 0; i<2 ; i++){
        char* account_sid="<아이디>";
        char* auth_token="<토큰>";
        char* from_number="+19787974155";
        char* to_number = "<번호>";
        char* message = "Help!";
        char *picture_url = "https://demo.twilio.com/logo.png";
        bool verbose = true;
        twilio_send_message(account_sid, auth_token, from_number,to_number, message,picture_url , true);
        

        if (twilio_send_message(account_sid,
                                auth_token,
                                message,
                                from_number,
                                to_number,
                                picture_url,
                                verbose
        ) == true) {
            ;
        } else {
            ;
        }
        sleep(1);
    }
    return NULL;
}
