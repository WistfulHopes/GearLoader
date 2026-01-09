#include "gearLoader_p.h"
#include <functional>
#include <iterator>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include "apiRegistry/apiRegistry.h"

//static std::unordered_map<std::string, std::vector<ModApi>> _apiRegistry;
static APIRegistry _apiRegistry;

SemanticVersion toSemanticVersion(std::string s) {
    SemanticVersion output;

    std::stringstream ss(s);
    std::string token;
    char delim = '.';
    int tokens[3] = {0, 0, 0};

    for (int i = 0; i < 3; i++) {
        std::getline(ss, token, delim);
        try {
            tokens[i] = std::stoi(token);
        } catch (const std::invalid_argument& e) {
            // log it maybe
            break;
        }
        if (ss.eof()) {
            break;
        }
    }

    output.major = tokens[0];
    output.minor = tokens[1];
    output.patchNum = tokens[2];

    return output;
}

using versionComparator = std::function<bool(SemanticVersion, SemanticVersion)>;
const std::unordered_map<std::string,versionComparator> operatorMap {
    {"<",  [](SemanticVersion a, SemanticVersion b) { return a < b; }},
    {"<=", [](SemanticVersion a, SemanticVersion b) { return a <= b; }},
    {"=",  [](SemanticVersion a, SemanticVersion b) { return a == b; }},
    {">",  [](SemanticVersion a, SemanticVersion b) { return a > b; }},
    {">=", [](SemanticVersion a, SemanticVersion b) { return a >= b; }}
};


// selects '<=', '>=', '=', '<', and '>'
const std::regex operationRegex("^[<>]=|^[=<>]");

inline bool evaluate(GearLoaderContext* ctx, const char* versionConstraint, SemanticVersion version) {
    const std::string constraintStr(versionConstraint);
    SemanticVersion specifiedVersion;
    std::string versionOpStr = "";

    ctx->logger.log("[evaluate] ");

    auto regexIter =
        std::sregex_iterator(constraintStr.begin(), constraintStr.end(), operationRegex);
    std::smatch match = *regexIter;

    if (!match.empty()) {
        int substringOffset = match.position() + match.size();
        specifiedVersion = toSemanticVersion(constraintStr.substr(substringOffset));
        versionOpStr = match.str();
    } else {
        specifiedVersion = toSemanticVersion(constraintStr);
        versionOpStr = "=";
    }

    const versionComparator comparator = operatorMap.at(versionOpStr);
    if (comparator == NULL) {
        return false;
    }

    return comparator(version, specifiedVersion);
}


int32_t __stdcall RetrieveModApi(
    GearLoaderContext* ctx,
    const char* name,
    const char* versionConstraint,
    const void** pApi,
    SemanticVersion* retrievedVersion)
{
    ctx->logger.log(VERBOSE, "Mod api \"%s\" was requested by \"%s\"", name, ctx->manifest.name.c_str());

    ModApi retApi;
    bool success = _apiRegistry.get(name, retApi, &ctx->logger);

    if (!success) {
        ctx->logger.log(ERR, "No matching API was found (name: \"%s\" version: \"%s\")", name, versionConstraint);
        return 1;
    } else {
        *retrievedVersion = retApi.version;
        *pApi = retApi.api;
        return 0;
    }
}
int32_t __stdcall RegisterApi(
    GearLoaderContext* ctx,
    const void* api,
    const char* name,
    SemanticVersion version)
{
    ctx->logger.log(VERBOSE, "Registering mod api: \"%s\" v%d.%d.%d", name, version.major, version.minor, version.patchNum);

    bool success = _apiRegistry.put(api, name, version, &ctx->logger);

    if (!success) {
        ctx->logger.log(ERR, "API (name: \"%s\" v%d.%d.%d ) was already registered", name, version.major, version.minor, version.patchNum);
        return 1;
    } else {
        return 0;
    }
}

void __stdcall LogApi(GearLoaderContext* ctx, const char* str) {
    ctx->logger.log(DEBUG, "[MOD %s] %s", ctx->manifest.name.c_str(), str);
}

GearLoaderApi* GetGearLoaderAPI() {
    static GearLoaderApi _gearLoader = {
        sizeof(GearLoaderApi),
        GEARLOADER_VERSION_NUM,
        RetrieveModApi,
        RegisterApi,
        LogApi
    };
    return &_gearLoader;
}
