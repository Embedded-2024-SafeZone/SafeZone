# Safe Zone Project

## 개요
**Safe Zone Project**는 Raspberry Pi를 활용하여 하드웨어와 소프트웨어 기능을 결합해 개인 안전을 강화하기 위해 설계. 버튼 한 번으로 여러 가지 안전 메커니즘이 동시에 활성화

- LED와 부저를 사용한 지속적인 사이렌 기능
- 문 앞을 1분간 촬영한 영상을 이메일로 전송
- 외부 Api인 Twilio를 사용하여 SMS 문자 알림
- 도어 잠금/해제를 위한 서보 모터 작동

모든 기능은 C언어의 스레드를 사용하여 동시에 실행

---

## 시스템 구조

![image](https://github.com/user-attachments/assets/25549646-9d84-4763-bd5a-cc3e7f83f7e9)
- 버튼 입력을 받고 나면 라즈베리파이로 신호가 전달
- 신호를 전달 받으면 BtnState가 변경됨
- BtnState에 따라 잠금/해제, 녹화, 사이렌 등의 기능이 멀티 쓰레드로 반복 실행 또는 중단
- 긴급상황 알림은 즉시 twilio를 통해 SMS로 전송
- 1분간 녹화한 영상은 녹화 후 지정된 메일로 전송

---

## 주요 기능

1. **사이렌 기능**
   - LED와 부저를 활성화하여 시청각 경고를 제공
   - 수동 정지 시까지 지속 작동

   ### 구현 코드
   ```c
   // 사이렌 시작 함수
   void* StartSiren(void* args);

   // 사이렌 종료 함수
   void EndSiren(int gpio1, int gpio2);
   ```

2. **카메라 녹화**
   - 지정된 영역인 문 앞을 1분간 녹화
   - 영상을 로컬에 저장하고 미리 설정된 이메일 주소로 전송

   ### 구현 코드
   ```c
   // 영상 촬영 함수
   int record_video(const char* output_file, int duration_ms, int framerate, int width, int height);

   // 이메일 전송 함수
   int send_email_with_attachment(const char* email_address, const char* subject, const char* body, const char* attachment_path);

   // 버튼 누름 처리 함수
   void* handle_button_press(void* arg);
   ```

3. **SMS 알림**
   - Twilio API를 사용하여 지정된 전화번호로 긴급상황을 SMS 알림을 통해 전송

   ### 구현 코드
   ```c
   // Twilio SMS 전송 함수
   int twilio_send_message(char *account_sid, char *auth_token, char *message, char *from_number, char *to_number, char *picture_url, bool verbose);

   // SMS 알림 처리 함수
   void* consumerFunc(void* arg);
   ```

4. **서보 모터 제어**
   - 서보 모터를 작동하여 도어 잠금/해제를 수행하고, 기타 기계적 동작을 실행

---

## 개발 환경

### 하드웨어
- Raspberry Pi
- LED
- 부저
- Raspberry Pi 호환 카메라 모듈
- 서보 모터

### 소프트웨어
- **libcamera** (카메라 녹화를 위한 필수 소프트웨어)
- GCC (C 언어 컴파일러)
- Mutt (이메일 전송용 클라이언트)
- libcurl (Twilio API 사용을 위한 라이브러리)

---

## 설치 및 설정

1. **하드웨어 구성 연결**
   - LED, 부저, 카메라 모듈, 서보 모터를 Raspberry Pi의 적절한 GPIO 핀에 연결

2. **필수 패키지 설치**
   ```bash
   sudo apt update
   sudo apt install libcamera mutt gcc make libcurl4-openssl-dev
   ```

3. **이메일 설정**
   - Mutt를 설정하여 이메일 계정을 구성

4. **Twilio 설정**
   - Twilio 계정을 등록하고 계정 SID, 인증 토큰, 전화번호를 등록
   - 해당 정보를 스크립트에 추가

---

## 사용 방법

1. Raspberry Pi의 전원을 켜고 프로그램을 컴파일 후 실행
   ```bash
   sudo gcc -g -o safe_zone main.c Siren.c camera.c servomotor_off.c servomotor_on.c thread.c twilio.c -lpthread -lcurl -lm
   sudo ./safe_zone
   ```
2. 버튼을 눌러 잠금, 녹화 및 메시지 전송의 Safe Zone 기능을 수행
   - Safe Zone 내부의 버튼을 누름
   - 서보 모터가 작동하여 도어 잠금/해제를 수행
   - LED와 부저가 켜지며 사이렌 기능 활성화
   - 카메라가 지정 위치를 1분간 녹화 시작, 녹화 후 지정된 메일로 영상을 전송
   - SMS로 긴급상황 알림이 전송

---

## 코드 개요

### 메인 코드
GPIO 핀을 초기화하고, C 언어의 스레드를 설정하여 다음 작업을 동시에 실행

- LED와 부저 작동
- 카메라 녹화 및 이메일 전송
- SMS 알림
- 서보 모터 제어

```c
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
                pthread_join(cameraPthread, NULL);
            }
            delay(200);
        }
        prevState = currState;
    }
    
    return 0;
}
```

---

## 역할 분담

- **서보모터 및 도어 잠금 시스템 담당**: 20200030 강지혁
- **SMS 문자 알림 담당**: 20201125 조형욱
- **LED, 부저 및 내부 전선 연결 담당**: 20200513 박찬수 
- **외형 설계 및 제작 담당**: 20220352 김학수

---

## 실행 영상

https://github.com/user-attachments/assets/4e55b3ef-228d-4493-ac2c-16851b3efe77

![image](https://github.com/user-attachments/assets/60838961-1890-4cc4-a7a6-c306df144028)

---


