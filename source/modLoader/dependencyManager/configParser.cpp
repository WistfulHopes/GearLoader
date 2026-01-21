#include "dependencyManager/configParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "gearLoaderApi/gearLoader_p.h"
#include "common/versionParsing.h"

using namespace nlohmann;

// makes recursion work with inline helper function
ModManifest parseManifestJson(json data);
DependencyManifest parseDependencyJson(json data);


inline std::vector<DependencyManifest> parseDependencyArray(json data) {
    std::vector<DependencyManifest> output;
    
    for (auto& dep : data) {
        output.push_back(parseDependencyJson(dep));
    }
    
    return output;
}

DependencyManifest parseDependencyJson(json data) {
    // defaults
    DependencyManifest output = {
        name:       "",
        version: {0,0,0},
        versionOperator: Operator::EQ_OR_GREATER_THAN,
        optional:   false
    };

    if (data.contains("name")) output.name = data["name"];
    
    if (data.contains("version")) {
        ParseVersionQualifier(data["version"], output.versionOperator, output.version);
    }
    if (data.contains("optional")) {
        output.optional = data["optional"];
    }

    return output;
}
ModManifest parseManifestJson(json data) {
    ModManifest output = {
        name: "Unnamed mod",
        version: {0,0,0}
    };
    
    if (data.contains("name")) output.name = data["name"];
    if (data.contains("version")) output.version = ParseSemanticVersion(data["version"]);
    if (data.contains("modLoaderVersion")) output.modLoaderVersion = ParseSemanticVersion(data["version"]);
    if (data.contains("dependencies") && !data["dependencies"].empty()) {
        output.dependencies = parseDependencyArray(data["dependencies"]);
    }

    return output;
}

// grabs the first found dll in the given folder. Expected folder structure should include excatly 1 dll.
inline std::filesystem::path getDllPath(std::filesystem::path folder) {
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.path().extension() == ".dll") {
            return entry.path();
        }
    }
    return folder;
}

ModManifest ParseConfig(std::filesystem::path path) {
    std::ifstream configFile(path.string());
    json data;
    configFile >> data;

    ModManifest output = parseManifestJson(data);

    output.path = getDllPath(path.parent_path()).wstring();

    return output;
}
