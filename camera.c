#include <stdio.h>
#include <stdlib.h>

int main() {
    // 영상 촬영 시간 (밀리초 단위)
    int duration = 6000;

    // 출력 파일 이름
    const char *output_file = "infrared_video.mp4";

    // libcamera-vid 명령 생성
    char command[256];
    snprintf(command, sizeof(command),
             "libcamera-vid -o %s -t %d --framerate 30 --width 1920 --height 1080",
             output_file, duration);

    // 명령 실행
    printf("Executing: %s\n", command);
    int result = system(command);

    if (result == 0) {
        printf("Recording complete! Video saved as %s\n", output_file);
    } else {
        fprintf(stderr, "Failed to record video. Please check your setup.\n");
    }

    const char *cmd = "echo 'Email body' | sudo mutt -s 'Subject' -a /home/pi/infrared_video.mp4 -- rkdwlgur0202@naver.com";
    system(cmd);

    return result;
}