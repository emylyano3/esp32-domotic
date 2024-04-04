#include <string>
#include "ChannelManager.h"
#include "Logger.h"

ChannelManager::~ChannelManager() {
    delete pubsubClient;
}

void ChannelManager::init() {
    #ifdef LOGGING
    log("Initializing channel manager");
    #endif
    this->pubsubClient = new PubSubClient();
    this->pubsubClient->setClient(this->wifiClient);
    uint16_t port = static_cast<uint16_t>(atoi(this->config->getMqttPort()));
    this->pubsubClient->setServer(this->config->getMqttHost(), port);
    this->pubsubClient->setCallback(std::bind(
        &ChannelManager::receiveMqttMessage,
         this, 
        std::placeholders::_1, 
        std::placeholders::_2, 
        std::placeholders::_3));
    connectBroker();
}

void ChannelManager::handle() {
    if (!this->pubsubClient->loop()) {
        connectBroker();
    }
}

void ChannelManager::setChannels(std::vector<Channel>& channels) {
    this->channels = channels;
}

void ChannelManager::setConfig(ConfigDef* config) {
    this->config = config;
}

void ChannelManager::receiveMqttMessage(char* topic, uint8_t* payload, unsigned int length) {
    #ifdef LOGGING
    log("Mqtt message received on topic", topic);
    #endif
}

void ChannelManager::connectBroker() {
  if (this->mqttNextConnAtte <= millis() && this->mqttReconnections++ < MQTT_RECONNECTION_MAX_RETRIES) {
    this->mqttNextConnAtte = millis() + MQTT_RECONNECTION_RETRY_WAIT_MILLIS;
    #ifdef LOGGING
    log("Connecting MQTT broker as", getStationName());
    #endif    
    if (pubsubClient->connect(getStationName())) {
        this->mqttReconnections = 0;
        #ifdef LOGGING
        log("MQTT broker Connected");
        #endif    
        // subscribe station to any command
        std::string topic = getStationTopic("command/#");
        pubsubClient->subscribe(topic.c_str());
        #ifdef LOGGING
        log("Subscribed to", topic.c_str());
        #endif    
        // subscribe channels to any command
        for (size_t i = 0; i < this->channels.size(); ++i) {
            topic = getChannelTopic(i, "command/+");
            #ifdef LOGGING
            log("Subscribed to", topic.c_str());
            #endif    
            pubsubClient->subscribe(topic.c_str());
        }
    } else {
        #ifdef LOGGING
        log("Failed. RC:", pubsubClient->state());
        #endif    
    }
  }
}

const char* ChannelManager::getStationTopic(const char* suffix) {
    size_t length = strlen(this->config->getModuleType()) + 1;
    length += strlen(this->config->getModuleLocation()) + 1;
    length += strlen(this->config->getModuleName()) + 1;
    length += strlen(suffix) + 1;
    char* stationTopic = new char[length];
    snprintf(stationTopic, length, "%s/%s/%s/%s", this->config->getModuleType(), this->config->getModuleLocation(), this->config->getModuleName(), suffix);
    return stationTopic;
}

const char* ChannelManager::getChannelTopic(uint8_t channelIndex, const char* suffix) {
    size_t length = strlen(this->config->getModuleType()) + 1;
    length += strlen(this->config->getModuleLocation()) + 1;
    length += strlen(this->config->getModuleName()) + 1;
    length += strlen(getChannel(channelIndex)->getName()) + 1;
    length += strlen(suffix) + 1;
    char* channelTopic = new char[length];
    snprintf(channelTopic, length, "%s/%s/%s/%s/%s", this->config->getModuleType(), this->config->getModuleLocation(), this->config->getModuleName(), getChannel(channelIndex)->getName(), suffix);
    return channelTopic;
}

const char* ChannelManager::getStationName() {
    std::string stationName = this->config->getModuleType();
    stationName += "_";
    stationName += this->config->getModuleLocation();
    stationName += "_";
    stationName += this->config->getModuleName();
    return stationName.c_str();
}

Channel* ChannelManager::getChannel(uint8_t i) {
  if (i <= this->channels.size()) {
    return &this->channels[i];
  }
  return NULL;
}