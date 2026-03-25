#include "nativeFunctions.h"
#include <cstdint>
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

uint32_t __stdcall RenderText(
    const char* text,
    int32_t xPos,
    int32_t yPos,
    float zPos,
    uint8_t alpha,
    float size) noexcept
{
    asm (
        "push %[aSize]\n\t"
        "push %[aAlpha]\n\t"
        "push %[aZPos]\n\t"
        "push %[aYPos]\n\t"
        "push %[aXPos]\n\t"
        "call *%[fn]\n\t"
        "addl $24, %%esp"
        : // no output
        : [fn] "r" (_nativeRenderText),
          [aXPos] "g" (xPos),
          [aYPos] "g" (yPos),
          [aZPos] "g" (zPos),
          [aAlpha] "g" (alpha),
          [aSize] "g" (size),
          "c" (text) // ECX
        : "memory", "cc" // clobbered
    );
    
    return 0;
}

using NativeRenderPopUpText = void (__stdcall *)();
static NativeRenderPopUpText _nativeRenderPopUpText =
    reinterpret_cast<NativeRenderPopUpText>(getBaseAddress() + offsets::RENDER_POPUP_TEXT);

uint32_t __stdcall RenderPopUpText(int playerIndex, const char* text) {
    asm (
        "call *%[fn]"
        : // no output
        : [fn] "r" (_nativeRenderPopUpText),
          "a" (playerIndex),    // EAX
          "S" (text)            // ESI
        : "cc" // clobbered
    );

    return 0;
}

uint32_t __stdcall PlayCommonSoundEffect(uint32_t id) {
    uint32_t output;
    asm(
        "call *%[fn]"
        : "=a" (output)
        : [fn] "r" (getBaseAddress() + offsets::PLAY_SOUND_EFFECT_OFFSET),
          "S" (id)  // ESI
        : "cc" // clobbered
    );
    return output;
}

uint32_t __stdcall DrawSprite(GGXXACPR_DrawSpriteParams* params, int32_t flag) {
    asm(
        "push %[aFlag]\n\t"
        "call *%[fn]\n\t"
        "addl $4, %%esp"
        : // no output
        : [fn] "r" (getBaseAddress() + offsets::DRAW_SPRITE_OFFSET),
        [aFlag] "r" (flag),
        "c" (params) // ECX
        : "memory", "cc" // clobbered
    );
    return 0;
}

using NativeDrawQuad = void (__stdcall *)(int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t zPos, uint32_t color);

const BaseMod_NativeFunctionsApi* GetNativeFunctionsApi() {
    static const BaseMod_NativeFunctionsApi _api = {
        size: sizeof(BaseMod_NativeFunctionsApi),
        version: BASEMOD_API_VERSION_NUM,
        RenderText: RenderText,
        RenderPopUpText: RenderPopUpText,
        PlayCommonSoundEffect: PlayCommonSoundEffect,
        DrawSprite: DrawSprite,
        DrawQuad: reinterpret_cast<NativeDrawQuad>(getBaseAddress() + offsets::DRAW_QUAD),
    };

    return &_api;
}
