
#include "quack_parser.h"

#include "quack_codes.h"
#include "quack_config.h"

#ifdef PARSER_DEBUGGING
#include <cstdio>
#endif

QuackParser::QuackParser() : parsingState{ParsingState::NONE}, currentCommand{COMMAND_NONE} {
    // no-op
}

void
QuackParser::begin() {
    commandManager.begin();
}

// https://stackoverflow.com/questions/16826422/c-most-efficient-way-to-convert-string-to-int-faster-than-atoi
u32
QuackParser::parseU32(const u8* str, u16 len) {
    u32 n = 0;
    
    for(;len != 0; len--) {
        n = (n*10) + (*str - '0');
        ++str;
    }

    return n;
}

// parse(str, len) is built to be able to parse multiple lines at once,
// if necessary

void
QuackParser::parse(const u8* str, const u16 len) {
    u16 cursor = 0;
    
    const u8* paramStart = nullptr;
    u16 paramLen = 0;

#ifdef PARSER_DEBUGGING
    printf("[PARSER] starting...\n");
#endif

    if(parsingState == ParsingState::CONTINUE) {
        paramStart = str;
        parsingState = ParsingState::PARAMS;
    }

    for(;;cursor++) {
        paramLen = 0;

        while(str[cursor] != '\n' AND str[cursor] != '\r') {
            if(!(cursor < len)) goto end;

#ifdef PARSER_DEBUGGING
            printf("[PARSER] reading char: %c\n", str[cursor]);
#endif

            if(parsingState == ParsingState::PARAMS) {
                paramLen++;
                cursor++;
                continue;
            }

            currentCommand = str[cursor];
            paramStart = str + cursor + 1;
            parsingState = ParsingState::PARAMS;

            cursor++;
        }

        sendCommand(paramStart, paramLen);
    }

    return;

    end:
        if(parsingState == ParsingState::PARAMS) {
            if(paramLen) {
                sendCommand(paramStart, paramLen);
            }
            else if(NOT(str[cursor - 1] == '\n' || str[cursor - 1] == '\r')) {
                parsingState = ParsingState::CONTINUE;
            }
        }
}

void
QuackParser::sendCommand(const u8* params, const u16 len) {
    parsingState = ParsingState::COMMAND;

    if(currentCommand >= COMMAND_DELAY) {
        commandManager.command(currentCommand, parseU32(params, len));
        return;
    }
    commandManager.command(currentCommand, params, len);
}