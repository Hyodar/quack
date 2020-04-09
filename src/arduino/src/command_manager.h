
#ifndef COMMAND_MANAGER_H_
#define COMMAND_MANAGER_H_

/*****************************************************************************
 * command_manager.h:
 *  Simple class to receive and determine what each command does.
 * 
*****************************************************************************/

#include <quack_config.h>

// little hack here because of u16 typedef inside USBAPI.h
#define u16 __u16
#include <Arduino.h>
#undef u16

#include <quack_utils.h>

#include "quack_keyboard.h"

class CommandManager {

private:
    QuackKeyboard quackKeyboard;

    u32 currentDefaultDelay;
    u32 repeatNum;

    void doDefaultDelay();

    void locale(const u8* const param, const u16 len) const;
    void string(const u8* const param, const u16 len);
    void keys(const u8* const param, const u16 len);

    void doDelay(const u32 param) const;
    void defaultDelay(const u32 param);
    void repeat(const u32 param);
    void keycode(const u32 param) const;

public:
    CommandManager();

    void begin();

    void command(const u8 commandCode, const u32 param);
    void command(const u8 commandCode, const u8* const param, const u16 len);

};

#endif