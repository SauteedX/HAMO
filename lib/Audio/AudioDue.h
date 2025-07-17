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
    bool setupSPI();
    void setupDAC();
    void setupAudioPins();      // MAX98357A 핀 설정
    bool reinitializeSD();      // SD 카드 재초기화
    void playAudioFile(const char* filename);
    void stopAudio();
    void setVolume(uint8_t volume);
    void listFiles();
    bool selectAndPlayFile();
    bool selectAndPlayFile(int fileNum); // 추가!

private:
    void writeSample(int16_t sample);
    void processSample(int16_t sample);
    void printDirectory(File dir, int numTabs = 0);

    SPISettings spiSettings;
    uint8_t currentVolume;
    bool isPlaying;
    int workingCSPin;
};

#endif // AUDIO_DUE_H