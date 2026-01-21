#pragma once

#include <string>
#include <unordered_map>
#include "gearLoaderApi/gearLoader_p.h"
#include "common/versionParsing.h"


class APIRegistry {
public:
    bool put(const void* pApi, const char* name, SemanticVersion version, Logger* logger = nullptr) {
        std::string nameStr(name);
        ModApi modApi = {nameStr, version, pApi};
        if (_map.contains(nameStr) && _map[nameStr].size() > 0) {
            auto& vector = _map[nameStr];
            for (auto it = vector.begin(); it != vector.end(); it++) {
                if (it->version == version) {
                    if (logger) logger->log(ERR, "API \"%s\" already registered", name);
                    return false;
                } else if (it->version < version) {
                    if (logger) logger->log(VERBOSE,
                            "Inserting api at index %d for vector of size %d for api \"%s\" v%d.%d.%d",
                            std::distance(vector.begin(), it), vector.size(), name, version.major, version.minor, version.patchNum);
                    vector.insert(it, modApi);
                    return true;
                }
            }
            if (logger) logger->log(VERBOSE, "Inserting api at end of vector for api \"%s\" v%d.%d.%d",
                    name, version.major, version.minor, version.patchNum);
            vector.push_back(modApi);
            return true;
        } else {
            if (logger) logger->log(VERBOSE, "Creating new vector for api \"%s\" v%d.%d.%d",
                    name, version.major, version.minor, version.patchNum);
            _map[nameStr] = { modApi };
            return true;
        }
    }
    bool get(const char* name, ModApi& retApi, Logger* logger = nullptr) {
        return get(name, {0,0,0}, Operator::EQ_OR_GREATER_THAN, retApi, logger);
    }
    bool get(const char* name, SemanticVersion version, Operator versionOperator, ModApi& retApi, Logger* logger = nullptr) {
        std::string nameStr(name);

        if (_map.contains(nameStr) && _map[nameStr].size() > 0) {
            auto& vector = _map[nameStr];
            for (auto it = vector.begin(); it != vector.end(); it++) {
                if (CompareVersions(it->version, versionOperator, version)) {
                    if (logger) logger->log(VERBOSE, "Mod api [%s v%d.%d.%d] retrieved", it->name.c_str(), it->version.major, it->version.minor, it->version.patchNum);
                    retApi = *it;
                    return true;
                }
            }
        }
        if (logger) logger->log(ERR, "API wasn't found for mod \"%s\"", name);
        return false;
    }
    
private:
    std::unordered_map<std::string, std::vector<ModApi>> _map;
};
