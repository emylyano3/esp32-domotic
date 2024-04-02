#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include <vector>
#include <WiFi.h>
#include <PubSubClient.h>
#include "Channel.h"
#include "ConfigDef.h"

class ChannelManager {
    private:
        WiFiClient              wifiClient;
        ConfigDef*              config; 
        PubSubClient*           pubsubClient;
        std::vector<Channel>    channels;

    public:
        ChannelManager(){};
        ~ChannelManager();

        void init();
        void handle();
        void setChannels(std::vector<Channel>&  channels);
        void setConfig(ConfigDef* config);
};

#endif