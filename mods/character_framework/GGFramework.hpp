#pragma once
#include <memory>
#include <mutex>
#include <string>
#include <vector>

#include "safetyhook.hpp"

struct PushColli
{
    int16_t stand_width{};
    int16_t stand_height{};
    int16_t sit_width{};
    int16_t sit_height{};
    int16_t sky_width{};
    int16_t sky_height{};
    int16_t sky_base_height{};
};

struct HudNamePlate
{
    std::string path{};
    std::unique_ptr<char[]> data{};
    int32_t sprite_id{-1};
    bool load_failed{};
    int32_t width{};
};

struct BaseMod_NativeFunctionsApi;

class GGFramework
{
private:
    GGFramework()
    {
        initialize();
    }

    static GGFramework* instance_;
    static std::mutex mtx_;

    SafetyHookMid player_main_hook_{};
    SafetyHookMid hud_char_name_hook_{};
    SafetyHookMid hud_char_name_row_hook_{};
    SafetyHookMid hud_nameplate_hook_{};
    SafetyHookMid load_obj_file_hook_{};
    SafetyHookMid load_obj_file_hook_2_{};
    SafetyHookMid load_obj_file_hook_3_{};
    SafetyHookMid load_obj_file_hook_4_{};
    SafetyHookMid load_obj_file_hook_5_{};
    SafetyHookMid allocate_file_hook_{};
    SafetyHookMid allocate_file_hook_2_{};
    SafetyHookMid chara_select_hook_{};
    SafetyHookMid chara_select_hook_2_{};
    SafetyHookMid chara_select_hook_3_{};
    SafetyHookMid set_game_version_hook_{};
    SafetyHookMid load_dlc_hook_{};
    SafetyHookMid obj_file_link_hook_{};
    SafetyHookMid input_check_hook_{};
    SafetyHookMid taunt_check_hook_{};
    SafetyHookMid respect_check_hook_{};
    SafetyHookMid special_attack_check_hook_{};
    SafetyHookMid player_action_break_hook_{};
    SafetyHookMid char_pos_collision_execute_hook_{};
    SafetyHookMid char_pos_collision_execute_hook_2_{};
    SafetyHookMid char_pos_collision_execute_hook_3_{};
    SafetyHookMid char_pos_collision_execute_hook_4_{};
    SafetyHookMid char_pos_collision_execute_hook_5_{};
    SafetyHookMid char_pos_collision_execute_hook_6_{};
    SafetyHookMid char_pos_collision_execute_hook_7_{};
    SafetyHookMid char_pos_collision_execute_hook_8_{};
    SafetyHookMid char_pos_collision_execute_hook_9_{};
    SafetyHookMid char_pos_collision_execute_hook_10_{};
    SafetyHookMid char_pos_collision_execute_hook_11_{};
    SafetyHookMid char_pos_collision_execute_hook_12_{};
    SafetyHookMid char_pos_collision_execute_hook_13_{};
    SafetyHookMid char_pos_collision_execute_hook_14_{};
    SafetyHookMid normal_attack_disable_table_hook_{};
    SafetyHookMid normal_attack_disable_table_hook_2_{};
    SafetyHookMid cmn_attack_check_hook_{};
    SafetyHookMid player_throw_flag_set_hook_{};
    SafetyHookMid player_throw_flag_set_hook_2_{};
    SafetyHookMid throw_range_check_hook_{};
    SafetyHookMid throw_range_check_hook_2_{};
    SafetyHookMid air_throw_range_check_hook_x_{};
    SafetyHookMid air_throw_range_check_hook_x_2_{};
    SafetyHookMid air_throw_range_check_hook_y_{};
    SafetyHookMid air_throw_range_check_hook_y_bottom_{};
    SafetyHookMid player_throw_check_hook_{};
    SafetyHookMid player_throw_check_hook_2_{};
    SafetyHookMid player_throw_check_hook_3_{};
    SafetyHookMid player_throw_check_hook_4_{};

    static std::vector<void*> act_tbs;
    static std::vector<void*> obj_ids;
    static std::vector<const char*> chara_ids_;
    static std::vector<std::string> chara_paths_;
    static std::vector<int32_t(*)(struct CHARACTER_WORK*)> input_check_funcs_;
    static std::vector<int32_t(*)(CHARACTER_WORK*)> taunt_check_funcs_;
    static std::vector<int32_t(*)(CHARACTER_WORK*)> respect_check_funcs_;
    static std::vector<int32_t(*)(CHARACTER_WORK*)> special_attack_check_funcs_;
    static std::vector<PushColli> push_collis_;
    static std::vector<HudNamePlate> hud_nameplates_;
    static const BaseMod_NativeFunctionsApi* native_functions_;
    static std::vector<uint32_t> normal_attack_disables_;
    static std::vector<int16_t> near_slash_dists_;
    static std::vector<int16_t> throw_ranges_;
    static std::vector<int16_t> air_throw_ranges_x_;
    static std::vector<int16_t> air_throw_ranges_y_;
    static std::vector<int16_t> air_throw_ranges_y_bottom_;
    static std::vector<uint16_t> throw_act_nos_;
    static std::vector<std::vector<uint16_t>> throw_damage_no_tbs;
    static std::vector<uint16_t> air_throw_act_nos_;
    static std::vector<std::vector<uint16_t>> air_throw_damage_no_tbs;
    static int* game_version_;
    auto initialize() -> void;

