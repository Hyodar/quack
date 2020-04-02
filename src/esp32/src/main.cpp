#include <Arduino.h>

#include "quack_parser.h"

#define DECLARE_STR(str) (u8*) str, sizeof(str) - 1

QuackParser quackParser;

void setup() {
    quackParser.parse(DECLARE_STR("STRING abcde"));
}

void loop() {
    // put your main code here, to run repeatedly:
}