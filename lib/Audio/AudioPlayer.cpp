#include "AudioDue.h"

AudioPlayer::AudioPlayer() :
    spiSettings(2000000, MSBFIRST, SPI_MODE0),
    currentVolume(DEFAULT_VOLUME),
    isPlaying(false) {
}

bool AudioPlayer::begin() {
    setupSPI();
    setupDAC();
    
    pinMode(MAX98357A_SS, OUTPUT);
    pinMode(MAX98357A_MODE, OUTPUT);
    digitalWrite(MAX98357A_MODE, LOW);
    digitalWrite(MAX98357A_SS, HIGH);
    
    return true;
}

void AudioPlayer::setupSPI() {
    // Due의 기본 SPI 핀 사용
    SPI.begin();
    SPI.beginTransaction(spiSettings);

    // MAX98357A 핀 설정
    pinMode(MAX98357A_SCK, OUTPUT);
    pinMode(MAX98357A_MOSI, OUTPUT);
    
    // SD 카드 초기화
    if (!SD.begin(SD_CS_PIN)) {
        Serial.println("SD 카드 초기화 실패!");
        return;
    }
}

void AudioPlayer::setupDAC() {
    analogWriteResolution(12);
    pinMode(DAC0, OUTPUT);
}

void AudioPlayer::playAudioFile(const char* filename) {
    File audioFile = SD.open(filename);
    if (!audioFile) {
        Serial.println("파일 열기 실패!");
        return;
    }

    audioFile.seek(44);
    isPlaying = true;
    uint8_t buffer[AUDIO_BUFFER_SIZE];

    while (isPlaying && audioFile.available()) {
        int bytesRead = audioFile.read(buffer, AUDIO_BUFFER_SIZE);
        for (int i = 0; i < bytesRead; i += 2) {
            int16_t sample = (buffer[i+1] << 8) | buffer[i];
            processSample(sample);
        }
    }

    audioFile.close();
}

// AudioPlayer.cpp 수정
void AudioPlayer::stopAudio() {
    isPlaying = false;
    analogWrite(DAC0, 2048); // DAC 중간값으로 설정하여 노이즈 방지
}

void AudioPlayer::processSample(int16_t sample) {
    // 볼륨 적용
    sample = (sample * currentVolume) / MAX_VOLUME;
    
    // 12비트 DAC 범위로 변환 (0-4095)
    int dacValue = map(sample, -32768, 32767, 0, 4095);
    
    // DAC 출력
    analogWrite(DAC0, dacValue);
    
    // 샘플링 레이트 유지를 위한 딜레이
    // 44.1kHz = ~22.67us per sample
    delayMicroseconds(23);
}

void AudioPlayer::setVolume(uint8_t volume) {
    if (volume > MAX_VOLUME) {
        volume = MAX_VOLUME;
    }
    currentVolume = volume;
}

// 기존 코드에 아래 함수들을 추가합니다

void AudioPlayer::listFiles() {
    Serial.println("\n=== SD 카드 파일 목록 ===");
    File root = SD.open("/");
    printDirectory(root, 0);
    root.close();
    Serial.println("========================");
}

void AudioPlayer::printDirectory(File dir, int numTabs) {
    int fileIndex = 1;
    
    while (true) {
        File entry = dir.openNextFile();
        if (!entry) {
            break;
        }
        
        for (uint8_t i = 0; i < numTabs; i++) {
            Serial.print('\t');
        }
        
        if (!entry.isDirectory()) {
            // WAV 파일만 표시
            String fileName = String(entry.name());
            if (fileName.endsWith(".wav") || fileName.endsWith(".WAV")) {
                Serial.print(fileIndex);
                Serial.print(". ");
                Serial.print(entry.name());
                Serial.print("\t\t");
                Serial.print(entry.size());
                Serial.println(" bytes");
                fileIndex++;
            }
        }
        entry.close();
    }
}

bool AudioPlayer::selectAndPlayFile() {
    // 현재 재생 중인 오디오 중지
    stopAudio();
    
    // 파일 목록 표시
    listFiles();
    
    Serial.println("\n재생할 파일 번호를 입력하세요:");
    
    // 사용자 입력 대기
    while (!Serial.available()) {
        delay(100);
    }
    
    String input = Serial.readStringUntil('\n');
    int selection = input.toInt();
    
    // 선택한 번호의 파일 찾기
    File root = SD.open("/");
    int fileIndex = 1;
    File entry;
    String selectedFileName;
    
    while (true) {
        entry = root.openNextFile();
        if (!entry) {
            break;
        }
        
        String fileName = String(entry.name());
        if (fileName.endsWith(".wav") || fileName.endsWith(".WAV")) {
            if (fileIndex == selection) {
                selectedFileName = fileName;
                entry.close();
                break;
            }
            fileIndex++;
        }
        entry.close();
    }
    root.close();
    
    if (selectedFileName.length() > 0) {
        Serial.print("선택한 파일 재생 중: ");
        Serial.println(selectedFileName);
        playAudioFile(selectedFileName.c_str());
        return true;
    } else {
        Serial.println("잘못된 선택입니다.");
        return false;
    }
}