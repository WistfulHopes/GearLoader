#ifndef GEARLOADER_H

#if !defined(_WIN32) || defined(_WIN64)
    #error "Mods should be built targeting 32-bit windows (x86) (same target as +R)"
#endif

#define GEARLOADER_H


#define GEARLOADER_VERSION "2.0.0"
#define GEARLOADER_VERSION_NUM 0x020000

#if __cplusplus
    #include <cstdint>
#else
    #include <stdint.h>
#endif


typedef struct SemanticVersion {
    uint32_t major;
    uint32_t minor;
    uint32_t patchNum;
} SemanticVersion;

inline int compare(const SemanticVersion* a, const SemanticVersion* b) {
    int dif = a->major - b->major;
    if (dif != 0) {
        return dif;
    }
    dif = a->minor - b->minor;
    if (dif != 0) {
        return dif;
    }
    return a->patchNum - b->patchNum;
}


// Forward this incomplete type to API calls to provide calling context to the mod loader.
typedef struct GearLoaderContext GearLoaderContext;
typedef struct GearLoaderApi {
    uint32_t size;
    uint32_t version;
    int32_t __stdcall (*RetrieveModApi)(GearLoaderContext* ctx, const char* name, const char* versionConstraint, const void** pApi, SemanticVersion* retrievedVersion);
    int32_t __stdcall (*RegisterApi)(GearLoaderContext* ctx, const void* api, const char* name, SemanticVersion version);
    void __stdcall (*Log)(GearLoaderContext* ctx, const char* str);
} GearLoaderApi;

#ifdef __cplusplus
    #define GEAR_LOADER_EXPORT extern "C" __declspec(dllexport)
#else
    #define GEAR_LOADER_EXPORT __declspec(dllexport)
#endif
#define GEAR_LOADER_CALL __cdecl

// All mods should export an "Init" function that follows this signature
typedef void (GEAR_LOADER_CALL *ModInitFunc)(GearLoaderContext* ctx, GearLoaderApi* api);

#endif
