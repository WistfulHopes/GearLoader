#include "rescript_as.hpp"
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>

#include <cassert>
#include <iostream>
#include <ostream>

#include "safetyhook.hpp"
#include <Windows.h>

#include "character_framework.h"

namespace rescript_as {
    SafetyHookInline LoadObj_Hook;
    uintptr_t ModuleBase;

    void MessageCallback(const asSMessageInfo *msg, void *param) {
        std::string type = "ERR ";
        if (msg->type == asMSGTYPE_WARNING)
            type = "WARN";
        else if (msg->type == asMSGTYPE_INFORMATION)
            type = "INFO";

        std::cout << std::format("{} ({}, {}) : {} : {}\n", msg->section, msg->row, msg->col, type, msg->message) <<
                std::endl;
    }

    void DebugPrint(const std::string &in) {
        std::cout << "[ReScript DEBUG] " << in << std::endl;
    }

    void InitializeScriptEngine() {
        g_ScriptEngine = asCreateScriptEngine();

        int r = g_ScriptEngine->SetMessageCallback(asFUNCTION(MessageCallback), 0, asCALL_CDECL);
        assert(r >= 0);

        RegisterStdString(g_ScriptEngine);

        r = g_ScriptEngine->RegisterGlobalFunction("void DebugPrint(const string &in)", asFUNCTION(DebugPrint),
                                                   asCALL_CDECL);
        assert(r >= 0);
    }

    bool LoadScript(bool bIsP1, std::string &id) {
    }

    std::string CharaIdToStr(CharacterID id) {
        switch (id) {
            case CHRID_Sol:
                return "sl";
            case CHRID_Ky:
                return "ky";
            case CHRID_May:
                return "my";
        }
    }

    bool LoadObj_New() {
        const auto current_characters = reinterpret_cast<uint16_t *>(
            reinterpret_cast<uintptr_t>(ModuleBase) + 0x6d660c);
        const auto custom_characters = g_Context.character_framework_api->get_chara_ids();

        return LoadObj_Hook.call<bool>();
    }

    void InitializeHooks() {
        ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));

        LoadObj_Hook = safetyhook::create_inline((void *) (ModuleBase + 0x109e40), LoadObj_New);
    }
}
