#ifndef BASEMOD_H
#define BASEMOD_H

#define BASEMOD_NAME "baseMod"
#define BASEMOD_API_VERSION "0.1.0"
#define BASEMOD_API_VERSION_NUM 0x000100

#ifdef __cplusplus
    #include <cstdint>
#else
    #include <stdint.h>
#endif

#include "gearLoader/ggxxacpr_c.h"

#ifdef __cplusplus
extern "C" {
#endif


struct BaseMod_NativeFunctionsApi {
    uint32_t size;
    uint32_t version;

    /**
     *  \brief Draws text to the screen during battle
     * 
     *  
     */
    void __stdcall (*renderText)(const char* text, int32_t xPos, int32_t yPos, float zPos, uint8_t alpha, float size);
};

struct BaseMod_GameDataApi {
    uint32_t size;
    uint32_t version;

    GGXXACPR_Entity* __stdcall(*getPlayer)(int player_index);
    GGXXACPR_Camera* __stdcall(*getCamera)();
    int32_t __stdcall(*isInGame)();
    // Returns enum `GGXXACPR_GameMode`
    uint32_t __stdcall(*getGameMode)();
    // raw pointer to the games' D3D9 device. Include `d3d9.h` and cast to 
    //  IDirect3DDevice9 to use. This is a borrowed pointer. Do not call `Release()`.
    void* __stdcall(*getD3D9Device)();
    // int16_t* __stdcall(*getAirThrowRanges_PR)();
    // int16_t* __stdcall(*getAirThrowRanges_AC)();
    // int16_t* __stdcall(*getAirThrowRangesUpper)();
    // int16_t* __stdcall(*getAirThrowRangesLower)();
    // int16_t* __stdcall(*getGroundThrowRanges_PR)();
    // int16_t* __stdcall(*getGroundThrowRanges_PR)();
};

typedef uint32_t BaseMod_HookId;
typedef struct BaseMod_HookContext BaseMod_HookContext;

typedef struct BaseMod_PeekMessageArgs {
    // MSG
    void* lpMsg;
    // HWND
    void* hWnd;
    uint32_t wMsgFilterMin;
    uint32_t wMsgFilterMax;
    uint32_t wRemoveMsg;
} BaseMod_PeekMessageArgs;
typedef struct BaseMod_PeekMessageInfo {
    BaseMod_PeekMessageArgs args;
    int success;
} BaseMod_PeekMessageInfo;
typedef void(__stdcall *BaseMod_PeekMessageHook)(void* userData, const BaseMod_HookContext* ctx, const BaseMod_PeekMessageInfo* info);

// Reserved
typedef struct BaseMod_GameUpdateInfo BaseMod_GameUpdateInfo;
// Parameters reserved
typedef void(__stdcall *BaseMod_GameUpdateHook)(void* userData, const BaseMod_HookContext* ctx, const BaseMod_GameUpdateInfo* info);

typedef struct BaseMod_DrawInfo {
    // include `d3d9.h` and cast to IDirect3DDevice9.
    void* device;
} BaseMod_DrawInfo;
typedef void(__stdcall *BaseMod_DrawHook)(void* userData, const BaseMod_HookContext* ctx, const BaseMod_DrawInfo* info);


// Adds callbacks to managed function hooks.
struct BaseMod_HookApi {
    uint32_t size;
    uint32_t version;

    BaseMod_HookId __stdcall (*afterPeekMessage)(BaseMod_PeekMessageHook hookFn, void* userData);
    BaseMod_HookId __stdcall (*beforeGameUpdate)(BaseMod_GameUpdateHook hookFn, void* userData);
    BaseMod_HookId __stdcall (*afterGameUpdate)(BaseMod_GameUpdateHook hookFn, void* userData);
    // Called inside the game's begin/end scene context before ending the scene.
    BaseMod_HookId __stdcall (*beforeEndScene)(BaseMod_DrawHook hookFn, void* userData);
    // Called outside the game's begin scene context before present is called.
    BaseMod_HookId __stdcall (*beforePresent)(BaseMod_DrawHook hookFn, void* userData);
    uint32_t __stdcall (*removeHook)(BaseMod_HookId id);
};


// Root struct for the base mod API. A pointer to an instance of
//  this struct will be given by `IGearLoaderApi.RetrieveModAPI()`.
typedef struct BaseMod_Api {
    uint32_t size;
    uint32_t version;

    // Allows invoking native game functions
    const struct BaseMod_NativeFunctionsApi* NativeFunctions;
    // Safe acces to notable game data
    const struct BaseMod_GameDataApi* GameData;
    // Function hooking manager
    const struct BaseMod_HookApi* Hooks;
} BaseMod_Api;

#ifdef __cplusplus
}   // extern "C"
#endif

#endif
