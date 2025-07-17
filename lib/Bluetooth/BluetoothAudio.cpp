#include "BluetoothAudio.h"
#include "../Config/PinConfig.h"


BluetoothAudio::BluetoothAudio() : connected(false), hasNewCmd(false), latestCmd(0) {}

bool BluetoothAudio::begin() {
    pinMode(BT_STATE_PIN, INPUT);
    pinMode(BT_EN_PIN, OUTPUT);

    // BT-05 모듈 리셋
    digitalWrite(BT_EN_PIN, LOW);
    delay(100);
    digitalWrite(BT_EN_PIN, HIGH);
    delay(100);

    Serial2.begin(38400);  // BT-05/HC-05 기본 속도

    // 필수적이지는 않지만, 이름/모드 세팅 (커스텀 가능)
    sendATCommand("AT+NAME=HAMO-HC05");
    waitForResponse("OK");
    sendATCommand("AT+ROLE=0");        // Slave
    waitForResponse("OK");
    sendATCommand("AT+RESET");
    delay(500);

    return true;
}

void BluetoothAudio::process() {
    connected = digitalRead(BT_STATE_PIN) == HIGH;
    while (Serial2.available()) {
        latestCmd = Serial2.read();   // 가장 최근 명령 저장 (한 글자 처리)
        hasNewCmd = true;
    }
}

bool BluetoothAudio::hasCommand() {
    return hasNewCmd;
}

char BluetoothAudio::readCommand() {
    hasNewCmd = false;
    return latestCmd;
}

void BluetoothAudio::sendATCommand(const char* command) {
    Serial2.print(command);
    Serial2.print("\r\n");
    delay(200);
}

bool BluetoothAudio::waitForResponse(const char* expected, unsigned long timeout) {
    unsigned long startTime = millis();
    String response = "";
    while (millis() - startTime < timeout) {
        while (Serial2.available()) {
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
