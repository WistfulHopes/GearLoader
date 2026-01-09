#pragma once

#include <algorithm>
#include <atomic>
#include <vector>
#include "baseMod_p.h"


template <typename HookFunc>
struct HookEntry {
    BaseMod_HookId id;
    HookFunc hook;
    void* userData;
};

template <typename R, typename... Args>
class ManagedHookCallbacks {
    public:
        using HookFunc = R(__stdcall *)(void*, Args...);

        void invokeAll(Args const&... args) const {
            std::vector<HookEntry<HookFunc>> snapshot;
            {
                std::lock_guard<std::mutex> lock(_lock);
                snapshot = _callbacks;
            }
            for (HookEntry<HookFunc> entry : snapshot) {
                entry.hook(entry.userData, args...);
            }
        }
        void registerHook(BaseMod_HookId id, HookFunc hook, void* userData) {
            std::lock_guard<std::mutex> lock(_lock);
            _callbacks.push_back({id, hook, userData});
        }
        bool unregisterHook(BaseMod_HookId id) {
            std::lock_guard<std::mutex> lock(_lock);

            auto it = std::find_if(
                _callbacks.begin(),
                _callbacks.end(),
                [id](const HookEntry<HookFunc>& entry) {
                    return entry.id == id;
                }
            );

            if (it != _callbacks.end()) {
                _callbacks.erase(it);
                return true;
            }

            return false;
        }

    private:
        std::vector<HookEntry<HookFunc>> _callbacks;
        mutable std::mutex _lock;
};
