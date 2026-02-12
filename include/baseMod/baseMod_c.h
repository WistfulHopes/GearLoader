#ifndef BASEMOD_H
#define BASEMOD_H

#define BASEMOD_NAME "baseMod"
#define BASEMOD_API_VERSION "0.1.0"
#define BASEMOD_API_VERSION_NUM 0x000100
#define BASEMOD_MAJOR_VERSION_MASK 0xFF0000

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
    /// \brief The size of the struct in bytes
    uint32_t size;
    /// \brief The API version
    uint32_t version;

    /**
     *  \brief Draws text to the screen during battle using the game's text glyph system.
     * 
     *  This function must be called before the game begins its drawing process.
     *      It is recommend to call this function in the `AfterGameUpdate` hook.
     * 
     *  \param text A pointer to the text string to be displayed. The string format is a subset of ASCII.
     *      The character set is limited to upper case letters, numbers, and the following special symbols: "-+.!?/():&"
     *      The following characters map to additional special characters:
     *          ",~acegiwxyz" maps to "•±on•utabyx".
     *          '>' = END character from the highscore initals screen.
     *          "bdfh" = down/left/right/up arrows
     *  \param xPos Internal resolution screen-space coordinate (640x480). Left edge is 0, right is 640.
     *  \param yPos Internal resolution screen-space coordinate (640x480). Top edge is 0, bottom is 480.
     *  \param zPos The draw order/depth buffer value. Lower values draw later / appear in front of other text and sprites.
     *  \param alpha Transparency value [0-255].
     *  \param size Scaling value, standard size is 1.0f which results in a text glyph of size 12x15px (internal resolution).
     *  \return 0 if no error, else error code.
     */
    uint32_t __stdcall (*RenderText)(const char* text, int32_t xPos, int32_t yPos, float zPos, uint8_t alpha, float size);
};

struct BaseMod_GameDataApi {
    /// \brief The size of the struct in bytes
    uint32_t size;
    /// \brief The API version
    uint32_t version;

    /**
     *  \brief `0` for player 1, `1` for player 2.
     */
    GGXXACPR_Entity* __stdcall(*GetPlayer)(int playerIndex);
    /**
     *  \brief Gets the current state of a player's controller input
     */
    enum GGXXACPR_RawControllerInput __stdcall(*GetPlayerInput)(int playerIndex);
    /**
     *  \brief Gets the camera struct. See `GGXXACPR_Camera`.
     */
    GGXXACPR_Camera* __stdcall(*GetCamera)();
    /**
     *  \brief returns a non-zero value if the game is on the battle screen.
     */
    int32_t __stdcall(*IsInGame)();
    /**
     *  \brief Returns a pointer to the current job mode, see enum `GGXXACPR_JobMode`. This variable
     *      determines what scene the game is set to such as "TitleScreen", "Battle", "MissionMenu".
     */
    int32_t __stdcall(*GetJobMode)();
    /**
     *  \brief See enum `GGXXACPR_GameModeFeatureFlags`. Returns the current game mode feature flags.
     */
    uint32_t __stdcall(*GetGameModeFeatureFlags)();
    /**
     *  \brief Enum `GGXXACPR_MainMenuItem`. Returns the game mode selected from the main menu.
     * 
     *  This value defaults to MAIN_MENU_ITEM_ARCADE and is
     *      set when selecting an option on the main menu.
     */
    uint32_t __stdcall(*GetMainMenuSelection)();
    /**
     *  \brief Returns the D3D9 device pointer.
     * 
     *  Include `d3d9.h` and cast to IDirect3DDevice9 to use. This is
     *      a borrowed pointer. Do not call `Release()`.
     */
    void* __stdcall(*GetD3D9Device)();
};

typedef uint32_t BaseMod_HookId;
typedef struct BaseMod_HookContext BaseMod_HookContext;

