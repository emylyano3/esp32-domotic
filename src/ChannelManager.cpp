#include <string>
#include <LittleFS.h>
#include "ChannelManager.h"
#include "Logger.h"
#include "Utils.h"

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
    if (strcmp(getStationTopic("command/hrst").c_str(), topic) == 0) {
        moduleHardReset();
    } else if (strcmp(getStationTopic("command/rst").c_str(), topic) == 0) {
        moduleSoftReset();
    } else {
        for (size_t i = 0; i < this->channels.size(); ++i) {
            Channel *channel = getChannel(i);
            const char* a = Utils::concat(channel->getName(), "/command/enable").c_str();
            Serial.printf("Concatenado: %s\n", a);
            if (Utils::endsWith(topic, a)) {
                if (enableChannelCommand(channel, payload, length)) {
                    saveChannelsSettings();
                }
                this->pubsubClient->publish(getChannelTopic(i, "feedback/enable").c_str(), channel->isEnabled() ? "1" : "0");
            } else if (Utils::endsWith(topic, Utils::concat(channel->getName(), "/command/timer").c_str())) {
                if (updateChannelTimerCommand(channel, payload, length)) {
                    saveChannelsSettings();
                }
                std::string sTimer = std::to_string(channel->getTimer());
                this->pubsubClient->publish(getChannelTopic(i, "feedback/timer").c_str(), sTimer.c_str());
            } else if (Utils::endsWith(topic, Utils::concat(channel->getName(), "/command/rename").c_str())) {
                if (renameChannelCommand(channel, payload, length)) {
                    saveChannelsSettings();
                }
            } else if (Utils::endsWith(topic, Utils::concat(channel->getName(), "/command/state").c_str())) {
                // command/state topic is used to change the state on the channel with a desired value. So, receiving a mqtt
                // message with this purpose has sense only if the channel is an output one.
                if (channel->isEnabled() && channel->isOutput()) {
                    changeOutputChannelStateCommand(channel, payload, length);
                }
                //TODO REvisar si debe ser mappedState o rawState 
                this->pubsubClient->publish(getChannelTopic(i, "feedback/state").c_str(), channel->getMappedState() == LOW ? "1" : "0");
            }
        }
    }
}

bool ChannelManager::enableChannelCommand(Channel* channel, uint8_t* payload, unsigned int length) {
    #ifdef LOGGING
    // log("Enabling channel", channel->getName());
    #endif
    return true;
}

bool ChannelManager::updateChannelTimerCommand(Channel* channel, uint8_t* payload, unsigned int length) {
    #ifdef LOGGING
    log("Updating channel timer", channel->getName());
    #endif
    return true;
}

bool ChannelManager::renameChannelCommand(Channel* channel, uint8_t* payload, unsigned int length) {
    #ifdef LOGGING
    log("Renaming channel", channel->getName());
    #endif
    return true;
}

void ChannelManager::changeOutputChannelStateCommand(Channel* channel, uint8_t* payload, unsigned int length) {
    #ifdef LOGGING
    log("Changing channel state", channel->getName());
    #endif
}

void ChannelManager::saveChannelsSettings() {
    #ifdef LOGGING
    log("Saving channels settings");
    #endif
}

void ChannelManager::moduleHardReset () {
    #ifdef LOGGING
    log("Doing a module hard reset");
    #endif
    LittleFS.format();
    WiFi.disconnect();
    delay(200);
    ESP.restart();
}

void ChannelManager::moduleSoftReset () {
    #ifdef LOGGING
    log("Doing a module soft reset");
    #endif
    WiFi.disconnect();
    delay(200);
    ESP.restart();
}

void ChannelManager::connectBroker() {
    if (this->mqttNextConnAtte <= millis() && this->mqttReconnections++ < MQTT_RECONNECTION_MAX_RETRIES) {
        this->mqttNextConnAtte = millis() + MQTT_RECONNECTION_RETRY_WAIT_MILLIS;
        #ifdef LOGGING
        log("Connecting MQTT broker as", getStationName().c_str());
        #endif    
        if (pubsubClient->connect(getStationName().c_str())) {
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

std::string ChannelManager::getStationTopic(const char* suffix) {
    std::string stationTopic;
    stationTopic += this->config->getModuleType();
    stationTopic += '/';
    stationTopic += this->config->getModuleLocation();
    stationTopic += '/';
    stationTopic += this->config->getModuleName();
    stationTopic += '/';
    stationTopic += suffix;
    return stationTopic;
}

std::string ChannelManager::getChannelTopic(uint8_t channelIndex, const char* suffix) {
    std::string channelTopic;
    channelTopic += this->config->getModuleType();
    channelTopic += '/';
    channelTopic += this->config->getModuleLocation();
    channelTopic += '/';
    channelTopic += this->config->getModuleName();
    channelTopic += '/';
    channelTopic += getChannel(channelIndex)->getName();
    channelTopic += '/';
    channelTopic += suffix;
    return channelTopic;
}

std::string ChannelManager::getStationName() {
    std::string stationName = this->config->getModuleType();
    stationName += "_";
    stationName += this->config->getModuleLocation();
    stationName += "_";
    stationName += this->config->getModuleName();
    return stationName;
}

Channel* ChannelManager::getChannel(uint8_t i) {
    if (i < this->channels.size()) {
    return &this->channels[i];
    }
    return NULL;
}