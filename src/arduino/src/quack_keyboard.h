
#ifndef QUACK_KEYBOARD_H_
#define QUACK_KEYBOARD_H_

/*****************************************************************************
 * quack_keyboard.h:
 *  Keyboard controller class.
 * 
*****************************************************************************/

#include "quack_report.h"

#include "usb_hid_keys.h"

class QuackKeyboard {

private:
    QuackReport quackReport;

public:
    QuackKeyboard();

    void begin() const;

    void send() const;
    void release();

    void addHIDKey(const u8 keycode, const u8 modifier = KEY_NONE);
    void addHIDModifier(const u8 modifier);

    void write(const u8* const str, const u16 len);
};

#endif