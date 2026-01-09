#include "hookManager.h"
#include <atomic>
#include <vector>
#include "baseMod_p.h"
#include "managedHook.h"
#include "offsets.h"

// DEBUG temp
#include <string>
#include <cstdio>
#include <iostream>


static ManagedHookCallbacks<void, const BaseMod_HookContext*, const BaseMod_PeekMessageInfo*> _afterPeekMessageCallbacks;
static ManagedHookCallbacks<void, const BaseMod_HookContext*, const BaseMod_GameUpdateInfo*> _beforeGameUpdateCallbacks;
static ManagedHookCallbacks<void, const BaseMod_HookContext*, const BaseMod_GameUpdateInfo*> _afterGameUpdateCallbacks;
static ManagedHookCallbacks<void, const BaseMod_HookContext*, const BaseMod_DrawInfo*> _beforeEndSceneCallbacks;
static ManagedHookCallbacks<void, const BaseMod_HookContext*, const BaseMod_DrawInfo*> _beforePresentCallbacks;

static std::atomic<BaseMod_HookId> _nextId{1};


BaseMod_HookId __stdcall afterPeekMessage(BaseMod_PeekMessageHook hook, void* userData) {
    if (!hook) return 0;

    BaseMod_HookId id = _nextId.fetch_add(1, std::memory_order_relaxed);
    _afterPeekMessageCallbacks.registerHook(id, hook, userData);

    return id;
}

BaseMod_HookId __stdcall beforeGameUpdate(BaseMod_GameUpdateHook hook, void* userData) {
    if (!hook) return 0;

    BaseMod_HookId id = _nextId.fetch_add(1, std::memory_order_relaxed);
    _beforeGameUpdateCallbacks.registerHook(id, hook, userData);

    return id;
}

BaseMod_HookId __stdcall afterGameUpdate(BaseMod_GameUpdateHook hook, void* userData) {
    if (!hook) return 0;

    BaseMod_HookId id = _nextId.fetch_add(1, std::memory_order_relaxed);
    _afterGameUpdateCallbacks.registerHook(id, hook, userData);


    return id;
}

BaseMod_HookId __stdcall beforeEndScene(BaseMod_DrawHook hook, void* userData) {
    if (!hook) return 0;

    BaseMod_HookId id = _nextId.fetch_add(1, std::memory_order_relaxed);
    _beforeEndSceneCallbacks.registerHook(id, hook, userData);


    return id;
}

BaseMod_HookId __stdcall beforePresent(BaseMod_DrawHook hook, void* userData) {
    if (!hook) return 0;

    BaseMod_HookId id = _nextId.fetch_add(1, std::memory_order_relaxed);
    _beforePresentCallbacks.registerHook(id, hook, userData);


    return id;
}

// This could stand to be smarter
uint32_t __stdcall removeHook(BaseMod_HookId id) {
    bool success = _afterPeekMessageCallbacks.unregisterHook(id);
    if (success) return success;
    success = _beforeGameUpdateCallbacks.unregisterHook(id);
    if (success) return success;
    success = _afterGameUpdateCallbacks.unregisterHook(id);
    if (success) return success;
    success = _beforeEndSceneCallbacks.unregisterHook(id);
    if (success) return success;
    success = _beforePresentCallbacks.unregisterHook(id);

    return success;
}


BOOL WINAPI PeekMessageW_Wrapper(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
    BOOL result = PeekMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

    const BaseMod_HookContext ctx = { HookType::PEEK_MESSAGE };
    const BaseMod_PeekMessageArgs args = {
        (MSG*)lpMsg, (void*)hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg
    };
    const BaseMod_PeekMessageInfo info = {args, result};

    _afterPeekMessageCallbacks.invokeAll(&ctx, &info);

    return result;
}


static void(__stdcall *const NativeCommonSimUpdate)() =
    reinterpret_cast<void(__stdcall *)()>(getBaseAddress() + offsets::COMMON_SIM_UPDATE_FUNC);
void __stdcall CommonSimUpdateWrapper() {
    BaseMod_HookContext ctx = { HookType::UPDATE };
    // TODO: is there any info that should be passed?
    _beforeGameUpdateCallbacks.invokeAll(&ctx, nullptr);

    NativeCommonSimUpdate();

    _afterGameUpdateCallbacks.invokeAll(&ctx, nullptr);
}

static void* peekMessageHookOriginalBytes;
inline void InstallPeekMessageHook() {
    void* injectAddress = getBaseAddress() + offsets::PEEK_MESSAGE_FUNCTION_POINTER;
    void* hookAddress = reinterpret_cast<void*>(PeekMessageW_Wrapper);

    Patch(injectAddress, &hookAddress, sizeof(hookAddress), &peekMessageHookOriginalBytes);
}
static void* updateGameStateOriginalBytes;
inline void InstallGameUpdateHook() {
    void* injectAddress = getBaseAddress() + offsets::COMMON_SIM_UPDATE_FUNCTION_CALL;
    intptr_t hookAddress = reinterpret_cast<intptr_t>(CommonSimUpdateWrapper);

    // CallsiteAddr + 5 + relJumpOffset = FuncAddress
    // relJumpOffset = FuncAddress - CallsiteAddr - 5
    intptr_t relativeJump = hookAddress - reinterpret_cast<intptr_t>(injectAddress) - sizeof(injectAddress);

    Patch(injectAddress, &relativeJump, sizeof(relativeJump), &updateGameStateOriginalBytes);
}
inline void InstallEndSceneHook() {
    //const void* injectAddress = getBaseAddress() + offsets::
}
inline void InstallPresentHook() {
    //const void* injectAddress = getBaseAddress() + offsets::
}

void InstallHooks() {
    InstallPeekMessageHook();
    InstallGameUpdateHook();
    InstallEndSceneHook();
    InstallPresentHook();
}

static const BaseMod_HookApi _hookApi = {
    sizeof(BaseMod_HookApi),
    BASEMOD_API_VERSION_NUM,
    
    afterPeekMessage,
    beforeGameUpdate,
    afterGameUpdate,
    beforeEndScene,
    beforePresent,
    removeHook,
};
const BaseMod_HookApi* GetHookApi() { return &_hookApi; }
