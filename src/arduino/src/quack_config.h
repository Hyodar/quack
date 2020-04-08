
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

#define DEBUGGING_ENABLED

#define COMMAND_DEBUGGING
#define DISPLAY_DEBUGGING
#define FRAME_DEBUGGING
#define INTERFACE_DEBUGGING
#define KEYBOARD_DEBUGGING
#define PARSER_DEBUGGING

#define SERIAL_BAUDRATE         115200
#define SERIAL1_BAUDRATE        9600
#define BUFFER_SIZE             480
#define QUACKLINES_BUFFER       2
#define DEFAULT_DELAY           1000
#define QUACKFRAME_HEADER_SIZE  (2 + 1 + 2)

#endif