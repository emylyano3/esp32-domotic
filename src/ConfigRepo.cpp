#include <ArduinoJson.h> 
#include <LittleFS.h>
#include <vector>
#include "ConfigRepo.h"
#include "Logger.h"
#include "Utils.h"

ConfigRepo::ConfigRepo() {
    #ifdef ESP32
    if (LittleFS.begin(true)) {
    #else
    if (LittleFS.begin()) {
    #endif
        #ifdef LOGGING
        log("FS inicializado");
        #endif
    } else {
        #ifdef LOGGING
        log("Error al inicializar el FS");
        #endif
    }
}

bool ConfigRepo::reset() {
    return LittleFS.format();
}

bool ConfigRepo::load(ConfigDef* config) {
    File configFile = LittleFS.open(config->getConfigFilePath(), "r");
    if (!configFile) {
        #ifdef LOGGING
        log("failed to open config file");
        #endif
        return false;
    }
    #ifdef LOGGING
    log("reading config file");
    #endif
    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, buf.get());
    if (!error) {
        config->updateMqttHost(doc["mqtt_host"]);
        config->updateMqttPort(doc["mqtt_port"]);
        config->updateModuleName(doc["name"]);
        config->updateModuleLocation(doc["location"]);
        configFile.close();
        return true; 
    } else {
        #ifdef LOGGING
        log("failed to load json config");
        #endif
        configFile.close();
        return false; 
    }
}

bool ConfigRepo::save(ConfigDef* config) {
    File configFile = LittleFS.open(config->getConfigFilePath(), "w");
    if (!configFile) {
        #ifdef LOGGING
        log("failed to open file for writing");
        #endif
        return false;
    }
    JsonDocument doc;
    doc["mqtt_host"] = config->getMqttHost();
    doc["mqtt_port"] = config->getMqttPort();
    doc["location"] = config->getModuleLocation();
    doc["name"] = config->getModuleName();
    if (serializeJson(doc, configFile) == 0) {
        #ifdef LOGGING
        log("Failed to write to file");
        #endif
        configFile.close();
        return false;
    }
    configFile.close();
    return true;
}

bool ConfigRepo::save(ConfigDef* config, std::vector<Channel*>& channels) {
    File file = LittleFS.open(config->getChannelsFilePath(), "w");
    if (file) {
        JsonDocument doc;
        for (uint8_t i = 0; i < channels.size(); ++i) {
            doc[String(channels[i]->getId()) + "_n"] = channels[i]->getName();
            doc[String(channels[i]->getId()) + "_t"] = channels[i]->getTimer();
            doc[String(channels[i]->getId()) + "_e"] = channels[i]->isEnabled();
        } 
        serializeJson(doc, file);
        #ifdef LOGGING
        log("Channel configuration file saved");
        serializeJsonPretty(doc, Serial);
        Serial.println();
        #endif
        file.close();
        return true;
    } else {
        #ifdef LOGGING
        log("Failed to open config file for writing");
        #endif
        return false;
    }
}

bool ConfigRepo::load(ConfigDef* config, std::vector<Channel*>& channels) {
    File configFile = LittleFS.open(config->getChannelsFilePath(), "r");
     if (!configFile) {
        #ifdef LOGGING
        log("failed to open channels settings file");
        #endif
        return false;
    }
    log("Reading channels settings file");
    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, buf.get());
    if (!error) {
        #ifdef LOGGING
        serializeJsonPretty(doc, Serial);
        Serial.println();
        #endif
        for (uint8_t i = 0; i < channels.size(); ++i) {
            channels[i]->updateName(doc[Utils::concat(channels[i]->getId(), "_n")]);
            channels[i]->setTimer(doc[Utils::concat(channels[i]->getId(), "_t")]);
            channels[i]->setEnabled(doc[Utils::concat(channels[i]->getId(), "_e")]);
        }
        configFile.close();
        return true; 
    } else {
        #ifdef LOGGING
        log("failed to deserialize json");
        #endif
        configFile.close();
        return false; 
    }
}