#ifndef OTAUpdate_h
#define OTAUpdate_h

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ElegantOTA.h>

class OTAUpdate {
    
    WebServer server;
    
    static void onStart();
    static void onProgress(size_t current, size_t final);
    static void onEnd(bool success);

    public:
        OTAUpdate() : server(80){};

        void init();
        void handle();
};
#endif