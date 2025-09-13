#include <SPI.h>
#include <SD.h>
#include <ArduinoSound.h>
#include <I2S.h>

const int chipSelect = SDCARD_SS_PIN;  // 내장 SD카드 슬롯 사용
SDWaveFile waveFile;

String file_name;
void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);  // 동작 표시

    Serial1.begin(115200);
    while (!Serial1) {}
    Serial1.println("Hello!");

    // SD카드 초기화
    if (!SD.begin(chipSelect)) {
        Serial1.println("SD카드를 인식하지 못했습니다.");
        while(1);  // 멈춤
    }

    // 재생할 WAV 파일 열기 (예시: "test.wav")


    AudioOutI2S.volume(80.0f);  // 볼륨 80% (0~100)
}

void loop() {
    if (Serial1.available()) {
        file_name = (Serial1.readStringUntil('\n') + ".wav");
    }
    if (!waveFile) {
        Serial1.println("유효하지 않은 WAV 파일입니다.");
        delay(10000);
        return;
    }
    Serial1.println("재생 시작...");

    // WaveFile 객체 생성
    waveFile = SDWaveFile(file_name);

    // WAV 파일 포맷 체크


    Serial1.print("샘플레이트: ");
    Serial1.println(waveFile.sampleRate());
    Serial1.print("비트수: ");
    Serial1.println(waveFile.bitsPerSample());
    Serial1.print("채널 수: ");
    Serial1.println(waveFile.channels());
    Serial1.print("길이(초): ");
    Serial1.println(waveFile.duration());

    AudioOutI2S.play(waveFile);  // WAV 파일을 I2S로 출력

    while(AudioOutI2S.isPlaying());  // 재생 중이면 대기

    Serial1.println("재생 종료!");
    delay(2000);  // 2초 후 다시 재생
}
