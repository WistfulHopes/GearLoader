#pragma once

#include <filesystem>
#include <string>
#include <sstream>
#include <unordered_map>
#include <vector>
#include "gearLoader/gearLoader.hpp"
#include "logger/logger.h"

#define MAX_LOADED_MODS 256

GearLoaderApi* GetGearLoaderAPI();

inline std::ostream& operator << (std::ostream& outs, const SemanticVersion& ver) {
    return outs << ver.major << '.' << ver.minor << '.' << ver.patchNum;
}
enum class Operator {
    EQ_OR_GREATER_THAN,
    GREATER_THAN,
    EQUAL,
    LESS_THAN,
    EQ_OR_LESS_THAN,
};

struct DependencyManifest {
    std::string name;
    SemanticVersion version;
    Operator versionOperator;
    bool optional;

    bool operator==(const DependencyManifest& other) const {
        return name == other.name &&
            version == other.version &&
            versionOperator == other.versionOperator &&
            optional == other.optional;
    };
};

struct ModManifest {
    std::string name;
    SemanticVersion version;
    SemanticVersion modLoaderVersion;
    bool ignore;
    std::filesystem::path path;
    std::vector<DependencyManifest> dependencies;

    bool operator==(const ModManifest& other) const {
        return name == other.name &&
            version == other.version &&
            modLoaderVersion == other.modLoaderVersion &&
            ignore == other.ignore &&
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
    SemanticVersion version = GEARLOADER_VERSION_SEM_VER;
    ModManifest* manifest;
    int loadOrder;
    Logger* logger;
};
