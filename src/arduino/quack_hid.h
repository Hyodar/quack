
#ifndef QUACK_HID_H_
#define QUACK_HID_H_

/*****************************************************************************
 * quack_hid.h:
 *  Mock class of HID.h library.
 * 
*****************************************************************************/

#include "quack_utils.h"
#include "quack_config.h"

#ifdef HID_ENABLED

#include <HID.h>

#else

#include <cstdio>

struct HIDSubDescriptor {
    HIDSubDescriptor(const u8* keyboardDescriptor, u32 len) {}
};

struct HID {
    void AppendDescriptor(HIDSubDescriptor* h) { 
        printf("[HID] Appending descriptor.\n");
    }

    void SendReport(u8 id, const void* data, u32 len) {
        printf("[HID] Sending report.\n");
    }
};

#endif

#endif