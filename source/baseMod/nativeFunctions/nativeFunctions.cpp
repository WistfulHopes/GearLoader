#include "baseMod_p.h"
#include "offsets.h"


using nativeRenderText = void (*)(int32_t xPos, int32_t yPos, float zPos, uint8_t alpha, float size);
static nativeRenderText _nativeRenderText =
    reinterpret_cast<nativeRenderText>(getBaseAddress() + offsets::RENDER_TEXT);

void __stdcall RenderText(
    const char* text,
    int32_t xPos,
    int32_t yPos,
    float zPos,
    uint8_t alpha,
    float size) noexcept
{
    asm (
        ".intel_syntax noprefix\n"
        "mov eax, %[text]\n"
        ".att_syntax\n"
        :
        : [text] "r" (text)
        : "eax"
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
