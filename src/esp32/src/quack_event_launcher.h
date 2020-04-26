
#ifndef QUACK_EVENT_LAUNCHER_H_
#define QUACK_EVENT_LAUNCHER_H_

#include <ESPAsyncWebServer.h>

class QuackEventLauncher {

private:
    AsyncEventSource eventSource;

public:
    QuackEventLauncher();

    void begin();
    void handleOTA();
    void launch(const char* event, const char* data = ":D");

    AsyncEventSource* getEventSource();
};

#endif