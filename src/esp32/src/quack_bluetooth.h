
#ifndef QUACK_BLUETOOTH_H_
#define QUACK_BLUETOOTH_H_

#include <BluetoothSerial.h>
#include <SPIFFS.h>

#include "quack_utils.h"

class QuackParser;

class QuackBluetooth {

enum State {
    CONNECTED       = 1,
    STREAM_START    = 2,
    READING         = 3,
    CONTINUOUS_READ = 4,
    WAITING_END     = 5,
};

enum Resource {
    // Params: None | Return: Event
    STOP        = 1,

    // Params: None | Return: Json
    LIST        = 2,

    // Params: (Code:char[]) | Return: Event
    RUN_RAW     = 3,

    // Params: (Filename:char[]) | Return: Event
    RUN_FILE    = 4,

    // Params: (Filename: char[]) | Return: stream
    OPEN        = 5,

    // Params: (Filename:char[], FileBytes:stream) | Return: Event
    SAVE        = 6,
};

private:
    QuackParser* parser;
    State state;

    u8 buf[480];
    u16 bufSize;

    Resource activeResource;
    File activeFile;

    BluetoothSerial serial;

    bool isEnabled;

    void readResourceParams();
    void respondRequest();

public:
    QuackBluetooth();
    void begin(QuackParser* const quackParser);
    void loop();
    
    const bool available();
    void sendEvent(const char* const event, const char* const data);

    const bool getIsEnabled() const;
    void setIsEnabled(const bool _isEnabled);

};

#endif