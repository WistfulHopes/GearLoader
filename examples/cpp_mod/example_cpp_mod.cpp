// All mods that implement the Init function should include the mod loader header
#include "gearLoader/gearLoader.hpp"
// For each mod api retrieved, include its public header
#include "baseMod/baseMod.hpp"


static BaseMod_HookId _exampleSurvivalHookId;

struct ExampleModContext {
    BaseMod::Api* baseModApi;
};
static ExampleModContext _modContext;


// Basic god mode for survival
void __stdcall ExampleSurvivalModeCheat(
        ExampleModContext* modContext,
        const BaseMod_HookContext* ctx,
        const BaseMod_GameUpdateInfo* info) {

    // Get reference to base mod API from the modContext parameter
    BaseMod::Api* api = modContext->baseModApi;

    // Obtain game data from baseMod's game data API.
    ggxxacpr::Player player1 = api->GameData.GetPlayer(0);
    bool isInGame = api->GameData.IsInGame();
    ggxxacpr::JobMode jobMode = api->GameData.GetJobMode();
    ggxxacpr::GameModeFeatureFlags modeFlags = api->GameData.GetGameModeFeatureFlags();

    // Check if game has initialized player structs with isValid()
    // Check if it's in game and surival mode
    if (player1.isValid() && isInGame &&
            jobMode == ggxxacpr::JobMode::BATTLE &&
            (modeFlags & ggxxacpr::GameModeFeatureFlags::SURVIVAL) == ggxxacpr::GameModeFeatureFlags::SURVIVAL) {
        // Set player variables
        player1.setHealth(400);
        player1.setTension(10000);

        // Draw text
        api->NativeFunctions.RenderText(
            "GOD MODE ACTIVE",
            20,     // X pos
            120,    // Y pos
            365.0f, // draw depth (Z pos)
            0xFF,   // Alpha
            1.0f);  // Size
    }
}

// Called by mod loader immediatey after loading this mod.
//  This function should serve as the mod entry point.
//  All dependencies listed in config.json will already
//  be loaded in the process when this method is called.
GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* c_api) {

    // Construct C++ wrapper class
    GearLoader::Api gearLoaderApi(c_api, ctx);

    // Obtain dependency mod API from mod loader
    const BaseMod_Api* baseModCApi;
    SemanticVersion retrievedVer;
    int result = gearLoaderApi.RetrieveModApi<BaseMod_Api>(
        BASEMOD_NAME,   // Mod name goes here. The BaseMod header has a macro for this.
        ">=0.1.0",      // Version constraint
        &baseModCApi,   // retVal output. This is where the <BaseMod_Api> templated type comes in.
        &retrievedVer   // retVal output for api version
    );

    // If result is non-zero an error occured (e.g. API not found).
    if (result > 0) {
        // Error handling
        return;
    }

    // Construct C++ Wrapper for BaseMod Api
    BaseMod::Api *baseModApi = new BaseMod::Api(baseModCApi);

    // Create context for hook
    _modContext = { baseModApi };

    // Register hook with BaseMod's hooking API
    _exampleSurvivalHookId = baseModApi->Hooks.AfterGameUpdate<ExampleModContext>(
        ExampleSurvivalModeCheat,
        &_modContext);
}
