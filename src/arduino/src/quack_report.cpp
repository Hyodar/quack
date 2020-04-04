
#include "quack_report.h"

QuackReport::QuackReport() : modifiers{KEY_NONE}, reserved{KEY_NONE},
                             keys{KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE, KEY_NONE} {
    // no-op
}

void
QuackReport::clear() {
    modifiers = KEY_NONE;
    reserved = KEY_NONE;
    
    keys[0] = KEY_NONE;
    keys[1] = KEY_NONE;
    keys[2] = KEY_NONE;
    keys[3] = KEY_NONE;
    keys[4] = KEY_NONE;
    keys[5] = KEY_NONE;
}