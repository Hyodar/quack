
#include "quack_webserver.h"

#ifdef WEBSERVER_ENABLED

#include <Arduino.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ESPmDNS.h>
#include <WiFi.h>

#include "web_files.h"
#include "web_info.h"

#include <quack_utils.h>
#include "quack_parser.h"
#include "quack_event_launcher.h"

const char* MAGIC   = "4027432687"; // 0xF00DBEEF
const char* ACK     = "ACK";
const char* NACK    = "NACK";

QuackWebserver::QuackWebserver() : server{80}, parser{nullptr} {
    // no-op
}

void
QuackWebserver::reply(AsyncWebServerRequest* request, int code,
                     const char* type, const uint8_t* data, size_t len) const {
    AsyncWebServerResponse* response = request->beginResponse_P(
        code, type, data, len
    );

    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

const bool
QuackWebserver::testMagic(AsyncWebServerRequest* request) const {
    if(!request->hasParam("Magic", true)) {
        return false;
    }
    else {
        if(request->getParam("Magic", true)->value() != MAGIC) {
            return false;
        }
    }
    return true;
}

void
QuackWebserver::beginWifi() {
    WiFi.mode(WIFI_AP_STA);
    // WiFi.softAP(NW_HOSTNAME);
    WiFi.begin(NW_SSID, NW_PASSWORD);
    
    if(WiFi.waitForConnectResult() != WL_CONNECTED) {
        DEBUGGING_PRINTF("STA: Failed!\n");
        WiFi.disconnect(false);
        delay(1000);
        WiFi.begin(NW_SSID, NW_PASSWORD);
    }

    DEBUGGING_PRINT(WiFi.localIP());
}

void
QuackWebserver::begin(QuackParser* _parser, QuackEventLauncher* quackEventLauncher) {
    
    // Parser ================================================================

    parser = _parser;

    // mDNS ==================================================================

    MDNS.begin(NW_HOSTNAME);
    MDNS.addService("http", "tcp", 80);

    // SPIFFS ================================================================

    // SPIFFS.format();
    // SPIFFS.begin();

    // Server ================================================================

    GET_CALLBACKS;

    server.on("/", [this](AsyncWebServerRequest* request) {
        reply(request, 200, "text/html", INDEX_HTML, sizeof(INDEX_HTML));
    });

    server.onNotFound([this](AsyncWebServerRequest* request) {
        reply(request, 404, "text/html", HTTP404_HTML, sizeof(HTTP404_HTML));
    });

    server.on("/run_raw", HTTP_POST, [this](AsyncWebServerRequest* request) {
        DEBUGGING_PRINTF("Received run raw command\n");
        if(!request->hasParam("Code", true)) {
            request->send(400, "text/plain", NACK);
            return;
        }

        request->send(200, "text/plain", ACK);

        DEBUGGING_PRINTF("Filling buffer\n");
        parser->fillBuffer((const u8* const) request->getParam("Code", true)->value().c_str());
    });

    server.on("/run_file", [this](AsyncWebServerRequest* request) {
        if(!request->hasParam("Filename", true)) {
            request->send(400, "text/plain", NACK);
            return;
        }
        
        parser->setFile((const char* const) request->getParam("Filename", true)->value().c_str());
        request->send(200, "text/plain", ACK);
    });

    server.on("/stop", [this](AsyncWebServerRequest* request) {
        if(!testMagic(request)) {
            request->send(400, "text/plain", NACK);
            return;
        }

        parser->stop();
        request->send(200, "text/plain", ACK);
    });

    server.on("/list", [this](AsyncWebServerRequest* request) {
        if(!testMagic(request)) {
            request->send(400, "text/plain", NACK);
            return;
        }

        DEBUGGING_PRINTF("Listing directory: \n");

        fs::File root = SPIFFS.open("/");
        String json = "{\n\"dirFiles\": [";

        File file = root.openNextFile();
        
        while(file){
            if(!file.isDirectory()){
                json += '"';
                json += file.name();
                json += '"';
            }
            file = root.openNextFile();

            if(file) {
                json += ",";
            }
        }

        json += "]\n}";
        root.close();

        DEBUGGING_PRINTF("Resulting JSON output: %s\n", json.c_str());

        request->send(200, "application/json", json);
    });

    server.on("/save", HTTP_POST, [this](AsyncWebServerRequest* request) {
        if(!request->hasParam("Script-File", true, true)) {
            request->send(400, "text/plain", NACK);
            return;
        }

    }, [](AsyncWebServerRequest* request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
        if(!index){
            request->_tempFile = SPIFFS.open(filename, "w");
        }
        if(request->_tempFile){
            if(len){
                request->_tempFile.write(data, len);
            }
            if(final){
                request->send(200, "text/plain", ACK);
                request->_tempFile.close();
            }
        }
    });

    server.on("/open", [this](AsyncWebServerRequest* request) {
        if(!request->hasParam("Filename", true)) {
            request->send(400, "text/plain", NACK);
            return;
        }

        if(!SPIFFS.exists(request->getParam("Filename", true)->value())) {
            request->send(404, "text/plain", NACK);
            return;
        }

        request->send(
            SPIFFS,
            request->getParam("Filename", true)->value(),
            "text/plain"
        );
    });

    server.addHandler(quackEventLauncher->getEventSource());

    server.begin();
}

#endif