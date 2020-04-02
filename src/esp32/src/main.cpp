#include <Arduino.h>

#include "quack.h"
#include "quack_webserver.h"

#define DECLARE_STR(str) (u8*) str, sizeof(str) - 1

Quack quack;
QuackWebserver quackWebserver;

void loop2(void* params) {
    Serial.println("[THREADING] Initializing loop2");
    for(;;) {
        //quack.runInterface();
        quackWebserver.loop();
        delay(1); // watchdog
    }
}

void setup() {
    // quackParser.parse(DECLARE_STR("STRING abcde"));
    quack.begin();
    quackWebserver.begin();

    // create new task in FreeRTOS API
    xTaskCreatePinnedToCore(loop2, "loop2", 8192, NULL, 1, NULL, 0);
}

void loop() {
    
}