typedef struct BaseMod_PeekMessageArgs {
    void* lpMsg;    // MSG from `winuser.h`
    void* hWnd;     // HWND from `WinDef.h`
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
    /// \brief The size of the struct in bytes
    uint32_t size;
    /// \brief The API version
    uint32_t version;

    /**
     *  \brief Registers a hook to the PeekMessage hook.
     * 
     *  Use PeekMessage hooks to read windows message such as low level keyboard input.
     * 
     *  \param hookFn The callback function, see type `BaseMod_PeekMessageHook`.
     *  \param userData A generic pointer to state data the callback function needs.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    BaseMod_HookId __stdcall (*AfterPeekMessage)(BaseMod_PeekMessageHook hookFn, void* userData);
    /**
     *  \brief Registers a hook to run before the game state updates.
     * 
     *  Use this to apply changes to the game state right before it runs an update.
     * 
     *  \param hookFn The callback function, see type `BaseMod_GameUpdateHook`.
     *  \param userData A generic pointer to state data the callback function needs.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    BaseMod_HookId __stdcall (*BeforeGameUpdate)(BaseMod_GameUpdateHook hookFn, void* userData);
    /**
     *  \brief Registers a hook to run after the game state updates.
     * 
     *  Use this to read game state as soon as it updates or to overwrite
     *      the game state right after the game updates it.
     * 
     *  \param hookFn The callback function, see type `BaseMod_GameUpdateHook`.
     *  \param userData A generic pointer to state data the callback function needs.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    BaseMod_HookId __stdcall (*AfterGameUpdate)(BaseMod_GameUpdateHook hookFn, void* userData);
    /**
     *  \brief Registers a hook to run before the call to `IDirect3DDevice9::EndScene`.
     * 
     *  Use this to add additional graphics logic to the game's main scene.
     * 
     *  \param hookFn The callback function, see type `BaseMod_DrawHook`.
     *  \param userData A generic pointer to state data the callback function needs.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    BaseMod_HookId __stdcall (*BeforeEndScene)(BaseMod_DrawHook hookFn, void* userData);
    /**
     *  \brief Registers a hook to run after the call to `IDirect3DDevice9::EndScene`
     *      but before the call to `IDirect3DDevice9::Present`.
     * 
     *  Use this to add a new scene to the current frame.
     * 
     *  \param hookFn The callback function, see type `BaseMod_DrawHook`.
     *  \param userData A generic pointer to state data the callback function needs.
     * 
     *  \return A hook id value that can be passed to `RemoveHook`.
     */
    // Called outside the game's begin scene context before present is called.
    BaseMod_HookId __stdcall (*BeforePresent)(BaseMod_DrawHook hookFn, void* userData);
    /**
     *  \brief Removes a hook from the registry.
     *  \param id The `BaseMod_HookId` of the hook to be removed.
     *  \return 0 if no error, else an error code.
     */
    uint32_t __stdcall (*RemoveHook)(BaseMod_HookId id);
};


// Root struct for the base mod API. A pointer to an instance of
//  this struct will be given by `IGearLoaderApi.RetrieveModAPI()`.
typedef struct BaseMod_Api {
    /// \brief The size of the struct in bytes
    uint32_t size;
    /**
     *  \brief the API version.
     * 
     *  The mod loader will create this struct with the value given by the `BASEMOD_API_VERSION_NUM`
     *      macro. Compare this value with the `BASEMOD_API_VERSION_NUM` macro to detect version
     *      differences between the installed mod loader version and the targeted mod loader API version.
     *      The format is a semantic version 0x00AABBCC where 0xAA is major, 0xBB is minor, and 0xCC is patch number.
     */
    uint32_t version;
    /// \brief API for invoking native game functions
    const struct BaseMod_NativeFunctionsApi* NativeFunctions;
    /// \brief Access to notable game data
    const struct BaseMod_GameDataApi* GameData;
    /// \brief Function hooking manager
    const struct BaseMod_HookApi* Hooks;
} BaseMod_Api;

#ifdef __cplusplus
}   // extern "C"
#endif

#endif
