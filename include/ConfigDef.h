#ifndef ConfigDef_h
#define ConfigDef_h

class ConfigDef {
    private:
        const char* configFilePath;
        char* mqttHost;
        char* mqttPort;
        char* moduleName;
        char* moduleLocation;

    public:
        ConfigDef(const char* configFilePath);
        ~ConfigDef();
        
        const char* getConfigFilePath();

        char* getMqttHost();
        char* getMqttPort();
        char* getModuleName();
        char* getModuleLocation();
};
#endif