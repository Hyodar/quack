
#include "quack_parser.h"

#include "quack_display.h"
#include "usb_hid_keys.h"
#include "quack_codes.h"
#include "locale_us.h"

#include <cstring>
#include <Arduino.h>

const char* const LINE_SEPARATOR = "\n";

QuackParser::QuackLine::QuackLine() : lineOrder{0}, state{QuackLine::State::FREE_TO_PARSE} {
    // no-op
}

QuackParser::QuackParser() : state{QuackParser::State::NONE},
                             activeLine{0}, nextOrder{0}, orderCount{0},
                             keyboardLocale{&locale_us}, buffer{0},
                             activeFile{}, activeCommand{COMMAND_NONE},
                             quackDisplay{nullptr} {
    // no-op
}

void
QuackParser::begin(QuackDisplay* _quackDisplay) {
    quackDisplay = _quackDisplay;
}

#define LINE quackLines[activeLine].data

void
QuackParser::replaceKey(const u8 str) {
    if(str < keyboardLocale->asciiLen) {
        LINE.addParameterByte(pgm_read_byte(keyboardLocale->ascii + str * 2 + 1));
        u8 modifier = pgm_read_byte(keyboardLocale->ascii + str * 2);
        if(modifier) LINE.addParameterByte(modifier);
        return;
    }

    for(u8 i = 0; i < keyboardLocale->extendedAsciiLen; i++) {
        if(str == pgm_read_byte(keyboardLocale->extendedAscii + i * 3)) {
            LINE.addParameterByte(pgm_read_byte(keyboardLocale->extendedAscii + i * 3 + 2));
            u8 modifier = pgm_read_byte(keyboardLocale->extendedAscii + i * 3 + 1);
            if(modifier) LINE.addParameterByte(modifier);
            return;
        }
    }
}

void
QuackParser::replaceKeyword(const u8* const str, const u16 len) {
    switch (len) {
        case 1:
            replaceKey(*str);
            break;
        case 2:
            LINE.addParameterByte(KEY_F1 + (str[1] - '1'));
            break;
        case 3:
            switch(str[0]) {
                case 'A':
                    LINE.addParameterByte(KEY_LEFTALT);
                    break;
                case 'T':
                    LINE.addParameterByte(KEY_TAB);
                    break;
                case 'F':
                    LINE.addParameterByte(KEY_F10 + (str[2] - '0'));
                    break;
                case 'G':
                    LINE.addParameterByte(KEY_LEFTMETA);
                    break;
                default:
                    LINE.addParameterByte(KEY_END);
            }
            break;
        case 4:
            switch(str[0]) {
                case 'C':
                    LINE.addParameterByte(KEY_LEFTCTRL);
                    break;
                case 'H':
                    LINE.addParameterByte(KEY_HOME);
                    break;
                default:
                    LINE.addParameterByte(KEY_PROPS);
            }
            break;
        case 5:
            switch(str[0]) {
                case 'E':
                    LINE.addParameterByte(KEY_ENTER);
                    break;
                case 'P':
                    LINE.addParameterByte(KEY_PAUSE);
                    break;
                default:
                    if(str[1] == 'H') {
                        LINE.addParameterByte(KEY_LEFTSHIFT);
                    }
                    else {
                        LINE.addParameterByte(KEY_SPACE);
                    }
            }
            break;
        case 6:
            switch(str[0]) {
                case 'D':
                    LINE.addParameterByte(KEY_BACKSPACE);
                    break;
                case 'E':
                    LINE.addParameterByte(KEY_ESC);
                    break;
                case 'I':
                    LINE.addParameterByte(KEY_INSERT);
                    break;
                default:
                    LINE.addParameterByte(KEY_PAGEUP);
            }
            break;
        case 7:
            if(str[0] == 'N') {
                LINE.addParameterByte(KEY_NUMLOCK);
            }
            else {
                LINE.addParameterByte(KEY_UP);
            }    
            break;
        case 8:
            if(str[0] == 'C') {
                LINE.addParameterByte(KEY_CAPSLOCK);
            }
            else {
                LINE.addParameterByte(KEY_PAGEDOWN);
            }
            break;
        case 9:
            if(str[0] == 'L') {
                LINE.addParameterByte(KEY_LEFT);
            }
            else {
                LINE.addParameterByte(KEY_DOWN);
            }
            break;
        default:
            switch(str[0]) {
                case 'R':
                    LINE.addParameterByte(KEY_RIGHT);
                    break;
                case 'S':
                    LINE.addParameterByte(KEY_SCROLLLOCK);
                    break;
                default:
                    LINE.addParameterByte(KEY_SYSRQ);
            }
    }
}

void
QuackParser::parseKeysParams(const u8* const str) {
    u16 start = 0;
    u16 length = 0;

    for(u16 i = 0; str[i]; i++, length++) {
        if(str[i] == ' ') {
            replaceKeyword(str + start, length);
            
            start = i + 1;
            length = 0;
        }
    }

    if(length) {
        // got to end of line, but didn't parse last key
        replaceKeyword(str + start, length);
    }

}

