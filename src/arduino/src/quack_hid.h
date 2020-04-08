
#ifndef QUACK_HID_H_
#define QUACK_HID_H_

/*****************************************************************************
 * quack_hid.h:
 *  Mock class of HID.h library.
 * 
*****************************************************************************/

#include <quack_utils.h>
#include <quack_config.h>

#ifdef HID_ENABLED

#include <HID.h>

#else

#define u16 __u16
#include <Arduino.h>
#undef u16

#include <quack_utils.h>

struct HIDSubDescriptor {
    HIDSubDescriptor(const u8* keyboardDescriptor, u32 len) {}
};

struct HID {
    void AppendDescriptor(HIDSubDescriptor* h) { 
        DEBUGGING_PRINT(F("[HID] Appending descriptor.\n"));
    }

    void SendReport(u8 id, const void* data, u32 len) {
        DEBUGGING_PRINT(F("[HID] Sending report.\n"));
    }
};

#endif

#endif