
#include "quack_parser.h"

#define DECLARE_STR(str) (u8*) str, sizeof(str) - 1

int
main(void) {
    QuackParser quackParser;

    quackParser.parse(DECLARE_STR("&1000\n#abcde\n(10\n%\xb9\n"));

    return 0;
}