#include <string>
#include <LittleFS.h>
#include "ChannelManager.h"
#include "Logger.h"
#include "ConfigRepo.h"
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
    checkOutputChannels();
}


void ChannelManager::checkOutputChannels() {
//TODO implementar la posibilidad de lock desde el cliente
//   if (_behaviourLocked) {
//     return;
//   }
    for (size_t i = 0; i < this->channels.size(); ++i) {
        Channel* channel = getChannel(i);
        // Timer is checked just if the channel state was changed from the logic inside this lib (locally changed)
        if (channel->isOutput() && channel->checkTimer()) {
            this->pubsubClient->publish(getChannelTopic(i, "feedback/state").c_str(), Utils::getLogicState(channel->getCurrentState()));
        }
    }
}

void ChannelManager::setChannels(std::vector<Channel*>& channels) {
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
            Channel* channel = getChannel(i);
            if (strcmp(getChannelTopic(channel, "command/enable").c_str(), topic) == 0) {
                if (enableChannelCommand(channel, payload, length)) {
                    saveChannelsSettings();
                }
                this->pubsubClient->publish(getChannelTopic(i, "feedback/enable").c_str(), channel->isEnabled() ? "1" : "0");
            } else if (strcmp(getChannelTopic(channel, "command/timer").c_str(), topic) == 0) {
                if (updateChannelTimerCommand(channel, payload, length)) {
                    saveChannelsSettings();
                }
                std::string sTimer = std::to_string(channel->getTimer());
                this->pubsubClient->publish(getChannelTopic(i, "feedback/timer").c_str(), sTimer.c_str());
            } else if (strcmp(getChannelTopic(channel, "command/rename").c_str(), topic) == 0) {
                if (renameChannelCommand(channel, payload, length)) {
                    saveChannelsSettings();
                }
            } else if (strcmp(getChannelTopic(channel, "command/state").c_str(), topic) == 0) {
                // command/state topic is used to change the state on the channel with a desired value. So, receiving a mqtt
                // message with this purpose has sense only if the channel is an output one.
                if (channel->isOutput()) {
                    if (!changeOutputChannelStateCommand(channel, payload, length)) {
                        log("Channel not updated");
                    }
                } else {
                    log("Can not change state on input channel");
                }
                this->pubsubClient->publish(getChannelTopic(i, "feedback/state").c_str(), Utils::getLogicState(channel->getCurrentState()));
            } else if (strcmp(getChannelTopic(channel, "command/read").c_str(), topic) == 0) {
                if (!channel->isOutput()) {
                    
                } else {
                    log("Can not read state from output channel");
                }
            }
        }
    }
}

bool ChannelManager::enableChannelCommand(Channel* channel, uint8_t* payload, unsigned int length) {
    #ifdef LOGGING
    log("Updating channel enablement", channel->getName());
    #endif
    if (length != 1 || !payload) {
        #ifdef LOGGING
        log("Invalid payload. Ignoring.");
        #endif
        return false;
    }
    bool stateChanged = false;
    switch (payload[0]) {
        case '0':
            stateChanged = channel->isEnabled();
            channel->setEnabled(false);
            break;
        case '1':
            stateChanged = !channel->isEnabled();
            channel->setEnabled(true);
            break;
        default:
            #ifdef LOGGING
            log("Invalid state", payload[0]);
            #endif
            break;
    }
    #ifdef LOGGING
    log("Same state. No update done.");
    #endif
    return stateChanged;
}

bool ChannelManager::updateChannelTimerCommand(Channel* channel, uint8_t* payload, unsigned int length) {
    #ifdef LOGGING
    log("Updating channel timer", channel->getName());
    #endif
    if (length < 1 || !payload) {
        #ifdef LOGGING
        log("Invalid payload");
        #endif
        return false;
    }
    char buff[length + 1];
    for (uint16_t i = 0 ; i < length; ++ i) {
        buff[i] = payload[i];
    }
    buff[length] = '\0';
    long newTimer = Utils::toInt(buff) * 1000; // received in seconds set in millis
    #ifdef LOGGING
    log("New timer in seconds", newTimer);
    #endif
    bool timerChanged = channel->getTimer() != newTimer;
    channel->setTimer(newTimer);
    return timerChanged;
}

bool ChannelManager::renameChannelCommand(Channel* channel, uint8_t* payload, unsigned int length) {
    #ifdef LOGGING
    log("Renaming channel", channel->getName());
    #endif
    if (length < 1 || !payload) {
        #ifdef LOGGING
        log("Invalid payload in rename channel");
        #endif
        return false;
    }
    char buffer[length + 1];
    strncpy(buffer, (char*)payload, length);
    buffer[length] = '\0';
    if (channel->updateName(buffer)) {
        this->pubsubClient->unsubscribe(getChannelTopic(channel, "command/+").c_str());
        delay(200);
        this->pubsubClient->subscribe(getChannelTopic(channel, "command/+").c_str());
        return true;
    }
    return false;
}

bool ChannelManager::changeOutputChannelStateCommand(Channel* channel, uint8_t* payload, unsigned int length) {
    #ifdef LOGGING
    log("Changing channel state", channel->getName());
    #endif
    if (length < 1 || !payload) {
        #ifdef LOGGING
        log("Invalid payload");
        #endif
        return false;
    }
    if (!channel->isEnabled()) {
        #ifdef LOGGING
        log("Channel is disabled");
        #endif
        return false;
    }
    //TODO delegarle la interpretacion del payload al channel (a traves del ingoing value mapper) 
    // a traves de la funcion  de mapeo. La funcion ya deberia resolver la logica invertida (si aplica)
    return updateChannelState(channel, Utils::interpretMqttBinaryPayload(payload, length));
}

bool ChannelManager::updateChannelState (Channel* channel, int state) {
    if (channel->getCurrentState() == state) {
        #ifdef LOGGING
        log("Channel is in same state, skipping", state);
        #endif
        return false;
    } 
    channel->write(state);
    return true;
}

void ChannelManager::saveChannelsSettings() {
    #ifdef LOGGING
    log("Saving channels settings");
    #endif
    ConfigRepo repo;
    repo.save(this->config, this->channels);
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
    return getChannelTopic(getChannel(channelIndex), suffix);
}

std::string ChannelManager::getChannelTopic(Channel* channel, const char* suffix) {
    std::string channelTopic;
    channelTopic += this->config->getModuleType();
    channelTopic += '/';
    channelTopic += this->config->getModuleLocation();
    channelTopic += '/';
    channelTopic += this->config->getModuleName();
    channelTopic += '/';
    channelTopic += channel->getName();
    channelTopic += '/';
    return channelTopic += suffix;
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
        return this->channels[i];
    }
    return NULL;
}