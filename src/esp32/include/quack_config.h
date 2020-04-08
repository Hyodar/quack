
#ifndef CONFIG_H_
#define CONFIG_H_

/*****************************************************************************
 * quack_config.h:
 *  Shortcut file to set configurations across multiple files through defines.
 * 
*****************************************************************************/

// #define HID_ENABLED
#define ARDUINO_ENABLED
#define SERIAL_ENABLED
// #define DISPLAY_ENABLED
#define ESP_ENABLED

// #define TESTING_WITHOUT_KEYBOARD
// #define PARSER_DEBUGGING
// #define KEYBOARD_DEBUGGING
// #define INTERFACE_DEBUGGING
#define FRAME_DEBUGGING
// #define QUACK_DEBUGGING
// #define STRING_HELPERS
// #define USEFUL_REMINDERS

#define SERIAL_BAUDRATE     115200
#define SERIAL2_BAUDRATE    9600
#define BUFFER_SIZE         480
#define QUACKLINES_BUFFER   2

#endif