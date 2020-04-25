#include <Arduino.h>

#include "quack.h"
#include "quack_webserver.h"

#define DECLARE_STR(str) (u8*) str, sizeof(str) - 1

Quack quack;
QuackWebserver quackWebserver;

void
loop2(void* params) {
    Serial.println("[THREADING] Initializing loop2");
    for(;;) {
        quack.runInterface();
        delay(1); // watchdog
    }
}

void
loop3(void* params) {
    for(;;) {
        quack.runDisplay();
        delay(7);
    }
}

void
setup() {
    quack.begin();
    quackWebserver.begin(quack.getParser(), quack.getEventLauncher());

    // create new tasks in FreeRTOS API
    xTaskCreatePinnedToCore(loop2, "loop2", 8192, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(loop3, "loop3", 8192, NULL, 1, NULL, 1);
}

void
loop() {
    quack.runParser();
}