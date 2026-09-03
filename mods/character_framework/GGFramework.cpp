#include "Windows.h"
#include "GGFramework.hpp"
#include "GG.h"
#include "baseMod/baseMod_c.h"
#include "gearLoader/ggxxacpr_c.h"

#include <cstdio>
#include <iostream>

#include <cmath>

#include "../rescript/include/file.hpp"

GGFramework* GGFramework::instance_ = nullptr;
std::mutex GGFramework::mtx_{};
std::vector<void*> GGFramework::act_tbs{};
std::vector<void*> GGFramework::obj_ids{};
std::vector<const char*> GGFramework::chara_ids_{};
std::vector<std::string> GGFramework::chara_paths_{};
int* GGFramework::game_version_{};
std::vector<int32_t(*)(CHARACTER_WORK*)> GGFramework::input_check_funcs_{};
std::vector<int32_t(*)(CHARACTER_WORK*)> GGFramework::taunt_check_funcs_{};
std::vector<int32_t(*)(CHARACTER_WORK*)> GGFramework::respect_check_funcs_{};
std::vector<int32_t(*)(CHARACTER_WORK*)> GGFramework::special_attack_check_funcs_{};
std::vector<PushColli> GGFramework::push_collis_{};
std::vector<CustomSprite> GGFramework:: hud_nameplates_{};
std::vector<CustomSprite> GGFramework::hud_portraits_{};
const BaseMod_NativeFunctionsApi* GGFramework::native_functions_{};
std::vector<uint32_t> GGFramework::normal_attack_disables_{};
std::vector<int16_t> GGFramework::near_slash_dists_{};
std::vector<int16_t> GGFramework::throw_ranges_{};
std::vector<int16_t> GGFramework::air_throw_ranges_x_{};
std::vector<int16_t> GGFramework::air_throw_ranges_y_{};
std::vector<int16_t> GGFramework::air_throw_ranges_y_bottom_{};
std::vector<uint16_t> GGFramework::throw_act_nos_{};
std::vector<std::vector<uint16_t>> GGFramework::throw_damage_no_tbs{};
std::vector<uint16_t> GGFramework::air_throw_act_nos_{};
std::vector<std::vector<uint16_t>> GGFramework::air_throw_damage_no_tbs{};

namespace
{
    HMODULE base;

    constexpr uint32_t g_ExCharaFileIdBegin = 0x8C2;
    constexpr uint32_t g_CharaFileIdBegin = 0x8DC;
    constexpr uint32_t g_ModCharaFileCount = 23;

    constexpr uint32_t g_TextureSlotCount = 0xAF0;
    constexpr uint32_t g_TextureSlotStride = 56;

    int32_t pending_hud_portrait_slot = -1;

    auto get_texture_slot_size(const uint32_t slot) -> int32_t
    {
        return *reinterpret_cast<int32_t*>(reinterpret_cast<uintptr_t>(base) + 0x6dc14c
                                          + g_TextureSlotStride * slot);
    }

    auto get_texture_slot_width(const uint32_t slot) -> uint16_t
    {
        return *reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(base) + 0x6dc152
                                           + g_TextureSlotStride * slot);
    }

    auto get_resource_sprite_offset(char* buffer) -> void
    {
        const auto address = reinterpret_cast<uint32_t>(buffer);
        auto* data = reinterpret_cast<uint32_t*>(buffer);

        while (*data != 0xFFFFFFFF)
        {
            *data += address;
            ++data;
        }
        *data = 0;
    }

    // /*
    int sl_reload_obj_ids [] { 0x1a, 0x2c, 0x42, -1 };
    int* obj_id_tb[] { sl_reload_obj_ids };
    // */
}

auto GGFramework::get_mod_chara_id_idx(const uint32_t chara_id, uint32_t& out_idx) -> bool
{
    if (chara_id <= static_cast<uint32_t>(CHRID_Justice)) return false;

    out_idx = chara_id - static_cast<uint32_t>(CHRID_Justice) - 1;
    return true;
}

auto GGFramework::get_mod_chara_path(const uint32_t idx) -> const char*
{
    if (idx >= chara_paths_.size()) return nullptr;

    return chara_paths_[idx].c_str();
}

