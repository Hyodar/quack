
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
                             activeFile{}, quackDisplay{nullptr} {
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
    if(len == 1) {
        replaceKey(*str);
        return;
    }

    else if(len == 3)  {
        if(str[0] == 'A') LINE.addParameterByte(KEY_LEFTALT);
        else if(str[0] == 'E') LINE.addParameterByte(KEY_END);
        else if(str[0] == 'G') LINE.addParameterByte(KEY_LEFTMETA);
        else LINE.addParameterByte(KEY_TAB);
    }

    else if(len == 4) {
        if(str[0] == 'C') LINE.addParameterByte(KEY_LEFTCTRL);
        else if(str[0] == 'H') LINE.addParameterByte(KEY_HOME);
        else LINE.addParameterByte(KEY_PROPS);
    }

    else if(len == 5) {
        if(str[0] == 'E') LINE.addParameterByte(KEY_ENTER);
        else if(str[0] == 'P') LINE.addParameterByte(KEY_PAUSE);
        else if(str[0] == 'S' && str[1] == 'H') LINE.addParameterByte(KEY_LEFTSHIFT);
        else LINE.addParameterByte(KEY_SPACE);
    }

    else if(len == 6) {
        if(str[0] == 'D') LINE.addParameterByte(KEY_BACKSPACE);
        else if(str[0] == 'E') LINE.addParameterByte(KEY_ESC);
        else if(str[0] == 'I') LINE.addParameterByte(KEY_INSERT);
        else LINE.addParameterByte(KEY_PAGEUP);
    }

    else if(len == 7) {
        if(str[0] == 'N') LINE.addParameterByte(KEY_NUMLOCK);
        else LINE.addParameterByte(KEY_UP);
    }

    else if(len == 8) {
        if(str[0] == 'C') LINE.addParameterByte(KEY_CAPSLOCK);
        else LINE.addParameterByte(KEY_PAGEDOWN);
    }

    else if(len == 9) {
        if(str[0] == 'L') LINE.addParameterByte(KEY_LEFT);
        else LINE.addParameterByte(KEY_DOWN);
    }

    else if(len >= 10) {
        if(str[0] == 'R') LINE.addParameterByte(KEY_RIGHT);
        else if(str[0] == 'S') LINE.addParameterByte(KEY_SCROLLLOCK);
        else LINE.addParameterByte(KEY_SYSRQ);
    }

    else if(str[0] == 'F') {
        if(len == 2) {
            LINE.addParameterByte(KEY_F1 + (str[1] - '1'));
        }
        else {
            LINE.addParameterByte(KEY_F10 + (str[2] - '0'));
        }
    }
}

void
QuackParser::parseKeysParams(const u8* const str) {
    // replace keys by their HID combinations and return
    // the new length

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
    // replace keys by their HID combinations and return
    // the new length

    for(u16 i = 0; str[i]; i++) {
        replaceKey(str[i]);
    }

}

const u16
QuackParser::getCommandCode(const u8* const str, const u8 len, const bool continuation) const {
    // tried to parse based on usage frequency here,
    // but could probably use some improvements
    
    if(len == 6) {
        // ~LOCALE~, STRING, REPEAT
        if(str[0] == 'S') {
            if(continuation) return COMMAND_CONTINUESTRING;
            return COMMAND_STRING;
        }
        //else if(str[0] == 'R') {
            return COMMAND_REPEAT;
        //}
        //return COMMAND_LOCALE;
    }
    
    if(len == 5) {
        return COMMAND_DELAY;
    }
    
    if(len == 7) {
        if(str[0] == 'D') {
            return COMMAND_DISPLAY;
        }
        return COMMAND_KEYCODE;
    }

    if(str[0] == 'K') {
        return COMMAND_KEYS;
    }

    if(str[0] == 'D') {
        return COMMAND_DEFAULTDELAY;
    }

    return COMMAND_NONE;
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
QuackParser::parse(const u8* const str, const bool continuation) {
    if(!updateActiveLine()) {
        return false;
    }

    LINE.reset();

    for(u16 i = 0; str[i]; i++) DEBUGGING_PRINTF("%c", str[i]);
    DEBUGGING_PRINTF("\n");

    // structure: <COMMAND_NAME> <PARAMS>
    u16 cursor = 0;

    while(str[cursor] != ' ') cursor++;

    u8 commandCode = getCommandCode(str, cursor, continuation);
    cursor++; // skip space char

    if(commandCode <= COMMAND_DISPLAY) {
        if(commandCode == COMMAND_DISPLAY) {
            quackDisplay->write(str + cursor);
            quackLines[activeLine].state = QuackLine::State::FREE_TO_PARSE;
            return true;
        }
        else { // COMMAND_NONE
            quackLines[activeLine].state = QuackLine::State::FREE_TO_PARSE;
            return true;
        }
    }

    quackLines[activeLine].lineOrder = orderCount++;
    LINE.setCommandCode(commandCode);
    
    if(commandCode == COMMAND_STRING) {
        parseStringParams(str + cursor);
    }
    else if(commandCode == COMMAND_KEYS) {
        parseKeysParams(str + cursor);
    }
    else {
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