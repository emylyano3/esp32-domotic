#include "ChannelManager.h"
#include "Logger.h"

ChannelManager::~ChannelManager() {
    delete pubsubClient;
}

void ChannelManager::init() {
    log("Initializing channel manager");
    this->pubsubClient = new PubSubClient();
    this->pubsubClient->setClient(this->wifiClient);
    uint16_t port = (uint16_t) String(this->config->getMqttPort()).toInt();
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
    log("Mqtt message received on topic", topic);
}


void ChannelManager::connectBroker() {
  if (this->mqttNextConnAtte <= millis() && this->mqttReconnections++ < MQTT_RECONNECTION_MAX_RETRIES) {
    this->mqttNextConnAtte = millis() + MQTT_RECONNECTION_RETRY_WAIT_MILLIS;
    log("Connecting MQTT broker as", getStationName());
    if (pubsubClient->connect(getStationName())) {
        this->mqttReconnections = 0;
        log("MQTT broker Connected");
        // subscribe station to any command
        String topic = getStationTopic("command/#");
        pubsubClient->subscribe(topic.c_str());
        log("Subscribed to", topic.c_str());
        // subscribe channels to any command
        for (size_t i = 0; i < this->channels.size(); ++i) {
            topic = getChannelTopic(i, "command/+");
            log("Subscribed to", topic.c_str());
            pubsubClient->subscribe(topic.c_str());
        }
        //   if (_mqttConnectionCallback) {
        //     _mqttConnectionCallback();
        //   }
    } else {
      log("Failed. RC:", pubsubClient->state());
    }
  }
}

String ChannelManager::getStationTopic (String suffix) {
  return String(this->config->getModuleType()) + "/" + this->config->getModuleLocation() + "/" + this->config->getModuleName() + "/" + suffix;
}

String ChannelManager::getChannelTopic (uint8_t channelIndex, String suffix) {
  return String(this->config->getModuleType()) + "/" + this->config->getModuleLocation() + "/" + this->config->getModuleName() + "/" + getChannel(channelIndex)->getName() + "/" + suffix;
  // std::string s2 = std::string(getModuleType()) + "/" + getModuleLocation() + "/" + getModuleName() + "/" + channel->name + "/" + suffix.c_str();
}

Channel* ChannelManager::getChannel(uint8_t i) {
  if (i <= this->channels.size()) {
    return &this->channels[i];
  }
  return NULL;
}

const char* ChannelManager::getStationName () {
    size_t size = strlen(this->config->getModuleType()) + strlen(this->config->getModuleLocation()) + strlen(this->config->getModuleName()) + 4;
    String sn;
    sn.concat(this->config->getModuleType());
    sn.concat("_");
    sn.concat(this->config->getModuleLocation()); 
    sn.concat("_");
    sn.concat(this->config->getModuleName());
    return sn.c_str();
}