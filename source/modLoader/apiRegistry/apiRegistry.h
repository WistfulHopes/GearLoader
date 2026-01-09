#pragma once

#include <string>
#include <unordered_map>
#include "gearLoaderApi/gearLoader_p.h"

// TODO: This isn't being used right now, see `gearLoaderApi.cpp` to see where it could be used

class APIRegistry {
public:
    bool put(const void* pApi, const char* name, SemanticVersion version, Logger* logger = nullptr) {
        std::string nameStr(name);
        
        if (_map.contains(nameStr)) {
            if (logger) logger->log(ERR, "API \"%s\" already registered", name);
            return false;
        } else {
            _map[nameStr] = ModApi{nameStr, version, pApi};
            return true;
        }
    }
    bool get(const char* name, ModApi& retApi, Logger* logger = nullptr) {
        return get(name, ">=0.0.0", retApi, logger);
    }
    bool get(const char* name, const char* versionConstriant, ModApi& retApi, Logger* logger = nullptr) {
        std::string nameStr(name);

        if (_map.contains(nameStr)) {
            retApi = _map[nameStr];
            return true;
        } else {
            if (logger) logger->log(ERR, "API wasn't found for mod \"%s\"", name);
            return false;
        }
    }
private:
    std::unordered_map<std::string, ModApi> _map;
};
