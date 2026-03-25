#include "baseMod_p.h"
#include <iostream>
#include <windows.h>
#include <cstddef>
#include "gearLoader/gearLoader_c.h"
#include "gearLoader/ggxxacpr_c.h"

#include "nativeFunctions/nativeFunctions.h"
#include "gameData/gameData.h"
#include "hookManager/hookManager.h"
#include "modMenu/modMenu.h"


static const SemanticVersion semVer = {0,1,0};

std::byte* getBaseAddress() {
    static std::byte* _baseAddress = reinterpret_cast<std::byte*>(GetModuleHandle(nullptr));
    return _baseAddress;
}

static const BaseMod_Api _api = {
    sizeof(BaseMod_Api),
    BASEMOD_API_VERSION_NUM,

    GetNativeFunctionsApi(),
    GetGameDataApi(),
    GetHookApi()
};

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* modLoaderApi) {
    InstallHooks();
    InstallModMenu();
    modLoaderApi->RegisterApi(ctx, &_api, BASEMOD_NAME, semVer);
    std::cout << "[baseMod] Initialized" << std::endl;
}