auto GGFramework::file_id_to_mod_path(const uint32_t file_id) -> const char*
{
    bool is_ex_chara = *reinterpret_cast<bool*>(reinterpret_cast<uintptr_t>(base) + 0x6d64f8);
    auto start = is_ex_chara ? g_ExCharaFileIdBegin : g_CharaFileIdBegin;

    if (file_id < start) return nullptr;

    const uint32_t idx = file_id - start;
    if (idx >= g_ModCharaFileCount) return nullptr;

    const auto* path = get_mod_chara_path(idx);
    return path;
}

auto GGFramework::get_chara_mod_path(const int plno) -> const char*
{
    const auto current_characters = reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(base) + 0x6d660c);
    const uint32_t chara_id = current_characters[plno];

    uint32_t idx{};
    if (!get_mod_chara_id_idx(chara_id, idx)) return nullptr;

    const auto* path = get_mod_chara_path(idx);
    return path;
}

auto GGFramework::set_native_functions(const BaseMod_NativeFunctionsApi* api) -> void
{
    native_functions_ = api;
}

auto GGFramework::acquire_texture_slot() -> int32_t
{
    for (int32_t slot = static_cast<int32_t>(g_TextureSlotCount) - 1; slot >= 0; --slot)
    {
        if (get_texture_slot_size(static_cast<uint32_t>(slot)) <= 4) return slot;
    }

    return -1;
}

auto GGFramework::ensure_hud_sprites_registration() -> void
{
    if (native_functions_ == nullptr) return;

    auto ensure_registration = [](std::vector<CustomSprite> &sprites) {
        for (auto&[path, data, sprite_id, load_failed, width] : sprites)
        {
            if (load_failed || path.empty()) continue;
            if (sprite_id >= 0
                && get_texture_slot_size(static_cast<uint32_t>(sprite_id)) > 4) continue;

            if (data == nullptr)
            {
                UniqueFile file = open_file(path.c_str(), "rb");
                if (file == nullptr)
                {
                    load_failed = true;
                    continue;
                }

                std::fseek(file.get(), 0, SEEK_END);
                const long size = std::ftell(file.get());
                std::fseek(file.get(), 0, SEEK_SET);

                if (size <= 0)
                {
                    load_failed = true;
                    continue;
                }

                auto new_data = std::make_unique<char[]>(static_cast<size_t>(size));

                if (const size_t read = std::fread(new_data.get(), 1, static_cast<size_t>(size), file.get());
                    read != static_cast<size_t>(size))
                {
                    load_failed = true;
                    continue;
                }

                get_resource_sprite_offset(new_data.get());
                data = std::move(new_data);
            }

            const int32_t slot = acquire_texture_slot();
            if (slot < 0 || native_functions_->RegisterSprites(static_cast<uint32_t>(slot),
                                                               data.get(), 1) == 0)
            {
                load_failed = true;
                continue;
            }

            sprite_id = slot;
            width = get_texture_slot_width(static_cast<uint32_t>(slot));
        }
    };

    ensure_registration(hud_nameplates_);
    ensure_registration(hud_portraits_);
}

auto GGFramework::get_mod_hud_nameplate(const int plno) -> const CustomSprite*
{
    if (plno < 0 || plno > 1) return nullptr;

    const auto current_characters = reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(base) + 0x6d660c);

    uint32_t idx{};
    if (!get_mod_chara_id_idx(current_characters[plno], idx)) return nullptr;
    if (idx >= hud_nameplates_.size()) return nullptr;

    const auto& nameplate = hud_nameplates_[idx];
    if (nameplate.sprite_id < 0 || nameplate.width <= 0) return nullptr;

    return &nameplate;
}

auto GGFramework::get_mod_hud_portrait(int plno) -> const CustomSprite * {
    if (plno < 0 || plno > 1) return nullptr;

    const auto current_characters = reinterpret_cast<uint16_t*>(reinterpret_cast<uintptr_t>(base) + 0x6d660c);

    uint32_t idx{};
    if (!get_mod_chara_id_idx(current_characters[plno], idx)) return nullptr;
    if (idx >= hud_portraits_.size()) return nullptr;

    const auto& portrait = hud_portraits_[idx];
    if (portrait.sprite_id < 0 || portrait.width <= 0) return nullptr;

    return &portrait;
}

