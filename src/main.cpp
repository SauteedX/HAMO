#include <SPI.h>
#include <SD.h>
#include <ArduinoSound.h>
#include <I2S.h>

const int chipSelect = SDCARD_SS_PIN;  // 내장 SD카드 슬롯 사용
SDWaveFile waveFile;

String file_name;
bool isPlaying = false;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial1.begin(115200);
    while (!Serial1) {}
    Serial1.println("Hello!");

    if (!SD.begin(chipSelect)) {
        Serial1.println("SD카드를 인식하지 못했습니다.");
        while(1);
    }

    AudioOutI2S.volume(80.0f);  // 볼륨 80% (0~100)
}

void loop() {
    // 새 파일명 명령 도착 시 바로 새 파일 재생
    if (Serial1.available()) {
        // 새 명령이 오면 기존 재생 중단
        if (isPlaying && AudioOutI2S.isPlaying()) {
            AudioOutI2S.stop();
        }

        file_name = Serial1.readStringUntil('\n');
        file_name.trim(); // 공백 제거
        file_name += ".wav";

        // 파일이 존재하는지 체크
        if (!SD.exists(file_name.c_str())) {
            Serial1.println("파일을 찾을 수 없습니다: " + file_name);
            isPlaying = false;
            return;
        }

        waveFile = SDWaveFile(file_name.c_str());
        if (!waveFile) {
            Serial1.println("유효하지 않은 WAV 파일: " + file_name);
            isPlaying = false;
            return;
        }

        Serial1.println("재생 시작: " + file_name);
        Serial1.print("샘플레이트: ");
        Serial1.println(waveFile.sampleRate());
        Serial1.print("비트수: ");
        Serial1.println(waveFile.bitsPerSample());
        Serial1.print("채널 수: ");
        Serial1.println(waveFile.channels());
        Serial1.print("길이(초): ");
        Serial1.println(waveFile.duration());

        AudioOutI2S.play(waveFile);
        isPlaying = true;
    }

    // 재생 완료 감지 후 메시지 출력 (재생 끝날 때마다 1회)
    if (isPlaying && !AudioOutI2S.isPlaying()) {
        Serial1.println("재생 종료!");
        isPlaying = false;
    }
}
