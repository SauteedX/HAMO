#include <Arduino.h>

// 핀 정의
const int LED_PIN = 13;        // 내장 LED
const int BUTTON_PIN = 2;      // 버튼 입력
const int ANALOG_PIN = A0;     // 아날로그 입력 (예: 포텐셔미터)

// 변수 선언
int buttonState = 0;
int analogValue = 0;
unsigned long previousMillis = 0;
const long interval = 1000;    // LED 깜빡임 간격 (1초)
bool ledState = false;

void setup() {
  // 시리얼 통신 초기화
  Serial.begin(9600);
  
  // 핀 모드 설정
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  
  Serial.println("Arduino Uno 테스트 시작");
}

void loop() {
  // 1. LED 깜빡임 테스트
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;
    digitalWrite(LED_PIN, ledState);
  }
  
  // 2. 버튼 상태 읽기
  buttonState = digitalRead(BUTTON_PIN);
  if (buttonState == LOW) {  // 버튼이 눌렸을 때 (풀업 저항으로 인해 LOW가 눌린 상태)
    Serial.println("버튼이 눌렸습니다!");
  }
  
  // 3. 아날로그 값 읽기
  analogValue = analogRead(ANALOG_PIN);
  
  // 4. 시리얼 모니터에 정보 출력 (500ms 마다)
  static unsigned long lastPrint = 0;
  if (currentMillis - lastPrint >= 500) {
    lastPrint = currentMillis;
    
    Serial.println("\n--- 상태 정보 ---");
    Serial.print("LED 상태: ");
    Serial.println(ledState ? "켜짐" : "꺼짐");
    Serial.print("버튼 상태: ");
    Serial.println(buttonState == LOW ? "눌림" : "안눌림");
    Serial.print("아날로그 값: ");
    Serial.println(analogValue);
  }
}