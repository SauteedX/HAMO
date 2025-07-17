#ifndef BLUETOOTHAUDIO_H
#define BLUETOOTHAUDIO_H

#include <Arduino.h>
#include "../Config/PinConfig.h"




class BluetoothAudio {
public:
    BluetoothAudio();
    bool begin();                       // 블루투스 모듈 초기화
    void process();                     // 수신 루프 (실시간 수신에만 필요)
    bool hasCommand();                  // 명령 도착 여부
    char readCommand();                 // 1글자 명령 읽기(큐 방식)
    void sendATCommand(const char* command);
    bool waitForResponse(const char* expected, unsigned long timeout = 1000);
    bool isConnected();
private:
    volatile bool connected;
    volatile bool hasNewCmd;
    volatile char latestCmd;            // 최근 들어온 명령
};

#endif
