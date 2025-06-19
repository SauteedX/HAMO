#include <Arduino.h>
#include <Wire.h>
#include <../lib/LiquidCrystal_I2C.h>

const int PR_PIN = A0; //pressure          INPUT PHASE                          핀 지정자 헤더파일로 분리예정
const int TM_PIN = A1; //temperature
const int LM_PIN = A2; //lumen;LIGHT
const int RTCM_PIN = 3; //RTC
const int MIC_PIN = A3; //MIC

const int SCL_PIN = 21; //DUE SCL            LCD PHASE
const int SDA_PIN = 20; //DUE SDA

const int CARBON_PIN = 4; //carbon        OUTPUT PHASE
const int BMS_PIN = 5; //battery
const int SD_PIN = 6; //SD

LiquidCrystal_I2C lcd(0x27, 16, 2); //temporal 16x2 code

bool initSensors();

// 에러 처리를 위한 함수 정의
[[noreturn]] void errorHandle(const char* errorMessage) {
    Serial.println(errorMessage);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(errorMessage);
    int t=0;
    while(1) {
        lcd.setCursor(0, 1);
        lcd.print("Holding:");
        lcd.print(t++);
        lcd.print(" sec");
        delay(1000);
    }
}

bool checkSensors() {
    // 각 센서의 값을 읽어서 유효한 범위인지 확인
    // 센서 추가시 체크함수에 추가 필수!
    int prValue = analogRead(PR_PIN);
    int tmValue = analogRead(TM_PIN);
    int lmValue = analogRead(LM_PIN);
    int rtcmValue = digitalRead(RTCM_PIN);
    int micValue = analogRead(MIC_PIN);

    return (prValue >= 0 && prValue <= 1023 &&
            tmValue >= 0 && tmValue <= 1023 &&
            lmValue >= 0 && lmValue <= 1023 &&
            rtcmValue >= 0 && rtcmValue <= 1023 &&
            micValue >= 0 && micValue <= 1023);
}

void setup() {
    Serial.begin(9600);
    Serial.println("Init Start!");
    lcd.init();
    lcd.backlight();
    lcd.setCursor(0, 0);
    lcd.print("HAMO INIT...");

    // 센서들의 실제 값을 확인
    if (!checkSensors()) {//initiation; self diagnosis
        errorHandle("SensorInitErr");
    }
    lcd.setCursor(0, 1);
    lcd.print("OK!");
    delay(2000);
    lcd.clear();

    lcd.setCursor(0, 0);
    lcd.print("Face Normal.");
    lcd.setCursor(0, 1);
    lcd.print("Interaction: Hello");
    delay(2000);
}

void loop() {
    int pr = analogRead(PR_PIN);//Sensor Read
    int tm = analogRead(TM_PIN);
    int lm = analogRead(LM_PIN);
    int rtcm = digitalRead(RTCM_PIN);
    int mic = digitalRead(MIC_PIN);
    Serial.println(mic);


    //Sensor Value Process

    //H.W. Control

    //BT Audio

    //APP output
}
//Compile Success.

//XEN 2505072320