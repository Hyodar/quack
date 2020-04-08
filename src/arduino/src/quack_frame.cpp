
#include "quack_frame.h"
#include "quack_codes.h"

#define u16 __u16
#include <Arduino.h>
#undef u16

#include <quack_utils.h>

#include <FastCRC.h>

QuackFrame::QuackFrame() : commandCode{COMMAND_NONE},
                           length{0}, params{nullptr} {
    // no-op
}

const bool
QuackFrame::deserialize(const u8* const str, const u16 len, FastCRC16* CRC16) {

#ifdef FRAME_DEBUGGING
    DEBUGGING_PRINT(F("[FRAME] Starting deserialization.\n"));
#endif

    if(CRC16->ccitt(str + 2, len - 2) != DESERIALIZE_U16(str)) {
#ifdef FRAME_DEBUGGING
        DEBUGGING_PRINT(F("[FRAME] [!] Checksum ERROR! Will ask for a resend.\n"));
#endif
        return false; // wrong checksum
    }

#ifdef FRAME_DEBUGGING
    DEBUGGING_PRINT(F("[FRAME] Checksum OK! Continuing deserialization.\n"));
#endif

    commandCode = str[2];
    length = DESERIALIZE_U16(str + 3);
    params = str + 5;

#ifdef FRAME_DEBUGGING
        DEBUGGING_PRINT(F("[FRAME] Finished frame deserialization.\n"));
#endif

    return true;
}