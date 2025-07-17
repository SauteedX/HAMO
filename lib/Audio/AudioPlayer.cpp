
#include "AudioDue.h"

AudioPlayer::AudioPlayer() :
    spiSettings(2000000, MSBFIRST, SPI_MODE0),
    currentVolume(DEFAULT_VOLUME),
    isPlaying(false),
    workingCSPin(-1) {
}

bool AudioPlayer::begin() {
    Serial.println("AudioPlayer 초기화 시작...");

    // 보드 타입 확인
#if defined(ARDUINO_SAM_DUE)
    Serial.println("Arduino Due 감지됨");
#elif defined(ARDUINO_ARC32_TOOLS)
    Serial.println("Arduino/Genuino 101 감지됨");
#else
    Serial.println("알 수 없는 보드 - 일반 모드");
#endif

    // SD 카드 초기화를 먼저 시도
    if (!setupSPI()) {
        Serial.println("SPI/SD 초기화 실패!");
        return false;
    }

    // 중요: SD 카드 초기화 후 DAC 설정
    setupDAC();

    // MAX98357A 핀 설정 (SD 카드 초기화 후에 실행)
    setupAudioPins();

    Serial.println("AudioPlayer 초기화 완료!");
    return true;
}

bool AudioPlayer::setupSPI() {
    Serial.println("=== SD 카드 진단 시작 ===");

    // SPI 초기화
    SPI.begin();

    // 보드별 SPI 설정
#if defined(ARDUINO_SAM_DUE)
    SPI.setClockDivider(SPI_CLOCK_DIV128);
    Serial.println("Arduino Due SPI 설정 완료");
#elif defined(ARDUINO_ARC32_TOOLS)
    SPI.setClockDivider(128);
    Serial.println("Arduino/Genuino 101 SPI 설정 완료");
#else
    SPI.setClockDivider(SPI_CLOCK_DIV128);
    Serial.println("일반 보드 SPI 설정 완료");
#endif

    // 보드별 CS 핀 우선순위 설정
    int csPins[7];

#if defined(ARDUINO_SAM_DUE)
    int duePins[] = {10, 4, 7, 8, 9, 53, 18};
    for (int i = 0; i < 7; i++) {
        csPins[i] = duePins[i];
    }
#elif defined(ARDUINO_ARC32_TOOLS)
    int pins101[] = {10, 4, 7, 8, 9, 5, 6};
    for (int i = 0; i < 7; i++) {
        csPins[i] = pins101[i];
    }
#else
    int generalPins[] = {10, 4, 7, 8, 9, 5, 6};
    for (int i = 0; i < 7; i++) {
        csPins[i] = generalPins[i];
    }
#endif

    // 각 CS 핀별로 진단 수행
    for (int i = 0; i < 7; i++) {
        Serial.print("\n--- CS 핀 ");
        Serial.print(csPins[i]);
        Serial.println(" 진단 ---");

        // 핀 초기화
        pinMode(csPins[i], OUTPUT);
        digitalWrite(csPins[i], HIGH);
        delay(1000);

        Serial.print("핀 ");
        Serial.print(csPins[i]);
        Serial.print(" 초기화 완료, SD.begin() 시도... ");

        if (SD.begin(csPins[i])) {
            Serial.println("성공!");
            workingCSPin = csPins[i];

            // SD 카드 상세 진단
            Serial.println("SD 카드 상세 진단:");

            File root = SD.open("/");
            if (root) {
                Serial.println("✓ 루트 디렉토리 열기 성공");

                if (root.isDirectory()) {
                    Serial.println("✓ 루트가 디렉토리임을 확인");

                    int fileCount = 0;
                    root.rewindDirectory();

                    Serial.println("파일 목록 스캔 중...");
                    while (true) {
                        File entry = root.openNextFile();
                        if (!entry) {
                            break;
                        }
                        fileCount++;

                        if (fileCount <= 5) {
                            Serial.print("파일 ");
                            Serial.print(fileCount);
                            Serial.print(": ");
                            Serial.print(entry.name());
                            Serial.print(" (");
                            Serial.print(entry.size());
                            Serial.print(" bytes, ");
                            Serial.print(entry.isDirectory() ? "DIR" : "FILE");
                            Serial.println(")");
                        }

                        entry.close();
                        delay(10);
                    }

                    Serial.print("총 ");
                    Serial.print(fileCount);
                    Serial.println("개 항목 발견");

                } else {
                    Serial.println("✗ 루트가 디렉토리가 아님");
                }

                root.close();
                return true;

            } else {
                Serial.println("✗ 루트 디렉토리 열기 실패");
            }
        } else {
            Serial.println("실패");
        }
    }

    Serial.println("\n*** 모든 CS 핀 시도 실패 ***");
    return false;
}

