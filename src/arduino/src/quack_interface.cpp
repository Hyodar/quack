
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
                DEBUGGING_PRINT("[INTERFACE] Found start separator. Changing state to FILLING_BUFFER.\n");
#endif
                streamState = StreamState::FILLING_BUFFER;
                break;
            }

#ifdef INTERFACE_DEBUGGING
                DEBUGGING_PRINT("[INTERFACE] Read byte on WAITING_START state: ");
                DEBUGGING_PRINT(receivedByte, HEX);
                DEBUGGING_PRINT(".\n");
#endif

        }
    }

    while(available && recBuffer.length <= BUFFER_SIZE) {
        const u8 receivedByte = Serial1.read();

#ifdef INTERFACE_DEBUGGING
        DEBUGGING_PRINT("[INTERFACE] Receiving byte from serial: ");
        DEBUGGING_PRINT(receivedByte, HEX);
        DEBUGGING_PRINT(".\n");
#endif
        if(receivedByte == FRAME_SEPARATOR) {
            if(recBuffer.length >= QUACKFRAME_HEADER_SIZE) {
                // finished getting header
#ifdef INTERFACE_DEBUGGING
                DEBUGGING_PRINT("[INTERFACE] Received frame from Serial. Changing state to WAITING_START and waiting buffer read.\n");
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
    DEBUGGING_PRINT("[INTERFACE] Flushing. Resetting buffer and changing state back to WAITING_START.\n");
#endif

    recBuffer.length = 0;
    streamState = StreamState::WAITING_START;

}