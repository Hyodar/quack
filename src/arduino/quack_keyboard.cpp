
#include "quack_config.h"
#include "quack_utils.h"
#include "quack_codes.h"
#include "quack_hid.h"

#include "locale_us.h"

#include "quack_keyboard.h"

#ifdef KEYBOARD_DEBUGGING
#include <cstdio>
#endif

#ifdef ARDUINO_ENABLED
const u8 keyboardDescriptor[] PROGMEM {
#else
const u8 keyboardDescriptor[] {
#endif
    //  Keyboard
    0x05, 0x01, //   USAGE_PAGE (Generic Desktop)  // 47
    0x09, 0x06, //   USAGE (Keyboard)
    0xa1, 0x01, //   COLLECTION (Application)
    0x85, 0x02, //   REPORT_ID (2)
    0x05, 0x07, //   USAGE_PAGE (Keyboard)

    0x19, 0xe0, //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7, //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00, //   LOGICAL_MINIMUM (0)
    0x25, 0x01, //   LOGICAL_MAXIMUM (1)
    0x75, 0x01, //   REPORT_SIZE (1)

    0x95, 0x08, //   REPORT_COUNT (8)
    0x81, 0x02, //   INPUT (Data,Var,Abs)
    0x95, 0x01, //   REPORT_COUNT (1)
    0x75, 0x08, //   REPORT_SIZE (8)
    0x81, 0x03, //   INPUT (Cnst,Var,Abs)

    0x95, 0x06, //   REPORT_COUNT (6)
    0x75, 0x08, //   REPORT_SIZE (8)
    0x15, 0x00, //   LOGICAL_MINIMUM (0)
    0x25, 0x73, //   LOGICAL_MAXIMUM (115)
    0x05, 0x07, //   USAGE_PAGE (Keyboard)

    0x19, 0x00, //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x73, //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00, //   INPUT (Data,Ary,Abs)
    0xc0,       //   END_COLLECTION
};

QuackKeyboard::QuackKeyboard() : quackHIDLocale{&locale_us} {
    static HIDSubDescriptor node(keyboardDescriptor, sizeof(keyboardDescriptor));

    HID().AppendDescriptor(&node);
}

void
QuackKeyboard::setLocale(QuackHIDLocale* _quackHIDLocale) {
    quackHIDLocale = _quackHIDLocale;
}

void
QuackKeyboard::send() {
#ifdef KEYBOARD_DEBUGGING
    printf("[KEYBOARD] Sending report. Report description: {\n");
    printf("\tkeys: [");
    for(u8 i = 0; i < 6; i++) {
        printf("%d, ", quackReport.keys[i]);
    }
    printf("],\n");
    printf("\tmodifiers: [");
    for(u8 i = 0; i < 8; i++) {
        printf("%d, ", (quackReport.modifiers & (1 << i)) >> i);
    }
    printf("]\n}\n");
#endif

    HID().SendReport(HID_SEND_ID, (u8*) &quackReport, sizeof(QuackReport));
}

void
QuackKeyboard::release() {
#ifdef KEYBOARD_DEBUGGING
    printf("[KEYBOARD] Releasing keys.\n");
#endif
    quackReport.clear();
    send();
}

void
QuackKeyboard::addHIDKey(const u8 keycode, const u8 modifier) {
    for(u8 i = 0; i < 6; i++) {
        if(NOT quackReport.keys[i]) {
            quackReport.modifiers |= modifier;
            quackReport.keys[i] = keycode;
            send();
            return;
        }
    }
}

void
QuackKeyboard::addHIDModifier(const u8 keycode) {
    quackReport.modifiers |= keycode;

    send();
}

void
QuackKeyboard::pressKey(const u8 keycode) {
#ifdef KEYBOARD_DEBUGGING
    printf("[KEYBOARD] Pressing key: %d\n", keycode);
#endif
    if(keycode < quackHIDLocale->asciiLen) {
        u8 modifiers = pgm_read_byte(quackHIDLocale->ascii + keycode * 2);
        u8 key = pgm_read_byte(quackHIDLocale->ascii + keycode * 2 + 1);

        addHIDKey(key, modifiers);
    }
}

void
QuackKeyboard::pressFKey(const u8 fkey_code) {
#ifdef KEYBOARD_DEBUGGING
    printf("[KEYBOARD] Pressing F%d", fkey_code);
#endif
    addHIDKey(KEY_F1 + fkey_code - 1);
}

// extra can be either a modifier or a special key, like PRINTSCREEN
void
QuackKeyboard::pressExtra(const u8 extra_code) {
    // modifier
    if(extra_code <= KEYCODE_GUI) {
        addHIDModifier(extra_code - KEYCODE_CTRL + KEY_MOD_LCTRL);
    }
    else {
        if(extra_code <= KEYCODE_SPACE) {
            addHIDKey(extra_code - KEYCODE_ENTER + KEY_ENTER);
        }
        else if(extra_code <= KEYCODE_PAGEUP) {
            addHIDKey(extra_code - KEYCODE_PRINTSCREEN + KEY_SYSRQ);
        }
        else if(extra_code <= KEYCODE_NUMLOCK) {
            addHIDKey(extra_code - KEYCODE_END + KEY_END);
        }
        else {
            if(extra_code == KEYCODE_CAPSLOCK) {
                addHIDKey(KEY_CAPSLOCK);
            }
            else {
                addHIDKey(KEY_PROPS);
            }
        }
    }
#ifdef KEYBOARD_DEBUGGING

#endif
}

void
QuackKeyboard::pressUTF8(const u32 utf8_char) {
#ifdef KEYBOARD_DEBUGGING

#endif
}

