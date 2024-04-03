#ifndef CHANNEL_MANAGER_H
#define CHANNEL_MANAGER_H

#include <vector>
#include <functional>
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

// using MqttCallbackFunction = std::function<void(char*, uint8_t*, unsigned int)>;

class ChannelManager {
    private:
        unsigned long           mqttNextConnAtte     = 0;
        unsigned int            mqttReconnections    = 0;

        WiFiClient              wifiClient;
        ConfigDef*              config; 
        PubSubClient*           pubsubClient;
        std::vector<Channel>    channels;
        // MqttCallbackFunction    mqttCallback;

        void        connectBroker();
        
        String      getStationTopic(String suffix);
        String      getChannelTopic (uint8_t channelIndex, String suffix);
        Channel*    getChannel(uint8_t i);
        const char* getStationName();

    public:
        ChannelManager(){};
        ~ChannelManager();

        void init();
        void handle();
        void setChannels(std::vector<Channel>&);
        void setConfig(ConfigDef*);
        // void setMqttCallback(MqttCallbackFunction);
        void receiveMqttMessage(char*, uint8_t*, unsigned int);
};

#endif