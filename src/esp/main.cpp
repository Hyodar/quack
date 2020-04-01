
#include "quack_config.h"
#include "quack_utils.h"

#include <thread>
#include <cstdio>

// #include "quack_serial.h"
// #include "quack.h"

// #include "fastcrc/FastCRC.h"

#define DECLARE_STR(str) (u8*) str, sizeof(str) - 1

// Quack quack;

#include "quack_parser.h"

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

    QuackParser quackParser;

    quackParser.parse(DECLARE_STR("STRING abcde"));
    const QuackParser::QuackFrame* const frame = quackParser.getProcessedLine();
    printf("Resulting frame: {\n");
    printf("\tChecksum: %d\n", frame->checksum);
    printf("\tCommandCode: %d\n", frame->commandCode);
    printf("\tLength: %d\n", frame->length);
    printf("\tParams: ");
    for(u16 i = 0; i < frame->length; i++) printf("%d, ", frame->params[i]);
    printf("\n}\n");

    std::thread interfaceThread(interface);
    std::thread parserThread(parser);

    interfaceThread.join();
    parserThread.join();

    return 0;
}