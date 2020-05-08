
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
#define DISPLAY_ENABLED
#define ESP_ENABLED

#define BLUETOOTH_ENABLED
#define BLUETOOTH_PASSWORD           "moe moe kyun"
#define BLUETOOTH_MAX_PASSWORD_TRIES 5

// #define WEBSERVER_ENABLED

// #define TESTING_WITHOUT_KEYBOARD
// #define PARSER_DEBUGGING
// #define KEYBOARD_DEBUGGING
// #define INTERFACE_DEBUGGING
#define FRAME_DEBUGGING
#define DISPLAY_DEBUGGING
// #define QUACK_DEBUGGING
// #define STRING_HELPERS
// #define USEFUL_REMINDERS

#define FRAME_PARAM_SIZE        480
#define FRAME_HEADER_SIZE       2 + 1 + 2
#define FRAME_BUFFER_SIZE       (FRAME_HEADER_SIZE + FRAME_PARAM_SIZE)

#define SERIAL_BAUDRATE     115200
#define SERIAL2_BAUDRATE    9600
#define QUACKLINES_BUFFER   2

#endif