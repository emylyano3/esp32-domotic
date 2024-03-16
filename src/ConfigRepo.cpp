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
    File configFile = LittleFS.open(config->location, "r");
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
        strcpy(config->mqttServer, doc["mqtt_server"]);
        strcpy(config->mqttPort, doc["mqtt_port"]);
        strcpy(config->moduleLocation, doc["location"]);
        strcpy(config->moduleName, doc["name"]);
        configFile.close();
        return true; 
    } else {
        log("failed to load json config");
        configFile.close();
        return false; 
    }  
}

bool ConfigRepo::save(ConfigDef* config) {
    File configFile = LittleFS.open(config->location, "w");
    if (!configFile) {
        log("failed to open file for writing");
        return false;
    }
    JsonDocument doc;
    doc["mqtt_server"] = config->mqttServer;
    doc["mqtt_port"] = config->mqttPort;
    doc["location"] = config->moduleLocation;
    doc["name"] = config->moduleName;
    if (serializeJson(doc, configFile) == 0) {
        log("Failed to write to file");
        configFile.close();
        return false;
    }
    configFile.close();
    return true;
}