void AudioPlayer::setupDAC() {
#if defined(ARDUINO_SAM_DUE)
    // Arduino Due용 DAC 설정
    analogWriteResolution(12);
    pinMode(DAC0, OUTPUT);
    Serial.println("Arduino Due DAC0 설정 완료");
#elif defined(ARDUINO_ARC32_TOOLS)
    // Arduino/Genuino 101용 PWM 설정
    pinMode(5, OUTPUT);
    Serial.println("Arduino/Genuino 101 PWM 설정 완료");
#else
    // 일반 보드용 PWM 설정
    pinMode(9, OUTPUT);
    Serial.println("일반 보드 PWM 설정 완료");
#endif
}

void AudioPlayer::setupAudioPins() {
    // MAX98357A 핀 설정을 별도 함수로 분리
    Serial.println("MAX98357A 핀 설정 중...");

    pinMode(MAX98357A_SS, OUTPUT);
    pinMode(MAX98357A_MODE, OUTPUT);
    digitalWrite(MAX98357A_MODE, LOW);
    digitalWrite(MAX98357A_SS, HIGH);

    Serial.println("MAX98357A 핀 설정 완료");
}

bool AudioPlayer::reinitializeSD() {
    Serial.println("=== SD 카드 재초기화 시도 ===");

    // SPI 재초기화
    SPI.end();
    delay(100);
    SPI.begin();

#if defined(ARDUINO_ARC32_TOOLS)
    SPI.setClockDivider(128);
#else
    SPI.setClockDivider(SPI_CLOCK_DIV128);
#endif

    // CS 핀 재설정
    pinMode(workingCSPin, OUTPUT);
    digitalWrite(workingCSPin, HIGH);
    delay(500);

    // SD 카드 재초기화
    Serial.print("CS 핀 ");
    Serial.print(workingCSPin);
    Serial.print("으로 재초기화 시도... ");

    if (SD.begin(workingCSPin)) {
        Serial.println("성공!");

        // 간단한 검증
        File root = SD.open("/");
        if (root) {
            Serial.println("✓ 루트 디렉토리 접근 성공");
            root.close();
            return true;
        } else {
            Serial.println("✗ 루트 디렉토리 접근 실패");
            return false;
        }
    } else {
        Serial.println("실패!");
        return false;
    }
}

void AudioPlayer::playAudioFile(const char* filename) {
    Serial.print("오디오 파일 재생 시도: ");
    Serial.println(filename);

    File audioFile = SD.open(filename);
    if (!audioFile) {
        Serial.println("파일 열기 실패!");
        return;
    }

    Serial.print("파일 크기: ");
    Serial.print(audioFile.size());
    Serial.println(" bytes");

    // WAV 헤더 건너뛰기
    audioFile.seek(44);
    isPlaying = true;
    uint8_t buffer[AUDIO_BUFFER_SIZE];

    Serial.println("오디오 재생 시작...");

    while (isPlaying && audioFile.available()) {
        int bytesRead = audioFile.read(buffer, AUDIO_BUFFER_SIZE);

        for (int i = 0; i < bytesRead; i += 2) {
            if (i + 1 < bytesRead) {
                int16_t sample = (buffer[i+1] << 8) | buffer[i];
                processSample(sample);
            }
        }
    }

    audioFile.close();
    Serial.println("오디오 재생 완료");
}

void AudioPlayer::stopAudio() {
    isPlaying = false;

#if defined(ARDUINO_SAM_DUE)
    analogWrite(DAC0, 2048);
#elif defined(ARDUINO_ARC32_TOOLS)
    analogWrite(5, 127);
#else
    analogWrite(9, 127);
#endif
}

void AudioPlayer::processSample(int16_t sample) {
    // 볼륨 적용
    sample = (sample * currentVolume) / MAX_VOLUME;

#if defined(ARDUINO_SAM_DUE)
    int dacValue = map(sample, -32768, 32767, 0, 4095);
    analogWrite(DAC0, dacValue);
#elif defined(ARDUINO_ARC32_TOOLS)
    int pwmValue = map(sample, -32768, 32767, 0, 255);
    analogWrite(5, pwmValue);
#else
    int pwmValue = map(sample, -32768, 32767, 0, 255);
    analogWrite(9, pwmValue);
#endif

    delayMicroseconds(23);
}

