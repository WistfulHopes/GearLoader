#include "gameData.h"
#include "baseMod_p.h"
#include "offsets.h"


GGXXACPR_Entity* __stdcall GetPlayer(int player_index) {
    std::byte *address = getBaseAddress() + offsets::PLAYER_1_PTR;
    return *reinterpret_cast<GGXXACPR_Entity**>(address);
}
GGXXACPR_Camera* __stdcall GetCamera() {
    std::byte *address = getBaseAddress() + offsets::CAMERA;
    return reinterpret_cast<GGXXACPR_Camera*>(address);
}
int32_t __stdcall IsInGame() {
    std::byte *address = getBaseAddress() + offsets::IN_GAME_FLAG;
    uint8_t* flagPtr = reinterpret_cast<uint8_t*>(address);
    return (*flagPtr) != 0;
}
uint32_t __stdcall GetGameMode() {
    std::byte *address = getBaseAddress() + offsets::GAME_MODE;
    return *reinterpret_cast<uint32_t*>(address);
}
void* __stdcall GetD3D9Device() {
    return getBaseAddress() + offsets::DIRECT3D9_DEVICE;
}

const BaseMod_GameDataApi* GetGameDataApi() {
    static const BaseMod_GameDataApi _gameDataApi = {
        size: sizeof(BaseMod_GameDataApi),
        version: BASEMOD_API_VERSION_NUM,
        getPlayer: GetPlayer,
        getCamera: GetCamera,
        isInGame: IsInGame,
        getGameMode: GetGameMode,
        getD3D9Device: GetD3D9Device
    };

    return &_gameDataApi;
}
