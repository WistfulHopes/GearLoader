#include "dependencyManager/configParser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "nlohmann/json.hpp"
#include "gearLoaderApi/gearLoader_p.h"
#include "common/versionParsing.h"

using namespace nlohmann;

// makes recursion work with inline helper function
// DependencyManifest parseDependencyJson(json data);


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
inline std::vector<DependencyManifest> parseDependencyArray(json data) {
    std::vector<DependencyManifest> output;
    
    for (auto& dep : data) {
        output.push_back(parseDependencyJson(dep));
    }
    
    return output;
}

// grabs the first found dll in the given folder. Expected folder structure should include
//  exactly one dll if not using the entry field in the config.json.
inline std::filesystem::path getFirstDllPath(std::filesystem::path folder) {
    for (const auto& entry : std::filesystem::directory_iterator(folder)) {
        if (entry.path().extension() == ".dll") {
            return entry.path();
        }
    }
    return folder;
}

ModManifest parseManifestJson(json data, std::filesystem::path path) {
    ModManifest output = {
        name: "Unnamed mod",
        version: {0,0,0},
        modLoaderVersion: GEARLOADER_VERSION_SEM_VER,
        ignore: false,
    };
    
    if (data.contains("name")) output.name = data["name"];
    if (data.contains("version")) output.version = ParseSemanticVersion(data["version"]);
    if (data.contains("modLoaderVersion")) output.modLoaderVersion = ParseSemanticVersion(data["modLoaderVersion"]);
    if (data.contains("ignore")) output.ignore = data["ignore"];
    if (data.contains("dependencies") && !data["dependencies"].empty()) {
        output.dependencies = parseDependencyArray(data["dependencies"]);
    }
    if (data.contains("entryPoint") && std::filesystem::exists(path / data["entryPoint"])) {
        output.path = path / data["entryPoint"];
    } else {
        output.path = getFirstDllPath(path);
    }

    return output;
}

ModManifest ParseConfig(std::filesystem::path path) {
    std::ifstream configFile(path.string());
    json data;
    configFile >> data;

    ModManifest output = parseManifestJson(data, path.parent_path());

    return output;
}
