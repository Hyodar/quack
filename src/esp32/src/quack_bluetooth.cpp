
#include "quack_bluetooth.h"

#include "quack_parser.h"

QuackBluetooth::QuackBluetooth() : parser{nullptr}, state{CONNECTED},
                                   buf{0}, bufSize{0} {
    // no-op
}

void
QuackBluetooth::begin(QuackParser* const quackParser) {
    parser = quackParser;

    serial.begin("MyESP32");

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
                    state = STREAM_START;
                }
                break;

            case STREAM_START:
                activeResource = Resource(serial.read());
                state = READING;
                break;

            case READING:
                readResourceParams();
                break;

            case CONTINUOUS_READ: {
                char c = serial.read();

                if(c) {
                    activeFile.write(c);
                }
                else {
                    activeFile.close();
                    state = STREAM_END;
                }
            }
                break;

            case WAITING_END:
                if(!serial.read()) {
                    buf[bufSize++] = '\0';
                    state = STREAM_END;
                }
                break;

            case STREAM_END:
                respondRequest();
                state = CONNECTED;
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
        bufSize = serial.readBytesUntil('\0', (uint8_t*) buf, 480); // filename
        
        if(activeResource == SAVE) {
            activeFile = SPIFFS.open((char*) buf, "w");
            state = CONTINUOUS_READ;
        }
        else {
            state = STREAM_END;
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
            serial.print("ack");
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

            serial.print(json.c_str());
        }
            break;
        case OPEN:
            activeFile = SPIFFS.open((char*) buf);

            serial.print(activeFile);

            activeFile.close();
            break;
    }

    serial.write('\0');
}