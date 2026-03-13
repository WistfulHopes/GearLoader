#include "baseMod_p.h"
#include "offsets.h"

/**
 *  Native Functions
 * 
 *  The internal functions of the game code are compiled to non-standard calling conventions
 *      and require hand-written assembly headers to invoke. The functions written in this file
 *      are minimal wrapper functions that only exist to adapt the calling convention.
 * 
 *  See `BaseMod_NativeFunctionsApi` for documentation on the native functions themselves.
 */

using NativeRenderText = void (__stdcall *)(int32_t xPos, int32_t yPos, float zPos, uint8_t alpha, float size);
static NativeRenderText _nativeRenderText =
    reinterpret_cast<NativeRenderText>(getBaseAddress() + offsets::RENDER_TEXT);
/**
 *  \param text ECX
 */
uint32_t __stdcall RenderText(
    const char* text,
    int32_t xPos,
    int32_t yPos,
    float zPos,
    uint8_t alpha,
    float size) noexcept
{
    // TODO: manually invoke `_nativeRenderText` via asm. ECX could
    //  potentially get clobbered by the compiler's invocation.
    asm (
        "movl %0, %%ecx"
        : // no output
        : "r" (text)
        : "%ecx" // clobbered
    );
    _nativeRenderText(xPos, yPos, zPos, alpha, size);
    
    return 0;
}

using NativeRenderPopUpText = void (__stdcall *)();
static NativeRenderPopUpText _nativeRenderPopUpText =
    reinterpret_cast<NativeRenderPopUpText>(getBaseAddress() + offsets::RENDER_POPUP_TEXT);
/**
 *  \param playerIndex EAX
 *  \param text ESI
 */
uint32_t __stdcall RenderPopUpText(int playerIndex, const char* text) {
    asm (
        "movl %0, %%eax\n\t"
        "movl %1, %%esi\n\t"
        "call %2"
        : // no output
        : "r" (playerIndex),
          "r" (text),
          "r" (_nativeRenderPopUpText)
        : "%eax", "%esi" // clobbered
    );

    return 0;
}


const BaseMod_NativeFunctionsApi* GetNativeFunctionsApi() {
    static const BaseMod_NativeFunctionsApi _api = {
        size: sizeof(BaseMod_NativeFunctionsApi),
        version: BASEMOD_API_VERSION_NUM,
        RenderText: RenderText,
        RenderPopUpText: RenderPopUpText,
    };

    return &_api;
}
