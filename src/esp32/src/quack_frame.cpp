
#include "quack_frame.h"

#include "quack_codes.h"

#ifdef FRAME_DEBUGGING
#include <Arduino.h>
#endif

QuackFrame::QuackFrame() : buffer{0}, length{0} {
    // no-op
}

void
QuackFrame::setCommandCode(const u8 commandCode) {
    DEBUGGING_PRINTF("[FRAME] Setting CommandCode to %d\n", commandCode);
    buffer[2] = commandCode;
}

void
QuackFrame::addParameterByte(const u8 byte) {
    buffer[FRAME_HEADER_SIZE + length] = byte;
    length++;
}

void
QuackFrame::copyBuffer(const u8* const buf) {
    u16 i;
    for(i = 0; buf[i]; i++) {
        buffer[FRAME_HEADER_SIZE + length + i] = buf[i];
    }

    length += i;
}

void
QuackFrame::serialize(FastCRC16* CRC16) {
    
    DEBUGGING_PRINTF("[FRAME] Final parameter length: %d\n", length);
    buffer[3] = (length >> 8) & 0xff;
    buffer[4] = length & 0xff;
    
    u16 checksum = CRC16->ccitt(
        buffer + 2,
        length + sizeof(u8) + sizeof(u16)
    );

    buffer[0] = (checksum >> 8) & 0xff;
    buffer[1] = checksum & 0xff;

#ifdef FRAME_DEBUGGING
    DEBUGGING_PRINTF("[FRAME] Serialized frame. Result: {\n");
    DEBUGGING_PRINTF("\tChecksum: %d,\n", DESERIALIZE_U16(buffer));
    DEBUGGING_PRINTF("\tCommandCode: %d,\n", buffer[2]);
    DEBUGGING_PRINTF("\tLength: %d,\n", DESERIALIZE_U16(buffer + 3));
    DEBUGGING_PRINTF("\tParams: {");
    for(u16 i = 0; i < DESERIALIZE_U16(buffer + 3); i++) DEBUGGING_PRINTF("%d, ", buffer[FRAME_HEADER_SIZE + i]);
    DEBUGGING_PRINTF("}\n}\n");
#endif
}

void
QuackFrame::reset() {
    DEBUGGING_PRINTF("[FRAME] Resetting frame.\n");
    length = 0; // reset length
    setCommandCode(COMMAND_NONE); // reset commandCode
}