void AudioPlayer::setVolume(uint8_t volume) {
    if (volume > MAX_VOLUME) {
        volume = MAX_VOLUME;
    }
    currentVolume = volume;
    Serial.print("볼륨 설정: ");
    Serial.println(volume);
}
void AudioPlayer::listFiles() {
    Serial.println("\n=== 파일 목록 (Genuino 101 호환) ===");

    if (workingCSPin == -1) {
        Serial.println("SD 카드가 초기화되지 않았습니다!");
        return;
    }

    Serial.print("CS 핀 ");
    Serial.print(workingCSPin);
    Serial.println(" 사용 중");

    // 항상 매번 새 핸들로!
    File root = SD.open("/");
    if (!root) {
        Serial.println("루트 디렉토리 열기 실패 - SD 카드 재초기화 시도");

        if (!reinitializeSD()) {
            Serial.println("SD 카드 재초기화 실패!");
            return;
        }
        root = SD.open("/");
        if (!root) {
            Serial.println("재초기화 후에도 루트 디렉토리 열기 실패!");
            return;
        }
    }

    Serial.println("루트 디렉토리 열기 성공");
    Serial.println("파일 목록:");

    int totalFiles = 0;
    int wavFiles = 0;

    // openNextFile()의 File은 반드시 지역변수로 받고 곧바로 close!
    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            break;
        }
        // SYSTEM~1 등 시스템 폴더는 일반 사용자에게 숨기려면 아래 한줄 사용가능
        // if (entry.name()[0] == '.') { entry.close(); continue; }

        totalFiles++;

        String fileName = String(entry.name());

        Serial.print(totalFiles);
        Serial.print(". ");
        Serial.print(fileName);
        Serial.print(" (");
        Serial.print(entry.size());
        Serial.print(" bytes");

        if (entry.isDirectory()) {
            Serial.print(", DIR");
        } else {
            Serial.print(", FILE");
            String upperName = fileName;
            upperName.toUpperCase();
            if (upperName.endsWith(".WAV")) {
                Serial.print(" <-- WAV 파일!");
                wavFiles++;
            }
        }
        Serial.println(")");

        entry.close(); // 중요!
    }

    root.close(); // 중요!!

    Serial.print("총 ");
    Serial.print(totalFiles);
    Serial.print("개 항목, ");
    Serial.print(wavFiles);
    Serial.println("개 WAV 파일");

    if (totalFiles == 0) {
        Serial.println("*** 파일이 감지되지 않음 - 하드웨어 문제일 가능성 ***");
        Serial.println("해결 방법:");
        Serial.println("1. SD 카드를 다시 삽입하세요");
        Serial.println("2. 전원을 껐다가 다시 켜세요");
        Serial.println("3. 다른 SD 카드를 시도해 보세요");
    } else if (wavFiles == 0) {
        Serial.println("*** WAV 파일이 없습니다! ***");
        Serial.println("SD 카드에 .WAV 확장자의 오디오 파일을 추가하세요.");
    } else {
        Serial.println("*** 파일 목록 정상 출력 완료! ***");
    }
}

void AudioPlayer::printDirectory(File dir, int numTabs) {
    // 호환성을 위해 유지
}

bool AudioPlayer::selectAndPlayFile() {
    stopAudio();
    listFiles();

    Serial.println("\n재생할 파일 번호를 입력하세요:");

    unsigned long startTime = millis();
    while (!Serial.available() && (millis() - startTime < 10000)) {
        delay(100);
    }

    if (!Serial.available()) {
        Serial.println("시간 초과");
        return false;
    }

    int selection = Serial.parseInt();
    Serial.print("선택: ");
    Serial.println(selection);

    if (selection <= 0) {
        Serial.println("잘못된 번호");
        return false;
    }

    File root = SD.open("/");
    if (!root) {
        Serial.println("루트 디렉토리 열기 실패");
        return false;
    }

    int currentIndex = 0;
    String selectedFile;

    while (true) {
        File entry = root.openNextFile();
        if (!entry) {
            break;
        }

        if (!entry.isDirectory()) {
            currentIndex++;
            if (currentIndex == selection) {
                selectedFile = String(entry.name());
                entry.close();
                break;
            }
        }
        entry.close();
    }

    root.close();

    if (selectedFile.length() > 0) {
        Serial.print("재생: ");
        Serial.println(selectedFile);
        playAudioFile(selectedFile.c_str());
        return true;
    } else {
        Serial.println("파일을 찾을 수 없습니다");
        return false;
    }
}
//첫번째 초기화 시에는 정상작동
//두번째 이후부터는 최신화/리딩이 안됨 시발