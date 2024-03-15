#ifndef ESP32Domotic_h
#define ESP32Domotic_h

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <PubSubClient.h>
#include "Channel.h"
#include "ConfigDef.h"

#ifndef ESP32
#define ESP32
#endif 

#ifndef MAX_CHANNELS
#define MAX_CHANNELS 5
#endif

class ESP32Domotic {

    uint8_t         channelsCount   = 0;
    const char*     moduleType      = "generic";

    uint16_t        wifiConnectTimeout   = 30;
    uint16_t        configPortalTimeout  = 60;
    uint16_t        configFileSize       = 200;

    unsigned long   mqttNextConnAtte     = 0;
    unsigned int    mqttReconnections    = 0;

    /* HTTP Update */
    WebServer           httpServer;
    //TODO Implementar OTA Update segun: https://randomnerdtutorials.com/esp32-over-the-air-ota-programming/
    //ESP32HTTPUpdate     httpUpdater;
    WiFiClient          wifiClient;
    PubSubClient        mqttClient;

    Channel*            channels[MAX_CHANNELS];
    ConfigDef*          config = new ConfigDef();

    bool connectWifi();
    
    public:

        ESP32Domotic() : mqttClient(wifiClient) {};
        ~ESP32Domotic();

        void init();
        void cycle();

         // Adds new channel to manage. Returns true if channel added successfuly.
        bool    addChannel(Channel* c);
        void    setModuleType(const char* type);

        void    setWifiConnectTimeout (uint16_t seconds);
        void    setConfigPortalTimeout (uint16_t seconds);
        void    setConfigFileSize (uint16_t bytes);
};

#endif