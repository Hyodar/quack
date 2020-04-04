
#include "quack_interface.h"

#include "quack_codes.h"
#include "quack_serial.h"

#ifdef INTERFACE_DEBUGGING
// little hack here because of u16 typedef inside USBAPI.h
#define u16 __u16
#include <Arduino.h>
#undef u16
#endif

#include "quack_utils.h"

QuackInterface::QuackBuffer::QuackBuffer() : data{0}, length{0} {

}

QuackInterface::QuackInterface() : streamState{StreamState::WAITING_START},
                                   recBuffer{} /*, dataLength{0}*/ {
    // no-op
}

void
QuackInterface::begin() const {
    Serial1.begin(BAUDRATE);
}

const bool
QuackInterface::update() {
    u16 available = Serial1.available();

    if(streamState == StreamState::WAITING_START) {
        for(; available > 0; available--) {
            const u8 receivedByte = Serial1.read();

            if(receivedByte == FRAME_SEPARATOR) {
#ifdef INTERFACE_DEBUGGING
                DEBUGGING_PRINTF("[INTERFACE] Found start separator. Changing state to FILLING_BUFFER.\n");
#endif
                streamState = StreamState::FILLING_BUFFER;
                break;
            }

#ifdef INTERFACE_DEBUGGING
                DEBUGGING_PRINTF("[INTERFACE] Read byte on WAITING_START state: %d.\n", receivedByte);
#endif

        }
    }

    while(available && recBuffer.length <= BUFFER_SIZE) {
        const u8 receivedByte = Serial1.read();

#ifdef INTERFACE_DEBUGGING
        DEBUGGING_PRINTF("[INTERFACE] Receving byte %d from Serial.\n", receivedByte);
#endif
        if(receivedByte == FRAME_SEPARATOR) {
            if(recBuffer.length >= QUACKFRAME_HEADER_SIZE) {
                // finished getting header
#ifdef INTERFACE_DEBUGGING
                DEBUGGING_PRINTF("[INTERFACE] Received frame from Serial. Changing state to WAITING_START and waiting buffer read.\n");
#endif
                streamState = StreamState::WAITING_START;
                return true;
            }
        }
        
        recBuffer.data[recBuffer.length++] = receivedByte;
        available--;

    }

    return false;
}

// const bool
// QuackInterface::update() {
//     u16 available = Serial1.available();

//     while(streamState < RECEIVING_PARAMS && available) {

// #ifdef INTERFACE_DEBUGGING
//         DEBUGGING_PRINTF("[INTERFACE] Receving frame header.\n");
// #endif

//         const u8 receivedByte = Serial1.read();

// #ifdef INTERFACE_DEBUGGING
//         DEBUGGING_PRINTF("[INTERFACE] Receving byte %d from Serial.\n", receivedByte);
// #endif

//         if(streamState >= RECEIVING_LENGTH_1) {
// #ifdef INTERFACE_DEBUGGING
//             DEBUGGING_PRINTF("[INTERFACE] Datalength byte %d: %d\n", streamState - RECEIVING_LENGTH_1, receivedByte);
// #endif

//             dataLength.bytes[streamState - RECEIVING_LENGTH_1] = receivedByte;
//         }

//         recBuffer.data[recBuffer.length++] = receivedByte;
//         available--;
//         streamState = StreamState(streamState + 1);
//     }

// #ifdef INTERFACE_DEBUGGING
//         //DEBUGGING_PRINTF("[INTERFACE] Received frame. Expected params length: %d %d.\n", dataLength.bytes[0], dataLength.bytes[1]);
// #endif

//     while(available) {
//         const u8 receivedByte = Serial1.read();

// #ifdef INTERFACE_DEBUGGING
//         DEBUGGING_PRINTF("[INTERFACE] Receving byte %d from Serial.\n", receivedByte);
// #endif

//         recBuffer.data[recBuffer.length++] = receivedByte;
//         available--;
//         dataLength.length--;

//         if(NOT dataLength.length) {
//             streamState = StreamState::RECEIVING_CHECKSUM_1;
//             return true;
//         }
//     }

//     return false;
// }

QuackInterface::QuackBuffer*
QuackInterface::getBuffer() {
    return &recBuffer;
}

void
QuackInterface::requestResend() const {
#ifdef ESP_ENABLED
    Serial1.write(FRAME_SEPARATOR);
    Serial1.write(WRONG_CHECKSUM);
    Serial1.write(FRAME_SEPARATOR);
#endif
}

void
QuackInterface::requestNext() const {
#ifdef ESP_ENABLED
    Serial1.write(FRAME_SEPARATOR);
    Serial1.write(FINISHED_PARSING);
    Serial1.write(FRAME_SEPARATOR);
#endif
}

void
QuackInterface::flush() {

#ifdef INTERFACE_DEBUGGING
        DEBUGGING_PRINTF("[INTERFACE] Parsing finished. Resetting buffer and changing state back to RECEIVING.\n");
#endif

    recBuffer.length = 0;
    // streamState = StreamState::RECEIVING;

}