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

private:
    void writeSample(int16_t sample);
    void processSample(int16_t sample);

    SPISettings spiSettings;
    uint8_t currentVolume;
    bool isPlaying;
};

#endif // AUDIO_DUE_H