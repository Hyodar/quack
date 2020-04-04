
#ifndef QUACK_SERIAL_H_
#define QUACK_SERIAL_H_

/*****************************************************************************
 * quack_serial.h:
 *  Mock class for Serial library.
 * 
*****************************************************************************/

#include "quack_config.h"

#ifdef SERIAL_ENABLED

#include <Arduino.h>

#else

#include <Arduino.h>
#include <queue>
#include "quack_utils.h"

class Serial {

private:
    std::queue<u8> buffer;

public:
    Serial() {}

    void begin(const u32 baudRate) const {
        DEBUGGING_PRINTF("[SERIAL] Beginning with baudrate %d.\n", baudRate); 
    }

    void write(const u8 byte) {
        buffer.push(byte);
        DEBUGGING_PRINTF("[SERIAL] Writing %c to buffer.\n", byte);
    }

    void write(const u8* const str, const u16 len) {
        for(u16 i = 0; i < len; i++) {
            write(str[i]);
        }
    }

    const u8 read() {
        if(!buffer.empty()) {
            const u8 byte = buffer.front();
            DEBUGGING_PRINTF("[SERIAL] Reading %c from buffer.\n", byte);
            
            buffer.pop();
            return byte;
        }
    }

    void flush() {
        DEBUGGING_PRINTF("[SERIAL] Flushing buffer.\n");

        while(!buffer.empty()) {
            buffer.pop();
        }
    }

    const u16 available() {
        //DEBUGGING_PRINTF("[SERIAL] Checking availability.\n");
        return buffer.size();
    }
};

extern Serial Serial1;

#endif

#endif