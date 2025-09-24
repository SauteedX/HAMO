#include <SPI.h>
#include <SD.h>
#include <ArduinoSound.h>
#include <I2S.h>

const int chipSelect = SDCARD_SS_PIN;
SDWaveFile waveFile;

String file_name;
bool isPlaying = false;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);

    Serial1.begin(115200);
    Serial1.println("Hello!");

    if (!SD.begin(chipSelect)) {
        Serial1.println("SD카드를 인식하지 못했습니다.");
        while(1);
    }

    AudioOutI2S.volume(30.0f);
}

void playCurrentFile() {
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

void loop() {
    // 새 파일명 명령 도착 시 바로 새 파일 반복재생
    if (Serial1.available()) {
        if (isPlaying && AudioOutI2S.isPlaying()) {
            AudioOutI2S.stop();
        }
        file_name = Serial1.readStringUntil('\n');
        file_name.trim();
        file_name += ".wav";

        if (!SD.exists(file_name.c_str())) {
            Serial1.println("파일을 찾을 수 없습니다: " + file_name);
            isPlaying = false;
            return;
        }
        playCurrentFile();
    }

    // 재생 끝나면 자동 반복
    if (isPlaying && !AudioOutI2S.isPlaying()) {
        playCurrentFile();
    }
}
