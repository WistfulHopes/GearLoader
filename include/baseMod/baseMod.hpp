#ifndef BASEMOD_HPP
#define BASEMOD_HPP

#include "baseMod_c.h"
#include "gearLoader/ggxxacpr.hpp"

namespace BaseMod {

    class Player {
    public:
        Player(GGXXACPR_Entity *ref)
            : raw(ref) {};
        
        bool isValid() {
            return raw != nullptr;
        }
        GGXXACPR_Entity* getRawPtr() {
            return raw;
        }

        const ggxxacpr::EntityId &id = static_cast<ggxxacpr::EntityId>(raw->id);
        const bool &isFacingRight = static_cast<bool>(raw->bIsFacingRight);
        const bool &isLeftSide = static_cast<bool>(raw->bIsLeftSide);
        const ggxxacpr::ActionState &actionState = static_cast<ggxxacpr::ActionState>(raw->actionState);

    private:
        GGXXACPR_Entity *raw;
    };
    class PlayerData {
    public:
        PlayerData(GGXXACPR_PlayerEntityData* ref)
            : raw(ref) {};

        const uint16_t &tension = raw->tension;
    private:
        GGXXACPR_PlayerEntityData* raw;
    };

    using HookId = BaseMod_HookId;
    using HookContext = BaseMod_HookContext;
    using PeakMessageArgs = BaseMod_PeekMessageArgs;
    using PeekMessageInfo = BaseMod_PeekMessageInfo;
    using GameUpdateInfo = BaseMod_GameUpdateInfo;
    using DrawInfo = BaseMod_DrawInfo;

    class NativeFunctionsApi {
    public:
        NativeFunctionsApi() : _ref(nullptr) {}
        NativeFunctionsApi(const BaseMod_NativeFunctionsApi* ref)
            : _ref(ref) {}
    private:
        const BaseMod_NativeFunctionsApi* _ref;
    };

    class GameDataApi {
    public:
        GameDataApi() : _ref(nullptr) {}
        GameDataApi(const BaseMod_GameDataApi* ref) : _ref(ref) {
            if (ref->version < BASEMOD_API_VERSION_NUM ||
                ref->size < sizeof(BaseMod_Api)) {
                // versioning err
            }
        }
        Player getPlayer(int player_index) {
            return _ref->getPlayer(player_index);
        }
        bool isInGame() {
            return _ref->isInGame() > 0;
        }
        ggxxacpr::GameMode getGameMode() {
            return static_cast<ggxxacpr::GameMode>(_ref->getGameMode());
        }
    private:
        const BaseMod_GameDataApi* _ref;

    };

    template<typename T>
    using PeekMessageHook = void (__stdcall *)(T* userData, const BaseMod_HookContext* ctx, const BaseMod_PeekMessageInfo* info);
    template<typename T>
    using GameUpdateHook = void(__stdcall *)(T* userData, const HookContext* ctx, const GameUpdateInfo* info);
    template<typename T>
    using DrawHook = void(__stdcall *)(T* userData, const BaseMod_HookContext* ctx, const BaseMod_DrawInfo* info);

    class HookApi {
    public:
        HookApi() : _ref(nullptr) {}
        HookApi(const BaseMod_HookApi* ref) : _ref(ref) {
            if (ref->version < BASEMOD_API_VERSION_NUM ||
                ref->size < sizeof(BaseMod_HookApi)) {
                // versioning err
            }
        }
        
        HookId afterPeekMessage(BaseMod_PeekMessageHook hookFn, void* userData) {
            return _ref->afterPeekMessage(hookFn, userData);
        }
        template<typename T>
        HookId afterPeekMessage(PeekMessageHook<T> hookFn, T* userData) {
            return _ref->afterPeekMessage(hookFn, userData);
        }

        HookId beforeGameUpdate(BaseMod_GameUpdateHook hookFn, void* userData) {
            return _ref->beforeGameUpdate(hookFn, userData);
        }
        template<typename T>
        HookId beforeGameUpdate(GameUpdateHook<T> hookFn, T* userData) {
            return _ref->beforeGameUpdate(hookFn, userData);
        }

        HookId afterGameUpdate(BaseMod_GameUpdateHook hookFn, void* userData) {
            return _ref->afterGameUpdate(hookFn, userData);
        }
        template<typename T>
        HookId afterGameUpdate(GameUpdateHook<T> hookFn, T* userData) {
            return _ref->afterGameUpdate(hookFn, userData);
        }

        HookId beforeEndScene(BaseMod_DrawHook hookFn, void* userData) {
            return _ref->beforeEndScene(hookFn, userData);
        }
        template<typename T>
        HookId beforeEndScene(DrawHook<T> hookFn, T* userData) {
            return _ref->beforeEndScene(hookFn, userData);
        }

        HookId beforePresent(BaseMod_DrawHook hookFn, void* userData) {
            return _ref->beforePresent(hookFn, userData);
        }
        template<typename T>
        HookId beforePresent(DrawHook<T> hookFn, T* userData) {
            return _ref->beforePresent(hookFn, userData);
        }

        uint32_t removeHook(HookId id) {
            return _ref->removeHook(id);
        }

    private:
        const BaseMod_HookApi* _ref;

    };

    class Api {
    public:
        Api() : _ref(nullptr) {}
        Api(const BaseMod_Api* ref)
            : _ref(ref)
            , NativeFunctions(_ref->NativeFunctions)
            , GameData(_ref->GameData)
            , Hooks(_ref->Hooks) {
            if (ref->version < BASEMOD_API_VERSION_NUM ||
                ref->size < sizeof(BaseMod_Api)) {
                // versioning err
            }
        }
        NativeFunctionsApi NativeFunctions;
        GameDataApi GameData;
        HookApi Hooks;
    private:
        const BaseMod_Api* _ref;
    };
}


#endif