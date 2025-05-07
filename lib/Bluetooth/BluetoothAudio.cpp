#include "BluetoothAudio.h"

BluetoothAudio::BluetoothAudio() : connected(false) {
}

bool BluetoothAudio::begin() {
    pinMode(BT_STATE_PIN, INPUT);
    pinMode(BT_EN_PIN, OUTPUT);

    // HC-05 모듈 리셋
    digitalWrite(BT_EN_PIN, LOW);
    delay(100);
    digitalWrite(BT_EN_PIN, HIGH);
    delay(100);

    // Serial2를 블루투스 통신용으로 사용
    Serial2.begin(38400);  // HC-05 기본 통신 속도

    // AT 명령어로 설정
    sendATCommand("AT+NAME=HAMO Speaker");
    if (!waitForResponse("OK")) return false;

    sendATCommand("AT+ROLE=0");  // Slave 모드
    if (!waitForResponse("OK")) return false;

    sendATCommand("AT+RESET");
    delay(1000);

    return true;
}

void BluetoothAudio::process() {
    connected = digitalRead(BT_STATE_PIN) == HIGH;

    if (connected && Serial2.available() >= AUDIO_BUFFER_SIZE) {
        int bytesRead = Serial2.readBytes(audioBuffer, AUDIO_BUFFER_SIZE);
        // 오디오 데이터 처리...
    }
}

void BluetoothAudio::sendATCommand(const char* command) {
    Serial2.println(command);
    delay(100);
}

bool BluetoothAudio::waitForResponse(const char* expected, unsigned long timeout) {
    unsigned long startTime = millis();
    String response = "";

    while (millis() - startTime < timeout) {
        if (Serial2.available()) {
            char c = Serial2.read();
            response += c;
            if (response.indexOf(expected) != -1) {
                return true;
            }
        }
    }
    return false;
}

bool BluetoothAudio::isConnected() {
    return connected;
}

void BluetoothAudio::stop() {
    // 필요한 정지 로직 구현
}

void BluetoothAudio::setVolume(uint8_t volume) {
    // 볼륨 제어 로직 구현
}