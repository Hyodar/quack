
#include "command_manager.h"

#include "quack_codes.h"

CommandManager::CommandManager(): quackKeyboard{},
                                  currentDefaultDelay{DEFAULT_DELAY},
                                  repeatNum{0} {
    // no-op
}

void
CommandManager::begin() {
    quackKeyboard.begin();
    // display moved to ESP
    // quackDisplay.begin();
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
    const bool isContinuation = (commandCode & COMMAND_CONTINUE_F);
    
    switch(commandCode & COMMAND_CODE_MASK) {
        case COMMAND_DELAY:
            doDelay(param);
            REPEAT_IF_NECESSARY(doDelay(param))
            break;
        case COMMAND_DEFAULTDELAY:
            defaultDelay(param);
            repeatNum = 0;
            break;
        case COMMAND_REPEAT:
            repeat(param);
            break;
        default:
            keycode(param);
            REPEAT_IF_NECESSARY(keycode(param))
    }

    if(!isContinuation) doDefaultDelay();
}

void
CommandManager::command(const u8 commandCode, const u8* const param, const u16 len) {
    const bool isContinuation = (commandCode & COMMAND_CONTINUE_F);

    switch(commandCode & COMMAND_CODE_MASK) {
        case COMMAND_LOCALE:
            locale(param, len);
            repeatNum = 0;
            break;
        case COMMAND_STRING:
            string(param, len);
            REPEAT_IF_NECESSARY(string(param, len))
            break;
        default:
            keys(param, len);
            REPEAT_IF_NECESSARY(keys(param, len))    
    }

    if(!isContinuation) doDefaultDelay();
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
CommandManager::doDelay(const u32 param) const {
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
