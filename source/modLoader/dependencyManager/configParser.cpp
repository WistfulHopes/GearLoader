#include "dependencyManager/configParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "gearLoaderApi/gearLoader_p.h"

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
        minVersion: {0,0,0},
        optional:   false
    };

    output.name = data["name"];
    
    if (data.contains("version")) {
        output.minVersion = toSemanticVersion(data["version"]);
    }
    if (data.contains("optional")) {
        output.optional = data["optional"];
    }


    return output;
}
ModManifest parseManifestJson(json data) {
    ModManifest output;
    
    output.name = data["name"];
    output.version = toSemanticVersion(data["version"]);
    if (!data["dependencies"].empty()) {
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
