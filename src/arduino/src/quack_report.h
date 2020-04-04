
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

    QuackReport();
    
    void clear();
};

#endif