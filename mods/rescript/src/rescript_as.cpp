#include "rescript_as.hpp"
#include <scriptstdstring/scriptstdstring.h>
#include <scriptbuilder/scriptbuilder.h>

#include <cassert>
#include <iostream>
#include <ostream>

#include "safetyhook.hpp"
#include <Windows.h>

#include "character_framework.h"
#include "file.hpp"

namespace rescript_as {
    static SafetyHookInline LoadObj_Hook;
    static uintptr_t ModuleBase;
    static bool g_ScriptLoaded[2][2]{};

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

    bool LoadScript(asIScriptModule *module, int type, std::string &id) {
        std::vector<char> script_buffer{};
        if (!read_binary_file(("Resource/ReScript/" + id + "_" + (type == 0 ? "ply" : "obj") + ".as").c_str(), script_buffer)) {
            return false;
        }

        CMemoryStream stream(script_buffer.data(), script_buffer.size());

        if (int result = module->LoadByteCode(&stream); result < 0) {
            DebugPrint("Failed to load script!");
            return false;
        }

        return true;
    }

    std::string CharaIdToStr(CharacterID id) {
        switch (id) {
            case CHRID_Sol:
                return "sl";
            case CHRID_Ky:
                return "ky";
            case CHRID_May:
                return "my";
            case CHRID_Millia:
                return "ml";
            case CHRID_Axl:
                return "ax";
            case CHRID_Potemkin:
                return "po";
            case CHRID_Chipp:
                return "ch";
            case CHRID_Eddie:
                return "zt";
            case CHRID_Baiken:
                return "bk";
            case CHRID_Faust:
                return "fa";
            case CHRID_Testament:
                return "ts";
            case CHRID_Jam:
                return "jm";
            case CHRID_Anji:
                return "an";
            case CHRID_Johnny:
                return "jy";
            case CHRID_Venom:
                return "ve";
            case CHRID_Dizzy:
                return "dz";
            case CHRID_Slayer:
                return "sy";
            case CHRID_INo:
                return "in";
            case CHRID_Zappa:
                return "zp";
            case CHRID_Bridget:
                return "yy";
            case CHRID_RoboKy:
                return "rk";
            case CHRID_ABA:
                return "ab";
            case CHRID_OrderSol:
                return "fr";
            case CHRID_Kliff:
                return "kr";
            case CHRID_Justice:
                return "js";
        }

        return "sl";
    }

    bool LoadObj_New() {
        const auto current_characters = reinterpret_cast<uint16_t *>(
            reinterpret_cast<uintptr_t>(ModuleBase) + 0x6d660c);
        const auto custom_characters = g_Context.character_framework_api->get_chara_ids();

        std::string chara_ids[2]{};

        for (int i = 0; i < 2; i++) {
            if (current_characters[i] > CHRID_Justice) {
                chara_ids[i] = custom_characters.strings[current_characters[i - CHRID_Justice]];
                continue;
            }

            chara_ids[i] = CharaIdToStr(static_cast<CharacterID>(current_characters[i]));
        }

        const auto module = g_ScriptEngine->GetModule("BattleModule", asGM_CREATE_IF_NOT_EXISTS);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                g_ScriptLoaded[i][j] = LoadScript(module, j, chara_ids[i]);
            }
        }

        return LoadObj_Hook.call<bool>();
    }

    void InitializeHooks() {
        ModuleBase = reinterpret_cast<uintptr_t>(GetModuleHandle(NULL));

        LoadObj_Hook = safetyhook::create_inline((void *) (ModuleBase + 0x109e40), LoadObj_New);
    }
}
