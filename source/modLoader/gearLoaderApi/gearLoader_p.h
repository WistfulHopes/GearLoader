#pragma once

#include <filesystem>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "gearLoader/gearLoader.hpp"
#include "logger/logger.h"

#define MAX_LOADED_MODS 256

Logger& GetLogger();
GearLoaderApi* GetGearLoaderAPI();

SemanticVersion toSemanticVersion(std::string s);
inline std::ostream& operator << (std::ostream& outs, const SemanticVersion& ver) {
    return outs << ver.major << '.' << ver.minor << '.' << ver.patchNum;
}
struct DependencyManifest {
    std::string name;
    SemanticVersion minVersion;
    bool optional;

    bool operator==(const DependencyManifest& other) const {
        return name == other.name && minVersion == other.minVersion;
    };
};

struct ModManifest {
    std::string name;
    SemanticVersion version;
    std::filesystem::path path;
    std::vector<DependencyManifest> dependencies;

    bool operator==(const ModManifest& other) const {
        return name == other.name &&
            version == other.version &&
            path == other.path &&
            &dependencies == &other.dependencies;
    };
};

inline std::ostream& operator << (std::ostream& outs, ModManifest& manifest) {
    return outs << manifest.name << '/' << manifest.version << '/' << manifest.path;
}

struct ModApi {
    std::string name;
    SemanticVersion version;
    const void* api;
};

struct GearLoaderContext {
    ModManifest* manifest;
    int loadOrder;
    Logger* logger;
};
