#include "baseMod_p.h"
#include "offsets.h"


using nativeRenderText = void (*)(int32_t xPos, int32_t yPos, float zPos, uint8_t alpha, float size);
static nativeRenderText _nativeRenderText =
    reinterpret_cast<nativeRenderText>(getBaseAddress() + offsets::RENDER_TEXT);

/**
 *  \brief A wrapper function that invokes the native render text function.
 * 
 *  The native function was optimized when compiled to a non-standard calling convention.
 *      The text pointer is passed by register EAX while the rest are passed on the stack.
 *      Hand-written assembly is needed to invoke the function with this calling convention.
 */
void __stdcall RenderText(
    const char* text,
    int32_t xPos,
    int32_t yPos,
    float zPos,
    uint8_t alpha,
    float size) noexcept
{
    asm (
        "movl %0, %%ecx"
        :   // no output
        : "r" (text)
        : "%ecx" // clobbered
    );
    _nativeRenderText(xPos, yPos, zPos, alpha, size);
}


const BaseMod_NativeFunctionsApi* GetNativeFunctionsApi() {
    static const BaseMod_NativeFunctionsApi _api = {
        size: sizeof(BaseMod_NativeFunctionsApi),
        version: BASEMOD_API_VERSION_NUM,
        renderText: RenderText
    };

    return &_api;
}
