
#include "command_manager.h"

#include "quack_codes.h"
#include "usb_hid_keys.h"
#include "quack_config.h"

#ifdef TESTING_WITHOUT_KEYBOARD
#include <cstdio>
#include <unistd.h>
#endif

#ifdef STRING_HELPERS
void
printstr(const u8* str, u16 len) {
    for(u16 i = 0; i < len; i++) putchar(str[i]);
}
#endif

CommandManager::CommandManager(): quackKeyboard{}, quackDisplay{},
                                  currentDefaultDelay{DEFAULT_DELAY},
                                  repeatNum{0}  {
    // no-op
}

void
CommandManager::begin() {
    quackKeyboard.begin();
}

void
CommandManager::doDefaultDelay() {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Default delaying for %d ms\n", currentDefaultDelay);
    usleep(currentDefaultDelay * 1000);
#else

#endif
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

#ifdef TESTING_WITHOUT_KEYBOARD
u8 translateLocale[][3] = {
    "US", "BR",
};
#endif

void
CommandManager::locale(const u8* const param, const u16 len) const {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Setting locale to: %s\n", translateLocale[param[0] - LOCALE_US]);
#else

#endif
}

void
CommandManager::string(const u8* const param, const u16 len) {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Typing string: "); printstr(param, len); putchar('\n');
#else

#endif
    quackKeyboard.write(param, len);
}

void
CommandManager::display(const u8* const param, const u16 len) {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Displaying string: "); printstr(param, len); putchar('\n');
#else
#endif
    quackDisplay.write(param, len);
}

#ifdef TESTING_WITHOUT_KEYBOARD
u8 translateKey[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

u8 translateMod[][13] = { // "UTF8_AHEAD"
    "CTRL",             "SHIFT",        "ALT",          "GUI",              "ENTER",
    "ESCAPE",           "DELETE",       "TAB",          "SPACE",            "CAPSLOCK",
    "MENU",             "HOME",         "INSERT",       "PAGEUP",           "PAGEDOWN",
    "RIGHTARROW",       "LEFTARROW",    "DOWNARROW",    "UPARROW",          "END",
    "SCROLLLOCK",       "PAUSE",        "NUMLOCK",      "PRINTSCREEN",
};
#endif

void
CommandManager::keys(const u8* const param, const u16 len) {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Pressing keys: TODO translation\n");
#endif

    for(u16 i = 0; i < len; i++) {
        if(param[i] >= KEY_LEFTCTRL) {
            if(param[i] <= KEY_LEFTMETA) {
                quackKeyboard.addHIDModifier(param[i]);
                continue;
            }
        }
        quackKeyboard.addHIDKey(param[i]);
        quackKeyboard.send();
    }

    quackKeyboard.release();
}

void
CommandManager::delay(const u32 param) const {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Delaying for %d ms\n", param);
    usleep(param * 1000);
#else

#endif
}

void
CommandManager::defaultDelay(const u32 param) {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Setting default delay to %d ms\n", param);
#else

#endif
    currentDefaultDelay = param;
}

void
CommandManager::repeat(const u32 param) {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Repeating next command for %d times\n", param);
#else

#endif
    repeatNum = param;
}

void
CommandManager::keycode(const u32 param) const {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Typing keycode %d\n", param);
#else

#endif
}
