
#ifndef QUACK_EVENT_LAUNCHER_H_
#define QUACK_EVENT_LAUNCHER_H_

#include <quack_config.h>

#ifdef WEBSERVER_ENABLED
#include <ESPAsyncWebServer.h>
#endif
#ifdef BLUETOOTH_ENABLED
#include <quack_bluetooth.h>
#endif

class QuackEventLauncher {

private:
#ifdef WEBSERVER_ENABLED
    AsyncEventSource eventSource;
#endif
#ifdef BLUETOOTH_ENABLED
    QuackBluetooth* bluetooth;
#endif

public:
    QuackEventLauncher();

#ifdef BLUETOOTH_ENABLED
    void begin(QuackBluetooth* _bluetooth);
#else
    void begin();
#endif

    void launch(const char* const event, const char* const data = ":D");

#ifdef WEBSERVER_ENABLED
    AsyncEventSource* getEventSource();
#endif
};

#endif