#pragma once

#include "baseMod_p.h"
#include <cstdint>
#include "gearLoader/ggxxacpr_c.h"
#include "offsets.h"

const BaseMod_NativeFunctionsApi* GetNativeFunctionsApi();

// Native functions for internal use
// uint32_t __stdcall PlayCommonSoundEffect(uint32_t id);
// uint32_t __stdcall DrawSprite(GGXXACPR_DrawSpriteParams* params, int32_t flag);
// uint32_t __stdcall RenderText(const char* text, int32_t xPos, int32_t yPos, float zPos, uint8_t alpha, float size) noexcept;

// Internal only:

inline bool does_fiber_exist(const char* name) {
    uint32_t output;
    asm(
        "call *%[fn]"
        : "=a" (output)
        : [fn] "r" (getBaseAddress() + offsets::DOES_FIBER_EXIST_FUNC_OFFSET),
          "a" (name)
        : "cc" // clobbered
    );
    return output;
}
// create_fiber
using create_fiber_t = GGXXACPR_FiberData* (__stdcall*)(
    void* entry_fiber,
    int size,
    int param_3,
    int param_4,
    const char* name);
inline GGXXACPR_FiberData* create_fiber(
    void* entry_fiber,
    int size,
    int param_3,
    int param_4,
    const char* name
) {
    return reinterpret_cast<create_fiber_t>(getBaseAddress() + offsets::CREATE_FIBER_FUNC_OFFSET)(
        entry_fiber, size, param_3, param_4, name);
}

inline uint32_t check_p1_or_p2_for_input(GGXXACPR_RawControllerInput input) {
    uint32_t output;
    asm(
        "call *%[fn]"
        : "=a" (output)
        : [fn] "r" (getBaseAddress() + offsets::CHECK_INPUT_P1DOP2P_FUNC_OFFSET),
          "c" (input)   // ECX
        : "cc" // clobbered
    );
    return output;
}
// create_help_and_options_fiber
inline void create_help_and_options_fiber() {
    reinterpret_cast<void (__stdcall*)()>(getBaseAddress() + offsets::CREATE_HELP_AND_OPTIONS_FIBER_FUNC_OFFSET)();
}
// get_string
inline const char* get_string(GGXXACPR_LocaleState* dictionary, int id) {
    const char* output;
    asm(
        "call *%[fn]"
        : "=a" (output)
        : [fn] "r" (getBaseAddress() + offsets::GET_STRING_FUNC_OFFSET),
          "a" (dictionary), // EAX
          "d" (id)          // EDX
        : "cc" // clobbered
    );
    return output;
}
using draw_menu_header_font_t = void (__stdcall*)(const char*, float, float, float, uint32_t, uint32_t, float);
inline void draw_menu_header_font(
    const char* text,
    float x,
    float y,
    float z,
    uint32_t alpha,
    uint32_t fontId,
    float size
) {
    reinterpret_cast<draw_menu_header_font_t>(
        getBaseAddress() + offsets::DRAW_MENU_HEADER_FONT_FUNC
        )(text, x, y, z, alpha, fontId, size);
}
inline void draw_menu_item_font(
    const char* text,
    int xPos,
    int yPos,
    float zPos,
    float alpha,
    int* buffer,
    int step,
    int param_8,
    int color
) {
    asm(
        "push %[aParam_8]\n\t"
        "push %[aStep]\n\t"
        "push %[aBuffer]\n\t"
        "push %[aAlpha]\n\t"
        "push %[aZPos]\n\t"
        "push %[aYPos]\n\t"
        "push %[aXPos]\n\t"
        "push %[aText]\n\t"
        "call *%[func]\n\t"
        "addl $32, %%esp"
        : // no output
        : [func] "rV" (getBaseAddress() + offsets::DRAW_MENU_ITEM_FONT_FUNC_OFFSET),
          [aText] "g" (text),
          [aXPos] "g" (xPos),
          [aYPos] "g" (yPos),
          [aZPos] "g" (zPos),
          [aAlpha] "g" (alpha),
          [aBuffer] "g" (buffer),
          [aStep] "g" (step),
          [aParam_8] "g" (param_8),
          "a" (color)  // eax
        : "memory", "cc"
    );
}

using draw_menu_arrow_t = void (__stdcall*)(uint32_t, int32_t, int32_t, int32_t, int32_t);
inline void draw_menu_arrow(uint32_t flags, int32_t x, int32_t y, int32_t z, int32_t translation) {
    reinterpret_cast<draw_menu_arrow_t>(getBaseAddress() + offsets::DRAW_MENU_ARROW)(
        flags, x, y, z, translation
    );
}

struct Native_MenuEntry {
    const char* Label;
    int32_t xOffset;
    int32_t yOffset;
    int32_t Value;
    uint32_t NumValues;
    const char** ValueLabels;
};
inline void draw_gauge_setting_ui(int32_t yPos, Native_MenuEntry* entry, uint8_t alpha, int32_t maxValue) {
    asm(
        "push %[aMaxValue]\n\t"
        "push %[aAlpha]\n\t"
        "push %[aEntry]\n\t"
        "call *%[fn]\n\t"
        "addl $12, %%esp"
        : // no output
        : [fn] "r" (getBaseAddress() + offsets::DRAW_GAUGE_SETTING_UI),
          [aEntry] "g" (entry),
          [aAlpha] "g" (alpha),
          [aMaxValue] "g" (maxValue),
          "c" (yPos) // ECX
        : "memory", "cc" // clobbered
    );
}

inline void modify_string(char* buffer) {
    asm(
        "call *%[fn]"
        : // no output
        : [fn] "r" (getBaseAddress() + offsets::MODIFY_STRING_FN),
           "c" (buffer) // ECX
        : "memory"
    );
}

// internal version
inline void DrawQuad(int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t zPos, uint32_t color) {
    reinterpret_cast<
        void (__stdcall *)(int32_t left, int32_t top, int32_t right, int32_t bottom, int32_t zPos, uint32_t color)>
        (getBaseAddress() + offsets::DRAW_QUAD)(
            left, top, right, bottom, zPos, color);
}