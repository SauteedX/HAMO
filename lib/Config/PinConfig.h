// PinConfig.h
#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// MAX98357A SPI 핀 정의
#define MAX98357A_SCK  3   // Due의 SCK
#define MAX98357A_MOSI 4   // Due의 MOSI (DIN으로 사용)
#define MAX98357A_SS   14  // Shutdown 핀
#define MAX98357A_MODE 13  // Mono/Stereo 선택

// 블루투스 통신 핀 정의
#define BT_RX_PIN 16
#define BT_TX_PIN 17
#define BT_STATE_PIN 18
#define BT_EN_PIN 19

// 모드 전환 관련 핀
#define MODE_SWITCH_PIN 2
#define BT_LED_PIN 11
#define SD_LED_PIN 12
#define SD_CS_PIN 10

// 오디오 관련 상수
#define SAMPLE_RATE 44100
#define BITS_PER_SAMPLE 16
#define AUDIO_BUFFER_SIZE 512
#define MAX_FILENAME_LENGTH 64

// 오디오 설정
#define BLUETOOTH_DEVICE_NAME "HAMO Speaker"
#define MAX_VOLUME 15
#define DEFAULT_VOLUME 10

// 타이밍 설정
#define DEBOUNCE_DELAY 200  // ms
#define LED_BLINK_INTERVAL 500  // ms

#endif // PIN_CONFIG_H