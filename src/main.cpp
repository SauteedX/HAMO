#include <SPI.h>
#include <SD.h>
#include <ArduinoSound.h>
#include <I2S.h>

const int chipSelect = SDCARD_SS_PIN;  // 내장 SD카드 슬롯 사용
SDWaveFile waveFile;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);  // 동작 표시

    Serial.begin(9600);
    while(!Serial);  // 시리얼 연결 대기

    Serial.println("SD카드 WAV I2S 오디오 재생 예제");

    // SD카드 초기화
    if (!SD.begin(chipSelect)) {
        Serial.println("SD카드를 인식하지 못했습니다.");
        while(1);  // 멈춤
    }

    // 재생할 WAV 파일 열기 (예시: "test.wav")
    const char* file_name = "WAVE0001.wav";
    File finfo = SD.open(file_name);

    if (!finfo) {
        Serial.println("파일을 열 수 없습니다.");
        while(1);
    }
    finfo.close();

    // WaveFile 객체 생성
    waveFile = SDWaveFile(file_name);

    // WAV 파일 포맷 체크
    if (!waveFile) {
        Serial.println("유효하지 않은 WAV 파일입니다.");
        while(1);
    }

    Serial.print("샘플레이트: ");
    Serial.println(waveFile.sampleRate());
    Serial.print("비트수: ");
    Serial.println(waveFile.bitsPerSample());
    Serial.print("채널 수: ");
    Serial.println(waveFile.channels());
    Serial.print("길이(초): ");
    Serial.println(waveFile.duration());

    AudioOutI2S.volume(80.0f);  // 볼륨 80% (0~100)
    Serial.println("재생 준비 완료!");
}

void loop() {
    Serial.println("재생 시작...");
    AudioOutI2S.play(waveFile);  // WAV 파일을 I2S로 출력

    while(AudioOutI2S.isPlaying());  // 재생 중이면 대기

    Serial.println("재생 종료!");
    delay(2000);  // 2초 후 다시 재생
}
