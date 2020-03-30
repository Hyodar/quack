
#ifndef QUACK_INTERFACE_H_
#define QUACK_INTERFACE_H_

/*****************************************************************************
 * quack_interface.h:
 *  Class to deal with data stream. In this case, Serial.
 * 
*****************************************************************************/

#include "quack_config.h"
#include "quack_utils.h"

#include "quack_parser.h"

class QuackInterface {

public:

    #define UNPACK_BUFFER(buffer) buffer->data, buffer->length
    struct QuackBuffer {
        u8 data[BUFFER_SIZE];
        u16 length;

        QuackBuffer();
    };

    // enum StreamState {
    //     NONE                    = 0,
    //     RECEIVING_CHECKSUM_1    = 1,
    //     RECEIVING_CHECKSUM_2    = 2,
    //     RECEIVING_COMMAND       = 3,
    //     RECEIVING_LENGTH_1      = 4,
    //     RECEIVING_LENGTH_2      = 5,
    //     RECEIVING_PARAMS        = 6,
    //     PARSING                 = 7
    // };

    enum StreamState {
        WAITING_START,
        FILLING_BUFFER
    };

private:
    StreamState streamState;

    QuackBuffer recBuffer;
    
    // union u16Length {
    //     u16 length;
    //     u8  bytes[2];
    // };

    // union u16Length dataLength;

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