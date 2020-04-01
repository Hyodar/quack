
#include "quack_frame.h"

#include "quack_codes.h"

QuackFrame::QuackFrame() : length{0} {
    // no-op
}

void
QuackFrame::setCommandCode(const u8 commandCode) {
    buffer[2] = commandCode;
}

void
QuackFrame::addParameterByte(const u8 byte) {
    buffer[HEADER_SIZE + length] = byte;
    length++;
}

void
QuackFrame::copyBuffer(const u8* const buf, const u16 bufLength) {
    for(u16 i = 0; i < bufLength; i++) {
        buffer[HEADER_SIZE + length + i] = buf[i];
    }

    length += bufLength;
}

void
QuackFrame::serialize(FastCRC16* CRC16) {
    
    buffer[3] = (length >> 8) & 0xff;
    buffer[4] = length & 0xff;
    
    u16 checksum = CRC16->ccitt(
        buffer + 2,
        length + sizeof(u8) + sizeof(u16)
    );

    buffer[0] = (checksum >> 8) & 0xff;
    buffer[1] = checksum & 0xff;

    length = 0; // reset length
    setCommandCode(COMMAND_NONE); // reset commandCode
}