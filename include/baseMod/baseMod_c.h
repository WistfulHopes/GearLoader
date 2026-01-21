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
     *  \brief Draws text to the screen during battle using the game's internal text glyphs.
     * 
     *  This function must be called before the game begins its drawing process.
     *      It is recommend to call this function in the `afterGameUpdate` hook.
     * 
     *  \param text A pointer to a text string to be displayed. The character set is limited
     *      to upper case letters, numbers, and the following special symbols: "-+.!?/():&"
     *      Some characters map to additional special characters:
     *          ",~acegiwxyz" maps to "•±on•utabyx".
     *          '>' = END character from the highscore initals screen.
     *          "bdfh" = down/left/right/up arrows
     *  \param xPos Internal resolution screen-space coordinate (640x480). Left edge is 0, right is 640.
     *  \param yPos Internal resolution screen-space coordinate (640x480). Top edge is 0, bottom is 480.
     *  \param zPos The draw order/depth buffer value. Lower values draw later / appear in front of other text and sprites.
     *  \param alpha Transparency value [0-255].
     *  \param size Scaling value, standard size is 1.0f which results in a text glyph of size 12x15px (internal resolution).
     */
    void __stdcall (*renderText)(const char* text, int32_t xPos, int32_t yPos, float zPos, uint8_t alpha, float size);
};

struct BaseMod_GameDataApi {
    uint32_t size;
    uint32_t version;

    /**
     *  \brief `0` for player 1, `1` for player 2.
     */
    GGXXACPR_Entity* __stdcall(*getPlayer)(int player_index);
    /**
     *  \brief Gets the current state of a player's controller input
     */
    enum GGXXACPR_RawControllerInput __stdcall(*getPlayerInput)(int player_index);
    /**
     *  \brief Gets the camera struct. See `GGXXACPR_Camera`.
     */
    GGXXACPR_Camera* __stdcall(*getCamera)();
    /**
     *  \brief returns a non-zero value if the game is on the battle screen.
     */
    int32_t __stdcall(*isInGame)();
    /**
     *  \brief Returns a pointer to the current job mode, see enum `GGXXACPR_JobMode`. This variable
     *      determines what scene the game is set to such as "TitleScreen", "Battle", "MissionMenu".
     */
    int32_t* __stdcall(*getJobMode)();
    /**
     *  \brief Enum `GGXXACPR_GameMode`.
     */
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
    /**
     *  \brief Removes a hook from the registry.
     */
    uint32_t __stdcall (*removeHook)(BaseMod_HookId id);
};


// Root struct for the base mod API. A pointer to an instance of
//  this struct will be given by `IGearLoaderApi.RetrieveModAPI()`.
typedef struct BaseMod_Api {
    uint32_t size;
    uint32_t version;

    /// \brief Allows invoking native game functions
    const struct BaseMod_NativeFunctionsApi* NativeFunctions;
    /// \brief Safe acces to notable game data
    const struct BaseMod_GameDataApi* GameData;
    /// \brief Function hooking manager
    const struct BaseMod_HookApi* Hooks;
} BaseMod_Api;

#ifdef __cplusplus
}   // extern "C"
#endif

#endif
