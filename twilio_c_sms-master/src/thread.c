#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <wiringPi.h>
#include "twilio.h"

int share_var = 1;
pthread_mutex_t mid; //뮤텍스 오브젝트
void *producerFunc(void *); //생산자 쓰레드 실행 함수
void *consumerFunc(void *); //소비자 쓰레드 실행 함수
int main(){
    pthread_t ptProduc, ptConsum;
    pthread_mutex_init(&mid, NULL); // 뮤텍스 초기화
    pthread_create(&ptProduc, NULL, producerFunc, NULL); //생산자 쓰레드 생성
    pthread_create(&ptConsum, NULL, consumerFunc, NULL); //소비자 쓰레드 생성
    pthread_join(ptProduc, NULL); //생산자 쓰레드 종료 대기
    pthread_join(ptConsum, NULL); //소비자 쓰레드 종료 대기
    pthread_mutex_destroy(&mid); // 뮤텍스 제거
    return 0;
}
void* producerFunc(void* arg){
    for(int i = 0; i<2 ; i++){
        pthread_mutex_lock(&mid); // 뮤텍스 잠금
        share_var++;
        pthread_mutex_unlock(&mid); // 뮤텍스 잠금 해제
        sleep(2);
    }
    return NULL;
}
void* consumerFunc(void* arg){
    for(int i = 0; i<2 ; i++){
        pthread_mutex_lock(&mid); // 뮤텍스 잠금
        char* account_sid="AC1751320dc8094d593bd610c4c36bafb8";
        char* auth_token="04ee95eec4c5c42b196bf1c9e1e0d21c";
        char* from_number="+19787974155";
        char* to_number = "+821043897972";
        char* message = "hi";
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
        pthread_mutex_unlock(&mid); // 뮤텍스 잠금 해제
        sleep(1);
    }
    return NULL;
}