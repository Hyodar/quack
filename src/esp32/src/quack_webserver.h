
#ifndef QUACK_WEBSERVER_H_
#define QUACK_WEBSERVER_H_

#include <ESPAsyncWebServer.h>

class QuackWebserver {

private:
    AsyncWebServer server;
    AsyncWebSocket ws;
    AsyncEventSource events;
    
    static void onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                          AwsEventType type, void * arg, uint8_t *data, size_t len);

public:
    QuackWebserver();

    void begin();

    void loop();

    void reply(AsyncWebServerRequest* request, const int code, const char* const type,
               const uint8_t* const data, const size_t len);

};

#endif