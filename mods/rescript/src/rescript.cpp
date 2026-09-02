// All mods that implement the Init function should include the mod loader header
#include "gearLoader/gearLoader.hpp"

#include "character_framework.h"

#include "rescript.h"

#include <iostream>

#include "rescript_as.hpp"

static const ReScript_Api _api = {
    sizeof(ReScript_Api),
    RESCRIPT_API_VERSION_NUM,
};

SemanticVersion getSemVer() {
    unsigned int verNum = RESCRIPT_API_VERSION_NUM;
    return {
        (verNum & 0xFF0000) >> 16,
        (verNum & 0x00FF00) >> 8,
        (verNum & 0x0000FF),
    };
}

// Called by mod loader immediately after loading this mod.
//  This function should serve as the mod entry point.
//  All dependencies listed in config.json will already
//  be loaded in the process when this method is called.
GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* c_api) {

    // Construct C++ wrapper class
    GearLoader::Api gearLoaderApi(c_api, ctx);

    gearLoaderApi.RegisterApi(&_api, RESCRIPT_NAME, getSemVer());

    // Obtain dependency mod API from mod loader
    const CharacterFramework_Api* characterApi;
    SemanticVersion retrievedVer;
    int result = gearLoaderApi.RetrieveModApi<CharacterFramework_Api>(
        CHARACTER_FRAMEWORK_NAME,   // Mod name goes here. The BaseMod header has a macro for this.
        CHARACTER_FRAMEWORK_API_VERSION, // Version constraint. Again, the BaseMod provides a macro.
        &characterApi,   // retVal output. This is where the <BaseMod_Api> templated type comes in.
        &retrievedVer   // retVal output for api version
    );

    // If result is non-zero an error occurred (e.g. API not found).
    if (result > 0) {
        // Error handling
        return;
    }

    rescript_as::g_Context = { characterApi };

    std::cout << "[ReScript] Initialized" << std::endl;
}
