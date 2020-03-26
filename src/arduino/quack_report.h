
#ifndef QUACK_REPORT_H_
#define QUACK_REPORT_H_

/*****************************************************************************
 * quack_report.h:
 *  Generic USB report.
 * 
*****************************************************************************/

#include "quack_utils.h"
#include "usb_hid_keys.h"

struct QuackReport {
    u8 modifiers;
    u8 reserved;
    u8 keys[6];

    QuackReport() : modifiers{KEY_NONE}, reserved{KEY_NONE},
                    keys{{KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE}} {}
    
    void clear() {
        modifiers = KEY_NONE;
        reserved = KEY_NONE;
        
        keys[0] = KEY_NONE;
        keys[1] = KEY_NONE;
        keys[2] = KEY_NONE;
        keys[3] = KEY_NONE;
        keys[4] = KEY_NONE;
        keys[5] = KEY_NONE;
    }
};

#endif