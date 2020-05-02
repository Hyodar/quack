
#ifndef QUACK_EVENT_LAUNCHER_H_
#define QUACK_EVENT_LAUNCHER_H_

#include <quack_config.h>

#ifdef WEBSERVER_ENABLED
#include <ESPAsyncWebServer.h>
#else
class AsyncEventSource;
#endif

class QuackEventLauncher {

#ifdef WEBSERVER_ENABLED
private:
    AsyncEventSource eventSource;

public:
    QuackEventLauncher();

    void begin();
    void launch(const char* event, const char* data = ":D");

    AsyncEventSource* getEventSource();
#else
public:
    QuackEventLauncher() {}

    void begin() {}
    void handleOTA() {}
    void launch(const char* event, const char* data = ":D") {}
    AsyncEventSource* getEventSource() { return nullptr; }
#endif
};

#endif