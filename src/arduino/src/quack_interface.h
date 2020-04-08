
#ifndef QUACK_INTERFACE_H_
#define QUACK_INTERFACE_H_

/*****************************************************************************
 * quack_interface.h:
 *  Class to deal with data stream. In this case, Serial.
 * 
*****************************************************************************/

#include <quack_config.h>
#include <quack_utils.h>

#include "quack_parser.h"

class QuackInterface {

public:

    #define UNPACK_BUFFER(buffer) buffer->data, buffer->length
    struct QuackBuffer {
        u8 data[BUFFER_SIZE];
        u16 length;

        QuackBuffer();
    };

    enum StreamState {
        WAITING_START,
        FILLING_BUFFER,
    };

private:
    StreamState streamState;

    QuackBuffer recBuffer;

public:

    QuackInterface();

    void begin() const;

    const bool update();

    QuackBuffer* getBuffer();

    void requestResend() const;
    void requestNext() const;

    void flush();

};

#endif