// All mods that implement the Init function should include the mod loader header
#include "gearLoader/gearLoader_c.h"
// For each mod api retrieved, include its public header
#include "baseMod/baseMod_c.h"


typedef struct UserHookContext {
    const BaseMod_Api* baseModApi;
} UserHookContext;

static BaseMod_HookId _exampleSurvivalHookId;
static const BaseMod_Api* _baseModApi;
static UserHookContext _userCtx;


// Basic god mode for survival
void __stdcall ExampleSurvivalModeCheat(void* userData, const BaseMod_HookContext* ctx, const BaseMod_GameUpdateInfo* info) {
    // Cast user state (Recommended method of accessing mod api from a hook)
    UserHookContext* userCtx = (UserHookContext*)userData;
    const BaseMod_Api* api = userCtx->baseModApi;

    // Obtain game data from baseMod's GameData API.
    GGXXACPR_Entity* player1 = api->GameData->GetPlayer(0);
    int isInGame = api->GameData->IsInGame();
    uint32_t jobMode = api->GameData->GetJobMode();
    uint32_t modeFlags = api->GameData->GetGameModeFeatureFlags();

    // If player1 is loaded, it's in game, and it's survival mode
    if (player1 && isInGame > 0 &&
            jobMode == JOB_MODE_BATTLE &&
            (modeFlags & GAME_MODE_FEATURE_FLAGS_SURVIVAL) > 0) {
        // Set HP to 400 for P1
        player1->health = 400;
        // Infinite tension
        if (player1->playerEntityDataPtr)
            player1->playerEntityDataPtr->tension = 10000;

        // Draw text
        api->NativeFunctions->RenderText(
            "GOD MODE ACTIVE",
            20,     // X pos
            120,    // Y pos
            365.0f, // draw depth (Z pos)
            0xFF,   // Alpha
            1.0f);  // Size
    }
}

// Called by mod loader immediatey after loading the mod.
//  This function should serve as the mod entry point.
//  All dependencies listed in config.json will already
//  be loaded in the process when this method is called.
GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {

    // Obtain dependency mod API from mod loader
    const void* retApi;
    SemanticVersion retVer;
    int32_t result = api->RetrieveModApi(
        ctx,            // Forward this pointer
        BASEMOD_NAME,   // Mod name goes here. The BaseMod header has a macro for this
        ">=0.1.0",      // Version constraint
        &retApi,        // retVal output. This is a pointer to the BaseMod's api
        &retVer         // retVal output for api version
    );

    // If result is non-zero an error occured (e.g. API not found).
    if (result > 0) {
        // Error handling
        return;
    }

    // Cast api pointer. Bundle it into a state struct that
    //  we'll give as context to the fixHealth callback.
    _baseModApi = (const BaseMod_Api*)retApi;
    _userCtx = (UserHookContext){ _baseModApi };

    // Register hook with BaseMod's hooking API
    _exampleSurvivalHookId = _baseModApi->Hooks->AfterGameUpdate(ExampleSurvivalModeCheat, &_userCtx);
}
