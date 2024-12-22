# Safe Zone Project

## 개요
**Safe Zone Project**는 Raspberry Pi를 활용하여 하드웨어와 소프트웨어 기능을 결합해 개인 안전을 강화하기 위해 설계되었습니다. 버튼 한 번으로 여러 가지 안전 메커니즘이 동시에 활성화됩니다:

- LED와 부저를 사용한 지속적인 경고음 (사이렌 기능)
- 문 앞을 1분간 촬영한 영상을 이메일로 전송
- Twilio를 사용한 SMS 문자 알림
- 도어 잠금/해제를 위한 서보 모터 작동

모든 기능은 C언어의 스레드를 사용하여 동시에 실행됩니다.

---

## 주요 기능

1. **사이렌 기능**
   - LED와 부저를 활성화하여 시청각 경고를 제공합니다.
   - 수동으로 정지시킬 때까지 지속 작동합니다.

   ### 구현 코드
   ```c
   // 사이렌 시작 함수
   void* StartSiren(void* args);

   // 사이렌 종료 함수
   void EndSiren(int gpio1, int gpio2);
   ```

2. **카메라 녹화**
   - 지정된 영역(예: 문 앞)을 1분간 녹화합니다.
   - 영상을 로컬에 저장하고 미리 설정된 이메일 주소로 전송합니다.

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
   - Twilio API를 사용하여 지정된 전화번호로 SMS 알림을 전송합니다.

   ### 구현 코드
   ```c
   // Twilio SMS 전송 함수
   int twilio_send_message(char *account_sid, char *auth_token, char *message, char *from_number, char *to_number, char *picture_url, bool verbose);

   // SMS 알림 처리 함수
   void* consumerFunc(void* arg);
   ```

4. **서보 모터 제어**
   - 서보 모터를 작동하여 도어 잠금/해제를 수행하거나 기타 기계적 동작을 실행합니다.

---

## 요구 사항

### 하드웨어
- Raspberry Pi (GPIO 지원 모델)
- LED
- 부저
- Raspberry Pi 호환 카메라 모듈
- 서보 모터

### 소프트웨어
- **libcamera** (카메라 녹화를 위한 필수 소프트웨어)
- GCC (C 언어 컴파일러)
- Mutt (이메일 전송용 유틸리티)
- libcurl (Twilio API 사용을 위한 필수 라이브러리)

---

## 설치 및 설정

1. **하드웨어 구성 연결**
   - LED, 부저, 카메라 모듈, 서보 모터를 Raspberry Pi의 적절한 GPIO 핀에 연결합니다.

2. **필수 패키지 설치**
   ```bash
   sudo apt update
   sudo apt install libcamera mutt gcc make libcurl4-openssl-dev
   ```

3. **이메일 설정**
   - Mutt를 설정하여 이메일 계정을 구성합니다. 

4. **Twilio 설정**
   - Twilio 계정을 등록하고 계정 SID, 인증 토큰, 전화번호를 확보합니다.
   - 해당 정보를 스크립트에 추가합니다.

---

## 사용 방법

1. Raspberry Pi의 전원을 켜고 프로그램을 컴파일 후 실행합니다:
   ```bash
   gcc -o safe_zone main.c -lpthread -lcurl
   ./safe_zone
   ```
2. 지정된 버튼을 눌러 Safe Zone 기능을 활성화합니다:
   - LED와 부저가 켜집니다.
   - 카메라가 1분간 녹화를 시작합니다.
   - SMS 알림이 전송됩니다.
   - 서보 모터가 작동합니다.

---

## 코드 개요

### 메인 코드
GPIO 핀을 초기화하고, C 언어의 스레드를 설정하여 다음 작업을 동시에 실행합니다:

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

bool BtnState;
int prevState = 0;
int currState = 0;

pthread_mutex_t mid;
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
```

---

## 제작자들

- **서보모터 및 도어 잠금 시스템 담당**: 20200030 강지혁
- **SMS 문자 알림 담당**: 20201125 조형욱
- **LED, 부저 및 내부 전선 연결 담당**: 20200513 박찬수 
- **외형 설계 및 제작 담당**: 20220352 김학수

---

## 실행 영상

https://github.com/user-attachments/assets/4e55b3ef-228d-4493-ac2c-16851b3efe77

---


