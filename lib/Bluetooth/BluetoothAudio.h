// BluetoothAudio.h
#ifndef BLUETOOTH_AUDIO_H
#define BLUETOOTH_AUDIO_H

#include <Arduino.h>
#include "../Config/PinConfig.h"

class BluetoothAudio {
public:
    BluetoothAudio();
    bool begin();
    void process();
    bool isConnected();
    void stop();
    void setVolume(uint8_t volume);

private:
    bool connected;
    uint8_t audioBuffer[AUDIO_BUFFER_SIZE];
    void sendATCommand(const char* command);
    bool waitForResponse(const char* expected, unsigned long timeout = 1000);
};

#endif // BLUETOOTH_AUDIO_H