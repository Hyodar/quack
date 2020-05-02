
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

    eventSource.onConnect([](AsyncEventSourceClient* client) {
        client->send("hello!", NULL, millis(), 1000);
    });
}

void
QuackEventLauncher::launch(const char* const event, const char* const data) {
#ifdef WEBSERVER_ENABLED
    eventSource.send(data, event, millis());
#endif
#ifdef BLUETOOTH_ENABLED
    if(bluetooth->getIsEnabled()) {
        bluetooth->sendEvent(event, data);
    }
#endif
}

AsyncEventSource*
QuackEventLauncher::getEventSource() {
    return &eventSource;
}