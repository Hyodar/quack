
#ifndef QUACK_KEYBOARD_H_
#define QUACK_KEYBOARD_H_

/*****************************************************************************
 * quack_keyboard.h:
 *  Keyboard controller class.
 * 
*****************************************************************************/

#include "quack_report.h"
#include "quack_hid_locale.h"

#include "usb_hid_keys.h"

class QuackKeyboard {

private:
    QuackReport quackReport;
    QuackHIDLocale* quackHIDLocale;

public:
    QuackKeyboard();

    void begin() const;

    void setLocale(QuackHIDLocale* _quackHIDLocale);

    void send() const;
    void release();

    void addHIDKey(const u8 keycode, const u8 modifier = KEY_NONE);
    void addHIDModifier(const u8 modifier);

    void pressKey(const u8 keycode);
    void pressFKey(const u8 fkey_code);
    void pressExtra(const u8 extra_code);
    void pressUTF8(const u32 utf8_char);

    void write(const u8* const str, const u16 len);
};

#endif