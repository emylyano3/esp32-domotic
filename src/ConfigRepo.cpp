#include <ArduinoJson.h> 
#include <LittleFS.h>
#include "ConfigRepo.h"
#include "Logger.h"

bool ConfigRepo::init() {
    if (LittleFS.begin()) {
        return true;
    }
    log("Error al inicializar el FS");
    return false;
}

bool ConfigRepo::reset() {
    return LittleFS.format();
}

bool ConfigRepo::load(ConfigDef* config) {
    File configFile = LittleFS.open(config->getConfigFilePath(), "r");
    if (!configFile) {
        log("failed to open file");
        return false;
    }
    log("reading config file");
    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, buf.get());
    if (!error) {
        strcpy(config->getMqttHost(), doc["mqtt_host"]);
        strcpy(config->getMqttPort(), doc["mqtt_port"]);
        strcpy(config->getModuleLocation(), doc["location"]);
        strcpy(config->getModuleName(), doc["name"]);
        configFile.close();
        return true; 
    } else {
        log("failed to load json config");
        configFile.close();
        return false; 
    }  
}

bool ConfigRepo::save(ConfigDef* config) {
    File configFile = LittleFS.open(config->getConfigFilePath(), "w");
    if (!configFile) {
        log("failed to open file for writing");
        return false;
    }
    JsonDocument doc;
    doc["mqtt_host"] = config->getMqttHost();
    doc["mqtt_port"] = config->getMqttPort();
    doc["location"] = config->getModuleLocation();
    doc["name"] = config->getModuleName();
    if (serializeJson(doc, configFile) == 0) {
        log("Failed to write to file");
        configFile.close();
        return false;
    }
    configFile.close();
    return true;
}