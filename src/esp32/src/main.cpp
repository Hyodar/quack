#include <Arduino.h>

#include "quack.h"

#define DECLARE_STR(str) (u8*) str, sizeof(str) - 1

Quack quack;

void loop2(void* params) {
    Serial.println("[THREADING] Initializing loop2");
    for(;;) {
        quack.runInterface();
        delay(1); // watchdog
    }
}

void setup() {
    // quackParser.parse(DECLARE_STR("STRING abcde"));
    quack.begin();

    // create new task in FreeRTOS API
    xTaskCreatePinnedToCore(loop2, "loop2", 8192, NULL, 1, NULL, 0);
}

void loop() {
    quack.runParser();
}