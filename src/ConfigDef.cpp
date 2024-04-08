#include <string.h> 
#include "ConfigDef.h"

ConfigDef::ConfigDef(const char* configFileLocation, const char* channelsFileLocation) {
    this->configFilePath = configFileLocation;
    this->channelsFilePath = channelsFileLocation;
}

ConfigDef::~ConfigDef() {}

const char* ConfigDef::getConfigFilePath(){
    return this->configFilePath;
}

const char* ConfigDef::getChannelsFilePath(){
    return this->channelsFilePath;
}

char* ConfigDef::getMqttHost(){
    return this->mqttHost;
}

char* ConfigDef::getMqttPort(){
    return this->mqttPort;
}

char* ConfigDef::getModuleName(){
    return this->moduleName;
}

char* ConfigDef::getModuleLocation(){
    return this->moduleLocation;
}

const char* ConfigDef::getModuleType(){
    return this->moduleType;
}

void ConfigDef::updateMqttHost(const char* value) {
    strncpy(this->mqttHost, value, MQTT_HOST_PARAM_LENGTH);
}

void ConfigDef::updateMqttPort(const char* value) {
    strncpy(this->mqttPort, value, MQTT_PORT_PARAM_LENGTH);
}

void ConfigDef::updateModuleName(const char* value) {
    strncpy(this->moduleName, value, MODULE_NAME_PARAM_LENGTH);
}

void ConfigDef::updateModuleLocation(const char* value) {
    strncpy(this->moduleLocation, value, MODULE_LOCATION_PARAM_LENGTH);
}

void ConfigDef::setModuleType(const char* type) {
  this->moduleType = type;
}