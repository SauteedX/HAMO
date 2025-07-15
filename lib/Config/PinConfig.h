#ifndef PIN_CONFIG_H
#define PIN_CONFIG_H

// MAX98357A SPI 핀 정의
#define MAX98357A_SCK  3   // Due의 SCK
#define MAX98357A_MOSI 4   // Due의 MOSI (DIN으로 사용)
#define MAX98357A_SS   14  // Shutdown 핀
#define MAX98357A_MODE 13  // Mono/Stereo 선택

// SD 카드 핀
#define SD_CS_PIN 10

// 보드별 오디오 출력 핀 정의
#if defined(ARDUINO_SAM_DUE)
    // Arduino Due는 내장 DAC 사용 (DAC0는 코드에서 직접 사용)
#elif defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO) || defined(__AVR__)
    // AVR 보드는 PWM 핀 사용
    #define PWM_DAC_PIN 9  // PWM 가능한 핀 (chiwawa 보드에 맞게 조정)
#endif

// 오디오 관련 상수
#define SAMPLE_RATE 44100
#define BITS_PER_SAMPLE 16
#define AUDIO_BUFFER_SIZE 512
#define MAX_FILENAME_LENGTH 64

// 오디오 설정
#define MAX_VOLUME 15
#define DEFAULT_VOLUME 10

#endif // PIN_CONFIG_H