
#include "quack_event_launcher.h"

#include <ArduinoOTA.h>

#include "web_info.h"

const char* EVENTSOURCE_PATH = "/events";
const char* HOSTNAME = "ESP32";

QuackEventLauncher::QuackEventLauncher() : eventSource{EVENTSOURCE_PATH} {
    // no-op
}

void
QuackEventLauncher::begin() {
    // ArduinoOTA ============================================================

    ArduinoOTA.onStart([this] {
        eventSource.send("AOTA_UPDATE_START", "ota");
    });

    ArduinoOTA.onEnd([this] {
        eventSource.send("AOTA_UPDATE_END", "ota");
    });

    ArduinoOTA.onProgress([this] (unsigned int progress, unsigned int total) {
        char p[32];
        sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
        eventSource.send(p, "ota");
    });

    ArduinoOTA.onError([this](ota_error_t error) {
        switch(error) {
            case OTA_AUTH_ERROR: 
                eventSource.send("Auth Failed", "ota");
                break;
            case OTA_BEGIN_ERROR:
                eventSource.send("Begin Failed", "ota");
                break;
            case OTA_CONNECT_ERROR: 
                eventSource.send("Connect Failed", "ota");
                break;
            case OTA_RECEIVE_ERROR: 
                eventSource.send("Receive Failed", "ota");
                break;
            case OTA_END_ERROR: 
                eventSource.send("End Failed", "ota");
                break;
        }
    });

    ArduinoOTA.setHostname(NW_HOSTNAME);
    ArduinoOTA.begin();

    // Events ================================================================

    eventSource.onConnect([](AsyncEventSourceClient* client) {
        client->send("hello!", NULL, millis(), 1000);
    });
}

void
QuackEventLauncher::launch(const char* event, const char* data) {
    eventSource.send(data, event, millis());
}

void
QuackEventLauncher::handleOTA() {
    ArduinoOTA.handle();
}

AsyncEventSource*
QuackEventLauncher::getEventSource() {
    return &eventSource;
}