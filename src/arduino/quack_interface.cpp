
#include "quack_interface.h"

#include "quack_serial.h"

#ifdef INTERFACE_DEBUGGING
#include <cstdio>
#endif

QuackInterface::QuackBuffer::QuackBuffer() : data{0}, length{0} {

}

QuackInterface::QuackInterface() : // streamState{StreamState::RECEIVING},
                                   recBuffer{} {
    // no-op
}

void
QuackInterface::begin() const {
    Serial1.begin(BAUDRATE);
}

const bool
QuackInterface::update() {
    u16 available = Serial1.available();

    while(available && recBuffer.length <= BUFFER_SIZE) {
        const u8 receivedByte = Serial1.read();

#ifdef INTERFACE_DEBUGGING
        printf("[INTERFACE] Receving byte %c from Serial.\n", receivedByte);
#endif

        recBuffer.data[recBuffer.length++] = receivedByte;
        available--;

        if(receivedByte == '\n') {

#ifdef INTERFACE_DEBUGGING
        printf("[INTERFACE] Received newline from Serial. Changing state to PARSING.\n");
#endif

            // streamState = StreamState::PARSING;
            return true;
        }
    }

    return false;
}

QuackInterface::QuackBuffer*
QuackInterface::getBuffer() {
    return &recBuffer;
}

void
QuackInterface::flush() {

#ifdef INTERFACE_DEBUGGING
        printf("[INTERFACE] Parsing finished. Resetting buffer and changing state back to RECEIVING.\n");
#endif

    recBuffer.length = 0;
    // streamState = StreamState::RECEIVING;

}