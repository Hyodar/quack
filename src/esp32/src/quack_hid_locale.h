
#ifndef QUACK_HID_LOCALE_H_
#define QUACK_HID_LOCALE_H_

#include <cstddef>
#include <quack_utils.h>

struct QuackHIDLocale {
    const u8* ascii;
    const u8 asciiLen;

    const u8* extendedAscii;
    const u8 extendedAsciiLen;

    const u8* utf8;
    const size_t utf8Len;

    QuackHIDLocale(const u8* _ascii, const u8 _asciiLen, const u8* _extendedAscii,
                   const u8 _extendedAsciiLen, const u8* _utf8, const size_t _utf8Len) 
                   : ascii{_ascii}, asciiLen{_asciiLen}, extendedAscii{_extendedAscii},
                   extendedAsciiLen{_extendedAsciiLen}, utf8{_utf8}, utf8Len{_utf8Len} {}
};

#endif