// All mods should include the mod loader header
#include "gearLoader/gearLoader.hpp"
// For each mod api retrieved, include its public header
#include "baseMod/baseMod.hpp"

static BaseMod::Api _baseModApi;
static BaseMod_HookId _fixHealthHookId;

struct FixHealthContext {
    BaseMod::Api baseModApi;
};
static FixHealthContext _fixHealthContext;

BaseMod::Api BaseModApi(BaseMod_Api* c_api = nullptr) {
    static BaseMod::Api _baseModApi(c_api);

    return _baseModApi;
}


// Basic god mode for survival
void __stdcall fixHealth(void* userData, const BaseMod_HookContext* ctx, const BaseMod_GameUpdateInfo* info) {
    // Cast and retrieve state from userData
    FixHealthContext* fhCtx = reinterpret_cast<FixHealthContext*>(userData);
    BaseMod::Api api = fhCtx->baseModApi;

    // Obtain game data view from baseMod's game data API.
    ggxxacpr::Player player1 = api.GameData.getPlayer(0);
    bool isInGame = api.GameData.isInGame();
    ggxxacpr::GameMode gameMode = api.GameData.getGameMode();

    // Check if game has initialized player structs with isValid()
    if (player1.isValid() && isInGame && gameMode == ggxxacpr::GameMode::SURVIVAL) {
        // Obtain raw pointer to modify game data and set HP to 400 for P1.
        player1.getRaw()->health = 400;
    }
}

// Called by mod loader immediatey after loading this mod.
//  This function should serve as the mod entry point.
//  All dependencies listed in config.json will already
//  be loaded in the process when this method is called.
GEAR_LOADER_EXPORT void GEAR_LOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* c_api) {

    // Construct C++ wrapper class
    GearLoader::Api api(c_api);

    // Obtain dependency mod API from mod loader
    const BaseMod_Api* modApi;
    SemanticVersion retrievedVer;
    int result = api.RetrieveModApi<BaseMod_Api>(
        ctx,            // Forward this parameter
        BASEMOD_NAME,   // Mod name goes here. The BaseMod header has a macro for this.
        ">=0.1.0",      // Version constraint
        &modApi,        // retVal output. This is where the <BaseMod_Api> templated type comes in.
        &retrievedVer   // retVal output for api version
    );

    // Construct C++ wrapper class for BaseMod Api
    _baseModApi = modApi;
    _fixHealthContext = { _baseModApi };

    // If result is non-zero an error occured (e.g. API not found).
    if (result > 0) {
        // Error handling
        return;
    }
    
    // Register hook with BaseMod's hooking API
    _fixHealthHookId = _baseModApi.Hooks.afterGameUpdate(fixHealth, &_fixHealthContext);
}
