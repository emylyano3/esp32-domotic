#ifndef ESP32Domotic_h
#define ESP32Domotic_h

#include <Arduino.h>
#include <WiFi.h>
#include <DNSServer.h>
#include "ConfigDef.h"
#include "OTAUpdate.h"
#include "Channel.h"
#include "ChannelManager.h"

#ifndef ESP32
#define ESP32
#endif 

#ifndef MAX_CHANNELS
#define MAX_CHANNELS 5
#endif

class ESP32Domotic {

    const char*     apSsid          = NULL;

    uint16_t        wifiConnectTimeout   = 30;
    uint16_t        configPortalTimeout  = 60;
    uint16_t        configFileSize       = 200;

    unsigned long   mqttNextConnAtte     = 0;
    unsigned int    mqttReconnections    = 0;

    ConfigDef*              config = new ConfigDef("/config.json");
    OTAUpdate*              otaUpdate;
    std::vector<Channel>    channels;
    ChannelManager*         channelsManager;

    bool connectWifi();
    void initOTAUpdate();
    void initChannelManager();

    public:

        ESP32Domotic(){};
        ~ESP32Domotic();

        void    init();
        void    cycle();

         // Adds new channel to manage. Returns true if channel added successfuly.
        bool    addChannel(Channel* c);
        void    setModuleType(const char* type);

        void    setWifiConnectTimeout (uint16_t seconds);
        void    setConfigPortalTimeout (uint16_t seconds);
        void    setConfigFileSize (uint16_t bytes);
        void    setPortalSSID (const char* ssid);

        ConfigDef*  getConfig();
};
#endif