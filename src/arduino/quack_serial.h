
#ifndef QUACK_SERIAL_H_
#define QUACK_SERIAL_H_

/*****************************************************************************
 * quack_serial.h:
 *  Mock class for Serial library.
 * 
*****************************************************************************/

#include "quack_config.h"

#ifdef SERIAL_ENABLED

#include <Serial.h>

#else

#include <cstdio>
#include <queue>
#include "quack_utils.h"

class Serial {

private:
    std::queue<u8> buffer;

public:
    Serial() {}

    void begin(u32 baudRate) {
        printf("[SERIAL] Beginning with baudrate %d.\n", baudRate); 
    }

    void write(u8 byte) {
        buffer.push(byte);
        printf("[SERIAL] Writing %c to buffer.\n", byte);
    }
    void write(const u8* const str, const u16 len) { for(u16 i = 0; i < len; i++) write(str[i]); }
    const u8 read() {
        if(!buffer.empty()) {
            u8 byte = buffer.front();
            printf("[SERIAL] Reading %c from buffer.\n", byte);
            buffer.pop();

            return byte;
        }
    }
    void flush() {
        while(!buffer.empty()) buffer.pop();
        printf("[SERIAL] Flushing buffer.\n");
    }
    u16 available() {
        //printf("[SERIAL] Checking availability.\n");
        return buffer.size();
    }
};

extern Serial Serial1;

#endif

#endif