
#include "quack_frame.h"
#include "quack_codes.h"

QuackFrame::QuackFrame() : checksum{0}, commandCode{COMMAND_NONE},
                           length{0}, params{nullptr} {
    // no-op
}

#include <cstdio>
const bool
QuackFrame::deserialize(const u8* const str, const u16 len, FastCRC16* CRC16) {
    for(u16 i = 2; i < len; i++) printf("%d, ", str[i]);
    putchar('\n');
    
    checksum = CRC16->ccitt(
        str + 2, // skip previous checksum
        len - 2
    );

    u16 sentChecksum = DESERIALIZE_U16(str);
    printf("Checksum %d", sentChecksum);

    if(checksum != sentChecksum) {
        return false; // wrong checksum
    }

    commandCode = str[2];
    length = DESERIALIZE_U16(str + 3);
    params = str + 5;

    return true;
}