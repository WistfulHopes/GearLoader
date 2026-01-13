#pragma once

#include <windows.h>
#include "baseMod_p.h"
#include "gearLoader/gearLoader_c.h"

const BaseMod_HookApi* GetHookApi();
void InstallHooks();


inline void Patch(void* address, void* data, size_t size, void* overWrittenBytes) {
    // pause main thread?

    DWORD oldProtect;
    WINBOOL success = VirtualProtect(address, size, PAGE_EXECUTE_READWRITE, &oldProtect);

    if (!success) {
        //err handling
    }

    memcpy(overWrittenBytes, address, size);
    memcpy(address, data, size);

    success = VirtualProtect(address, size, oldProtect, &oldProtect);

    if (!success) {
        //err handling
    }
}

struct PeekMessageHookEntry {
    BaseMod_HookId id;
    BaseMod_PeekMessageHook hook;
    void* hookContext;
};
struct GameUpdateHookEntry {
    BaseMod_HookId id;
    BaseMod_GameUpdateHook hook;
    void* hookContext;
};
struct DrawHookEntry {
    BaseMod_HookId id;
    BaseMod_DrawHook hook;
    void* hookContext;
};
