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
        audio.playAudioFile("audio.wav");
    }
}

//FINALLY FUCKING SUCCESS AFTER 33 FUCKING TRIALS HAHA