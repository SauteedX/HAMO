#include <Arduino.h>
#include "../lib/Audio/AudioDue.h"
#include "../lib/Bluetooth/BluetoothAudio.h"
#include "../lib/Config/PinConfig.h"

AudioPlayer audio;
BluetoothAudio bluetooth;
bool isBTMode = true;

void setup() {
    Serial.begin(115200);
    Serial2.begin(38400);


    // 오디오 플레이어 초기화
    if (!audio.begin()) {
        Serial.println("오디오 초기화 실패!");
        while (1);
    }

    // LED 핀 설정
    pinMode(BT_LED_PIN, OUTPUT);
    pinMode(SD_LED_PIN, OUTPUT);
    pinMode(MODE_SWITCH_PIN, INPUT_PULLUP);

    // 초기 모드 표시
    digitalWrite(BT_LED_PIN, HIGH);
    digitalWrite(SD_LED_PIN, LOW);
}

void loop() {
    if (Serial2.available()) {
        Serial.print("BT 응답: ");
        while (Serial2.available()) {
            Serial.write(Serial2.read());
        }
        Serial.println();
    }

    if (digitalRead(MODE_SWITCH_PIN) == LOW) {
        delay(DEBOUNCE_DELAY);
        if (digitalRead(MODE_SWITCH_PIN) == LOW) {
            isBTMode = !isBTMode;
            if (isBTMode) {
                audio.stopAudio();
            } else {
                bluetooth.stop();
                // SD 카드 모드로 전환시 파일 선택 메뉴 표시
                audio.selectAndPlayFile();
            }

            digitalWrite(BT_LED_PIN, isBTMode);
            digitalWrite(SD_LED_PIN, !isBTMode);

            while (digitalRead(MODE_SWITCH_PIN) == LOW);
            delay(DEBOUNCE_DELAY);
        }
    }

    if (isBTMode) {
        bluetooth.process();
    } else {
        // Serial에서 'n' 또는 'N'을 입력받으면 다음 곡 선택
        if (Serial.available()) {
            char input = Serial.read();
            if (input == 'n' || input == 'N') {
                audio.selectAndPlayFile();
            }
        }
    }
}

//FINALLY FUCKING SUCCESS AFTER 33 FUCKING TRIALS HAHA