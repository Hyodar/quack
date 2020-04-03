
#include "quack_webserver.h"

#include <ArduinoOTA.h>
#include <SPIFFS.h>
#include <FS.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <SPIFFSEditor.h>

#include "web_files.h"
#include "web_info.h"

#include "quack_parser.h"

const u16 parseU16(const u8* const str) {
    u32 n = 0;
    
    for(u16 i = 0; str[i] != '\0'; i++) {
        n = (n*10) + (str[i] - '0');
    }

    return n;
}

QuackWebserver::QuackWebserver() : server{80}, ws{"/ws"}, events{"/events"}, parser{nullptr} {
    // no-op
}

void
QuackWebserver::reply(AsyncWebServerRequest* request, int code, const char* type, const uint8_t* data, size_t len) {
    AsyncWebServerResponse* response = request->beginResponse_P(code, type, data, len);

    response->addHeader("Content-Encoding", "gzip");
    request->send(response);
}

void
QuackWebserver::onWsEvent(AsyncWebSocket* server, AsyncWebSocketClient* client,
                          AwsEventType type, void* arg, uint8_t* data, size_t len) {
    if(type == WS_EVT_CONNECT) {
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
        client->printf("Hello Client %u :)", client->id());
        client->ping();
    }
    else if(type == WS_EVT_DISCONNECT) {
        Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
    }
    else if(type == WS_EVT_ERROR) {
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    }
    else if(type == WS_EVT_PONG) {
        Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
    }
    else if(type == WS_EVT_DATA) {
        AwsFrameInfo* info = (AwsFrameInfo*) arg;
        String msg = "";
        if(info->final && info->index == 0 && info->len == len) {
            // the whole message is in a single frame and we got all of it's data
            Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

            if(info->opcode == WS_TEXT) {
                for(size_t i=0; i < info->len; i++) {
                    msg += (char) data[i];
                }
            }
            else {
                char buff[3];
                for(size_t i=0; i < info->len; i++) {
                    sprintf(buff, "%02x ", (uint8_t) data[i]);
                    msg += buff ;
                }
            }

            Serial.printf("%s\n",msg.c_str());

            if(info->opcode == WS_TEXT) {
                client->text("I got your text message");
            }
            else {
                client->binary("I got your binary message");
            }
        }
        else {
            // message is comprised of multiple frames or the frame is split into multiple packets
            if(info->index == 0){
                if(info->num == 0) {
                    Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                }
                Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
            }

            Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

            if(info->opcode == WS_TEXT){
                for(size_t i=0; i < len; i++) {
                    msg += (char) data[i];
                }
            } else {
                char buff[3];
                for(size_t i=0; i < len; i++) {
                    sprintf(buff, "%02x ", (uint8_t) data[i]);
                    msg += buff ;
                }
            }
            
            Serial.printf("%s\n",msg.c_str());

            if((info->index + len) == info->len){
                Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
                
                if(info->final){
                    Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                    
                    if(info->message_opcode == WS_TEXT) {
                        client->text("I got your text message");
                    }
                    else {
                        client->binary("I got your binary message");
                    }
                }
            }
        }
    }
}

void
QuackWebserver::begin(QuackParser* _parser) {
    
    // Parser ================================================================

    parser = _parser;

    // WiFi ==================================================================

    WiFi.mode(WIFI_AP_STA);
    // WiFi.softAP(hostName);
    WiFi.begin(ssid, password);
    
    if(WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.printf("STA: Failed!\n");
        WiFi.disconnect(false);
        delay(1000);
        WiFi.begin(ssid, password);
    }

    Serial.println(WiFi.localIP());

    // ArduinoOTA ============================================================

      //Send OTA events to the browser
    ArduinoOTA.onStart([this] {
        events.send("Update Start", "ota");
    });

    ArduinoOTA.onEnd([this] {
        events.send("Update End", "ota");
    });

    ArduinoOTA.onProgress([this](unsigned int progress, unsigned int total) {
        char p[32];
        sprintf(p, "Progress: %u%%\n", (progress/(total/100)));
        events.send(p, "ota");
    });

    ArduinoOTA.onError([this](ota_error_t error) {
        if(error == OTA_AUTH_ERROR) events.send("Auth Failed", "ota");
        else if(error == OTA_BEGIN_ERROR) events.send("Begin Failed", "ota");
        else if(error == OTA_CONNECT_ERROR) events.send("Connect Failed", "ota");
        else if(error == OTA_RECEIVE_ERROR) events.send("Recieve Failed", "ota");
        else if(error == OTA_END_ERROR) events.send("End Failed", "ota");
    });

    ArduinoOTA.setHostname(hostName);
    ArduinoOTA.begin();

    // mDNS ==================================================================

    MDNS.addService("http", "tcp", 80);

    // SPIFFS ================================================================

    //SPIFFS.format();
    SPIFFS.begin();

    // Server ================================================================

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    GET_CALLBACKS;

    server.on("/", [this](AsyncWebServerRequest* request) {
        reply(request, 200, "text/html", INDEX_HTML, sizeof(INDEX_HTML));
    });

    server.on("/run_raw", HTTP_POST, [this](AsyncWebServerRequest* request) {
        request->send(200);

        if(request->hasParam("Code") && request->hasParam("CodeLength")) {
            const u8* const code = (const u8*) request->getParam("Code")->value().c_str();
            const u16 length = parseU16((const u8*) request->getParam("CodeLength")->value().c_str());

            parser->parsingLoop(code, length);
        }
    });

    server.onNotFound([this](AsyncWebServerRequest* request) {
        reply(request, 404, "text/html", HTTP404_HTML, sizeof(HTTP404_HTML));
    });

    events.onConnect([](AsyncEventSourceClient* client) {
        client->send("hello!", NULL, millis(), 1000);
    });

    server.addHandler(&events);

    server.addHandler(new SPIFFSEditor(SPIFFS, http_username, http_password));

    server.onFileUpload([](AsyncWebServerRequest* request, const String& filename,
                           size_t index, uint8_t* data, size_t len, bool final) {
        if(!index) {
            Serial.printf("UploadStart: %s\n", filename.c_str());
        }

        Serial.printf("%s", (const char*)data);

        if(final) {
            Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
        }
    });

    server.onRequestBody([](AsyncWebServerRequest* request, uint8_t* data,
                            size_t len, size_t index, size_t total){
        if(!index) {
            Serial.printf("BodyStart: %u\n", total);
        }
        
        Serial.printf("%s", (const char*)data);

        if(index + len == total) {
            Serial.printf("BodyEnd: %u\n", total);
        }
    });

    server.begin();
}

void
QuackWebserver::loop() {
    ArduinoOTA.handle();
    ws.cleanupClients();
}