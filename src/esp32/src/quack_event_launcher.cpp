
#include "quack_event_launcher.h"

#ifdef WEBSERVER_ENABLED
#include "web_info.h"
const char* EVENTSOURCE_PATH = "/events";
#endif

#ifdef WEBSERVER_ENABLED
QuackEventLauncher::QuackEventLauncher() : eventSource{EVENTSOURCE_PATH} {
#else
QuackEventLauncher::QuackEventLauncher() {
#endif
    // no-op
}

#ifdef BLUETOOTH_ENABLED
void
QuackEventLauncher::begin(QuackBluetooth* _bluetooth) {

    bluetooth = _bluetooth;
#else
void
QuackEventLauncher::begin() {
#endif
    // Events ================================================================

#ifdef WEBSERVER_ENABLED
    eventSource.onConnect([](AsyncEventSourceClient* client) {
        client->send("hello!", NULL, millis(), 1000);
    });
#endif
}

void
QuackEventLauncher::launch(const char* const event, const char* const data) {
#if defined(WEBSERVER_ENABLED) && defined(BLUETOOTH_ENABLED)
    if(bluetooth->hasClient()) {
        bluetooth->sendEvent(event, data);
    }
    else {
        eventSource.send(data, event, millis());
    }

#else

#ifdef WEBSERVER_ENABLED
    eventSource.send(data, event, millis());
#endif
#ifdef BLUETOOTH_ENABLED
    if(bluetooth->hasClient()) {
        bluetooth->sendEvent(event, data);
    }
#endif

#endif
}

#ifdef WEBSERVER_ENABLED

AsyncEventSource*
QuackEventLauncher::getEventSource() {
    return &eventSource;
}

#endif