#include "gearLoader/gearLoader.hpp"
#include "baseMod/baseMod.hpp"
#include <d3d9.h>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <thread>
#include "testGraphics.h"

static int displayData = 0;
static int textZPos = 5;
static BaseMod::Api* bmApi;
static int logFrameTime = 0;

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
        uCtx->bmApi->GameData.GetJobMode() == ggxxacpr::JobMode::BATTLE &&
        displayData > 0
    ) {
        constexpr int xPosLeftCol = 10;
        constexpr int xPosRightCol = 340;
        auto& api = uCtx->bmApi->NativeFunctions;
        auto& data = uCtx->bmApi->GameData;
        auto p1 = uCtx->bmApi->GameData.GetPlayer(0);
        auto p2 = uCtx->bmApi->GameData.GetPlayer(1);
        auto cam = uCtx->bmApi->GameData.GetCamera();
        float zPos = static_cast<float>(textZPos);

        api.RenderCockpitFontText("ACT ID: " + std::to_string(p1.actId()), xPosLeftCol, 10, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("ACT TIMER: " + std::to_string(p1.actTimer()), xPosLeftCol, 30, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("POS: (" + std::to_string(p1.position().x) + "," + std::to_string(p1.position().y) + ")", 10, 50, zPos, 0xFF, 1.0f);

        api.RenderCockpitFontText("ACT ID: " + std::to_string(p2.actId()), xPosRightCol, 10, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("ACT TIMER: " + std::to_string(p2.actTimer()), xPosRightCol, 30, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("POS: (" + std::to_string(p2.position().x) + "," + std::to_string(p1.position().y) + ")", xPosRightCol, 50, zPos, 0xFF, 1.0f);

        api.RenderCockpitFontText("HEALTH: " + std::to_string(p1.health()), xPosLeftCol, 70, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("TENSION: " + std::to_string(p1.tension()), xPosLeftCol, 90, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("BURST: " + std::to_string(p1.burstMeter()), xPosLeftCol, 110, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("ACT STATE: 0z" + ToHexString(static_cast<int>(p1.actionState())), xPosLeftCol, 130, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("GRD STATE: 0z" + ToHexString(static_cast<int>(p1.guardState())), xPosLeftCol, 150, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("ATK STATE: 0z" + ToHexString(static_cast<int>(p1.attackState())), xPosLeftCol, 170, zPos, 0xFF, 1.0f);

        auto p1Input = uCtx->bmApi->GameData.GetPlayerInput(0);
        api.RenderCockpitFontText("INPUT: 0z" + ToHexString(static_cast<int>(p1Input)), xPosLeftCol, 190, zPos, 0xFF, 1.0f);

        api.RenderCockpitFontText("CAM POS: (" + std::to_string(cam.position().x) + "," + std::to_string(cam.position().y) + ")", xPosLeftCol, 220, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("CAM ZOOM: " + std::to_string(cam.zoom()), xPosLeftCol, 240, zPos, 0xFF, 1.0f);

        auto featureFlags = uCtx->bmApi->GameData.GetGameModeFeatureFlags();
        api.RenderCockpitFontText("MODE FLAGS: 0z" + ToHexString(static_cast<int>(featureFlags)), xPosLeftCol, 260, zPos, 0xFF, 1.0f);
        auto devicePointer = uCtx->bmApi->GameData.GetD3D9Device();
        api.RenderCockpitFontText("D3D DEVICE: 0z" + ToHexString(reinterpret_cast<int>(devicePointer)), xPosLeftCol, 280, zPos, 0xFF, 1.0f);

        auto playerInputArr = bmApi->GameData.GetPlayerInputStructArr();
        
        api.RenderCockpitFontText("INPUT RAW2: 0z" + ToHexString(static_cast<int>(playerInputArr[0].InputRaw2)), xPosRightCol, 100, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("INPUT RAW1: 0z" + ToHexString(static_cast<int>(playerInputArr[0].InputRaw1)), xPosRightCol, 120, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("INPUT RELEASE: 0z" + ToHexString(static_cast<int>(playerInputArr[0].InputRelease)), xPosRightCol, 140, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("INPUT PRESS1: 0z" + ToHexString(static_cast<int>(playerInputArr[0].InputPress1)), xPosRightCol, 160, zPos, 0xFF, 1.0f);
        api.RenderCockpitFontText("INPUT PRESS2: 0z" + ToHexString(static_cast<int>(playerInputArr[0].InputPress2)), xPosRightCol, 180, zPos, 0xFF, 1.0f);

        api.RenderMenuText("Menu Text test", xPosRightCol, 200, zPos, 1.0f, nullptr, 0, true, 0xFFFFFF);
        api.RenderMenuText("Colorful text", xPosRightCol, 220, zPos, 1.0f, nullptr, 0, false, 0xFF8080);
        api.RenderMenuTextCenterAligned("Centered\nText Test!", xPosRightCol + 60, 240, zPos, 1.0f, 0xFFFFFF, true);

        api.DrawQuad(610, 10, 630, 30, 1, 0x80FF0000);
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

    // Frame time
    static int count = 0;
    static std::chrono::duration<double, std::milli> frameTime;
    static std::chrono::system_clock::time_point lastTimeStamp;
    static double rollingSum = 0.0f;
    
    auto now = std::chrono::high_resolution_clock::now();
    frameTime = now - lastTimeStamp;
    rollingSum += frameTime.count();
    
    if ((count % 60) == 59 ) {
        if (logFrameTime != 0) std::cout << "Avg frame time: " << (rollingSum / 60.0f) << "ms" << std::endl;
        rollingSum = 0.0f;
    }
    
    lastTimeStamp = std::chrono::high_resolution_clock::now();
    count++;
    
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

void BASEMOD_CALL TestCommand() {
    std::cout << "TEST COMMAND" << std::endl;
}
void BASEMOD_CALL TriggerPopUp() {
    bmApi->NativeFunctions.RenderPopUpText(0, "POPUP TEST 1");
    bmApi->NativeFunctions.RenderPopUpText(1, "POPUP TEST 2");
}
static int sfxIndex = 0;
void BASEMOD_CALL PlaySFX() {
    bmApi->NativeFunctions.PlayCommonSoundEffect(sfxIndex);
}

void RegisterModMenu(BaseMod::ModMenuApi& api) {
    static int testEnumVal = 0;
    static const char*testEnumValLabels[3] = {
        "VALUE 1", "VALUE 2", "VALUE 3"
    };
    static int testNum = 0;
    static BaseMod::ModMenuEntry entries[10] {
        {"Command Test", nullptr, 0, 0, nullptr, TestCommand},
        {"Enum Test", &testEnumVal, 0, 2, testEnumValLabels, nullptr},
        {"Number Test", &testNum, 0, 100, nullptr, nullptr},
        {"Header", nullptr, 0, 0, nullptr, nullptr},
        {"  Indent 1", nullptr, 0, 0, nullptr, TestCommand},
        {"  Indent 2", nullptr, 0, 0, nullptr, TestCommand},
        {"  Indent 3", nullptr, 0, 0, nullptr, TestCommand},
        {"Scroll Test 1", nullptr, 0, 0, nullptr, TestCommand},
        {"Scroll Test 2", nullptr, 0, 0, nullptr, TestCommand},
        {"Scroll Test 3", nullptr, 0, 0, nullptr, TestCommand},
    };

    static const char* boolLabels[2] = {"OFF", "ON"};
    static BaseMod::ModMenuEntry testerEntries[5] {
        {"Print frame times", &logFrameTime, 0, 1, boolLabels, nullptr},
        {"Display Data", &displayData, 0, 1, boolLabels, nullptr},
        {"Data Text Z Pos", &textZPos, 0, 255, nullptr, nullptr},
        {"Trigger Pop-up", nullptr, 0,0, nullptr, TriggerPopUp},
        {"SFX Test", &sfxIndex, 0, 104, nullptr, PlaySFX},
    };

    api.RegisterMenuTab("TESTER", testerEntries, 5);
    api.RegisterMenuTab("TEST TAB 1", entries, 3);
    api.RegisterMenuTab("TEST TAB 2", entries, 10);
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

    bmApi = new BaseMod::Api(bmApi_c);
    _testCtx = { glApi, bmApi };

    std::stringstream ss1;
    ss1 << "d3d9 device ptr: 0x" << std::hex << bmApi->GameData.GetD3D9Device();
    glApi->Log(GearLoader::LogLevel::DEBUG, ss1.str());

    // TODO: test race condition by waiting
    using namespace std::chrono_literals;
    const auto start = std::chrono::high_resolution_clock::now();
    std::this_thread::sleep_for(1000ms);

    // Test Mod Menu stuff
    RegisterModMenu(bmApi->ModMenu);

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