auto GGFramework::initialize() -> void
{
    base = GetModuleHandle(nullptr);

    game_version_ = reinterpret_cast<int*>(reinterpret_cast<uintptr_t>(base) + 0x6d0538);

    // /* example
    register_chara_id("sl_reload");
    register_act_tb(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(base) + 0x5f08b8));
    register_obj_id(&obj_id_tb);
    register_hud_nameplate("Resource/demo/chrimg/sl_reload_name.bin");
    register_hud_portrait("Resource/demo/chrimg/sl_reload_face.bin");
    register_input_check_func(reinterpret_cast<int32_t(*)(CHARACTER_WORK*)>(reinterpret_cast<uintptr_t>(base) + 0x253530));
    register_taunt_check_func(reinterpret_cast<int32_t(*)(CHARACTER_WORK*)>(reinterpret_cast<uintptr_t>(base) + 0x252660));
    register_respect_check_func(reinterpret_cast<int32_t(*)(CHARACTER_WORK*)>(reinterpret_cast<uintptr_t>(base) + 0x252640));
    register_special_attack_check_func(reinterpret_cast<int32_t(*)(CHARACTER_WORK*)>(reinterpret_cast<uintptr_t>(base) + 0x253610));
    register_push_colli(PushColli {.stand_width = 0xBB8, .stand_height = 0x2710, .sit_width = 0xC80, .sit_height = 0x1B58,
        .sky_width = 0xBB8, .sky_height = 0x1B58, .sky_base_height = 0xFA0
    });
    register_normal_attack_disable(0xe025ffff);
    register_near_slash_dist(17000);
    register_throw_range(4300);
    register_air_throw_range_x(11000);
    register_air_throw_range_y(-10000);
    register_air_throw_range_y_bottom(3000);
    register_throw_act_no(0xD6);
    register_throw_damage_no_tb({
        0x00, 0xD7, 0x70, 0xBE, 0x69, 0xCC, 0xBC, 0x79, 0x6B, 0x6B, 0x6F, 0xAF, 0x80, 0x67, 0x6C, 0x76, 0x63, 0xA0, 0x9A,
        0x64, 0xD7, 0x70, 0x70, 0xD7, 0x7D, 0x7D,
    });
    register_air_throw_act_no(0xCC);
    register_air_throw_damage_no_tb({
        0x00, 0xCD, 0xAD, 0xAF, 0xAC, 0xB4, 0xC2, 0xC7, 0xA7, 0xAA, 0xCE, 0x9D, 0xD3, 0xC4, 0xB6, 0xAE, 0x9B, 0xCB, 0xA3,
        0x82, 0x9F, 0xAD, 0x90, 0xCD, 0xB1, 0xAE,
    });// */

    player_main_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x137b5e, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.eax) > CHRID_Justice)
        {
            if (ctx.eax - static_cast<uint32_t>(CHRID_Justice) > act_tbs.size()) return;
            auto result = act_tbs[ctx.eax - static_cast<uint32_t>(CHRID_Justice) - 1];

            if (result == nullptr) return;

            ctx.ebx = reinterpret_cast<uintptr_t>(result);
        }
    });

    load_obj_file_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x113d17, [](SafetyHookContext& ctx)
    {
        if (const auto* path = get_chara_mod_path(0)) ctx.eax = reinterpret_cast<uintptr_t>(path);
    });

    load_obj_file_hook_2_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x113d80, [](SafetyHookContext& ctx)
    {
        if (const auto* path = get_chara_mod_path(1)) ctx.eax = reinterpret_cast<uintptr_t>(path);
    });

    load_obj_file_hook_3_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x113dea, [](SafetyHookContext& ctx)
    {
        if (const auto* path = get_chara_mod_path(0)) ctx.eax = reinterpret_cast<uintptr_t>(path);
    });

    load_obj_file_hook_4_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x113e47, [](SafetyHookContext& ctx)
    {
        if (const auto* path = get_chara_mod_path(0)) ctx.eax = reinterpret_cast<uintptr_t>(path);
    });

    load_obj_file_hook_5_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x113e92, [](SafetyHookContext& ctx)
    {
        if (const auto* path = get_chara_mod_path(1)) ctx.eax = reinterpret_cast<uintptr_t>(path);
    });

    allocate_file_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1139a2, [](SafetyHookContext& ctx)
    {
        if (const auto* path = file_id_to_mod_path(ctx.esi)) ctx.edx = reinterpret_cast<uintptr_t>(path);
    });

    allocate_file_hook_2_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1139ef, [](SafetyHookContext& ctx)
    {
        if (const auto* path = file_id_to_mod_path(ctx.esi)) ctx.eax = reinterpret_cast<uintptr_t>(path);
    });

    chara_select_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1fe02d, [](SafetyHookContext& ctx)
    {
        if (ctx.eax >= static_cast<uintptr_t>(CHRID_Justice)) ctx.eax = 0;
    });

    chara_select_hook_2_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1F97AE, [](SafetyHookContext& ctx)
    {
        if (ctx.edi >= static_cast<uintptr_t>(CHRID_Justice)) ctx.edi = 0;
    });

    chara_select_hook_3_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1f98bd, [](SafetyHookContext& ctx)
    {
        if (ctx.eax >= static_cast<uintptr_t>(CHRID_Justice)) ctx.eax = 0;
    });

    set_game_version_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x117a08, [](SafetyHookContext& ctx)
    {
        if (ctx.eax == 1) ctx.eax = 2;
    });

    load_dlc_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x36ecc3, [](SafetyHookContext& ctx)
    {
        if (*reinterpret_cast<int*>(ctx.ebp + 8) == 1) *reinterpret_cast<int*>(ctx.ebp + 8) = 2;
    });

    obj_file_link_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1bfbf9, [](SafetyHookContext& ctx)
    {
        if (ctx.eax >= static_cast<uintptr_t>(CHRID_Justice))
        {
            const uint32_t idx = static_cast<uint32_t>(ctx.eax) - static_cast<uint32_t>(CHRID_Justice);
            if (idx >= obj_ids.size())
            {
                return;
            }

            const auto result = obj_ids[idx];
            if (result == nullptr) return;

            ctx.edx = *static_cast<uintptr_t*>(result);
        }
    });

    hud_char_name_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1ea237, [](SafetyHookContext& ctx)
    {
        ensure_hud_sprites_registration();

        if (const auto* nameplate = get_mod_hud_nameplate(static_cast<int>(ctx.edi)))
        {
            ctx.edx = 0;
            ctx.ecx = static_cast<uint32_t>(nameplate->width);
        }
    });

    hud_char_name_row_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1ea2c2, [](SafetyHookContext& ctx)
    {
        if (get_mod_hud_nameplate(static_cast<int>(ctx.edi)) != nullptr)
        {
            ctx.eax = 0;
        }
    });

    hud_nameplate_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1ea38a, [](SafetyHookContext& ctx)
    {
        const auto* nameplate = get_mod_hud_nameplate(static_cast<int>(ctx.edi));
        if (nameplate == nullptr) return;

        auto* params = reinterpret_cast<GGXXACPR_DrawSpriteParams*>(ctx.ecx);
        params->spriteId = nameplate->sprite_id;
        params->u0 = 0.0f;
        params->v0 = 0.0f;
        params->u1 = 1.0f;
        params->v1 = 1.0f;
        params->zm_x = 1.0f;
        params->zm_y = 1.0f;
    });

    hud_portrait_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1ef045, [](SafetyHookContext& ctx)
    {
        ensure_hud_sprites_registration();

        const auto* portrait = get_mod_hud_portrait(static_cast<int>(ctx.esi));
        pending_hud_portrait_slot = portrait != nullptr ? portrait->sprite_id : -1;
    });

    hud_portrait_hook_2_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1eaf21, [](SafetyHookContext& ctx)
    {
        const int32_t slot = pending_hud_portrait_slot;
        pending_hud_portrait_slot = -1;
        if (slot < 0) return;

        ctx.eax = static_cast<uint32_t>(slot);
    });

    hud_portrait_flash_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1eefff, [](SafetyHookContext& ctx)
    {
        ensure_hud_sprites_registration();

        const auto* portrait = get_mod_hud_portrait(static_cast<int>(ctx.esi));
        pending_hud_portrait_slot = portrait != nullptr ? portrait->sprite_id : -1;
    });

    hud_portrait_flash_hook_2_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1ecc6e, [](SafetyHookContext& ctx)
    {
        const int32_t slot = pending_hud_portrait_slot;
        pending_hud_portrait_slot = -1;
        if (slot < 0) return;

        ctx.eax = static_cast<uint32_t>(slot);
    });

    input_check_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x38f6df, [](SafetyHookContext& ctx)
    {
        if (const auto offset = reinterpret_cast<CHARACTER_WORK*>(ctx.esi); offset->idno > CHRID_Justice)
        {
            if (static_cast<uint32_t>(offset->idno) - static_cast<uint32_t>(CHRID_Justice) > input_check_funcs_.size()) return;
            auto result = input_check_funcs_[static_cast<uint32_t>(offset->idno) - static_cast<uint32_t>(CHRID_Justice) - 1];

            if (result == nullptr) return;

            ctx.eax = reinterpret_cast<uintptr_t>(result);
        }
    });

    taunt_check_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x38e787, [](SafetyHookContext& ctx)
    {
        if (const auto offset = reinterpret_cast<CHARACTER_WORK*>(ctx.esi); offset->idno > CHRID_Justice)
        {
            if (static_cast<uint32_t>(offset->idno) - static_cast<uint32_t>(CHRID_Justice) > taunt_check_funcs_.size()) return;
            auto result = taunt_check_funcs_[static_cast<uint32_t>(offset->idno) - static_cast<uint32_t>(CHRID_Justice) - 1];

            if (result == nullptr) return;

            ctx.ecx = reinterpret_cast<uintptr_t>(result);
        }
    });

    respect_check_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x38f71d, [](SafetyHookContext& ctx)
    {
        if (const auto offset = reinterpret_cast<CHARACTER_WORK*>(ctx.esi); offset->idno > CHRID_Justice)
        {
            if (static_cast<uint32_t>(offset->idno) - static_cast<uint32_t>(CHRID_Justice) > respect_check_funcs_.size()) return;
            auto result = respect_check_funcs_[static_cast<uint32_t>(offset->idno) - static_cast<uint32_t>(CHRID_Justice) - 1];

            if (result == nullptr) return;

            ctx.eax = reinterpret_cast<uintptr_t>(result);
        }
    });

    special_attack_check_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x38f778, [](SafetyHookContext& ctx)
    {
        if (const auto offset = reinterpret_cast<CHARACTER_WORK*>(ctx.esi); offset->idno > CHRID_Justice)
        {
            if (static_cast<uint32_t>(offset->idno) - static_cast<uint32_t>(CHRID_Justice) > special_attack_check_funcs_.size()) return;
            auto result = special_attack_check_funcs_[static_cast<uint32_t>(offset->idno) - static_cast<uint32_t>(CHRID_Justice) - 1];

            if (result == nullptr) return;

            ctx.eax = reinterpret_cast<uintptr_t>(result);
        }
    });

    player_action_break_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x139444, [](SafetyHookContext& ctx)
    {
        if (ctx.eax > 0x2c) ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x139576;
        else
        {
            if (ctx.eax > 0x18) ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x139458;
            else ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x13944a;
        }
    });

    char_pos_collision_execute_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14bcf1, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.eax) > CHRID_Justice)
        {
            if (ctx.eax - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.ebx = push_collis_[ctx.eax - static_cast<uint32_t>(CHRID_Justice) - 1].stand_width;
        }
    });

    char_pos_collision_execute_hook_2_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14bdc5, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.eax) > CHRID_Justice)
        {
            if (ctx.eax - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.ebx = push_collis_[ctx.eax - static_cast<uint32_t>(CHRID_Justice) - 1].stand_width;
        }
    });

    char_pos_collision_execute_hook_3_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14bd67, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax / 2;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.eax = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].sit_height;
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x14bd6e;
        }
    });

    char_pos_collision_execute_hook_4_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14be3a, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax / 2;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.eax = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].sit_height;
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x14be41;
        }
    });

    char_pos_collision_execute_hook_5_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14bd8f, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax / 2;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.eax = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].stand_height;
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x14bd96;
        }
    });

    char_pos_collision_execute_hook_6_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14be62, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax / 2;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.eax = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].stand_height;
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x14be69;
        }
    });

    char_pos_collision_execute_hook_7_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14bd09, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax / 2;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.eax = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].sky_base_height;
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x14bd10;
        }
    });

    char_pos_collision_execute_hook_8_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14bddd, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax / 2;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.eax = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].sky_base_height;
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x14bde4;
        }
    });

    char_pos_collision_execute_hook_9_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14bd65, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.edx = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].sit_width;
        }
    });

    char_pos_collision_execute_hook_10_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14be38, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.edx = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].sit_width;
        }
    });

    char_pos_collision_execute_hook_11_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14bd81, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.edx = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].sky_width;
        }
    });

    char_pos_collision_execute_hook_12_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14be54, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.edx = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].sky_width;
        }
    });

    char_pos_collision_execute_hook_13_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14bd04, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax / 2;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.ebx = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].sky_height;
        }
    });

    char_pos_collision_execute_hook_14_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x14bdd8, [](SafetyHookContext& ctx)
    {
        auto idno = ctx.eax / 2;
        if (static_cast<uint16_t>(idno) > CHRID_Justice)
        {
            if (idno - static_cast<uint32_t>(CHRID_Justice) > push_collis_.size()) return;
            ctx.ebx = push_collis_[idno - static_cast<uint32_t>(CHRID_Justice) - 1].sky_height;
        }
    });

    normal_attack_disable_table_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x2a9ffe, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.edx) > CHRID_Justice)
        {
            if (ctx.edx - static_cast<uint32_t>(CHRID_Justice) > normal_attack_disables_.size()) return;
            ctx.edx = normal_attack_disables_[ctx.edx - static_cast<uint32_t>(CHRID_Justice) - 1];
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x2aa005;
        }
    });

    normal_attack_disable_table_hook_2_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x2aa081, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.edx) > CHRID_Justice)
        {
            if (ctx.edx - static_cast<uint32_t>(CHRID_Justice) > normal_attack_disables_.size()) return;
            ctx.ecx = normal_attack_disables_[ctx.edx - static_cast<uint32_t>(CHRID_Justice) - 1];
        }
    });

    cmn_attack_check_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x38fe8a, [](SafetyHookContext& ctx)
    {
        if (*reinterpret_cast<uint16_t*>(ctx.edi) > CHRID_Justice)
        {
            if (*reinterpret_cast<uint16_t*>(ctx.edi) - static_cast<uint32_t>(CHRID_Justice) > near_slash_dists_.size()) return;
            ctx.edx = near_slash_dists_[*reinterpret_cast<uint16_t*>(ctx.edi) - static_cast<uint32_t>(CHRID_Justice) - 1];
        }
    });

    player_throw_flag_set_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x12065d, [](SafetyHookContext& ctx)
    {
        auto player_id = ctx.eax / 0x1A;
        auto enemy_id = ctx.edx;

        ctx.eip = reinterpret_cast<uintptr_t>(base) + (player_id > chara_paths_.size() + static_cast<uint32_t>(CHRID_Justice)
                                                       || enemy_id > chara_paths_.size() + static_cast<uint32_t>(CHRID_Justice)
                                                           ? 0x1206bd : 0x12066a);
    });

    player_throw_flag_set_hook_2_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x120629, [](SafetyHookContext& ctx)
    {
        auto player_id = ctx.eax / 0x1A;
        auto enemy_id = ctx.edx;

        ctx.eip = reinterpret_cast<uintptr_t>(base) + (player_id > chara_paths_.size() + static_cast<uint32_t>(CHRID_Justice)
                                                       || enemy_id > chara_paths_.size() + static_cast<uint32_t>(CHRID_Justice)
                                                           ? 0x1206bd : 0x12063a);
    });

    throw_range_check_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x12030D, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.ecx) > CHRID_Justice)
        {
            if (ctx.ecx - static_cast<uint32_t>(CHRID_Justice) > throw_ranges_.size()) return;
            ctx.edx = throw_ranges_[ctx.ecx - static_cast<uint32_t>(CHRID_Justice) - 1];
        }
    });

    throw_range_check_hook_2_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x120323, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.eax) > CHRID_Justice)
        {
            if (ctx.eax - static_cast<uint32_t>(CHRID_Justice) > throw_ranges_.size()) return;
            ctx.ecx = throw_ranges_[ctx.eax - static_cast<uint32_t>(CHRID_Justice) - 1];
        }
    });

    air_throw_range_check_hook_x_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1202AB, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.eax) > CHRID_Justice)
        {
            if (ctx.eax - static_cast<uint32_t>(CHRID_Justice) > air_throw_ranges_x_.size()) return;
            ctx.edx = air_throw_ranges_x_[ctx.eax - static_cast<uint32_t>(CHRID_Justice) - 1];
        }
    });

    air_throw_range_check_hook_x_2_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1202BD, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.eax) > CHRID_Justice)
        {
            if (ctx.eax - static_cast<uint32_t>(CHRID_Justice) > air_throw_ranges_x_.size()) return;
            ctx.edx = air_throw_ranges_x_[ctx.eax - static_cast<uint32_t>(CHRID_Justice) - 1];
        }
    });

    air_throw_range_check_hook_y_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1202eb, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.eax / 2) > CHRID_Justice)
        {
            if (ctx.eax / 2 - static_cast<uint32_t>(CHRID_Justice) > air_throw_ranges_y_.size()) return;
            ctx.eax = air_throw_ranges_y_[ctx.eax / 2 - static_cast<uint32_t>(CHRID_Justice) - 1];
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x1202f2;
        }
    });

    air_throw_range_check_hook_y_bottom_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1202e7, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.eax / 2) > CHRID_Justice)
        {
            if (ctx.eax / 2 - static_cast<uint32_t>(CHRID_Justice) > air_throw_ranges_y_bottom_.size()) return;
            ctx.edx = air_throw_ranges_y_bottom_[ctx.eax / 2 - static_cast<uint32_t>(CHRID_Justice) - 1];
        }
    });

    player_throw_check_hook_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1248d1, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.eax) > CHRID_Justice)
        {
            if (ctx.eax - static_cast<uint32_t>(CHRID_Justice) > throw_act_nos_.size()) return;
            ctx.ecx = throw_act_nos_[ctx.eax - static_cast<uint32_t>(CHRID_Justice) - 1];
        }
    });

    player_throw_check_hook_2_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x1248e2, [](SafetyHookContext& ctx)
    {
        auto player_no = ctx.edx / 0x1A;
        auto enemy_no = ctx.eax;
        if (static_cast<uint16_t>(player_no) > CHRID_Justice)
        {
            if (player_no - static_cast<uint32_t>(CHRID_Justice) > throw_damage_no_tbs.size()) return;
            const auto& throw_damage_no_tb = throw_damage_no_tbs[player_no - static_cast<uint32_t>(CHRID_Justice) - 1];
            if (enemy_no > throw_damage_no_tb.size() - 1)
            {
                ctx.ecx = 0xD7;
            }
            else
            {
                ctx.ecx = throw_damage_no_tb[enemy_no];
            }
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x1248ec;
        }
        if (static_cast<uint16_t>(enemy_no) > CHRID_Justice)
        {
            ctx.ecx = 0xD7;
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x1248ec;
        }
    });

    player_throw_check_hook_3_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x12486a, [](SafetyHookContext& ctx)
    {
        if (static_cast<uint16_t>(ctx.ebx) > CHRID_Justice)
        {
            if (ctx.ebx - static_cast<uint32_t>(CHRID_Justice) > air_throw_act_nos_.size()) return;
            ctx.edx = air_throw_act_nos_[ctx.ebx - static_cast<uint32_t>(CHRID_Justice) - 1];
        }
    });

    player_throw_check_hook_4_ = safetyhook::create_mid(reinterpret_cast<uintptr_t>(base) + 0x12487b, [](SafetyHookContext& ctx)
    {
        auto player_no = ctx.eax / 0x1A;
        auto enemy_no = ctx.ecx;
        if (static_cast<uint16_t>(player_no) > CHRID_Justice)
        {
            if (player_no - static_cast<uint32_t>(CHRID_Justice) > air_throw_damage_no_tbs.size()) return;
            const auto& throw_damage_no_tb = air_throw_damage_no_tbs[player_no - static_cast<uint32_t>(CHRID_Justice) - 1];
            if (enemy_no > throw_damage_no_tb.size() - 1)
            {
                ctx.edx = 0xCD;
            }
            else
            {
                ctx.edx = throw_damage_no_tb[enemy_no];
            }
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x124885;
        }
        if (static_cast<uint16_t>(enemy_no) > CHRID_Justice)
        {
            ctx.edx = 0xCD;
            ctx.eip = reinterpret_cast<uintptr_t>(base) + 0x124885;
        }
    });
}

