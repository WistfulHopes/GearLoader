#include "nativeFunctions.h"
#include <cstdint>
#include "baseMod_p.h"
#include "offsets.h"

/**
 *  Native Functions
 * 
 *  The internal functions of the game code are compiled to non-standard calling conventions
 *      and require hand-written assembly to invoke. The functions written in this file
 *      are minimal wrapper functions that only exist to adapt the calling convention.
 * 
 *  See `BaseMod_NativeFunctionsApi` for documentation on the native functions themselves.
 */

using NativeRenderCockpitFontText = void (__stdcall *)(int32_t xPos, int32_t yPos, float zPos, uint8_t alpha, float size);
static NativeRenderCockpitFontText _nativeRenderCockpitFontText =
    reinterpret_cast<NativeRenderCockpitFontText>(getBaseAddress() + offsets::RENDER_COCKPIT_FONT_TEXT);

uint32_t BASEMOD_CALL RenderCockpitFontText(
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
        : [fn] "g" (_nativeRenderCockpitFontText),
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

uint32_t BASEMOD_CALL RenderMenuText(
    const char* text,
    int32_t xPos,
    int32_t yPos,
    float zPos,
    float alpha,
    int32_t* animCounter,
    int32_t animSpeed,
    int32_t ignoreSpriteMask,
    uint32_t color
) {
    asm(
        "push %[aIgnoreSpriteMask]\n\t"
        "push %[aSpeed]\n\t"
        "push %[aCounter]\n\t"
        "push %[aAlpha]\n\t"
        "push %[aZPos]\n\t"
        "push %[aYPos]\n\t"
        "push %[aXPos]\n\t"
        "push %[aText]\n\t"
        "call *%[func]\n\t"
        "addl $32, %%esp"
        : // no output
        : [func] "g" (getBaseAddress() + offsets::DRAW_MENU_TEXT_FN),
          [aText] "g" (text),
          [aXPos] "g" (xPos),
          [aYPos] "g" (yPos),
          [aZPos] "g" (zPos),
          [aAlpha] "g" (alpha),
          [aCounter] "g" (animCounter),
          [aSpeed] "g" (animSpeed),
          [aIgnoreSpriteMask] "g" (ignoreSpriteMask),
          "a" (color)  // eax
        : "memory", "cc"
    );
    return 0;
}

uint32_t BASEMOD_CALL RenderMenuTextCenterAligned(
    const char* text,
    int32_t xPos,
    int32_t yPos,
    float zPos,
    float alpha,
    uint32_t color,
    int32_t ignoreSpriteMask
) {
    asm(
        "push %[aIgnoreSpriteMask]\n\t"
        "push %[aColor]\n\t"
        "push %[aAlpha]\n\t"
        "push %[aZPos]\n\t"
        "push %[aYPos]\n\t"
        "push %[aXPos]\n\t"
        "call *%[func]\n\t"
        "addl $32, %%esp"
        : // no output
        : [func] "g" (getBaseAddress() + offsets::DRAW_MENU_TEXT_CENTERED_FN),
          [aXPos] "g" (xPos),
          [aYPos] "g" (yPos),
          [aZPos] "g" (zPos),
          [aAlpha] "g" (alpha),
          [aColor] "g" (color),
          [aIgnoreSpriteMask] "g" (ignoreSpriteMask),
          "a" (text)  // eax
        : "memory", "cc"
    );
    return 0;
}

using NativeRenderPopUpText = void (__stdcall *)();
static NativeRenderPopUpText _nativeRenderPopUpText =
    reinterpret_cast<NativeRenderPopUpText>(getBaseAddress() + offsets::RENDER_POPUP_TEXT);

uint32_t BASEMOD_CALL RenderPopUpText(int playerIndex, const char* text) {
    asm (
        "call *%[fn]"
        : // no output
        : [fn] "g" (_nativeRenderPopUpText),
          "a" (playerIndex),    // EAX
          "S" (text)            // ESI
        : "cc" // clobbered
    );

    return 0;
}

uint32_t BASEMOD_CALL PlayCommonSoundEffect(uint32_t id) {
    uint32_t output;
    asm(
        "call *%[fn]"
        : "=a" (output)
        : [fn] "g" (getBaseAddress() + offsets::PLAY_SOUND_EFFECT_OFFSET),
          "S" (id)  // ESI
        : "cc" // clobbered
    );
    return output;
}

uint32_t BASEMOD_CALL DrawSprite(GGXXACPR_DrawSpriteParams* params, int32_t ignoreMask) {
    asm(
        "push %[aIgnoreMask]\n\t"
        "call *%[fn]\n\t"
        "addl $4, %%esp"
        : // no output
        : [fn] "g" (getBaseAddress() + offsets::DRAW_SPRITE_OFFSET),
          [aIgnoreMask] "g" (ignoreMask),
          "c" (params) // ECX
        : "memory", "cc" // clobbered
    );
    return 0;
}

using NativeDrawQuad = void (__stdcall *)(int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t zPos, uint32_t color);
uint32_t BASEMOD_CALL DrawQuad(int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t zPos, uint32_t color) {
    static NativeDrawQuad _native = reinterpret_cast<NativeDrawQuad>(getBaseAddress() + offsets::DRAW_QUAD);
    _native(left, top, right, bottom, zPos, color);
    return 0;
}

const BaseMod_NativeFunctionsApi* GetNativeFunctionsApi() {
    static const BaseMod_NativeFunctionsApi _api = {
        size: sizeof(BaseMod_NativeFunctionsApi),
        version: BASEMOD_API_VERSION_NUM,
        // v0.1.0
        RenderCockpitFontText: RenderCockpitFontText,
        // v0.2.0
        RenderMenuText: RenderMenuText,
        RenderMenuTextCenterAligned: RenderMenuTextCenterAligned,
        RenderPopUpText: RenderPopUpText,
        PlayCommonSoundEffect: PlayCommonSoundEffect,
        DrawSprite: DrawSprite,
        DrawQuad: DrawQuad,
    };

    return &_api;
}
