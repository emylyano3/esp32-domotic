#include "ChannelManager.h"

ChannelManager::~ChannelManager() {
    delete pubsubClient;
}

void ChannelManager::init() {
    this->pubsubClient = new PubSubClient();
    this->pubsubClient->setClient(this->wifiClient);
    // this->pubsubClient->
}

void ChannelManager::handle() {}

void ChannelManager::setChannels(std::vector<Channel>& channels) {
    this->channels = channels;
}

void ChannelManager::setConfig(ConfigDef* config) {
    this->config = config;
}
