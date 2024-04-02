#ifndef ConfigDef_h
#define ConfigDef_h

#ifndef MQTT_HOST_PARAM_LENGTH
#define MQTT_HOST_PARAM_LENGTH 16
#endif
#ifndef MQTT_PORT_PARAM_LENGTH
#define MQTT_PORT_PARAM_LENGTH 6
#endif
#ifndef MODULE_NAME_PARAM_LENGTH
#define MODULE_NAME_PARAM_LENGTH 32
#endif
#ifndef MODULE_LOCATION_PARAM_LENGTH
#define MODULE_LOCATION_PARAM_LENGTH 32
#endif

class ConfigDef {
    private:
        const char* configFilePath;
        char* mqttHost = new char[MQTT_HOST_PARAM_LENGTH];
        char* mqttPort = new char[MQTT_PORT_PARAM_LENGTH];
        char* moduleName = new char[MODULE_NAME_PARAM_LENGTH];
        char* moduleLocation = new char[MODULE_LOCATION_PARAM_LENGTH];

    public:
        ConfigDef(const char* configFilePath);
        ~ConfigDef();
        
        const char* getConfigFilePath();

        char* getMqttHost();
        char* getMqttPort();
        char* getModuleName();
        char* getModuleLocation();

        void  updateMqttHost(const char* value);
        void  updateMqttPort(const char* value);
        void  updateModuleName(const char* value);      
        void  updateModuleLocation(const char* value);
};
#endif