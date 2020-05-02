
#include "quack_bluetooth.h"

#include <quack_utils.h>
#include "quack_parser.h"

QuackBluetooth::QuackBluetooth() : parser{nullptr}, state{CONNECTED},
                                   buf{0}, bufSize{0} {
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
                    serial.write((uint8_t*) "ACK", 3);
                    serial.write('\0');
                    DEBUGGING_PRINTF("[BLUETOOTH] Sent ACK.\n");
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
                    respondRequest();
                    DEBUGGING_PRINTF("[BLUETOOTH] Responded to request.\n");
                    state = CONNECTED;
                }
                break;
        }
    }
}

void
QuackBluetooth::readResourceParams() {
    if(activeResource <= LIST) {
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
            fs::File root = SPIFFS.open("/");
            String json = "{\n\"dirFiles\": [";

            File file = root.openNextFile();
            
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