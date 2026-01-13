// All mods should include the mod loader header. Pure C mods only need the base C header.
#include "gearLoader/gearLoader_c.h"
// For each mod api retrieved, include its public header
#include "baseMod/baseMod_c.h"


typedef struct UserHookContext {
    const BaseMod_Api* baseModApi;
} UserHookContext;

static BaseMod_HookId _fixHealthHookId;
static const BaseMod_Api* _baseModApi;
static UserHookContext _fixedHealthCtx;


// Basic god mode for survival
void __stdcall ExampleSurvivalModeCheat(void* userData, const BaseMod_HookContext* ctx, const BaseMod_GameUpdateInfo* info) {
    // Cast user state (Recommended method of accessing mod api from a hook)
    UserHookContext* userCtx = (UserHookContext*)userData;
    const BaseMod_Api* api = userCtx->baseModApi;

    // Obtain game data from baseMod's GameData API.
    GGXXACPR_Entity* player1 = api->GameData->getPlayer(0);
    int isInGame = api->GameData->isInGame();
    uint32_t gameMode = api->GameData->getGameMode();

    // If player1 is loaded, we're in game, and it's survival mode
    if (player1 && isInGame > 0 && gameMode == MAIN_MENU_ITEM_SURVIVAL) {
        // Set HP to 400 for P1
        player1->health = 400;
        // Infinite tension
        if (player1->playerEntityDataPtr)
            player1->playerEntityDataPtr->tension = 10000;
    }
}

// Called by mod loader immediatey after loading the mod.
//  This function should serve as the mod entry point.
//  All dependencies listed in config.json will already
//  be loaded in the process when this method is called.
GEAR_LOADER_EXPORT void GEAR_LOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* api) {

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
    _fixedHealthCtx = (UserHookContext){ _baseModApi };

    // Register hook with BaseMod's hooking API
    _fixHealthHookId = _baseModApi->Hooks->afterGameUpdate(ExampleSurvivalModeCheat, &_fixedHealthCtx);
}
