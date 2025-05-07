#include <SPI.h>
#include <SD.h>
#include <../lib/I2S.h>
#include <../lib/I2SDoubleBuffer.h>
#include <../lib/BluetoothA2DPSink.h>

// 핀 설정
const int I2S_BCLK = 27;      // Bit Clock
const int I2S_LRC = 26;       // Left/Right Clock (WS)
const int I2S_DIN = 25;       // Data In
const int MAX98357A_SD = 14;  // Shutdown pin
const int MAX98357A_MODE = 13; // Mono/Stereo 선택 핀

// 모드 전환 관련 핀
const int MODE_SWITCH_PIN = 2;  // 모드 전환 버튼
const int BT_LED_PIN = 11;     // 블루투스 모드 LED
const int SD_LED_PIN = 12;     // SD카드 모드 LED
const int SD_CS_PIN = 10;      // SD 카드 CS 핀

// 오디오 설정
const int SAMPLE_RATE = 44100;
const int BITS_PER_SAMPLE = 16;
const int BUFFER_SIZE = 512;

// 모드 정의
enum Mode {
  BT_MODE,
  SD_MODE
};

// 전역 변수
Mode currentMode = BT_MODE;
BluetoothA2DPSink bt;
File audioFile;
volatile bool modeChanged = false;
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;

void setup() {
  Serial.begin(115200);

  // 핀 설정
  setupPins();

  // MAX98357A 초기화
  initializeMAX98357A();

  // SD 카드 초기화
  initializeSD();

  // 초기 모드 설정
  switchMode(currentMode);

  // 모드 전환 인터럽트 설정
  attachInterrupt(digitalPinToInterrupt(MODE_SWITCH_PIN), modeSwitchISR, FALLING);
}

void setupPins() {
  pinMode(MODE_SWITCH_PIN, INPUT_PULLUP);
  pinMode(BT_LED_PIN, OUTPUT);
  pinMode(SD_LED_PIN, OUTPUT);
  pinMode(MAX98357A_SD, OUTPUT);
  pinMode(MAX98357A_MODE, OUTPUT);

  // 초기 LED 상태
  digitalWrite(BT_LED_PIN, LOW);
  digitalWrite(SD_LED_PIN, LOW);
}

void initializeMAX98357A() {
  // 모노 모드 설정
  digitalWrite(MAX98357A_MODE, LOW);

  if (!I2S.begin(I2S_PHILIPS_MODE, SAMPLE_RATE, BITS_PER_SAMPLE)) {
    Serial.println("I2S 초기화 실패!");
    while (1);
  }

  I2S.setPins(I2S_BCLK, I2S_LRC, I2S_DIN);
  digitalWrite(MAX98357A_SD, HIGH);
}

void initializeSD() {
  if (!SD.begin(SD_CS_PIN)) {
    Serial.println("SD 카드 초기화 실패!");
    blinkError();
  } else {
    Serial.println("SD 카드 초기화 성공");
  }
}

void loop() {
  // 모드 전환 확인
  if (modeChanged) {
    handleModeSwitch();
    modeChanged = false;
  }

  // 현재 모드에 따른 처리
  switch(currentMode) {
    case BT_MODE:
      handleBluetoothMode();
      break;
    case SD_MODE:
      handleSDMode();
      break;
  }
}

// 인터럽트 서비스 루틴
void modeSwitchISR() {
  unsigned long currentTime = millis();
  if (currentTime - lastDebounceTime > debounceDelay) {
    modeChanged = true;
    lastDebounceTime = currentTime;
  }
}

void handleModeSwitch() {
  currentMode = (currentMode == BT_MODE) ? SD_MODE : BT_MODE;
  switchMode(currentMode);
}

void switchMode(Mode newMode) {
  // 이전 모드 정리
  digitalWrite(MAX98357A_SD, LOW);  // 앰프 음소거
  delay(10);  // 노이즈 방지 딜레이

  if (currentMode == SD_MODE && audioFile) {
    audioFile.close();
  } else if (currentMode == BT_MODE) {
    bt.stop();
  }

  // 새 모드 설정
  switch(newMode) {
    case BT_MODE:
      Serial.println("블루투스 모드로 전환");
      digitalWrite(BT_LED_PIN, HIGH);
      digitalWrite(SD_LED_PIN, LOW);
      initializeBluetooth();
      break;

    case SD_MODE:
      Serial.println("SD 카드 모드로 전환");
      digitalWrite(BT_LED_PIN, LOW);
      digitalWrite(SD_LED_PIN, HIGH);
      initializeSDMode();
      break;
  }

  digitalWrite(MAX98357A_SD, HIGH);  // 앰프 활성화
}

void initializeBluetooth() {
  bt.set_stream_reader(audio_data_callback);
  bt.start("HAMO Speaker");
}

void initializeSDMode() {
  audioFile = SD.open("audio.wav");
  if (!audioFile) {
    Serial.println("오디오 파일 열기 실패!");
    blinkError();
  }
}

void handleBluetoothMode() {
  bt.loop();

  // 블루투스 상태 표시
  static unsigned long lastBlink = 0;
  if (!bt.is_connected()) {
    if (millis() - lastBlink > 500) {
      digitalWrite(BT_LED_PIN, !digitalRead(BT_LED_PIN));
      lastBlink = millis();
    }
  }
}

void handleSDMode() {
  static uint8_t audioBuffer[BUFFER_SIZE];

  if (audioFile && audioFile.available()) {
    int bytesRead = audioFile.read(audioBuffer, BUFFER_SIZE);
    if (bytesRead > 0) {
      static bool headerSkipped = false;
      if (!headerSkipped && bytesRead >= 44) {
        headerSkipped = true;
        bytesRead -= 44;
        writeMonoAudioData(audioBuffer + 44, bytesRead);
      } else if (headerSkipped) {
        writeMonoAudioData(audioBuffer, bytesRead);
      }
    } else {
      // 파일 끝에서 다시 시작
      audioFile.seek(44);
      headerSkipped = true;
    }
  }
}

// 스테레오를 모노로 변환
int16_t stereoToMono(int16_t left, int16_t right) {
  return (left + right) / 2;
}

// 블루투스 오디오 데이터 처리
void audio_data_callback(const uint8_t *data, uint32_t len) {
  size_t bytesWritten = 0;
  while (bytesWritten < len) {
    if (I2S.available()) {
      int16_t left = (data[bytesWritten] | (data[bytesWritten + 1] << 8));
      int16_t right = (data[bytesWritten + 2] | (data[bytesWritten + 3] << 8));
      int16_t mono = stereoToMono(left, right);
      I2S.write((uint8_t*)&mono, 2);
      bytesWritten += 4;
    }
  }
}

void writeMonoAudioData(uint8_t* buffer, int length) {
  for (int i = 0; i < length; i += 4) {
    if (i + 3 < length) {
      int16_t left = (buffer[i] | (buffer[i + 1] << 8));
      int16_t right = (buffer[i + 2] | (buffer[i + 3] << 8));
      int16_t mono = stereoToMono(left, right);
      while (!I2S.available());
      I2S.write((uint8_t*)&mono, 2);
    }
  }
}

// 에러 표시 함수
void blinkError() {
  for (int i = 0; i < 5; i++) {
    digitalWrite(BT_LED_PIN, HIGH);
    digitalWrite(SD_LED_PIN, HIGH);
    delay(100);
    digitalWrite(BT_LED_PIN, LOW);
    digitalWrite(SD_LED_PIN, LOW);
    delay(100);
  }
}//
// Created by XEN on 25. 5. 7.
//
