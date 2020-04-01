
#include "quack_frame.h"
#include "quack_codes.h"

QuackFrame::QuackFrame() : checksum{0}, commandCode{COMMAND_NONE},
                           length{0}, params{nullptr} {
    // no-op
}

const bool
QuackFrame::deserialize(const u8* const str, const u16 len, FastCRC16* CRC16) {
    
    checksum = CRC16->ccitt(
        str + 2, // skip previous checksum
        len - 2
    );

    if(checksum != DESERIALIZE_U16(str)) {
        return false; // wrong checksum
    }

    commandCode = str[2];
    length = DESERIALIZE_U16(str + 3);
    params = str + 5;

    return true;
}