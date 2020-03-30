
#include "quack_parser.h"

#include "quack_codes.h"
#include "quack_config.h"

#ifdef PARSER_DEBUGGING
#include <cstdio>
#endif

QuackParser::QuackFrame::QuackFrame() : checksum{0}, length{0}, commandCode{COMMAND_NONE}, params{nullptr} {
    // no-op
}

QuackParser::QuackParser() /*: parsingState{ParsingState::NONE}*/ {
    // no-op
}

void
QuackParser::begin() {
    commandManager.begin();
}

// https://stackoverflow.com/questions/16826422/c-most-efficient-way-to-convert-string-to-int-faster-than-atoi
const u32
QuackParser::parseU32(const u8* const str) {
    u32 n = 0;
    
    for(u16 i = 0; i < 4; i++) {
        n = (n*10) + (str[i] - '0');
    }

    return n;
}

const bool
QuackParser::parse(const u8* const str, const u16 len) {

#ifdef PARSER_DEBUGGING
    printf("[PARSER] Starting.\n");
#endif
    quackFrame.checksum = STR_TO_U16(str);
    quackFrame.commandCode = str[2];
    quackFrame.length = STR_TO_U16(str + 3);
    quackFrame.params = str + 5;

    if(!checkChecksum()) {
#ifdef PARSER_DEBUGGING
        printf("[PARSER] Checksum (%d) error! Requesting resend.\n", quackFrame.checksum);
#endif
        return false;
    }
#ifdef PARSER_DEBUGGING
    else {
        printf("[PARSER] Checksum (%d) OK! Sending command to CommandManager.\n", quackFrame.checksum);
    }
#endif

    sendCommand();
    
    return true;
}

const bool
QuackParser::checkChecksum() {
    // TODO import checksum
#ifdef PARSER_DEBUGGING
    printf("[PARSER] Computing checksum for '");
    for(u16 i = 0; i < quackFrame.length + 3; i++) {
        printf("%d ", (quackFrame.params - 3)[i]);
    }
    printf("'\n");
#endif
    return (CRC16.ccitt(quackFrame.params - 3, quackFrame.length + 3) == quackFrame.checksum);
}

void
QuackParser::sendCommand() {

    if(quackFrame.commandCode >= COMMAND_DELAY) {
        // u32 parameter
        commandManager.command(quackFrame.commandCode, parseU32(quackFrame.params));
        return;
    }
    commandManager.command(quackFrame.commandCode, quackFrame.params, quackFrame.length);
}