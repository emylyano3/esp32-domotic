#include "ESP32Domotic.h"
#include "ConfigRepo.h"
#include <WiFiManager.h>

//Setup y loop para que no chille el compilador
void setup(){}
void loop(){}

ESP32Domotic::~ESP32Domotic(){}

void ESP32Domotic::init() { 
  ConfigRepo repo;
  repo.load(this->config);

  if (connectWifi()) {
    Serial.println("Connected to wifi");
    repo.save(this->config);
  } else {
    Serial.println("Could not connect to wifi");
    ESP.restart();
  }
}

bool ESP32Domotic::connectWifi() {
  WiFiManager wifiManager;
  WiFiManagerParameter server("server", "mqtt server", this->config->mqttServer, 16);
  WiFiManagerParameter port("port", "mqtt port", this->config->mqttPort, 6);
  WiFiManagerParameter name("name", "module name", this->config->moduleName, 32);
  WiFiManagerParameter location("location", "module location", this->config->moduleLocation, 32);

  wifiManager.addParameter(&server);
  wifiManager.addParameter(&port);
  wifiManager.addParameter(&name);
  wifiManager.addParameter(&location);

  wifiManager.setMinimumSignalQuality();
  wifiManager.setConfigPortalTimeout(this->configPortalTimeout);
  wifiManager.setConnectTimeout(this->wifiConnectTimeout);
  if (wifiManager.autoConnect()) {
    strcpy(this->config->mqttServer, server.getValue());
    strcpy(this->config->mqttPort, port.getValue());
    strcpy(this->config->moduleName, name.getValue());
    strcpy(this->config->moduleLocation, location.getValue());
    return true;
  }
  return false;
}

void ESP32Domotic::cycle() {
}

bool ESP32Domotic::addChannel(Channel *channel) {
  if (this->channelsCount < MAX_CHANNELS) {
    this->channels[this->channelsCount++] = channel;
    #ifdef LOGGING
    debug(F("Channel added"), channel->name);
    #endif
    return true;
  } else {
    #ifdef LOGGING
    debug(F("No more channels suported"));
    #endif
    return false;
  }
}

void ESP32Domotic::setModuleType(const char* type) {
  this->moduleType = type;
}

void ESP32Domotic::setWifiConnectTimeout (uint16_t seconds) {
  this->wifiConnectTimeout = seconds;
}

void ESP32Domotic::setConfigPortalTimeout (uint16_t seconds) {
  this->configPortalTimeout = seconds;
}

void ESP32Domotic::setConfigFileSize (uint16_t bytes) {
  this->configFileSize = bytes;
}