
#ifndef QUACK_CODES_H_
#define QUACK_CODES_H_

/*****************************************************************************
 * quack_codes.h:
 *  Numeric codes for some definitions throughout the code.
 * 
*****************************************************************************/

// Communication Protocol ========

#define WRONG_CHECKSUM         0x01
#define FINISHED_PARSING       0x02
#define FRAME_SEPARATOR        0x00

// Commands ======================

// REM is not here because it's removed on the JS parser

#define COMMAND_NONE            0x21

// Params: string/bytes(1)
#define COMMAND_DISPLAY         0x22
#define COMMAND_LOCALE          0x23
#define COMMAND_STRING          0x24
#define COMMAND_KEYS            0x25

// Params: number(1)
#define COMMAND_DELAY           0x27
#define COMMAND_DEFAULTDELAY    0x28
#define COMMAND_REPEAT          0x29
#define COMMAND_KEYCODE         0x2a

// Flags
#define COMMAND_CONTINUE_F      (1 << 7)
#define COMMAND_CODE_MASK       0b01111111

#define IS_COMMAND_PARAM_INT(comm) ((comm) & COMMAND_CODE_MASK) >= COMMAND_DELAY

// Locales ========================

#define LOCALE_US               0x2b
#define LOCALE_BR               0x2c

// HID ============================

#define HID_SEND_ID             2

#endif