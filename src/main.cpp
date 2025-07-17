#include <Arduino.h>

#include "../lib/Bluetooth/BluetoothAudio.h"

// MAX98357A 핀 정의 (I2S)
#define I2S_BCLK   76   // BIT_CLOCK, 예시: Due 보드의 SCK1 (실핀번호 확인필수)
#define I2S_LRC    3    // WORD SELECT, 예시: D3
#define I2S_DIN    75   // DATA, 예시: MOSI1

// 블루투스 모듈 핀 정의 (BT-05)
#define BT_RX_PIN 17    // Arduino TX2
#define BT_TX_PIN 16    // Arduino RX2
#define BT_STATE_PIN 18 // 연결 상태핀(실제 결선에 맞게 조정)
#define BT_EN_PIN   19  // EN 핀(필요시, 실제 결선에 맞게 조정)

void setup() {
    Serial.begin(115200);
    while (!Serial);

    // 블루투스 EN/STATE 핀 선별 설정
    pinMode(BT_STATE_PIN, INPUT);
    pinMode(BT_EN_PIN, OUTPUT);

    // BT-05 모듈 리셋 시퀀스(Option)
    digitalWrite(BT_EN_PIN, LOW);
    delay(100);
    digitalWrite(BT_EN_PIN, HIGH);
    delay(100);

    // 블루투스 시리얼(UART2) 초기화
    Serial2.begin(38400);

    // MAX98357A (I2S) 관련, 실제 오디오 출력 원하면 I2S 라이브러리 필요
    // 현재 예제는 블루투스 명령만 수신 후 시리얼 응답

    Serial.println("=== BT-05 + MAX98357A 테스트 ===");
    Serial.println("블루투스에서 문자 입력하면 아두이노가 수신/처리합니다.");
    Serial.println("오디오 기능은 별도 I2S 오디오 구현 필요!");
}

void loop() {
    // 블루투스(BT-05)에서 들어온 데이터 읽기
    if (Serial2.available()) {
        String cmd = Serial2.readStringUntil('\n');
        Serial.print("[BT 입력]: ");
        Serial.println(cmd);

        // 명령 분기(간단 예시)
        pinMode(LED_BUILTIN, OUTPUT);
        if (cmd.startsWith("LEDON")) {
            Serial.println("LED를 켭니다!");
            digitalWrite(LED_BUILTIN, HIGH);
        } else if (cmd.startsWith("LEDOFF")) {
            Serial.println("LED를 끕니다!");
            digitalWrite(LED_BUILTIN, LOW);

        } else if (cmd == "PING") {
            Serial2.write("PONG"); // 블루투스 앱으로 응답
        }
        // (추가 명령 구현 가능)
    }

    // 필요에 따라 시리얼에서 명령을 받아 블루투스로 전송도 가능
    if (Serial.available()) {
        char c = Serial.read();
        Serial2.write(c); // 블루투스로 전달
    }
}
