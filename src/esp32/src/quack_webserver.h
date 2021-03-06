
#ifndef QUACK_WEBSERVER_H_
#define QUACK_WEBSERVER_H_

#include <quack_config.h>

#ifdef WEBSERVER_ENABLED

#include <ESPAsyncWebServer.h>

class QuackParser;
class QuackEventLauncher;

class QuackWebserver {

private:
    AsyncWebServer server;
    QuackParser* parser;
    
    const bool testMagic(AsyncWebServerRequest* request) const;

public:
    QuackWebserver();

    void begin(QuackParser* _parser, QuackEventLauncher* quackEventLauncher);
    void beginWifi();

    void reply(AsyncWebServerRequest* request, const int code,
               const char* const type, const uint8_t* const data,
               const size_t len) const;

};

#endif

#endif