
#ifndef QUACK_BLUETOOTH_H_
#define QUACK_BLUETOOTH_H_

#include <BluetoothSerial.h>
#include <SPIFFS.h>

#include "quack_utils.h"

class QuackParser;

class QuackBluetooth {

enum State {
    READING_PASSWORD    = 0,
    CONNECTED           = 1,
    STREAM_START        = 2,
    READING             = 3,
    CONTINUOUS_READ     = 4,
    WAITING_END         = 5,
};

enum Resource {
    // Params: None | Return: Event
    STOP        = 1,

    // Params: None | Return: Json
    LIST        = 2,

    // Params: None | Return: None
    LOG_OFF     = 3,

    // Params: (Code:char[]) | Return: Event
    RUN_RAW     = 4,

    // Params: (Filename:char[]) | Return: Event
    RUN_FILE    = 5,

    // Params: (Filename: char[]) | Return: stream
    OPEN        = 6,

    // Params: (Filename:char[], FileBytes:stream) | Return: Event
    SAVE        = 7,
};

private:
    QuackParser* parser;
    State state;

    u8 buf[480];
    u16 bufSize;

    Resource activeResource;
    File activeFile;

    BluetoothSerial serial;

    u8 passwordTries;

    void readResourceParams();
    void respondRequest();

public:
    QuackBluetooth();
    void begin(QuackParser* const quackParser);
    void loop();
    
    const bool available();
    void sendEvent(const char* const event, const char* const data);

    const bool hasClient();

};

#endif