auto GGFramework::get_instance() -> GGFramework*
{
    if (instance_ == nullptr)
    {
        std::lock_guard lock(mtx_);
        if (instance_ == nullptr)
        {
            instance_ = new GGFramework();
        }
    }
    return instance_;
}

auto GGFramework::get_chara_ids() -> std::vector<const char*>& {
    return chara_ids_;
}

auto GGFramework::register_act_tb(void* act_tb) -> void
{
    act_tbs.push_back(act_tb);
}

auto GGFramework::register_obj_id(void* obj_id) -> void
{
    obj_ids.push_back(obj_id);
}

auto GGFramework::register_chara_id(const std::string& id) -> void
{
    chara_ids_.push_back(id.c_str());
    chara_paths_.push_back(std::string("obj/") + id + ".bin");
}

auto GGFramework::register_input_check_func(int32_t(* func)(CHARACTER_WORK*)) -> void
{
    input_check_funcs_.push_back(func);
}

auto GGFramework::register_taunt_check_func(int32_t(* func)(CHARACTER_WORK*)) -> void
{
    taunt_check_funcs_.push_back(func);
}

auto GGFramework::register_respect_check_func(int32_t(* func)(CHARACTER_WORK*)) -> void
{
    respect_check_funcs_.push_back(func);
}

