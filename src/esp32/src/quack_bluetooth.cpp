
#include "quack_bluetooth.h"

#ifdef BLUETOOTH_ENABLED

#include <cstring>

#include <quack_utils.h>
#include "quack_parser.h"

QuackBluetooth::QuackBluetooth() : parser{nullptr}, state{NONE},
                                   buf{0}, bufSize{0}, passwordTries{1} {
    // no-op
}

void
QuackBluetooth::begin(QuackParser* const quackParser) {
    parser = quackParser;

    serial.begin("MyESP32");

    DEBUGGING_PRINTF("[BLUETOOTH] Beginning.\n");

    while(serial.available()) {
        serial.read();
    }
}

void
QuackBluetooth::loop() {
    while(serial.available()) {
        switch(state) {
            case NONE:
                if(!serial.read()) {
                    DEBUGGING_PRINTF("[BLUETOOTH] Received first null byte! Starting parsing.\n");
                    state = READING_PASSWORD;
                }
                break;
            case READING_PASSWORD:
                buf[bufSize++] = serial.read();
                DEBUGGING_PRINTF("%c", buf[bufSize - 1]);

                if(!buf[bufSize - 1]) {
                    // string ended
                    if(strcmp((char*) buf, BLUETOOTH_PASSWORD) == 0) {
                        sendEvent("bt-right-pwd", nullptr);
                        DEBUGGING_PRINTF("-> Right password!\n");
                        state = CONNECTED;
                    }
                    else {
                        sendEvent("bt-wrong-pwd", nullptr);
                        DEBUGGING_PRINTF("-> Wrong password!\n");
                        bufSize = 0;
                        if(passwordTries >= BLUETOOTH_MAX_PASSWORD_TRIES) {
                            sendEvent("bt-max-pwd-tries", nullptr);
                            serial.end();
                        }
                        else {
                            passwordTries++;
                        }
                    }
                }
                break;

            case CONNECTED:
                if(serial.read() == '\0') {
                    bufSize = 0;
                    buf[bufSize] = '\0';

                    DEBUGGING_PRINTF("[BLUETOOTH] Going to STREAM_START.\n");
                    state = STREAM_START;
                }
                break;

            case STREAM_START:
                activeResource = Resource(serial.read() - '0');
                DEBUGGING_PRINTF("[BLUETOOTH] Going to READING. activeResource: %d\n", activeResource);
                state = READING;
                break;

            case READING:
                if(activeResource != LIST && activeResource != OPEN) {
                    serial.write((uint8_t*) "R|ACK\0", 6);
                    DEBUGGING_PRINTF("[BLUETOOTH] Sent ACK.\n");
                } else {
                    serial.write((uint8_t*) "R|", 2);
                }

                DEBUGGING_PRINTF("[BLUETOOTH] Going to read resource params.\n");
                readResourceParams();
                break;

            case CONTINUOUS_READ: {
                char c = serial.read();

                if(c) {
                    activeFile.write(c);
                }
                else {
                    activeFile.close();
                    DEBUGGING_PRINTF("[BLUETOOTH] End file writing.\n");
                    state = WAITING_END;
                }
            }
                break;

            case WAITING_END:
                if(!serial.read()) {
                    buf[bufSize++] = '\0';
                    DEBUGGING_PRINTF("[BLUETOOTH] End command.\n");
                    state = CONNECTED;
                    respondRequest();
                    DEBUGGING_PRINTF("[BLUETOOTH] Responded to request.\n");
                }
                break;
        }
    }
}

void
QuackBluetooth::readResourceParams() {
    if(activeResource <= LOG_OFF) {
        state = WAITING_END; // no parameters
    }
    else {
        // filename or code
        bufSize = serial.readBytesUntil('\0', (uint8_t*) buf, 480);
        buf[bufSize++] = '\0';

        DEBUGGING_PRINTF("[BLUETOOTH] Buffer: {%s}\n", buf);
        
        if(activeResource == SAVE) {
            activeFile = SPIFFS.open((char*) buf, "w");
            state = CONTINUOUS_READ;
        }
        else {
            state = WAITING_END;
        }
    }
}

void
QuackBluetooth::respondRequest() {
    switch(activeResource) {
        case LOG_OFF:
            state = NONE;
            passwordTries = 1;
            bufSize = 0;
            break;
        case RUN_FILE:
            parser->setFile((char *) buf);
            break;
        case RUN_RAW:
            parser->fillBuffer(buf);
            break;
        case STOP:
            parser->stop();
            break;
        case SAVE:
            break;
        case LIST: {
            DEBUGGING_PRINTF("[BLUETOOTH] Listing directory.\n");
            fs::File root = SPIFFS.open("/");
            String json = "{\n\"dirFiles\": [";

            File file = root.openNextFile();
            
            DEBUGGING_PRINTF("[BLUETOOTH] Going through all files inside root.\n");
            while(file){
                if(!file.isDirectory()){
                    json += '"';
                    json += file.name();
                    json += '"';
                }
                file = root.openNextFile();

                if(file) {
                    json += ",";
                }
            }

            json += "]\n}";
            root.close();

            serial.write((uint8_t*) json.c_str(), json.length());
            serial.write('\0');
        }
            break;
        case OPEN:
            activeFile = SPIFFS.open((char*) buf);

            while(activeFile.available()) {
                serial.write(activeFile.read());
            }
            serial.write('\0');

            activeFile.close();
            break;
    }
}

const bool
QuackBluetooth::available() {
    return serial.available();
}

void
QuackBluetooth::sendEvent(const char* const event, const char* const data) {
    serial.write((uint8_t*) "E|", 2);
    for(u8 i = 0; event[i]; i++) {
        serial.write(event[i]);
    }
    serial.write('\0');
}

const bool
QuackBluetooth::hasClient() {
    return serial.hasClient();
}

#endif