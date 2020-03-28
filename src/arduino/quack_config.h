
#ifndef CONFIG_H_
#define CONFIG_H_

/*****************************************************************************
 * quack_config.h:
 *  Shortcut file to set configurations across multiple files through defines.
 * 
*****************************************************************************/

// #define HID_ENABLED
// #define ARDUINO_ENABLED
// #define SERIAL_ENABLED

#define TESTING_WITHOUT_KEYBOARD
#define PARSER_DEBUGGING
#define KEYBOARD_DEBUGGING
#define INTERFACE_DEBUGGING
#define STRING_HELPERS
#define USEFUL_REMINDERS

#define BAUDRATE      9600
#define BUFFER_SIZE   (384 * 2)
#define DEFAULT_DELAY 1000

#endif