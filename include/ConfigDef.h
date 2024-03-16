#ifndef ConfigDef_h
#define ConfigDef_h

class ConfigDef {

    public:
        ConfigDef(const char* location);
        ~ConfigDef();
    
        const char* location;

        char* mqttServer;
        char* mqttPort;
        char* moduleName;
        char* moduleLocation;

};
#endif