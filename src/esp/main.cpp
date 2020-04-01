
#include "quack_config.h"
#include "quack_utils.h"

#include <thread>
#include <cstdio>

// #include "quack_serial.h"
// #include "quack.h"

// #include "fastcrc/FastCRC.h"

// #define DECLARE_STR(str) (u8*) str, sizeof(str) - 1

// Quack quack;

void
setup() {
    // quack.begin();
}

void
loop1() {
    // quack.runInterface();
}

void
loop2() {
    // quack.runParser();
}

void
interface() {
    printf("[THREADING] Initializing interface thread...\n");
    for(;;) {
        loop1();
    }
}

void
parser() {
    printf("[THREADING] Initializing parser thread...\n");
    for(;;) {
        loop1();
    }
}

int
main(void) {

    setup();

    std::thread interfaceThread(interface);
    std::thread parserThread(parser);

    interfaceThread.join();
    parserThread.join();

    return 0;
}