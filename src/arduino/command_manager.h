
#ifndef COMMAND_MANAGER_H_
#define COMMAND_MANAGER_H_

/*****************************************************************************
 * command_manager.h:
 *  Simple class to receive and determine what each command does.
 * 
*****************************************************************************/

#include "quack_config.h"
#include "quack_utils.h"
#include "quack_keyboard.h"

class CommandManager {

public:
    CommandManager();

    void begin();

    void command(const u8 commandCode, u32 param);
    void command(const u8 commandCode, const u8* param, const u16 len);

private:
    QuackKeyboard quackKeyboard;

    u32 currentDefaultDelay;
    u32 repeatNum;

    void doDefaultDelay();

    void locale(const u8* param, const u16 len) const;
    void string(const u8* param, const u16 len);
    void display(const u8* param, const u16 len) const;
    void keys(const u8* param, const u16 len);

    void delay(const u32 param) const;
    void defaultDelay(const u32 param);
    void repeat(const u32 param);
    void keycode(const u32 param) const;

};

#endif