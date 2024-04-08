#ifndef ConfigRepo_h
#define ConfigRepo_h

#include <vector>
#include "ConfigDef.h"
#include "Channel.h"

class ConfigRepo {
    public:
        ConfigRepo();
        bool reset();
        bool load(ConfigDef*);
        bool save(ConfigDef*);
        bool save(ConfigDef*, std::vector<Channel*>&);
        bool load(ConfigDef*, std::vector<Channel*>&);
};
#endif