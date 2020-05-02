
#include "quack_event_launcher.h"

#ifdef WEBSERVER_ENABLED

#include "web_info.h"

const char* EVENTSOURCE_PATH = "/events";
const char* HOSTNAME = "ESP32";

QuackEventLauncher::QuackEventLauncher() : eventSource{EVENTSOURCE_PATH} {
    // no-op
}

void
QuackEventLauncher::begin() {
    // Events ================================================================

    eventSource.onConnect([](AsyncEventSourceClient* client) {
        client->send("hello!", NULL, millis(), 1000);
    });
}

void
QuackEventLauncher::launch(const char* event, const char* data) {
    eventSource.send(data, event, millis());
}

AsyncEventSource*
QuackEventLauncher::getEventSource() {
    return &eventSource;
}

#endif