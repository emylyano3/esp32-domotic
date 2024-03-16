#include <ArduinoJson.h> 
#include <LittleFS.h>
#include "ConfigRepo.h"

bool ConfigRepo::reset() {
    return true;   
}

bool ConfigRepo::load(ConfigDef* config) {
    File configFile = LittleFS.open(config->location, "r");
    if (!configFile) {
        Serial.println("failed to open file");
        return false;
    } 
    Serial.println("reading config file");
    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, buf.get());
    //serializeJson(doc, Serial);
    if (!error) {
        Serial.println("\nparsed json");
        strcpy(config->mqttServer, doc["mqtt_server"]);
        strcpy(config->mqttPort, doc["mqtt_port"]);
        strcpy(config->moduleLocation, doc["location"]);
        strcpy(config->moduleName, doc["name"]);
        configFile.close();
        return true; 
    } else {
        Serial.println("failed to load json config");
        configFile.close();
        return false; 
    }  
}

bool ConfigRepo::save(ConfigDef* config) {
    return true;   
}