#include "hookManager.h"
#include <atomic>
#include <d3d9.h>
#include <vector>
#include "baseMod_p.h"
#include "managedHook.h"
#include "offsets.h"


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


BOOL WINAPI PeekMessageWWrapper(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax, UINT wRemoveMsg) {
    BOOL result = PeekMessageW(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg);

    const BaseMod_HookContext ctx = { HookType::PEEK_MESSAGE };
    const BaseMod_PeekMessageArgs args = {
        (MSG*)lpMsg, (void*)hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg
    };
    const BaseMod_PeekMessageInfo info = {args, result};

    _afterPeekMessageCallbacks.invokeAll(&ctx, &info);

    return result;
}
static void* peekMessageHookOriginalBytes;
inline void InstallPeekMessageHook() {
    void* injectAddress = getBaseAddress() + offsets::PEEK_MESSAGE_FUNCTION_POINTER;
    void* hookAddress = reinterpret_cast<void*>(PeekMessageWWrapper);
    
    Patch(injectAddress, &hookAddress, sizeof(hookAddress), &peekMessageHookOriginalBytes);
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
static void* updateGameStateOriginalBytes;
inline void InstallGameUpdateHook() {
    void* injectAddress = getBaseAddress() + offsets::COMMON_SIM_UPDATE_FUNCTION_CALL + 1;
    intptr_t hookAddress = reinterpret_cast<intptr_t>(CommonSimUpdateWrapper);

    // CallsiteAddr + instructionSize + relJumpOffset = FuncAddress
    // relJumpOffset = FuncAddress - CallsiteAddr - instructionSize
    intptr_t relativeJump = hookAddress - reinterpret_cast<intptr_t>(injectAddress) - sizeof(injectAddress);

    Patch(injectAddress, &relativeJump, sizeof(relativeJump), &updateGameStateOriginalBytes);
}

static void (__stdcall *const NativeSetGraphicsContext)() =
    reinterpret_cast<void(__stdcall *)()>(getBaseAddress() + offsets::SET_GRAPHICS_CONTEXT_FUNC);
static IDirect3DDevice9* _nativeDevice = *reinterpret_cast<IDirect3DDevice9**>(getBaseAddress() + offsets::DIRECT3D9_DEVICE);
void __stdcall SetGraphicsContextWrapper() {
    NativeSetGraphicsContext();

    BaseMod_HookContext ctx = { HookType::DRAW };
    BaseMod_DrawInfo info = { _nativeDevice };
    _beforeEndSceneCallbacks.invokeAll(&ctx, &info);
}

static void* setGraphicsContextCallOriginalBytes;
inline void InstallEndSceneHook() {
    // This inject address is a CALL instruction of a function called right before EndScene.
    //  this hook will replace the function called here with a wrapper function (`SetGraphicsContextWrapper`).
    //  +1 to skip over the opcode so we have the address of the operand.
    void* injectAddress = getBaseAddress() + offsets::SET_GRAPHICS_CONTEXT_CALL + 1;
    intptr_t hookAddress = reinterpret_cast<intptr_t>(SetGraphicsContextWrapper);

    // The operand we're overwriting is a relative address, so we need to calculate the new offset here:
    //      CallsiteAddr + instructionSize + relJumpOffset = FuncAddress
    //      relJumpOffset = FuncAddress - CallsiteAddr - instructionSize
    intptr_t relativeJump = hookAddress - reinterpret_cast<intptr_t>(injectAddress) - sizeof(injectAddress);

    Patch(injectAddress, &relativeJump, sizeof(relativeJump), &setGraphicsContextCallOriginalBytes);
}

HRESULT __stdcall PresentWrapper(IDirect3DDevice9* dummyDevice, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion) {
    static IDirect3DDevice9* nativeDevice = *reinterpret_cast<IDirect3DDevice9**>(getBaseAddress() + offsets::DIRECT3D9_DEVICE);
    static BaseMod_HookContext ctx = { HookType::DRAW };
    static BaseMod_DrawInfo info = { nativeDevice };

    _beforePresentCallbacks.invokeAll(&ctx, &info);
    
    return nativeDevice->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
}
// This is a fake IDirect3DDevice9 struct that will be inserted before the Present call.
//  In the code where this will be inserted, only the Present function is referenced.
typedef HRESULT (__stdcall *D3D9Present)(IDirect3DDevice9* device, const RECT* pSourceRect, const RECT* pDestRect, HWND hDestWindowOverride, const RGNDATA* pDirtyRegion);
struct DummyD3D9DeviceVTable {
    std::byte _padding[0x44];
    D3D9Present Present;
};
struct DummyD3D9Device {
    DummyD3D9DeviceVTable* VTable;
};
static IDirect3DDevice9* _originalDevicePtr;
inline void InstallPresentHook() {
    static DummyD3D9DeviceVTable _dummyVTable { Present: PresentWrapper };
    static DummyD3D9Device _dummyDevice = { VTable: &_dummyVTable };
    static DummyD3D9Device* _pDummyDevice = &_dummyDevice;
    static DummyD3D9Device** _payload = &_pDummyDevice;

    // This inject address is an instruction that loads the d3d9 device.
    //  +1 to skip over the opcode so we have the address of the operand we want to overwrite.
    void* injectAddress = getBaseAddress() + offsets::BEFORE_PRESENT_DEVICE_REFERENCE + 1;

    // Replacing the absolute address of the d3d9 device with the address of our dummy struct.
    Patch(injectAddress, &_payload, sizeof(&_payload), &_originalDevicePtr);
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
