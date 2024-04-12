#ifndef OTAUpdate_h
#define OTAUpdate_h

#ifdef ESP32
#include <WebServer.h>
#else
#include <ESP8266WebServer.h>
#endif
#include <ElegantOTA.h>

#if defined(ESP32)
    using ESP_WebServer = WebServer;
#else
    using ESP_WebServer = ESP8266WebServer;
#endif

class OTAUpdate {

    ESP_WebServer server;
    
    static void onStart();
    static void onProgress(size_t current, size_t final);
    static void onEnd(bool success);

    public:
        OTAUpdate() : server(80){};

        void init();
        void handle();
};
#endif