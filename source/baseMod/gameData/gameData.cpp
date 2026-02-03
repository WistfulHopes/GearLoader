#include "gameData.h"
#include "baseMod_p.h"
#include "offsets.h"


GGXXACPR_Entity* __stdcall GetPlayer(int playerIndex) {
    std::byte *address = getBaseAddress() + (playerIndex == 0 ?
        offsets::PLAYER_1_PTR :
        offsets::PLAYER_2_PTR);
    return *reinterpret_cast<GGXXACPR_Entity**>(address);
}
GGXXACPR_RawControllerInput __stdcall GetPlayerInput(int playerIndex) {
    std::byte *address = getBaseAddress() + offsets::PLAYER_INPUT + (playerIndex * 0xA8);
    return *reinterpret_cast<GGXXACPR_RawControllerInput*>(address);
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
int32_t* __stdcall GetJobMode() {
    std::byte *address = getBaseAddress() + offsets::JOB_MODE;
    return reinterpret_cast<int32_t*>(address);
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
        
        GetPlayer: GetPlayer,
        GetPlayerInput: GetPlayerInput,
        GetCamera: GetCamera,
        IsInGame: IsInGame,
        GetJobMode: GetJobMode,
        GetGameMode: GetGameMode,
        GetD3D9Device: GetD3D9Device
    };

    return &_gameDataApi;
}
