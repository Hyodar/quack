
#ifndef QUACK_HID_LOCALE_H_
#define QUACK_HID_LOCALE_H_

#include "quack_utils.h"

struct QuackHIDLocale {
    u8* ascii;
    u8 asciiLen;

    u8* extendedAscii;
    u8 extendedAsciiLen;

    u8* utf8;
    size_t utf8_len;
};

#endif