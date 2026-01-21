// All mods should include the mod loader header
#include "gearLoader/gearLoader.hpp"
// For each mod api retrieved, include its public header
#include "baseMod/baseMod.hpp"

#include <iostream>
#include <utility>


static BaseMod_HookId _fixHealthHookId;

struct ExampleModContext {
    BaseMod::Api* baseModApi;
    const BaseMod_Api* c_api;
};
static ExampleModContext _modContext;


// Basic god mode for survival
void __stdcall fixHealth(void* userData, const BaseMod_HookContext* ctx, const BaseMod_GameUpdateInfo* info) {
    // Cast and retrieve state from userData
    ExampleModContext* modContext = reinterpret_cast<ExampleModContext*>(userData);
    BaseMod::Api* api = modContext->baseModApi;

    // Obtain game data view from baseMod's game data API.
    ggxxacpr::Player player1 = api->GameData.getPlayer(0);
    bool isInGame = api->GameData.isInGame();
    ggxxacpr::MenuItem gameMode = api->GameData.getGameMode();

    // Check if game has initialized player structs with isValid()
    if (player1.isValid() && isInGame && gameMode == ggxxacpr::MenuItem::SURVIVAL) {
        std::cout << "[ex_cpp] Setting P1 stats" << std::endl;
        player1.set_health(400);
        player1.set_tension(10000);
    }

    int32_t rawVal = *modContext->c_api->GameData->getJobMode();
    std::cout << rawVal << std::endl;
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
    // _baseModApi = BaseMod::Api(modApi);
    // _fixHealthContext = { &_baseModApi };
    _modContext = { new BaseMod::Api(modApi), modApi };

    // If result is non-zero an error occured (e.g. API not found).
    if (result > 0) {
        // Error handling
        return;
    }
    
    // Register hook with BaseMod's hooking API
    _fixHealthHookId = _modContext.baseModApi->Hooks.afterGameUpdate(fixHealth, &_modContext);
}
