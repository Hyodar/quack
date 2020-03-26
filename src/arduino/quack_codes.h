
#ifndef QUACK_CODES_H_
#define QUACK_CODES_H_

/*****************************************************************************
 * quack_codes.h:
 *  Numeric codes for some definitions throughout the code.
 * 
*****************************************************************************/

#define LOCALE_US               78
#define LOCALE_BR               79

// Removed on JS parser
// #define COMMAND_REM          33

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

// Keys
#define KEYCODE_ENTER           42
#define KEYCODE_MENU            43
#define KEYCODE_DELETE          44
#define KEYCODE_HOME            45
#define KEYCODE_INSERT          46
#define KEYCODE_PAGEUP          47
#define KEYCODE_PAGEDOWN        48
#define KEYCODE_UPARROW         49
#define KEYCODE_DOWNARROW       50
#define KEYCODE_LEFTARROW       51
#define KEYCODE_RIGHTARROW      52
#define KEYCODE_TAB             53
#define KEYCODE_END             54
#define KEYCODE_ESCAPE          55
#define KEYCODE_F1              56
#define KEYCODE_F2              57
#define KEYCODE_F3              58
#define KEYCODE_F4              59
#define KEYCODE_F5              60
#define KEYCODE_F6              61
#define KEYCODE_F7              62
#define KEYCODE_F8              63
#define KEYCODE_F9              64
#define KEYCODE_F10             65
#define KEYCODE_F11             66
#define KEYCODE_F12             67
#define KEYCODE_SPACE           68
#define KEYCODE_PAUSE           69
#define KEYCODE_CAPSLOCK        70
#define KEYCODE_NUMLOCK         71
#define KEYCODE_PRINTSCREEN     72
#define KEYCODE_SCROLLLOCK      73
#define KEYCODE_CTRL            74
#define KEYCODE_SHIFT           75
#define KEYCODE_ALT             76
#define KEYCODE_GUI             77
#define KEYCODE_A               78
#define KEYCODE_B               79
#define KEYCODE_C               80
#define KEYCODE_D               81
#define KEYCODE_E               82
#define KEYCODE_F               83
#define KEYCODE_G               84
#define KEYCODE_H               85
#define KEYCODE_I               86
#define KEYCODE_J               87
#define KEYCODE_K               88
#define KEYCODE_L               89
#define KEYCODE_M               90
#define KEYCODE_N               91
#define KEYCODE_O               92
#define KEYCODE_P               93
#define KEYCODE_Q               94
#define KEYCODE_R               95
#define KEYCODE_S               96
#define KEYCODE_T               97
#define KEYCODE_U               98
#define KEYCODE_V               99
#define KEYCODE_W               100
#define KEYCODE_X               101
#define KEYCODE_Y               102
#define KEYCODE_Z               103

#endif