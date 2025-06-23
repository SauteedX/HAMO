// AudioDue.h
#ifndef AUDIO_DUE_H
#define AUDIO_DUE_H

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include "../Config/PinConfig.h"

class AudioPlayer {
public:
    AudioPlayer();
    bool begin();
    void setupSPI();
    void setupDAC();
    void playAudioFile(const char* filename);
    void stopAudio();
    void setVolume(uint8_t volume);
    void listFiles();           // 파일 목록 표시
    bool selectAndPlayFile();   // 파일 선택 및 재생

private:
    void writeSample(int16_t sample);
    void processSample(int16_t sample);
    void printDirectory(File dir, int numTabs = 0);  // 디렉토리 내용 출력

    SPISettings spiSettings;
    uint8_t currentVolume;
    bool isPlaying;
};

#endif // AUDIO_DUE_H