#ifndef ConfigDef_h
#define ConfigDef_h

class ConfigDef {

    public:
        ConfigDef();
        ~ConfigDef();
    
        char* mqttServer;
        char* mqttPort;
        char* moduleName;
        char* moduleLocation;

};
#endif