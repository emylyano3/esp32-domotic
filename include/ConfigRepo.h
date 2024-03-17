#ifndef ConfigRepo_h
#define ConfigRepo_h

#include "ConfigDef.h"

class ConfigRepo {
    public:
        bool reset();
        bool init();
        bool load(ConfigDef* config);
        bool save(ConfigDef* config);
};
#endif