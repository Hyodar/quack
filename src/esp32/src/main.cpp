#include <Arduino.h>

#include "quack.h"
#ifdef WEBSERVER_ENABLED
#include "quack_webserver.h"
#endif

#define DECLARE_STR(str) (u8*) str, sizeof(str) - 1

Quack quack;
#ifdef WEBSERVER_ENABLED
QuackWebserver quackWebserver;
#endif

void
loop2(void* params) {
    DEBUGGING_PRINTF("[THREADING] Initializing loop2.\n");
    for(;;) {
        quack.runInterface();
        delay(1); // watchdog
    }
}

void
loop3(void* params) {
    DEBUGGING_PRINTF("[THREADING] Initializing loop3.\n");
    for(;;) {
        quack.runDisplay();
        delay(7);
    }
}

#ifdef BLUETOOTH_ENABLED
void
loop4(void* params) {
    DEBUGGING_PRINTF("[THREADING] Initializing loop4.\n");

    while(!quack.checkBluetooth()) {
        delay(10);
    }

    for(;;) {
        quack.runBluetooth();
        delay(1);
    }
}
#endif

void
setup() {
    SPIFFS.begin();
    quack.begin();
#ifdef WEBSERVER_ENABLED
    quackWebserver.beginWifi();
#endif
#ifdef WEBSERVER_ENABLED
    quackWebserver.begin(quack.getParser(), quack.getEventLauncher());
#endif

    // create new tasks in FreeRTOS API
    xTaskCreatePinnedToCore(loop2, "loop2", 2000, NULL, 1, NULL, 0);
    xTaskCreatePinnedToCore(loop3, "loop3", 2000, NULL, 1, NULL, 1);
#ifdef BLUETOOTH_ENABLED
    xTaskCreatePinnedToCore(loop4, "loop4", 5000, NULL, 1, NULL, 0);
#endif
}

void
loop() {
    quack.runParser();
}