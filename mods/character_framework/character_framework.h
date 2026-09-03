#ifndef CHARACTER_FRAMEWORK_H
#define CHARACTER_FRAMEWORK_H

#define CHARACTER_FRAMEWORK_NAME "Character Framework"
#define CHARACTER_FRAMEWORK_API_VERSION "1.2.0"
#define CHARACTER_FRAMEWORK_API_VERSION_NUM 0x010200
#define CHARACTER_FRAMEWORK_CALL __stdcall
#include "GG.h"

#ifdef __cplusplus
    #include <cstdint>
#else
    #include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct PushColli_C
{
    int16_t stand_width{};
    int16_t stand_height{};
    int16_t sit_width{};
    int16_t sit_height{};
    int16_t sky_width{};
    int16_t sky_height{};
    int16_t sky_base_height{};
};

// Selects this character's name graphic in the shared HUD name sheet: the columns
// x1..x2 of row `row`. The game holds these for the vanilla ids only, so a modded
// character must supply its own or the HUD nameplate draws from uninitialised stack.
struct HudCharName_C
{
    int32_t x1{};
    int32_t x2{};
    int32_t row{};
};

struct StringVector {
    const char** strings{};
    uint32_t num{};
};

struct CharacterFramework_Api {
    uint32_t size;
    uint32_t version;

    void* CHARACTER_FRAMEWORK_CALL (*get_instance)();
    StringVector CHARACTER_FRAMEWORK_CALL (*get_chara_ids)();
    void CHARACTER_FRAMEWORK_CALL (*register_act_tb)(void* act_tb);
    void CHARACTER_FRAMEWORK_CALL (*register_obj_id)(void* obj_id);
    void CHARACTER_FRAMEWORK_CALL (*register_chara_id)(const char* id);
    void CHARACTER_FRAMEWORK_CALL (*register_input_check_func)(int32_t (*func)(CHARACTER_WORK*));
    void CHARACTER_FRAMEWORK_CALL (*register_taunt_check_func)(int32_t (*func)(CHARACTER_WORK*));
    void CHARACTER_FRAMEWORK_CALL (*register_respect_check_func)(int32_t (*func)(CHARACTER_WORK*));
    void CHARACTER_FRAMEWORK_CALL (*register_special_attack_check_func)(int32_t (*func)(CHARACTER_WORK*));
    void CHARACTER_FRAMEWORK_CALL (*register_push_colli)(const PushColli_C* push_colli);
    void CHARACTER_FRAMEWORK_CALL (*register_normal_attack_disable)(uint32_t disable);
    void CHARACTER_FRAMEWORK_CALL (*register_near_slash_dist)(int16_t dist);
    void CHARACTER_FRAMEWORK_CALL (*register_throw_range)(int16_t range);
    void CHARACTER_FRAMEWORK_CALL (*register_air_throw_range_x)(int16_t range);
    void CHARACTER_FRAMEWORK_CALL (*register_air_throw_range_y)(int16_t range);
    void CHARACTER_FRAMEWORK_CALL (*register_air_throw_range_y_bottom)(int16_t range);
    void CHARACTER_FRAMEWORK_CALL (*register_throw_act_no)(uint16_t no);
    void CHARACTER_FRAMEWORK_CALL (*register_throw_damage_no_tb)(const uint16_t* damage_no_tb, uint16_t size);
    void CHARACTER_FRAMEWORK_CALL (*register_air_throw_act_no)(uint16_t no);
    void CHARACTER_FRAMEWORK_CALL (*register_air_throw_damage_no_tb)(const uint16_t* damage_no_tb, uint16_t size);

    // Added in api 1.1.0; check `size` before calling.
    void CHARACTER_FRAMEWORK_CALL (*register_hud_nameplate)(const char* path);

    // Added in api 1.2.0; check `size` before calling.
    void CHARACTER_FRAMEWORK_CALL (*register_hud_portrait)(const char* path);
};

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // CHARACTER_FRAMEWORK_H