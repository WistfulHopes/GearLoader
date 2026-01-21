#include "gearLoader_p.h"
#include <functional>
#include <iterator>
#include <regex>
#include <string>
#include <unordered_map>
#include <vector>
#include "apiRegistry.h"
#include "common/versionParsing.h"


static APIRegistry _apiRegistry;


int32_t __stdcall RetrieveModApi(
    GearLoaderContext* ctx,
    const char* name,
    const char* versionConstraint,
    const void** pApi,
    SemanticVersion* retrievedVersion)
{
    if (!ctx || ctx->version != SemanticVersion GEARLOADER_VERSION_SEM_VER) {
        return 2;
    }

    ctx->logger->log(VERBOSE, "Mod api \"%s\" was requested by \"%s\"", name, ctx->manifest->name.c_str());

    SemanticVersion ver {0,0,0};
    Operator op = Operator::EQ_OR_GREATER_THAN;
    ParseVersionQualifier(versionConstraint, op, ver);

    ModApi retApi;
    bool success = _apiRegistry.get(name, ver, op, retApi, ctx->logger);
    if (!success) {
        ctx->logger->log(ERR, "No matching API was found (name: \"%s\" version: \"%s\")", name, versionConstraint);
        return 1;
    } else {
        *pApi = retApi.api;
        *retrievedVersion = retApi.version;
        return 0;
    }
}

int32_t __stdcall RegisterApi(
    GearLoaderContext* ctx,
    const void* api,
    const char* name,
    SemanticVersion version)
{
    if (!ctx || ctx->version != SemanticVersion GEARLOADER_VERSION_SEM_VER) {
        return 2;
    }

    ctx->logger->log(VERBOSE, "Registering mod api: \"%s\" v%s", name, ToString(version).c_str());

    bool success = _apiRegistry.put(api, name, version, ctx->logger);

    if (!success) {
        ctx->logger->log(ERR, "API (name: \"%s\" v%s) was already registered", name, ToString(version).c_str());
        return 1;
    } else {
        return 0;
    }
}

uint32_t __stdcall LogApi(GearLoaderContext* ctx, int logLevel, const char* str) {
    if (!ctx || ctx->version != SemanticVersion GEARLOADER_VERSION_SEM_VER) {
        return 2;
    }

    LogLevel level = DEBUG;
    if (logLevel >= 0 && logLevel <= 4) {
        level = static_cast<LogLevel>(logLevel);
    }

    ctx->logger->log(level, "[MOD %s] %s", ctx->manifest->name.c_str(), str);

    return 0;
}

GearLoaderApi* GetGearLoaderAPI() {
    static GearLoaderApi _gearLoader = {
        size: sizeof(GearLoaderApi),
        version: GEARLOADER_VERSION_NUM,
        RetrieveModApi: RetrieveModApi,
        RegisterApi: RegisterApi,
        Log: LogApi
    };
    return &_gearLoader;
}
