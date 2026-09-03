// All mods that implement the Init function should include the mod loader header
#include "gearLoader/gearLoader.hpp"

#include "character_framework.h"

#include <iostream>

#include "baseMod/baseMod_c.h"

#include "GGFramework.hpp"

void* CHARACTER_FRAMEWORK_CALL GGFramework_get_instance()
{
    return GGFramework::get_instance();
}

StringVector CHARACTER_FRAMEWORK_CALL GGFramework_get_chara_ids()
{
    auto& ids = GGFramework::get_chara_ids();

    return { .strings = ids.data(), .num = ids.size() };
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_act_tb(void* act_tb)
{
    GGFramework::register_act_tb(act_tb);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_obj_id(void* obj_id)
{
    GGFramework::register_obj_id(obj_id);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_chara_id(const char* id)
{
    GGFramework::register_chara_id(std::string(id));
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_input_check_func(int32_t(* func)(CHARACTER_WORK*))
{
    GGFramework::register_input_check_func(func);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_taunt_check_func(int32_t(* func)(CHARACTER_WORK*))
{
    GGFramework::register_taunt_check_func(func);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_respect_check_func(int32_t(* func)(CHARACTER_WORK*))
{
    GGFramework::register_respect_check_func(func);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_special_attack_check_func(int32_t(* func)(CHARACTER_WORK*))
{
    GGFramework::register_special_attack_check_func(func);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_push_colli(const PushColli_C* push_colli)
{
    GGFramework::register_push_colli(*(PushColli*)push_colli);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_normal_attack_disable(uint32_t disable)
{
    GGFramework::register_normal_attack_disable(disable);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_near_slash_dist(int16_t dist)
{
    GGFramework::register_near_slash_dist(dist);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_throw_range(int16_t range)
{
    GGFramework::register_throw_range(range);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_air_throw_range_x(int16_t range)
{
    GGFramework::register_air_throw_range_x(range);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_air_throw_range_y(int16_t range)
{
    GGFramework::register_air_throw_range_y(range);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_air_throw_range_y_bottom(int16_t range)
{
    GGFramework::register_air_throw_range_y_bottom(range);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_throw_act_no(uint16_t no)
{
    GGFramework::register_throw_act_no(no);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_throw_damage_no_tb(const uint16_t* damage_no_tb, uint16_t size)
{
    GGFramework::register_throw_damage_no_tb(std::vector(damage_no_tb, damage_no_tb + size));
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_air_throw_act_no(uint16_t no)
{
    GGFramework::register_air_throw_act_no(no);
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_air_throw_damage_no_tb(const uint16_t* damage_no_tb, uint16_t size)
{
    GGFramework::register_air_throw_damage_no_tb(std::vector(damage_no_tb, damage_no_tb + size));
}

void CHARACTER_FRAMEWORK_CALL GGFramework_register_hud_nameplate(const char* path)
{
    GGFramework::register_hud_nameplate(std::string(path));
}

static const CharacterFramework_Api _api = {
    sizeof(CharacterFramework_Api),
    CHARACTER_FRAMEWORK_API_VERSION_NUM,

    GGFramework_get_instance,
    GGFramework_get_chara_ids,
    GGFramework_register_act_tb,
    GGFramework_register_obj_id,
    GGFramework_register_chara_id,
    GGFramework_register_input_check_func,
    GGFramework_register_taunt_check_func,
    GGFramework_register_respect_check_func,
    GGFramework_register_special_attack_check_func,
    GGFramework_register_push_colli,
    GGFramework_register_normal_attack_disable,
    GGFramework_register_near_slash_dist,
    GGFramework_register_throw_range,
    GGFramework_register_air_throw_range_x,
    GGFramework_register_air_throw_range_y,
    GGFramework_register_air_throw_range_y_bottom,
    GGFramework_register_throw_act_no,
    GGFramework_register_throw_damage_no_tb,
    GGFramework_register_air_throw_act_no,
    GGFramework_register_air_throw_damage_no_tb,
    GGFramework_register_hud_nameplate
};

SemanticVersion getSemVer() {
    unsigned int verNum = CHARACTER_FRAMEWORK_API_VERSION_NUM;
    return {
        (verNum & 0xFF0000) >> 16,
        (verNum & 0x00FF00) >> 8,
        (verNum & 0x0000FF),
    };
}

//  Called by mod loader immediately after loading this mod.
//  This function should serve as the mod entry point.
//  All dependencies listed in config.json will already
//  be loaded in the process when this method is called.
GEARLOADER_EXPORT void GEARLOADER_CALL Init(GearLoaderContext* ctx, GearLoaderApi* c_api) {

    // Construct C++ wrapper class
    GearLoader::Api gearLoaderApi(c_api, ctx);

    gearLoaderApi.RegisterApi(&_api, CHARACTER_FRAMEWORK_NAME, getSemVer());

    const BaseMod_Api* base_mod_api{};
    SemanticVersion base_mod_version{};
    if (gearLoaderApi.RetrieveModApi<BaseMod_Api>(BASEMOD_NAME, BASEMOD_API_VERSION,
                                                 &base_mod_api, &base_mod_version) == 0)
    {
        GGFramework::set_native_functions(base_mod_api->NativeFunctions);
    }

    GGFramework::get_instance();
    std::cout << "[Character Framework] Initialized" << std::endl;
}
