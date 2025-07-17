#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// SPI 핀 정의 - Due 기본 SPI
#define SPI_MISO_PIN 50  // Due의 기본 MISO
#define SPI_MOSI_PIN 51  // Due의 기본 MOSI
#define SPI_SCK_PIN  52  // Due의 기본 SCK

// MAX98357A I2S 핀 정의 (기존 SPI 대신 I2S로 변경)
#define I2S_BCLK    SPI_SCK_PIN   // I2S 비트 클록
#define I2S_DATA    SPI_MOSI_PIN  // I2S 데이터
#define I2S_LRCLK   53            // I2S 워드 선택 (새로 추가)
#define MAX98357A_SD    14        // Shutdown 핀
#define MAX98357A_MODE  13        // Mono/Stereo 선택

// SD 카드 핀
#define SD_CS_PIN 10

// 보드별 오디오 출력 핀 정의
#if defined(ARDUINO_SAM_DUE)
    // Arduino Due는 내장 DAC 사용 (DAC0는 코드에서 직접 사용)
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(__AVR__)
    // AVR 보드는 PWM 핀 사용
    #define PWM_DAC_PIN 9  // PWM 가능한 핀 (chiwawa 보드에 맞게 조정)
#endif

// 블루투스 핀 정의 추가
#define BT_RX_PIN      16
#define BT_TX_PIN      17
#define BT_STATE_PIN   18
#define BT_EN_PIN      19

// UI 핀 정의
#define MODE_SWITCH_PIN 2
#define BT_LED_PIN     11
#define SD_LED_PIN     12

// 오디오 관련 상수
#define SAMPLE_RATE      44100
#define BITS_PER_SAMPLE 16
#define AUDIO_BUFFER_SIZE 512
#define MAX_FILENAME_LENGTH 64

// 오디오 설정
#define MAX_VOLUME     15
#define DEFAULT_VOLUME 10

#endif // PIN_CONFIG_H