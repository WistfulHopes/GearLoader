#pragma once

#include <angelscript.h>
#include <cstring>

struct CharacterFramework_Api;

namespace rescript_as {
    struct ReScriptContext {
        const CharacterFramework_Api* character_framework_api;
    };
    static ReScriptContext g_Context;

    class CMemoryStream : public asIBinaryStream {
    public:
        CMemoryStream(const char* buffer, size_t size) : ptr(buffer), size(size), cursor(0) {}

        int Read(void* dest, asUINT bytes) override {
            if (cursor + bytes > size) return -1;
            memcpy(dest, ptr + cursor, bytes);
            cursor += bytes;
            return 0;
        }

        int Write(const void* src, asUINT bytes) override { return -1; }

    private:
        const char* ptr;
        size_t size;
        size_t cursor;
    };

    static asIScriptEngine* g_ScriptEngine;

    void InitializeScriptEngine();
}
