
#include "quack_config.h"
#include "quack_utils.h"
#include "quack_hid.h"
#include "quack_keyboard.h"

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

QuackKeyboard::setLocale(QuackHIDLocale* _quackHIDLocale) {
    quackHIDLocale = _quackHIDLocale;
}

void QuackKeyboard::send() {
    HID().SendReport(HID_SEND_ID, (u8*) &quackReport, sizeof(QuackReport));
}

void QuackKeyboard::release() {
    quackReport.clear();
    send();
}

void QuackKeyboard::press(u8 quack_keycode) {
    
}

