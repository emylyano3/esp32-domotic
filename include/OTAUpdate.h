#ifndef OTAUpdate_h
#define OTAUpdate_h

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ElegantOTA.h>

class OTAUpdate {
    
    WebServer server;
    
    public:
        OTAUpdate() : server(80){};

        void init();
        void handle();
};
#endif