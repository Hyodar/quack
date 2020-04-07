
#include "quack_interface.h"

#include "quack_codes.h"
#include "quack_serial.h"

// little hack here because of u16 typedef inside USBAPI.h
#define u16 __u16
#include <Arduino.h>
#undef u16

#include "quack_utils.h"

QuackInterface::QuackBuffer::QuackBuffer() : data{0}, length{0} {

}

QuackInterface::QuackInterface() : streamState{StreamState::WAITING_START},
                                   recBuffer{} /*, dataLength{0}*/ {
    // no-op
}

void
QuackInterface::begin() const {
    Serial.begin(SERIAL_BAUDRATE);
    Serial1.begin(SERIAL1_BAUDRATE);
    delay(1000);

    // empty read buffer
    while(Serial1.available()) {
        Serial1.read();
    }
}

const bool
QuackInterface::update() {
    u16 available = Serial1.available();

    if(streamState == StreamState::WAITING_START) {
#ifdef INTERFACE_DEBUGGING
        DEBUGGING_PRINT(F("[INTERFACE] Waiting start.\n"));
        delay(1000);
#endif
        for(; available > 0; available--) {
            const u8 receivedByte = Serial1.read();

            if(receivedByte == FRAME_SEPARATOR) {
#ifdef INTERFACE_DEBUGGING
                DEBUGGING_PRINT(F("[INTERFACE] Found start separator. Changing state to FILLING_BUFFER.\n"));
#endif
                available--;

                streamState = StreamState::FILLING_BUFFER;
                break;
            }

#ifdef INTERFACE_DEBUGGING
                DEBUGGING_PRINT(F("[INTERFACE] Read byte on WAITING_START state: "));
                DEBUGGING_PRINT(receivedByte, HEX);
                DEBUGGING_PRINT(F(".\n"));
#endif

        }
    }

    if(streamState == StreamState::FILLING_BUFFER) {
#ifdef INTERFACE_DEBUGGING
        DEBUGGING_PRINT(F("[INTERFACE] Filling buffer.\n"));
        delay(1000);
#endif
        while(available && recBuffer.length <= BUFFER_SIZE) {
            const u8 receivedByte = Serial1.read();

#ifdef INTERFACE_DEBUGGING
            DEBUGGING_PRINT(F("[INTERFACE] Receiving byte from serial: "));
            DEBUGGING_PRINT(receivedByte, HEX);
            DEBUGGING_PRINT(F(".\n"));
#endif
            if(receivedByte == FRAME_SEPARATOR) {
                if(recBuffer.length >= QUACKFRAME_HEADER_SIZE) {
                    // finished getting header
#ifdef INTERFACE_DEBUGGING
                    DEBUGGING_PRINT(F("[INTERFACE] Received frame from Serial. Changing state to WAITING_START and waiting buffer read.\n"));
#endif
                    streamState = StreamState::WAITING_START;
                    return true;
                }
            }
            
            recBuffer.data[recBuffer.length++] = receivedByte;
            available--;

        }
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
#endif
}

void
QuackInterface::requestNext() const {
#ifdef ESP_ENABLED
    Serial1.write(FRAME_SEPARATOR);
    Serial1.write(FINISHED_PARSING);
#endif
}

void
QuackInterface::flush() {

#ifdef INTERFACE_DEBUGGING
    DEBUGGING_PRINT(F("[INTERFACE] Flushing. Resetting buffer and changing state back to WAITING_START.\n"));
#endif

    recBuffer.length = 0;
    streamState = StreamState::WAITING_START;

}