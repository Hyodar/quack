
#include "quack_serial.h"
#include "quack.h"

#define DECLARE_STR(str) (u8*) str, sizeof(str) - 1

Quack quack;

void
setup() {
    quack.begin();
}

void
loop() {
    quack.run();
}

int
main(void) {

    setup();

    Serial1.write(DECLARE_STR("&1000\n#abcde\n%a\n"));

    for(;;) {
        loop();
    }

    return 0;
}