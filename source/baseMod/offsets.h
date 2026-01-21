#pragma once

#include <cstdint>

#define OFFSETS_FOR_STEAM_BUILD_ID "12866140"

namespace offsets {

constexpr uint32_t IN_GAME_FLAG = 0x7101F4;
constexpr uint32_t GAME_VER_FLAG = 0x6D0538;    // 0=AC, 1=+R
constexpr uint32_t GAME_MODE = 0x70FFB4;

// Injection Addresses
constexpr uint32_t MESSAGE_LOOP_END = 0x222413;
constexpr uint32_t PRESENT_CALL_INSTRUCTION = 0x2271D8;
constexpr uint32_t GRAPHICS_HOOK_BREAKPOINT = 0x2271DA;
constexpr uint32_t PEEK_MESSAGE_FUNCTION_POINTER = 0x3BD348;
constexpr uint32_t MESSAGE_LOOP_REL_JMP_OFFSET_BYTE_ADDR = 0x222414;
constexpr uint32_t GRAPHICS_HOOK_TARGET_FUNCTION_CALL = 0x22717A;
constexpr uint32_t GRAPHICS_HOOK_TARGET_FUNCTION_ADDRESS = 0x2227E0;
constexpr uint32_t GET_PRESENT_FUNCTION_POINTER_INSTRUCTIONS = 0x2271C5;
constexpr uint32_t UPDATE_GAME_STATE_RET_INSTRUCTION = 0x3A1B78;
constexpr uint32_t COMMON_SIM_UPDATE_FUNCTION_CALL = 0x3A199E;
constexpr uint32_t SET_GRAPHICS_CONTEXT_CALL = 0x227109;

// Native function address
constexpr uint32_t RENDER_TEXT = 0x1E9610;
constexpr uint32_t COMMON_SIM_UPDATE_FUNC = 0x1BD880;
constexpr uint32_t SET_GRAPHICS_CONTEXT_FUNC = 0x225C40;

// Hack Addresses
constexpr uint32_t FIX_BACKGROUND_STATE_INSTRUCTION = 0x21C363;
constexpr uint32_t BACKGROUND_STATE = 0x6D6420;  // see enum BackgroundState

// DirectX
constexpr uint32_t DIRECT3D9_DEVICE = 0x710580;

// Entities
constexpr uint32_t PLAYER_1_PTR = 0x6D1378;
constexpr uint32_t PLAYER_2_PTR = 0x6D4C84;
constexpr uint32_t ENTITY_ARR_HEAD_TAIL_PTR = 0x6D27A8;
constexpr uint32_t ENTITY_LIST_PTR = 0x6D137C;

// Player Data
constexpr uint32_t PLAYER_INPUT = 0x6D0E80;

// Camera
constexpr uint32_t CAMERA = 0x6D5CD0;
constexpr uint32_t VIEW_HEIGHT = 0x6C118C;
constexpr uint32_t VIEW_WIDTH = 0x6C14E4;
constexpr uint32_t WINDOW_MODE = 0x6C1510; // 0=Window 1=Full 2=Borderless

// Pushboxes
constexpr uint32_t PUSHBOX_STANDING_WIDTH_ARRAY = 0x571564;
constexpr uint32_t PUSHBOX_STANDING_HEIGHT_ARRAY = 0x571E6C;
constexpr uint32_t PUSHBOX_CROUCHING_WIDTH_ARRAY = 0x573154;
constexpr uint32_t PUSHBOX_CROUCHING_HEIGHT_ARRAY = 0x573B38;
constexpr uint32_t PUSHBOX_AIR_WIDTH_ARRAY = 0x573B6C;
constexpr uint32_t PUSHBOX_AIR_HEIGHT_ARRAY = 0x573BA0;
// Y offset values for Airborne pushboxes (Almost always equal to abs(YPos)+4000 except for Kliff)
constexpr uint32_t PUSHBOX_P1_JUMP_OFFSET = 0x6D6378;
constexpr uint32_t PUSHBOX_P2_JUMP_OFFSET = 0x6D637C;
constexpr uint32_t PUSHBOX_EDGE_DISTANCE = 0x6D638C;

// Throws
constexpr uint32_t PLUSR_GROUND_THROW_RANGE_ARRAY = 0x57005C;
constexpr uint32_t AC_GROUND_THROW_RANGE_ARRAY = 0x56FF6C;
constexpr uint32_t PLUSR_AIR_THROW_HORIZONTAL_RANGE_ARRAY = 0x5708DC;
constexpr uint32_t AC_AIR_THROW_HORIZONTAL_RANGE_ARRAY = 0x570174;
constexpr uint32_t AIR_THROW_LOWER_RANGE_ARRAY = 0x5709B4;
constexpr uint32_t AIR_THROW_UPPER_RANGE_ARRAY = 0x570A8C;
constexpr uint32_t COMMAND_GRAB_ID_P1 = 0x6D6384;
constexpr uint32_t COMMAND_GRAB_ID_P2 = 0x6D6388;
constexpr uint32_t COMMAND_GRAB_RANGE_LOOKUP_TABLE = 0x572110;
// one byte [P1Throwable, P2Throwable, P1ThrowActive P2ThrowActive]
constexpr uint32_t GLOBAL_THROW_FLAGS = 0x6D5D7C;

// Pause Menus
// 0 = not paused, 1 or 2 = paused (not sure the difference between 1 and 2)
constexpr uint32_t TRAINING_MODE_PAUSE_STATE = 0x7109E4;
constexpr uint32_t TRAINING_MODE_PAUSE_DISPLAY = 0x6CBD20;

// Replay
// 1 = normal, 0 = do not simulate, -1 = rewinding (stays at 0 for frame stepping)
constexpr uint32_t GLOBAL_REPLAY_SIMULATE = 0x7D5788;
constexpr uint32_t REPLAY_FRAME_COUNT = 0x7D57D8;

// Program flow
constexpr int32_t JOB_MODE = 0x7109EC;

}