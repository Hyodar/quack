
#include "quack_parser.h"

#include "usb_hid_keys.h"
#include "quack_codes.h"
#include "locale_us.h"

#include <cstdio>

QuackParser::QuackLine::QuackLine() : state{QuackLineState::FREE_TO_PARSE}, lineOrder{0} {
    // no-op
}

QuackParser::QuackParser() : activeLine{0}, nextOrder{0}, orderCount{0}, keyboardLocale{&locale_us} {
    // no-op
}

void
QuackParser::begin() {

}

/*
void
QuackParser::changeLocale(const u8* str) {
    // change locale here
    // possible inputs: "US", "BR"?
}
*/

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
            pgm_read_byte(keyboardLocale->extendedAscii + i * 3 + 2);
            u8 modifier = pgm_read_byte(keyboardLocale->extendedAscii + i * 3 + 1);
            if(modifier) LINE.addParameterByte(modifier);
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
        else if(str[0]) LINE.addParameterByte(KEY_HOME);
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
QuackParser::parseKeysParams(const u8* const str, const u16 len) {
    // replace keys by their HID combinations and return
    // the new length

    u16 start = 0;
    u16 length = 0;

    for(u16 i = 0; i < len; i++, length++) {
        if(str[i] == ' ') {
            replaceKeyword(str + start, length);
            
            start = i + 1;
            length = 0;
        }
    }

    if(len) {
        // got to end of line, but didn't parse last key
        replaceKeyword(str + start, len);
    }

}

void
QuackParser::parseStringParams(const u8* const str, const u16 len) {
    // replace keys by their HID combinations and return
    // the new length

    for(u16 i = 0; i < len; i++) {
        replaceKey(str[i]);
    }

}

const u16
QuackParser::getCommandCode(const u8* const str, const u8 len) const {
    // tried to parse based on usage frequency here,
    // but could probably use some improvements
    
    if(len == 6) {
        // LOCALE, STRING, REPEAT
        if(str[0] == 'S') {
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

    return COMMAND_DEFAULTDELAY;

}

const bool
QuackParser::updateActiveLine() {
    for(u8 i = 0; i < QUACKLINES_BUFFER; i++) {
        if(quackLines[i].state == QuackLineState::FREE_TO_PARSE) {
            activeLine = i;
            quackLines[i].state = QuackLineState::PARSING;

            return true;
        }
    }

    return false;
}

const bool
QuackParser::parse(const u8* const str, const u16 len) {
    if(!updateActiveLine()) {
        return false;
    }

    // structure: <COMMAND_NAME> <PARAMS>
    u16 cursor = 0;

    while(str[cursor] != ' ') {
        cursor++;
    }

    u8 commandCode = getCommandCode(str, cursor);
    cursor++; // skip space char

    /*
    if(commandCode == COMMAND_LOCALE) {
        // this one is processed here
        changeLocale(str);
        quackLines[activeLine].state = QuackLineState::FREE_TO_PARSE;
        return;
    }
    */

    quackLines[activeLine].lineOrder = orderCount++;
    LINE.setCommandCode(commandCode);
    
    if(commandCode == COMMAND_STRING) {
        parseStringParams(str + cursor, len - cursor);
    }
    else if(commandCode == COMMAND_KEYS) {
        parseKeysParams(str + cursor, len - cursor);
    }
    else {
        LINE.copyBuffer((str + cursor), (len - cursor));
    }

    LINE.serialize(&CRC16);

    quackLines[activeLine].state = QuackLineState::DONE_PARSING;

    return true;
}

const QuackFrame* const
QuackParser::getProcessedLine() {
    for(u16 i = 0; i < QUACKLINES_BUFFER; i++) {
        if(quackLines[i].lineOrder == nextOrder) {
            if(quackLines[i].state == QuackLineState::DONE_PARSING) {
                quackLines[i].state = QuackLineState::SENDING;
                nextOrder++;
                return &(quackLines[i].data);
            }
            return nullptr;
        }
    }

    return nullptr;
}