auto GGFramework::register_special_attack_check_func(int32_t(* func)(CHARACTER_WORK*)) -> void
{
    special_attack_check_funcs_.push_back(func);
}

auto GGFramework::register_push_colli(const PushColli& push_colli) -> void
{
    push_collis_.push_back(push_colli);
}

auto GGFramework::register_hud_nameplate(const std::string& path) -> void
{
    CustomSprite nameplate{};
    nameplate.path = path;
    hud_nameplates_.push_back(std::move(nameplate));
}

auto GGFramework::register_hud_portrait(const std::string &path) -> void {
    CustomSprite portrait{};
    portrait.path = path;
    hud_portraits_.push_back(std::move(portrait));
}

auto GGFramework::register_normal_attack_disable(const uint32_t disable) -> void
{
    normal_attack_disables_.push_back(disable);
}

auto GGFramework::register_near_slash_dist(const int16_t dist) -> void
{
    near_slash_dists_.push_back(dist);
}

auto GGFramework::register_throw_range(const int16_t range) -> void
{
    throw_ranges_.push_back(range);
}

auto GGFramework::register_air_throw_range_x(const int16_t range) -> void
{
    air_throw_ranges_x_.push_back(range);
}

auto GGFramework::register_air_throw_range_y(const int16_t range) -> void
{
    air_throw_ranges_y_.push_back(range);
}

auto GGFramework::register_air_throw_range_y_bottom(const int16_t range) -> void
{
    air_throw_ranges_y_bottom_.push_back(range);
}

auto GGFramework::register_throw_act_no(const uint16_t no) -> void
{
    throw_act_nos_.push_back(no);
}

auto GGFramework::register_throw_damage_no_tb(const std::vector<uint16_t> &tb) -> void
{
    throw_damage_no_tbs.push_back(tb);
}

auto GGFramework::register_air_throw_act_no(const uint16_t no) -> void
{
    air_throw_act_nos_.push_back(no);
}

auto GGFramework::register_air_throw_damage_no_tb(const std::vector<uint16_t> &tb) -> void
{
    air_throw_damage_no_tbs.push_back(tb);
}
