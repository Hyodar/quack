
#ifndef QUACK_CODES_H_
#define QUACK_CODES_H_

/*****************************************************************************
 * quack_codes.h:
 *  Numeric codes for some definitions throughout the code.
 * 
*****************************************************************************/

// Commands ======================

// REM is not here because it's removed on the JS parser

#define COMMAND_NONE            33

// Params: string/bytes(1)
#define COMMAND_LOCALE          34
#define COMMAND_STRING          35
#define COMMAND_DISPLAY         36
#define COMMAND_KEYS            37

// Params: number(1)
#define COMMAND_DELAY           38
#define COMMAND_DEFAULTDELAY    39
#define COMMAND_REPEAT          40
#define COMMAND_KEYCODE         41

#define IS_COMMAND_PARAM_INT(comm) comm >= COMMAND_DELAY

// Keys ==========================

#define BYTES_TO_UINT(byte_ptr) *((u32*) byte_ptr)

// means the next 4 bytes are a utf8 character
#define KEYCODE_UTF8_AHEAD      0x03

// means the next byte is a F key number
#define KEYCODE_F_KEY           0x02

// 0x05 a 0x08
#define KEYCODE_CTRL            0x05
#define KEYCODE_SHIFT           0x06
#define KEYCODE_ALT             0x07
#define KEYCODE_GUI             0x08

#define KEYCODE_ENTER           0x0b
#define KEYCODE_ESCAPE          0x0c
#define KEYCODE_DELETE          0x0d
#define KEYCODE_TAB             0x0e
#define KEYCODE_SPACE           0x0f

#define KEYCODE_PRINTSCREEN     0x10
#define KEYCODE_SCROLLLOCK      0x11
#define KEYCODE_PAUSE           0x12
#define KEYCODE_INSERT          0x13
#define KEYCODE_HOME            0x14
#define KEYCODE_PAGEUP          0x15

#define KEYCODE_END             0x16
#define KEYCODE_PAGEDOWN        0x17
#define KEYCODE_RIGHTARROW      0x18
#define KEYCODE_LEFTARROW       0x19
#define KEYCODE_DOWNARROW       0x1a
#define KEYCODE_UPARROW         0x1b
#define KEYCODE_NUMLOCK         0x1c

#define KEYCODE_CAPSLOCK        0x1d

#define KEYCODE_MENU            0x1e

#define BYTE_TO_FKEY(byte) (byte << 3)
#define FKEY_TO_BYTE(byte) (byte >> 3)

#define KEYCODE_FKEY1             BYTE_TO_FKEY(0x01)
#define KEYCODE_FKEY2             BYTE_TO_FKEY(0x02)
#define KEYCODE_FKEY3             BYTE_TO_FKEY(0x03)
#define KEYCODE_FKEY4             BYTE_TO_FKEY(0x04)
#define KEYCODE_FKEY5             BYTE_TO_FKEY(0x05)
#define KEYCODE_FKEY6             BYTE_TO_FKEY(0x06)
#define KEYCODE_FKEY7             BYTE_TO_FKEY(0x07)
#define KEYCODE_FKEY8             BYTE_TO_FKEY(0x08)
#define KEYCODE_FKEY9             BYTE_TO_FKEY(0x09)
#define KEYCODE_FKEY10            BYTE_TO_FKEY(0x0a)
#define KEYCODE_FKEY11            BYTE_TO_FKEY(0x0b)
#define KEYCODE_FKEY12            BYTE_TO_FKEY(0x0c)
#define KEYCODE_FKEY13            BYTE_TO_FKEY(0x0d)
#define KEYCODE_FKEY14            BYTE_TO_FKEY(0x0e)
#define KEYCODE_FKEY15            BYTE_TO_FKEY(0x0f)
#define KEYCODE_FKEY16            BYTE_TO_FKEY(0x10)
#define KEYCODE_FKEY17            BYTE_TO_FKEY(0x11)
#define KEYCODE_FKEY18            BYTE_TO_FKEY(0x12)
#define KEYCODE_FKEY19            BYTE_TO_FKEY(0x13)
#define KEYCODE_FKEY20            BYTE_TO_FKEY(0x14)
#define KEYCODE_FKEY21            BYTE_TO_FKEY(0x15)
#define KEYCODE_FKEY22            BYTE_TO_FKEY(0x16)
#define KEYCODE_FKEY23            BYTE_TO_FKEY(0x17)
#define KEYCODE_FKEY24            BYTE_TO_FKEY(0x18)

// Locales ========================

#define LOCALE_US               105
#define LOCALE_BR               106

// HID ============================

#define HID_SEND_ID             2

#endif