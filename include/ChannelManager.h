#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include <vector>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Channel.h"
#include "ConfigDef.h"

#ifndef MQTT_RECONNECTION_MAX_RETRIES
#define MQTT_RECONNECTION_MAX_RETRIES 1000
#endif
#ifndef MQTT_RECONNECTION_RETRY_WAIT_MILLIS
#define MQTT_RECONNECTION_RETRY_WAIT_MILLIS 10 * 1000
#endif

class ChannelManager {
    private:
        unsigned long           mqttNextConnAtte     = 0;
        unsigned int            mqttReconnections    = 0;

        WiFiClient              wifiClient;
        ConfigDef*              config;
        PubSubClient*           pubsubClient;
        std::vector<Channel>    channels;

        void        connectBroker();
        
        void        moduleHardReset ();
        void        moduleSoftReset ();
        void        saveChannelsSettings();
        bool        enableChannelCommand(Channel*, uint8_t*, unsigned int);
        bool        updateChannelTimerCommand(Channel*, uint8_t*, unsigned int);
        bool        renameChannelCommand(Channel*, uint8_t*, unsigned int);
        void        changeOutputChannelStateCommand(Channel*, uint8_t*, unsigned int);

        std::string getChannelTopic(uint8_t, const char*);
        std::string getChannelTopic(Channel*, const char*);
        std::string getStationTopic(const char*);
        std::string getStationName();
        Channel*    getChannel(uint8_t i);

    public:
        ChannelManager(){};
        ~ChannelManager();

        void init();
        void handle();
        void setChannels(std::vector<Channel>&);
        void setConfig(ConfigDef*);
        void receiveMqttMessage(char*, uint8_t*, unsigned int);
};

#endif