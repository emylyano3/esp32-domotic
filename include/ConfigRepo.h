#ifndef ConfigRepo_h
#define ConfigRepo_h

#include "ConfigDef.h"
#include "Channel.h"

class ConfigRepo {
    public:
        bool reset();
        bool init();
        bool load(ConfigDef*);
        bool save(ConfigDef*);
        bool save(ConfigDef*, std::vector<Channel>&);
};
#endif