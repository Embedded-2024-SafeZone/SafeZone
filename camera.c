#include <stdio.h>
#include <stdlib.h>
#include "global.h"

// 영상 촬영 함수
int record_video(const char* output_file, int duration_ms, int framerate, int width, int height) {
    char command[256];
    snprintf(command, sizeof(command),
        "libcamera-vid -o %s -t %d --framerate %d --width %d --height %d",
        output_file, duration_ms, framerate, width, height);

    printf("Executing: %s\n", command);
    int result = system(command);

    if (result == 0) {
        printf("Recording complete! Video saved as %s\n", output_file);
    }
    else {
        fprintf(stderr, "Failed to record video. Please check your setup.\n");
    }

    return result;
}

// 이메일 전송 함수
int send_email_with_attachment(const char* email_address, const char* subject, const char* body, const char* attachment_path) {
    char command[256];
    snprintf(command, sizeof(command),
        "echo '%s' | sudo mutt -s '%s' -a %s -- %s",
        body, subject, attachment_path, email_address);

    printf("Executing: %s\n", command);
    int result = system(command);

    if (result == 0) {
        printf("Email sent successfully to %s\n", email_address);
    }
    else {
        fprintf(stderr, "Failed to send email. Please check your setup.\n");
    }

    return result;
}

void* handle_button_press(void* arg) {
    const char* video_file = "infrared_video.mp4";
    int duration_ms = 60000;
    int framerate = 30;
    int width = 1920;
    int height = 1080;
    const char* email_address = "rkdwlgur0202@naver.com";
    const char* email_subject = "Subject";
    const char* email_body = "Email body";
    while(BtnState){
        // 영상 촬영
        if (record_video(video_file, duration_ms, framerate, width, height) == 0) {
            // 촬영 성공 시 이메일 전송
            send_email_with_attachment(email_address, email_subject, email_body, video_file);
        }
    }
    return NULL;
}   