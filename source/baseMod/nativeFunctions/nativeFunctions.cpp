#include "baseMod_p.h"
#include "offsets.h"


using NativeRenderText = void (*)(int32_t xPos, int32_t yPos, float zPos, uint8_t alpha, float size);
static NativeRenderText _nativeRenderText =
    reinterpret_cast<NativeRenderText>(getBaseAddress() + offsets::RENDER_TEXT);

/**
 *  \brief A wrapper function that invokes the native render text function.
 * 
 *  The native function was compiled to a non-standard calling convention.
 *      The text pointer is passed by register ECX while the rest are passed on the stack.
 *      A short assembly preamble is needed to invoke the function with this calling convention.
 */
uint32_t __stdcall RenderText(
    const char* text,
    int32_t xPos,
    int32_t yPos,
    float zPos,
    uint8_t alpha,
    float size) noexcept
{
    asm (
        "movl %0, %%ecx"
        : // no output
        : "r" (text)
        : "%ecx" // clobbered
    );
    _nativeRenderText(xPos, yPos, zPos, alpha, size);
    
    return 0;
}


const BaseMod_NativeFunctionsApi* GetNativeFunctionsApi() {
    static const BaseMod_NativeFunctionsApi _api = {
        size: sizeof(BaseMod_NativeFunctionsApi),
        version: BASEMOD_API_VERSION_NUM,
        RenderText: RenderText
    };

    return &_api;
}
