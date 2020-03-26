
#ifndef QUACK_KEYBOARD_H_
#define QUACK_KEYBOARD_H_

#include "quack_report.h"
#include "quack_hid_locale.h"

class QuackKeyboard {

private:
    QuackReport quackReport;
    QuackHIDLocale* quackHIDLocale;

public:
    QuackKeyboard();

    void setLocale(QuackHIDLocale* _quackHIDLocale);

    void send();
    void release();

    // press uses the quack keycode pattern
    void press(u8 quack_keycode);
    
    // type uses the hid keycode pattern
    void type(u8 hid_keycode);
    
    void write(u8* str, u16 len);
};

#endif