void
QuackParser::parseStringParams(const u8* const str) {
    for(u16 i = 0; str[i]; i++) {
        replaceKey(str[i]);
    }
}

const u8
QuackParser::getCommandCode(const u8* const str) {
    // tried to parse based on usage frequency here,
    // but could probably use some improvements
    u16 commandLength = 1;
    while(str[commandLength - 1] != ' ' && commandLength < sizeof("DEFAULT_DELAY")) {
        commandLength++;
    }
    
    u8 commandCode;
    switch(commandLength) {
        case 4:
            commandCode = COMMAND_KEYS;
            break;
        case 5:
            commandCode = COMMAND_DELAY;
            break;
        case 6:
            commandCode = (str[0] == 'S')? COMMAND_STRING : COMMAND_REPEAT;
            break;
        case 7:
            commandCode = (str[0] == 'D')? COMMAND_DISPLAY : COMMAND_KEYCODE;
            break;
        case 13:
            commandCode = COMMAND_DEFAULTDELAY;
            break;
        default:
            commandCode = (commandLength == sizeof("DEFAULT_DELAY"))
                          ? activeCommand | COMMAND_CONTINUE_F
                          : COMMAND_DEFAULTDELAY;
    }

    activeCommand = commandCode;
    LINE.setCommandCode(commandCode);

    return commandLength;
}

const bool
QuackParser::updateActiveLine() {
    for(u8 i = 0; i < QUACKLINES_BUFFER; i++) {
        if(quackLines[i].state == QuackLine::State::FREE_TO_PARSE) {
            activeLine = i;
            quackLines[i].state = QuackLine::State::PARSING;

            return true;
        }
    }

    return false;
}

const bool
QuackParser::runLocalCommand(const u8* const params) {
    // run local commands inside ESP

    if(activeCommand <= COMMAND_DISPLAY) {
        if(activeCommand == COMMAND_DISPLAY) {
            quackDisplay->write(params);
            quackLines[activeLine].state = QuackLine::State::FREE_TO_PARSE;
            return true;
        }
        else { // COMMAND_NONE
            quackLines[activeLine].state = QuackLine::State::FREE_TO_PARSE;
            return true;
        }
    }
    return false;
}

const bool
QuackParser::parse(const u8* const str) {
    if(!updateActiveLine()) {
        return false;
    }

    LINE.reset();

    DEBUGGING_PRINTF("%s\n", str);

    const u16 cursor = getCommandCode(str);

    if(runLocalCommand(str + cursor)) {
        return true;
    }

    quackLines[activeLine].lineOrder = orderCount++;
    
    switch(activeCommand) {
        case COMMAND_STRING:
            parseStringParams(str + cursor);
            break;
        case COMMAND_KEYS:
            parseKeysParams(str + cursor);
            break;
        default:
            LINE.copyBuffer(str + cursor);
    }

    LINE.serialize(&CRC16);

    quackLines[activeLine].state = QuackLine::State::DONE_PARSING;

    return true;
}

QuackParser::QuackLine*
QuackParser::getProcessedLine() {
    for(u16 i = 0; i < QUACKLINES_BUFFER; i++) {
        if(quackLines[i].lineOrder == nextOrder) {
            if(quackLines[i].state == QuackLine::State::DONE_PARSING) {
                quackLines[i].state = QuackLine::State::SENDING;
                nextOrder++;
                return quackLines + i;
            }
            return nullptr;
        }
    }

    return nullptr;
}

const bool
QuackParser::canParse() {
    for(u16 i = 0; i < QUACKLINES_BUFFER; i++) {
        if(quackLines[i].state == QuackParser::QuackLine::State::FREE_TO_PARSE) {
            return true;
        }
    }

    return false;
}

void
QuackParser::parsingLoop() {
    if(!state) {
        return;
    }
    
    if(state == QuackParser::State::BUFFER_PARSING) {

        u8* str = (u8*) strtok((char*) buffer, LINE_SEPARATOR);

        while(str != NULL && state) {

            while(!canParse()) {}
            parse(str);

            str = (u8*) strtok(NULL, LINE_SEPARATOR);
        }

        state = QuackParser::State::NONE;
    }
    else { // FILE_PARSING
        
        u16 bytesRead;

        while(activeFile.available() && state) {
            bytesRead = activeFile.readBytesUntil('\n', buffer, sizeof(buffer) - 1);
            buffer[bytesRead] = '\0';

            while(!canParse()) {}
            parse(buffer);
        }

        activeFile.close();
        state = QuackParser::State::NONE;
    }
}

void
QuackParser::fillBuffer(const u8* const str) {
    
    strcpy((char*) buffer, (char*) str);
    
    DEBUGGING_PRINTF("Buffer: {\n%s}\n", buffer);

    state = QuackParser::State::BUFFER_PARSING;
}

void
QuackParser::stop() {
    state = QuackParser::State::NONE;
}

void
QuackParser::setFile(const char* const filename) {
    DEBUGGING_PRINTF("Setting file to %s\n", filename);
    activeFile = SPIFFS.open(filename, "r");

    state = QuackParser::State::FILE_PARSING;
}