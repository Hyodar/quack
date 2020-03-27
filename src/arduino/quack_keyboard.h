
#ifndef QUACK_KEYBOARD_H_
#define QUACK_KEYBOARD_H_

#include "quack_report.h"
#include "quack_hid_locale.h"

#include "usb_hid_keys.h"

class QuackKeyboard {

private:
    QuackReport quackReport;
    QuackHIDLocale* quackHIDLocale;

public:
    QuackKeyboard();

    void setLocale(QuackHIDLocale* _quackHIDLocale);

    void send();
    void release();

    void addHIDKey(const u8 keycode, const u8 modifier = KEY_NONE);
    void addHIDModifier(const u8 modifier);

    void pressKey(const u8 keycode);
    void pressFKey(const u8 fkey_code);
    void pressExtra(const u8 extra_code);
    void pressUTF8(const u32 utf8_char);
    
    void type(const u8 hid_keycode);

    void write(const u8* str, const u16 len);
};

#endif