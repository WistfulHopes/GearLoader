#include "gearLoader/gearLoader.hpp"
#include "baseMod/baseMod.hpp"
#include <d3d9.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include "testGraphics.h"

struct TestContext {
    GearLoader::Api* glApi;
    BaseMod::Api* bmApi;
};
static TestContext _testCtx;

inline std::string ToHexString(int value) {
    std::stringstream ss;
    ss << std::setfill('0') << std::setw(sizeof(int)*2) << std::uppercase << std::hex << value;
    return ss.str();
}

void __stdcall testPeekMessageHook(
    TestContext* uCtx,
    const BaseMod_HookContext* hCtx,
    const BaseMod_PeekMessageInfo* info
) {
    static bool tested = false;

    if (!tested) {
        uCtx->glApi->Log(GearLoader::LogLevel::DEBUG, "peekMessage hook called");
        tested = true;
    }
}
void __stdcall testBeforeGameUpdateHook(
    TestContext* uCtx,
    const BaseMod_HookContext* hCtx,
    const BaseMod_GameUpdateInfo* info
) {
    static bool tested = false;

    if (!tested) {
        uCtx->glApi->Log(GearLoader::LogLevel::DEBUG, "BeforeGameUpdate hook called");
        tested = true;
    }
}
void __stdcall testAfterGameUpdateHook(
    TestContext* uCtx,
    const BaseMod_HookContext* hCtx,
    const BaseMod_GameUpdateInfo* info
) {
    static bool tested = false;
    static int timer = 0;

    if (!tested) {
        uCtx->glApi->Log(GearLoader::LogLevel::DEBUG, "AfterGameUpdate hook called");
        tested = true;
    }

    if (
        uCtx->bmApi->GameData.IsInGame() &&
        uCtx->bmApi->GameData.GetJobMode() == ggxxacpr::JobMode::BATTLE
    ) {
        auto& api = uCtx->bmApi->NativeFunctions;
        auto& data = uCtx->bmApi->GameData;
        auto p1 = uCtx->bmApi->GameData.GetPlayer(0);
        auto p2 = uCtx->bmApi->GameData.GetPlayer(1);
        auto cam = uCtx->bmApi->GameData.GetCamera();

        api.RenderText("ACT ID: " + std::to_string(p1.actId()), 10, 10, 1.0f, 0xFF, 1.0f);
        api.RenderText("ACT TIMER: " + std::to_string(p1.actTimer()), 10, 30, 1.0f, 0xFF, 1.0f);
        api.RenderText("POS: (" + std::to_string(p1.position().x) + "," + std::to_string(p1.position().y) + ")", 10, 50, 1.0f, 0xFF, 1.0f);

        api.RenderText("ACT ID: " + std::to_string(p2.actId()), 340, 10, 1.0f, 0xFF, 1.0f);
        api.RenderText("ACT TIMER: " + std::to_string(p2.actTimer()), 340, 30, 1.0f, 0xFF, 1.0f);
        api.RenderText("POS: (" + std::to_string(p2.position().x) + "," + std::to_string(p1.position().y) + ")", 340, 50, 1.0f, 0xFF, 1.0f);

        api.RenderText("HEALTH: " + std::to_string(p1.health()), 10, 70, 1.0f, 0xFF, 1.0f);
        api.RenderText("TENSION: " + std::to_string(p1.tension()), 10, 90, 1.0f, 0xFF, 1.0f);
        api.RenderText("BURST: " + std::to_string(p1.burstMeter()), 10, 110, 1.0f, 0xFF, 1.0f);
        api.RenderText("ACT STATE: 0z" + ToHexString(static_cast<int>(p1.actionState())), 10, 130, 1.0f, 0xFF, 1.0f);
        api.RenderText("GRD STATE: 0z" + ToHexString(static_cast<int>(p1.guardState())), 10, 150, 1.0f, 0xFF, 1.0f);
        api.RenderText("ATK STATE: 0z" + ToHexString(static_cast<int>(p1.attackState())), 10, 170, 1.0f, 0xFF, 1.0f);

        auto p1Input = uCtx->bmApi->GameData.GetPlayerInput(0);
        api.RenderText("INPUT: 0z" + ToHexString(static_cast<int>(p1Input)), 10, 190, 1.0f, 0xFF, 1.0f);

        api.RenderText("CAM POS: (" + std::to_string(cam.position().x) + "," + std::to_string(cam.position().y) + ")", 10, 220, 1.0f, 0xFF, 1.0f);
        api.RenderText("CAM ZOOM: " + std::to_string(cam.zoom()), 10, 240, 1.0f, 0xFF, 1.0f);

        auto featureFlags = uCtx->bmApi->GameData.GetGameModeFeatureFlags();
        api.RenderText("MODE FLAGS: 0z" + ToHexString(static_cast<int>(featureFlags)), 10, 260, 1.0f, 0xFF, 1.0f);
        auto devicePointer = uCtx->bmApi->GameData.GetD3D9Device();
        api.RenderText("D3D DEVICE: 0z" + ToHexString(reinterpret_cast<int>(devicePointer)), 10, 280, 1.0f, 0xFF, 1.0f);

        if (timer == 0) {
            api.RenderPopUpText(0, "TEST P1");
        } else if (timer == 50) {
            api.RenderPopUpText(1, "TEST P2");
        }
    }

    timer = (timer + 1) % 100;
}
void __stdcall testEndSceneGraphicsHook(
    TestContext* uCtx,
    const BaseMod_HookContext* hCtx,
    const BaseMod_DrawInfo* info
) {
    static bool tested = false;
    IDirect3DDevice9* device = reinterpret_cast<IDirect3DDevice9*>(uCtx->bmApi->GameData.GetD3D9Device());
    HRESULT result;

    if (!tested) {
        D3DDEVICE_CREATION_PARAMETERS createParams;
        device->GetCreationParameters(&createParams);
        uCtx->glApi->Log(GearLoader::LogLevel::DEBUG, "EndScene graphics hook called");
        uCtx->glApi->Log(GearLoader::LogLevel::DEBUG, "device pointer from baseMod: 0x" + ToHexString(reinterpret_cast<int>(device)));
        uCtx->glApi->Log(GearLoader::LogLevel::DEBUG, "Device behavior flags: 0x" + ToHexString(createParams.BehaviorFlags));

        IDirect3DDevice9* device = reinterpret_cast<IDirect3DDevice9*>(uCtx->bmApi->GameData.GetD3D9Device());
        tested = true;
    }
    static D3DRECT clearRect = D3DRECT { 20, 20, 40, 40 };
    result = device->Clear(1, &clearRect, D3DCLEAR_TARGET, 0xFFFF00FF, 0.0f, 0);
    if (result != D3D_OK) std::cout << "IDirect3DDevice9::Clear failed in End Scene hook" << std::endl;
}
void __stdcall testPresentGraphicsHook(
    TestContext* uCtx,
    const BaseMod_HookContext* hCtx,
    const BaseMod_DrawInfo* info
) {
    static bool tested = false;

    if (!tested) {
        uCtx->glApi->Log(GearLoader::LogLevel::DEBUG, "Present graphics hook called");
        tested = true;
    }
    
    IDirect3DDevice9* device = reinterpret_cast<IDirect3DDevice9*>(uCtx->bmApi->GameData.GetD3D9Device());
    D3DVIEWPORT9 viewport { };
    HRESULT result = device->GetViewport(&viewport);
    if (result != D3D_OK) std::cout << "IDirect3DDevice9::GetViewport failed in Present hook" << std::endl;
    static D3DRECT clearRect = D3DRECT {
        static_cast<LONG>(viewport.Width - 40),
        static_cast<LONG>(viewport.Height - 40),
        static_cast<LONG>(viewport.Width - 20),
        static_cast<LONG>(viewport.Height - 20)
    };
    result = device->Clear(1, &clearRect, D3DCLEAR_TARGET, 0xFF00FFFF, 0.0f, 0);
    if (result != D3D_OK) std::cout << "IDirect3DDevice9::Clear failed in Present hook" << std::endl;
}

GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* c_api) {
    GearLoader::Api* glApi = new GearLoader::Api(c_api, ctx);

    const BaseMod_Api* bmApi_c;
    SemanticVersion retVer;
    int result = glApi->RetrieveModApi<BaseMod_Api>(
        BASEMOD_NAME,
        "=0.1.0",
        &bmApi_c,
        &retVer
    );

    if (result > 0) {
        glApi->Log(GearLoader::LogLevel::ERR, "[testMod] failed to retrieve baseMod api");
        return;
    }

    BaseMod::Api *bmApi = new BaseMod::Api(bmApi_c);
    _testCtx = { glApi, bmApi };

    std::stringstream ss1;
    ss1 << "d3d9 device ptr: 0x" << std::hex << bmApi->GameData.GetD3D9Device();
    glApi->Log(GearLoader::LogLevel::DEBUG, ss1.str());

    // TODO: test race condition by waiting
    using namespace std::chrono_literals;
    const auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(5000ms);

    // Test hooks
    bmApi->Hooks.AfterPeekMessage<TestContext>(testPeekMessageHook, &_testCtx);
    bmApi->Hooks.BeforeGameUpdate<TestContext>(testBeforeGameUpdateHook, &_testCtx);
    bmApi->Hooks.AfterGameUpdate<TestContext>(testAfterGameUpdateHook, &_testCtx);
    bmApi->Hooks.BeforeEndScene<TestContext>(testEndSceneGraphicsHook, &_testCtx);
    bmApi->Hooks.BeforePresent<TestContext>(testPresentGraphicsHook, &_testCtx);

    const auto end = std::chrono::high_resolution_clock::now();
    const std::chrono::duration<double, std::milli> elapsed = end - start;
    std::stringstream ss2;
    ss2 << "Time: " << elapsed.count() << "ms";
    glApi->Log(GearLoader::LogLevel::DEBUG, ss2.str());
}
