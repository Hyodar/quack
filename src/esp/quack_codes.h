
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

// UTF8_AHEAD is used to mark that the next character is a utf8 character,
// so it'll read the next 4 bytes as a utf8 character and not as 4 charac-
// ters

#define KEYCODE_UTF8_AHEAD      0xb8

#define KEYCODE_CTRL            0xb9
#define KEYCODE_SHIFT           0xba
#define KEYCODE_ALT             0xbb
#define KEYCODE_GUI             0xbc

#define KEYCODE_ENTER           0xbd
#define KEYCODE_MENU            0xbe
#define KEYCODE_DELETE          0xbf
#define KEYCODE_HOME            0xc0
#define KEYCODE_INSERT          0xc1
#define KEYCODE_PAGEUP          0xc2
#define KEYCODE_PAGEDOWN        0xc3
#define KEYCODE_UPARROW         0xc4
#define KEYCODE_DOWNARROW       0xc5
#define KEYCODE_LEFTARROW       0xc6
#define KEYCODE_RIGHTARROW      0xc7
#define KEYCODE_TAB             0xc8
#define KEYCODE_END             0xc9
#define KEYCODE_ESCAPE          0xca
#define KEYCODE_F1              0xcb
#define KEYCODE_F2              0xcc
#define KEYCODE_F3              0xcd
#define KEYCODE_F4              0xce
#define KEYCODE_F5              0xcf
#define KEYCODE_F6              0xd0
#define KEYCODE_F7              0xd1
#define KEYCODE_F8              0xd2
#define KEYCODE_F9              0xd3
#define KEYCODE_F10             0xd4
#define KEYCODE_F11             0xd5
#define KEYCODE_F12             0xd6
#define KEYCODE_SPACE           0xd7
#define KEYCODE_PAUSE           0xd8
#define KEYCODE_CAPSLOCK        0xd9
#define KEYCODE_NUMLOCK         0xda
#define KEYCODE_PRINTSCREEN     0xdb
#define KEYCODE_SCROLLLOCK      0xdc

// Locales ========================

#define LOCALE_US               105
#define LOCALE_BR               106

// HID ============================

#define HID_SEND_ID             2

#endif