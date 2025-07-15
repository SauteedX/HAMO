#include <Arduino.h>
#include "../lib/Audio/AudioDue.h"
#include "../lib/Config/PinConfig.h"
#include <SD.h>
#include <SPI.h>

AudioPlayer audio;

void setup() {
    // Arduino/Genuino 101은 시리얼 초기화 후 대기가 필요할 수 있습니다
    Serial.begin(115200);
    
    // 시리얼 연결 대기 (최대 5초)
    unsigned long startTime = millis();
    while (!Serial && (millis() - startTime < 5000)) {
        delay(100);
    }
    
    Serial.println("시리얼 연결 확인됨");
    Serial.flush();
    
    // 오디오 플레이어 초기화
    Serial.println("오디오 플레이어 초기화 중...");
    if (!audio.begin()) {
        Serial.println("오디오 초기화 실패!");
        while (1) {
            delay(1000);
            Serial.println("오디오 초기화 실패 - 재시도...");
        }
    }

    Serial.println("=== HAMO 오디오 플레이어 ===");
    Serial.println("명령어 안내:");
    Serial.println("l: 파일 목록 보기");
    Serial.println("1-9: 파일 선택 및 재생");
    Serial.println("s: 재생 중지");
    Serial.println("=========================");
    
    // 초기 파일 목록 표시
    Serial.println("초기 파일 목록 로딩...");
    audio.listFiles();
    Serial.println("초기화 완료!");
}

void loop() {
    // 주기적으로 상태 확인
    static unsigned long lastHeartbeat = 0;
    if (millis() - lastHeartbeat > 10000) { // 10초마다
        Serial.println("시스템 정상 작동 중...");
        lastHeartbeat = millis();
    }
    
    if (Serial.available()) {
        char input = Serial.read();
        Serial.print("입력 받음: ");
        Serial.println(input);
        
        if (input == 'l' || input == 'L') {
            Serial.println("파일 목록 요청됨");
            audio.listFiles();
        }
        else if (input == 's' || input == 'S') {
            Serial.println("재생 중지 요청됨");
            audio.stopAudio();
            Serial.println("재생 중지됨");
        }
        else if (input >= '1' && input <= '9') {
            Serial.println("파일 선택 요청됨");
            audio.selectAndPlayFile();
        }
    }
}