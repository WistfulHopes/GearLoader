#pragma once

#include "baseMod/baseMod_c.h"
#include <cstddef>

std::byte* getBaseAddress();

enum class HookType {
    NONE,
    PEEK_MESSAGE,
    UPDATE,
    DRAW
};
struct BaseMod_HookContext {
    HookType type;
};
