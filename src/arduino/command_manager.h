
#ifndef COMMAND_MANAGER_H_
#define COMMAND_MANAGER_H_

#include "config.h"
#include "utils.h"

class CommandManager {

public:
    CommandManager(): currentDefaultDelay{DEFAULT_DELAY}, repeatNum{0} {}

private:
    u32 currentDefaultDelay;
    u32 repeatNum;

    void doDefaultDelay();

    void locale(const u8* param, const u16 len) const;
    void string(const u8* param, const u16 len) const;
    void display(const u8* param, const u16 len) const;
    void keys(const u8* param, const u16 len) const;

    void delay(const u32 param) const;
    void defaultDelay(const u32 param);
    void repeat(const u32 param);
    void keycode(const u32 param) const;

public:
    void command(u8 commandCode, u32 param);
    void command(u8 commandCode, const u8* param, const u16 len);

};

#endif