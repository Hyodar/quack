
#include "command_manager.h"

#include "quack_codes.h"
#include "quack_config.h"

// little hack here because of u16 typedef inside USBAPI.h
#define u16 __u16
#include <Arduino.h>
#undef u16
#include <unistd.h>

#include "quack_utils.h"

CommandManager::CommandManager(): quackKeyboard{}, quackDisplay{},
                                  currentDefaultDelay{DEFAULT_DELAY},
                                  repeatNum{0} {
    // no-op
}

void
CommandManager::begin() {
    quackKeyboard.begin();
    quackDisplay.begin();
}

void
CommandManager::doDefaultDelay() {
#ifdef COMMAND_DEBUGGING
    DEBUGGING_PRINT(F("[COMMANDS] Default delaying for "));
    DEBUGGING_PRINT(currentDefaultDelay);
    DEBUGGING_PRINT(F(" ms.\n"));
#endif
    delay(currentDefaultDelay);
}

#define REPEAT_IF_NECESSARY(func)\
if(repeatNum) {\
    for(; repeatNum > 1; repeatNum--) {\
        doDefaultDelay();\
        func;\
    }\
}\

void
CommandManager::command(const u8 commandCode, const u32 param) {
    if(commandCode == COMMAND_DELAY) {
        delay(param);

        REPEAT_IF_NECESSARY(delay(param))
    }
    else if(commandCode == COMMAND_DEFAULTDELAY) {
        defaultDelay(param);

        // no need to repeat this
        repeatNum = 0;

        // no need to defaultDelay after this
        return;
    }
    else if(commandCode == COMMAND_REPEAT) {
        repeat(param);
        
        // no need to repeat this

        // no need to defaultDelay after this
        return;
    }
    else {
        keycode(param);

        REPEAT_IF_NECESSARY(keycode(param))
    }

    doDefaultDelay();
}

void
CommandManager::command(const u8 commandCode, const u8* const param, const u16 len) {
    if(commandCode == COMMAND_LOCALE) {
        locale(param, len);

        // no need to repeat this

        repeatNum = 0;

        // no need to defaultDelay this
        return;
    }
    else if(commandCode == COMMAND_STRING) {
        string(param, len);

        REPEAT_IF_NECESSARY(string(param, len))
    }
    else if(commandCode == COMMAND_DISPLAY) {
        display(param, len);

        REPEAT_IF_NECESSARY(display(param, len))
    }
    else {
        keys(param, len);

        REPEAT_IF_NECESSARY(keys(param, len))
    }

    doDefaultDelay();
}

void
CommandManager::locale(const u8* const param, const u16 len) const {
#ifdef COMMAND_DEBUGGING
    DEBUGGING_PRINT(F("[COMMANDS] Setting locale to: "));
    DEBUGGING_PRINT(param[0]);
    DEBUGGING_PRINT(param[1]);
    DEBUGGING_PRINT(F(".\n"));
#endif
}

void
CommandManager::string(const u8* const param, const u16 len) {
#ifdef COMMAND_DEBUGGING
    DEBUGGING_PRINT(F("[COMMANDS] Typing string: "));
    DEBUGGING_PRINTSTR(param, len);
    DEBUGGING_PRINT('\n');
#endif
    quackKeyboard.write(param, len);
}

void
CommandManager::display(const u8* const param, const u16 len) {
#ifdef COMMAND_DEBUGGING
    DEBUGGING_PRINT(F("[COMMANDS] Displaying string: "));
    DEBUGGING_PRINTSTR(param, len);
    DEBUGGING_PRINT('\n');
#endif
    quackDisplay.write(param, len);
}

void
CommandManager::keys(const u8* const param, const u16 len) {
#ifdef COMMAND_DEBUGGING
    DEBUGGING_PRINT(F("[COMMANDS] Pressing keys: "));
#endif

    for(u16 i = 0; i < len; i++) {
#ifdef COMMAND_DEBUGGING
    DEBUGGING_PRINT(param[i], HEX);
#endif
        if(param[i] >= KEY_LEFTCTRL) {
            if(param[i] <= KEY_LEFTMETA) {
                quackKeyboard.addHIDModifier(param[i]);
                continue;
            }
        }
        quackKeyboard.addHIDKey(param[i]);
    }

    quackKeyboard.release();
}

void
CommandManager::delay(const u32 param) const {
#ifdef COMMAND_DEBUGGING
    DEBUGGING_PRINT(F("[COMMANDS] Delaying for "));
    DEBUGGING_PRINT(param);
    DEBUGGING_PRINT(F(" ms.\n"));
#endif
    delay(param);
}

void
CommandManager::defaultDelay(const u32 param) {
#ifdef COMMAND_DEBUGGING
    DEBUGGING_PRINT(F("[COMMANDS] Setting delay to "));
    DEBUGGING_PRINT(param);
    DEBUGGING_PRINT(F(" ms.\n"));
#endif
    currentDefaultDelay = param;
}

void
CommandManager::repeat(const u32 param) {
#ifdef COMMAND_DEBUGGING
    DEBUGGING_PRINT(F("[COMMANDS] Will repeat next command for "));
    DEBUGGING_PRINT(param);
    DEBUGGING_PRINT(F(" times.\n"));
#endif
    repeatNum = param;
}

void
CommandManager::keycode(const u32 param) const {
#ifdef COMMAND_DEBUGGING
    DEBUGGING_PRINT(F("[COMMANDS] Typing keycode "));
    DEBUGGING_PRINT(param, HEX);
    DEBUGGING_PRINT(F(".\n"));
#endif
}
