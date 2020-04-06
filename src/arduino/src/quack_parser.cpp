
#include "quack_parser.h"

#include "quack_codes.h"
#include "quack_config.h"

// little hack here because of u16 typedef inside USBAPI.h
#define u16 __u16
#include <Arduino.h>
#undef u16

#include "quack_utils.h"

QuackParser::QuackParser() /*: parsingState{ParsingState::NONE}*/ {
    // no-op
}

void
QuackParser::begin() {
    commandManager.begin();
}

// https://stackoverflow.com/questions/16826422/c-most-efficient-way-to-convert-string-to-int-faster-than-atoi
const u32
QuackParser::parseU32(const u8* const str, const u16 len) {
    u32 n = 0;
    
    for(u16 i = 0; i < len; i++) {
        n = (n*10) + (str[i] - '0');
    }

    return n;
}

const bool
QuackParser::parse(const u8* const str, const u16 len) {

#ifdef PARSER_DEBUGGING
    DEBUGGING_PRINT(F("[PARSER] Starting.\n"));
#endif

    if(!quackFrame.deserialize(str, len, &CRC16)) {
#ifdef PARSER_DEBUGGING
        DEBUGGING_PRINT(F("[PARSER] Checksum error! Requesting resend.\n"));
#endif
        return false;
    }
#ifdef PARSER_DEBUGGING
    else {
        DEBUGGING_PRINT(F("[PARSER] Checksum OK! Sending command to CommandManager.\n"));
    }
#endif

    sendCommand();
    
    return true;
}

void
QuackParser::sendCommand() {

    if(quackFrame.commandCode >= COMMAND_DELAY) {
        // u32 parameter
        commandManager.command(quackFrame.commandCode, parseU32(quackFrame.params, quackFrame.length));
        return;
    }
    commandManager.command(quackFrame.commandCode, quackFrame.params, quackFrame.length);
}