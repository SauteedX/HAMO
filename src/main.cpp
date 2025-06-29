#include <Arduino.h>
#include "../lib/Audio/AudioDue.h"
#include "../lib/Config/PinConfig.h"

AudioPlayer audio;

void setup() {
    Serial.begin(115200);

    // 오디오 플레이어 초기화
    if (!audio.begin()) {
        Serial.println("오디오 초기화 실패!");
        while (1);
    }

    Serial.println("=== HAMO 오디오 플레이어 ===");
    Serial.println("명령어 안내:");
    Serial.println("l: 파일 목록 보기");
    Serial.println("1-9: 파일 선택 및 재생");
    Serial.println("s: 재생 중지");
    Serial.println("=========================");
    
    // 초기 파일 목록 표시
    audio.listFiles();
}

void loop() {
    if (Serial.available()) {
        char input = Serial.read();
        
        if (input == 'l' || input == 'L') {
            audio.listFiles();
        }
        else if (input == 's' || input == 'S') {
            audio.stopAudio();
            Serial.println("재생 중지됨");
        }
        else if (input >= '1' && input <= '9') {
            audio.selectAndPlayFile();
        }
    }
}