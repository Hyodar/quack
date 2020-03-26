
#include "command_manager.h"

#include "quack_codes.h"
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

void
CommandManager::doDefaultDelay() {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Default delaying for %d ms\n", currentDefaultDelay);
    usleep(currentDefaultDelay * 1000);
#else

#endif
}

void
CommandManager::command(u8 commandCode, const u32 param) {
    if(commandCode == COMMAND_DELAY) {
        delay(param);

        if(repeatNum) {
            for(; repeatNum > 1; repeatNum--) {
                doDefaultDelay();
                delay(param);
            }
        }
    }
    else if(commandCode == COMMAND_DEFAULTDELAY) {
        defaultDelay(param);

        // no need to repeat this
        /*
        if(repeatNum) {
            for(; repeatNum > 1; repeatNum--) {
                doDefaultDelay();
                defaultDelay(param);
            }
        }
        */
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

        if(repeatNum) {
            for(; repeatNum > 1; repeatNum--) {
                doDefaultDelay();
                keycode(param);
            }
        }
    }

    doDefaultDelay();
}

void
CommandManager::command(u8 commandCode, const u8* param, const u16 len) {
    if(commandCode == COMMAND_LOCALE) {
        locale(param, len);

        // no need to repeat this
        /*
        if(repeatNum) {
            for(; repeatNum > 1; repeatNum--) {
                doDefaultDelay();
                locale(param, len);
            }
        }
        */
        repeatNum = 0;

        // no need to defaultDelay this
        return;
    }
    else if(commandCode == COMMAND_STRING) {
        string(param, len);

        if(repeatNum) {
            for(; repeatNum > 1; repeatNum--) {
                doDefaultDelay();
                string(param, len);
            }
        }
    }
    else if(commandCode == COMMAND_DISPLAY) {
        display(param, len);

        if(repeatNum) {
            for(; repeatNum > 1; repeatNum--) {
                doDefaultDelay();
                display(param, len);
            }
        }
    }
    else {
        keys(param, len);

        if(repeatNum) {
            for(; repeatNum > 1; repeatNum--) {
                doDefaultDelay();
                keys(param, len);
            }
        }
    }

    doDefaultDelay();
}

#ifdef TESTING_WITHOUT_KEYBOARD
u8 translateLocale[][3] = {
    "US", "BR",
};
#endif

void
CommandManager::locale(const u8* param, const u16 len) const {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Setting locale to: %s\n", translateLocale[param[0] - LOCALE_US]);
#else

#endif
}

void
CommandManager::string(const u8* param, const u16 len) const {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Typing string: "); printstr(param, len); putchar('\n');
#else

#endif
}

void
CommandManager::display(const u8* param, const u16 len) const {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Displaying string: "); printstr(param, len); putchar('\n');
#else

#endif
}

#ifdef TESTING_WITHOUT_KEYBOARD
u8 translateKey[][12] = {
    "ENTER",        "MENU",         "DELETE",   "HOME",         "INSERT",
    "PAGEUP",       "PAGEDOWN",     "UPARROW",  "DOWNARROW",    "LEFTARROW",
    "RIGHTARROW",   "TAB",          "END",      "ESCAPE",       "F1",
    "F2",           "F3",           "F4",       "F5",           "F6",
    "F7",           "F8",           "F9",       "F10",          "F11",
    "F12",          "SPACE",        "PAUSE",    "CAPSLOCK",     "NUMLOCK",
    "PRINTSCREEN",  "SCROLLLOCK",   "CTRL",     "SHIFT",        "ALT",
    "GUI",
};
#endif

void
CommandManager::keys(const u8* param, const u16 len) const {
#ifdef TESTING_WITHOUT_KEYBOARD
    printf("[COMMANDS] Pressing keys: "); 
    for(u16 i = 0; i < len - 1; i++) {
        printf("%s, ", translateKey[param[i] - KEYCODE_ENTER]);
    }
    printf("%s\n", translateKey[param[len-1] - KEYCODE_ENTER]);
#else

#endif
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
    currentDefaultDelay = param;
#endif
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
