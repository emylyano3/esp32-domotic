#include "ConfigDef.h"

ConfigDef::ConfigDef(const char* fileLocation) {
    this->configFilePath = fileLocation;
}

ConfigDef::~ConfigDef() {
    
}

const char* ConfigDef::getConfigFilePath(){
    return this->configFilePath;
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
