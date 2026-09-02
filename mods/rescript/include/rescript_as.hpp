#pragma once

#include <angelscript.h>

struct CharacterFramework_Api;

namespace rescript_as {
    struct ReScriptContext {
        const CharacterFramework_Api* character_framework_api;
    };
    static ReScriptContext g_Context;

    asIScriptEngine* g_ScriptEngine;

    void InitializeScriptEngine();
}