    static auto get_mod_chara_id_idx(uint32_t chara_id, uint32_t& out_idx) -> bool;
    static auto get_mod_chara_path(uint32_t idx) -> const char*;
    static auto file_id_to_mod_path(uint32_t file_id) -> const char*;
    static auto get_chara_mod_path(int plno) -> const char*;
    static auto get_mod_hud_nameplate(int plno) -> const HudNamePlate*;
    static auto ensure_hud_nameplate_registration() -> void;
    static auto acquire_texture_slot() -> int32_t;

public:
    GGFramework(const GGFramework&) = delete;

    /**
     * Retrieve the global GGFramework singleton.
     * @return The GGFramework singleton.
     */
    static auto get_instance() -> GGFramework*;

    /**
     * Retrieve all registered character IDs.
     * @return The registered character IDs.
     */
    static auto get_chara_ids() -> std::vector<const char*>&;

    /**
     * Register a set of actions for your character.
     * @param act_tb Array of function pointers per action.
     */
    static auto register_act_tb(void* act_tb) -> void;

    /**
     * Register a set of object IDs for your character.
     * @param obj_id Array of object IDs.
     */
    static auto register_obj_id(void* obj_id) -> void;

    /**
     * Register a character ID for your character. Used for finding the bin file in obj.
     * @param id Character ID, or the filename of your bin file (without extension).
     */
    static auto register_chara_id(const std::string& id) -> void;

    /**
     * Register an input handler function for your character. Used to define command inputs.
     * @param func The input handler function.
     */
    static auto register_input_check_func(int32_t (*func)(CHARACTER_WORK*)) -> void;

    /**
     * Register a taunt handler function for your character.
     * @param func The taunt handler function.
     */
    static auto register_taunt_check_func(int32_t (*func)(CHARACTER_WORK*)) -> void;

    /**
     * Register a respect handler function for your character.
     * @param func The taunt handler function.
     */
    static auto register_respect_check_func(int32_t (*func)(CHARACTER_WORK*)) -> void;
    static auto register_special_attack_check_func(int32_t (*func)(CHARACTER_WORK*)) -> void;

    /**
     * Register push collision data for your character.
     * @param push_colli The push collision data.
     */
    static auto register_push_colli(const PushColli& push_colli) -> void;

    /**
     * Register a HUD nameplate for your character.
     *
     * @param path Path to a resource .bin, relative to the working directory.
     */
    static auto register_hud_nameplate(const std::string& path) -> void;

    /**
     * Supply baseMod's native function API.
     * @param api The BaseMod_NativeFunctionsApi, or nullptr if baseMod is unavailable.
     */
    static auto set_native_functions(const BaseMod_NativeFunctionsApi* api) -> void;

    /**
     * Register disabled normal attacks for your character.
     * @param disable The disabled normal attacks.
     */
    static auto register_normal_attack_disable(uint32_t disable) -> void;

    /**
     * Register the maximum close slash distance for your character.
     * @param dist The maximum close slash distance.
     */
    static auto register_near_slash_dist(int16_t dist) -> void;

    /**
     * Register the ground throw range.
     * @param range The ground throw range.
     */
    static auto register_throw_range(int16_t range) -> void;

    /**
     * Register the air throw x-axis range.
     * @param range The air throw x-axis range.
     */
    static auto register_air_throw_range_x(int16_t range) -> void;

    /**
     * Register the air throw y-axis range.
     * @param range The air throw y-axis range.
     */
    static auto register_air_throw_range_y(int16_t range) -> void;

    /**
     * Register the air throw y-axis bottom.
     * @param range The air throw y-axis bottom.
     */
    static auto register_air_throw_range_y_bottom(int16_t range) -> void;

    /**
     * Register the throw action number.
     * @param no The throw action number.
     */
    static auto register_throw_act_no(uint16_t no) -> void;

    /**
     * Register the enemy's throw reaction numbers.
     * @param no The enemy's throw reaction numbers.
     */
    static auto register_throw_damage_no_tb(const std::vector<uint16_t> &tb) -> void;

    /**
     * Register the air throw action number.
     * @param no The air throw action number.
     */
    static auto register_air_throw_act_no(uint16_t no) -> void;

    /**
     * Register the enemy's air throw reaction numbers.
     * @param no The enemy's air throw reaction numbers.
     */
    static auto register_air_throw_damage_no_tb(const std::vector<uint16_t> &tb) -> void;
};
