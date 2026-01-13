/*-------------------------------------------------------------------------------------------------
* This header is a C++ extension of `ggxxacpr_c.h`
*
* Mod Developers:
*  This header is included by `baseMod.hpp`. If you're forgoing the basemod
*  include this header instead to interface with the game directly.
*
* GearLoader Develeopers:
*  This should only alias types from `ggxxacpr_c.h` and define helper classes/methods
*  This should not include any ABI 
*--------------------------------------------------------------------------------------------------*/

#ifndef GGXXACPR_HPP
#define GGXXACPR_HPP

#include "ggxxacpr_c.h"
#include <type_traits>
#include <utility>
#include <version>

#ifdef __cpp_lib_span
    #include <span>
#endif

namespace ggxxacpr {

    /* ========== #ENUMS ========== */

    enum class GameVersion {
        ACCENT_CORE = GAME_VERSION_ACCENT_CORE,
        PLUS_R = GAME_VERSION_PLUS_R,
    };

    enum class GameMode : uint32_t {
        ARCADE = MAIN_MENU_ITEM_ARCADE,
        MOM = MAIN_MENU_ITEM_MOM,
        VERSUS = MAIN_MENU_ITEM_VERSUS,
        ONLINE = MAIN_MENU_ITEM_ONLINE,
        VS_CPU = MAIN_MENU_ITEM_VS_CPU,
        TEAM_VERSUS = MAIN_MENU_ITEM_TEAM_VERSUS,
        TEAM_VS_CPU = MAIN_MENU_ITEM_TEAM_VS_CPU,
        TRAINING = MAIN_MENU_ITEM_TRAINING,
        SURVIVAL = MAIN_MENU_ITEM_SURVIVAL,
        MISSION = MAIN_MENU_ITEM_MISSION,
        STORY = MAIN_MENU_ITEM_STORY,
        GALLERY = MAIN_MENU_ITEM_GALLERY,
        REPLAY_THEATER = MAIN_MENU_ITEM_REPLAY_THEATER,
        SOUND_TEST = MAIN_MENU_ITEM_SOUND_TEST,
    };

    // Dictates high-level program flow
    enum class JobMod {
        AUTO_LOAD = JOB_MODE_AUTO_LOAD,
        AUTO_SAVE = JOB_MODE_AUTO_SAVE,
        TITLE_SCREEN = JOB_MODE_TITLE_SCREEN,
        INIT_GAME_INFO = JOB_MODE_INIT_GAME_INFO,
        GAME_INIT = JOB_MODE_GAME_INIT,
        BATTLE = JOB_MODE_BATTLE,
        DEMO = JOB_MODE_DEMO,
        TITLE_MAIN = JOB_MODE_TITLE_MAIN,
        MENUS = JOB_MODE_MENUS,
        LOAD_CHAR_SELECT = JOB_MODE_LOAD_CHAR_SELECT,
        LOAD_STAGE_SELECT = JOB_MODE_LOAD_STAGE_SELECT,
        LOAD_VERSUS_SCREEN = JOB_MODE_LOAD_VERSUS_SCREEN,
        LOAD_STORY_MODE_SPLASH_SCREEN = JOB_MODE_LOAD_STORY_MODE_SPLASH_SCREEN,
        LOAD_WIN_SCREEN = JOB_MODE_LOAD_WIN_SCREEN,
        LOAD_STORY_ENDING = JOB_MODE_LOAD_STORY_ENDING,
        OPTION_MENU = JOB_MODE_OPTION_MENU,
        DEBUG_SOUND_MENU = JOB_MODE_DEBUG_SOUND_MENU,
        MISSION_MENU = JOB_MODE_MISSION_MENU,
        STORY_INIT = JOB_MODE_STORY_INIT,
        SREXEC = JOB_MODE_SREXEC,
        CREXEC = JOB_MODE_CREXEC,
        MOVIE_EXECUTE = JOB_MODE_MOVIE_EXECUTE,
        MATC = JOB_MODE_MATC,
        MISSION_WIN = JOB_MODE_MISSION_WIN,
        SURVIVAL_START = JOB_MODE_SURVIVAL_START,
        GALLERY_MAIN = JOB_MODE_GALLERY_MAIN,
        SURVIVAL = JOB_MODE_SURVIVAL,
        TEAM_VS = JOB_MODE_TEAM_VS,
        FRAMERATE_TEST = JOB_MODE_FRAMERATE_TEST,
        REPLAY_SELECT = JOB_MODE_REPLAY_SELECT,
        SOUND_MODE_EXECUTE = JOB_MODE_SOUND_MODE_EXECUTE,
        VERSUS_SPLASH = JOB_MODE_VERSUS_SPLASH,
    };

    enum class GameModeFeatureFlags {
        NONE = GAME_MODE_FEATURE_FLAGS_NONE,
        P1_CONTROL = GAME_MODE_FEATURE_FLAGS_P1_CONTROL,
        P2_CONTROL = GAME_MODE_FEATURE_FLAGS_P2_CONTROL,
        TRAINING = GAME_MODE_FEATURE_FLAGS_TRAINING,
        SURVIVAL = GAME_MODE_FEATURE_FLAGS_SURVIVAL,
        DEMO = GAME_MODE_FEATURE_FLAGS_DEMO,
        VERSUS = GAME_MODE_FEATURE_FLAGS_VERSUS,
        VERSUS_CPU = GAME_MODE_FEATURE_FLAGS_VERSUS_CPU,
        STORY = GAME_MODE_FEATURE_FLAGS_STORY,
        MISSION = GAME_MODE_FEATURE_FLAGS_MISSION,
        WATCH = GAME_MODE_FEATURE_FLAGS_WATCH,
        FREE_PLAY = GAME_MODE_FEATURE_FLAGS_FREE_PLAY,
        TEAM_VERSUS = GAME_MODE_FEATURE_FLAGS_TEAM_VERSUS,
    };

    enum class EntityId {
        NONE = ENTITY_ID_NONE,
        SOL = ENTITY_ID_SOL,
        KY = ENTITY_ID_KY,
        MAY = ENTITY_ID_MAY,
        MILLIA = ENTITY_ID_MILLIA,
        AXL = ENTITY_ID_AXL,
        POTEMKIN = ENTITY_ID_POTEMKIN,
        CHIPP = ENTITY_ID_CHIPP,
        EDDIE = ENTITY_ID_EDDIE,
        BAIKEN = ENTITY_ID_BAIKEN,
        FAUST = ENTITY_ID_FAUST,
        TESTAMENT = ENTITY_ID_TESTAMENT,
        JAM = ENTITY_ID_JAM,
        ANJI = ENTITY_ID_ANJI,
        JOHNNY = ENTITY_ID_JOHNNY,
        VENOM = ENTITY_ID_VENOM,
        DIZZY = ENTITY_ID_DIZZY,
        SLAYER = ENTITY_ID_SLAYER,
        I_NO = ENTITY_ID_I_NO,
        ZAPPA = ENTITY_ID_ZAPPA,
        BRIDGET = ENTITY_ID_BRIDGET,
        ROBO_KY = ENTITY_ID_ROBO_KY,
        ABA = ENTITY_ID_ABA,
        ORDER_SOL = ENTITY_ID_ORDER_SOL,
        KLIFF = ENTITY_ID_KLIFF,
        JUSTICE = ENTITY_ID_JUSTICE,
        DIZZY_ENTITY_1 = ENTITY_ID_DIZZY_ENTITY_1, // Scythe / Ice Spike
        MILLIA_ENTITY_1 = ENTITY_ID_MILLIA_ENTITY_1, // Everything but hair pin and IK
        VENOM_ENTITY_1 = ENTITY_ID_VENOM_ENTITY_1, // Normal balls
        EDDIE_ENTITY_SHADOW = ENTITY_ID_EDDIE_ENTITY_SHADOW,
        KY_ENTITY = ENTITY_ID_KY_ENTITY,
        FAUST_ENTITY_1 = ENTITY_ID_FAUST_ENTITY_1, // Items except for the minis
        SOL_ENTITY = ENTITY_ID_SOL_ENTITY,
        MILLIA_ENTITY_2 = ENTITY_ID_MILLIA_ENTITY_2, // Hair pin / IK
        JAM_ENTITY = ENTITY_ID_JAM_ENTITY,
        VENOM_ENTITY_2 = ENTITY_ID_VENOM_ENTITY_2, // Charged ball
        VENOM_ENTITY_3 = ENTITY_ID_VENOM_ENTITY_3, // Dark Angel / IK
        MILLIA_ENTITY_3 = ENTITY_ID_MILLIA_ENTITY_3, // Main IK entity
        MAY_ENTITY_INSTANT_KILL_1 = ENTITY_ID_MAY_ENTITY_INSTANT_KILL_1,  // Jellyfish pirates
        BAIKEN_ENTITY = ENTITY_ID_BAIKEN_ENTITY,
        BAIKEN_ENTITY_INSTANT_KILL = ENTITY_ID_BAIKEN_ENTITY_INSTANT_KILL,
        CHIPP_ENTITY = ENTITY_ID_CHIPP_ENTITY,
        POTEMKIN_ENTITY = ENTITY_ID_POTEMKIN_ENTITY,
        JOHNNY_ENTITY = ENTITY_ID_JOHNNY_ENTITY,
        FAUST_ENTITY_2 = ENTITY_ID_FAUST_ENTITY_2, // Love explosion / IK
        EDDIE_ENTITY = ENTITY_ID_EDDIE_ENTITY,
        DIZZY_ENTITY_2 = ENTITY_ID_DIZZY_ENTITY_2, // Fish / Knife / Bubble / Supers
        MAY_ENTITY_1 = ENTITY_ID_MAY_ENTITY_1, // Dolphins
        MAY_ENTITY_2 = ENTITY_ID_MAY_ENTITY_2, // Supers
        MAY_ENTITY_3 = ENTITY_ID_MAY_ENTITY_3, // Hoop
        FAUST_ENTITY_3 = ENTITY_ID_FAUST_ENTITY_3,   // Minis / Love
        MAY_ENTITY_INSTANT_KILL_2 = ENTITY_ID_MAY_ENTITY_INSTANT_KILL_2,  // Granny / Mech girl pirates
        ANJI_ENTITY = ENTITY_ID_ANJI_ENTITY,
        AXL_ENTITY_1 = ENTITY_ID_AXL_ENTITY_1, // Rensen / Kokuu Geki / Super / Counter bandana visual / IK
        ANJI_ENTITY_BUTTERFLY = ENTITY_ID_ANJI_ENTITY_BUTTERFLY,
        TESTAMENT_ENTITY_1 = ENTITY_ID_TESTAMENT_ENTITY_1,   // Curse / Net / Nightmare Circular
        TESTAMENT_ENTITY_2 = ENTITY_ID_TESTAMENT_ENTITY_2,   // Tree / EXE Beast
        AXL_ENTITY_2 = ENTITY_ID_AXL_ENTITY_2, // Unblockable / Counter visual / Shiranami no Homura explosion
        TESTAMENT_ENTITY_CROW = ENTITY_ID_TESTAMENT_ENTITY_CROW,
        JOHNNY_ENTITY_COIN = ENTITY_ID_JOHNNY_ENTITY_COIN,
        I_NO_ENTITY = ENTITY_ID_I_NO_ENTITY,
        ZAPP_ENTITY_RAOU = ENTITY_ID_ZAPPA_ENTITY_RAOU,
        SLAYER_ENTITY = ENTITY_ID_SLAYER_ENTITY,
        ZAPPA_ENTITY = ENTITY_ID_ZAPPA_ENTITY,
        ZAPPA_ENTITY_TRIPLETS = ENTITY_ID_ZAPPA_ENTITY_TRIPLETS,
        BRIDGET_ENTITY_1 = ENTITY_ID_BRIDGET_ENTITY_1,
        ZAPPA_ENTITY_SWORD = ENTITY_ID_ZAPPA_ENTITY_SWORD,
        BRIDGET_ENTITY_2 = ENTITY_ID_BRIDGET_ENTITY_2, // KSMH~Stop / Maintenance Disaster / IK
        ZAPPA_ENTITY_DOG = ENTITY_ID_ZAPPA_ENTITY_DOG,
        ROBO_KY_ENTITY_1 = ENTITY_ID_ROBO_KY_ENTITY_1,  // 6P / 6H / 2D / Rocket / Install explosion
        ROBO_KY_ENTITY_2 = ENTITY_ID_ROBO_KY_ENTITY_2,  // 2H / Air Rocket Punch / Mini-Robo-Kys in IK
        ABA_ENTITY = ENTITY_ID_ABA_ENTITY,
        ORDER_SOL_ENTITY_1 = ENTITY_ID_ORDER_SOL_ENTITY_1,
        ORDER_SOL_ENTITY_2 = ENTITY_ID_ORDER_SOL_ENTITY_2,    // Little fire visual on Tyrant Rave (maybe others)
        KLIFF_ENTITY = ENTITY_ID_KLIFF_ENTITY,
        JUSTICE_ENTITY_1 = ENTITY_ID_JUSTICE_ENTITY_1, // Everything but nukes
        JUSTICE_ENTITY_2 = ENTITY_ID_JUSTICE_ENTITY_2, // Nukes
        BURST_ENTITY = ENTITY_ID_BURST_ENTITY,
    };

    enum class StageId {
        GREY = STAGE_ID_GREY,
        LONDON = STAGE_ID_LONDON,
        COLONY = STAGE_ID_COLONY,
        RUSSIA = STAGE_ID_RUSSIA,
        CHINA = STAGE_ID_CHINA,
        MAY_SHIP = STAGE_ID_MAY_SHIP,
        ZEPP = STAGE_ID_ZEPP,
        NIRVANA = STAGE_ID_NIRVANA,
        PARIS = STAGE_ID_PARIS,
        HELL = STAGE_ID_HELL,
        GROVE = STAGE_ID_GROVE,
        VERDANT = STAGE_ID_VERDANT,
        CASTLE = STAGE_ID_CASTLE,
        BABYLON = STAGE_ID_BABYLON,
        PHANTOM_CITY = STAGE_ID_PHANTOM_CITY,
        UNKNOWN = STAGE_ID_UNKNOWN,
        FRASCO = STAGE_ID_FRASCO,
        AD2172 = STAGE_ID_AD2172,
        GRAVE = STAGE_ID_GRAVE,
        HEAVEN = STAGE_ID_HEAVEN,
        KOREA = STAGE_ID_KOREA,

        GREY_RELOAD = STAGE_ID_GREY_RELOAD,
        LONDON_RELOAD = STAGE_ID_LONDON_RELOAD,
        COLONY_RELOAD = STAGE_ID_COLONY_RELOAD,
        RUSSIA_RELOAD = STAGE_ID_RUSSIA_RELOAD,
        CHINA_RELOAD = STAGE_ID_CHINA_RELOAD,
        MAY_SHIP_RELOAD = STAGE_ID_MAY_SHIP_RELOAD,
        ZEPP_RELOAD = STAGE_ID_ZEPP_RELOAD,
        NIRVANA_RELOAD = STAGE_ID_NIRVANA_RELOAD,
        PARIS_RELOAD = STAGE_ID_PARIS_RELOAD,
        HELL_RELOAD = STAGE_ID_HELL_RELOAD,
        GROVE_RELOAD = STAGE_ID_GROVE_RELOAD,
        VERDANT_RELOAD = STAGE_ID_VERDANT_RELOAD,
        CASTLE_RELOAD = STAGE_ID_CASTLE_RELOAD,
        BABYLON_RELOAD = STAGE_ID_BABYLON_RELOAD,
        PHANTOM_CITY_RELOAD = STAGE_ID_PHANTOM_CITY_RELOAD,
        UNKNOWN_RELOAD = STAGE_ID_UNKNOWN_RELOAD,
        FRASCO_RELOAD = STAGE_ID_FRASCO_RELOAD,
        AD2172_RELOAD = STAGE_ID_AD2172_RELOAD,
        GRAVE_RELOAD = STAGE_ID_GRAVE_RELOAD,
        HEAVEN_RELOAD = STAGE_ID_HEAVEN_RELOAD,
        KOREA_RELOAD = STAGE_ID_KOREA_RELOAD,

        GREY_SLASH = STAGE_ID_GREY_SLASH,
        LONDON_SLASH = STAGE_ID_LONDON_SLASH,
        COLONY_SLASH = STAGE_ID_COLONY_SLASH,
        RUSSIA_SLASH = STAGE_ID_RUSSIA_SLASH,
        CHINA_SLASH = STAGE_ID_CHINA_SLASH,
        MAY_SHIP_SLASH = STAGE_ID_MAY_SHIP_SLASH,
        ZEPP_SLASH = STAGE_ID_ZEPP_SLASH,
        NIRVANA_SLASH = STAGE_ID_NIRVANA_SLASH,
        PARIS_SLASH = STAGE_ID_PARIS_SLASH,
        HELL_SLASH = STAGE_ID_HELL_SLASH,
        GROVE_SLASH = STAGE_ID_GROVE_SLASH,
        VERDANT_SLASH = STAGE_ID_VERDANT_SLASH,
        CASTLE_SLASH = STAGE_ID_CASTLE_SLASH,
        BABYLON_SLASH = STAGE_ID_BABYLON_SLASH,
        PHANTOM_CITY_SLASH = STAGE_ID_PHANTOM_CITY_SLASH,
        UNKNOWN_SLASH = STAGE_ID_UNKNOWN_SLASH,
        FRASCO_SLASH = STAGE_ID_FRASCO_SLASH,
        AD2172_SLASH = STAGE_ID_AD2172_SLASH,
        GRAVE_SLASH = STAGE_ID_GRAVE_SLASH,
        HEAVEN_SLASH = STAGE_ID_HEAVEN_SLASH,
        KOREA_SLASH = STAGE_ID_KOREA_SLASH,
    };

    enum class ColliderId {
        DUMMY = COLLIDER_ID_DUMMY,
        HIT_BOX = COLLIDER_ID_HIT_BOX,
        HURT_BOX = COLLIDER_ID_HURT_BOX,
        // Signals the game to use the hitbox of the same array index in `Entity::extraHitboxSet`
        //  instead. This behavior can be seen on Dizzy's knife projectile (421S).
        USE_EXTRA = COLLIDER_ID_USE_EXTRA,
        // Rather than acting like a collider, contains dimensions for
        //  a push box adjustment. Can be seen on Sol's 6H.
        ADJUST_PUSH = COLLIDER_ID_ADJUST_PUSH,
        UNKNOWN_5 = COLLIDER_ID_UNKNOWN_5,
        UNKNOWN_6 = COLLIDER_ID_UNKNOWN_6,
    };

    // Bitflag enums
    enum class ActionState : uint32_t {
            NONE = ACTION_STATE_NONE,
            IS_ENTITY = ACTION_STATE_IS_ENTITY, /* Typically always set unless the enity  */
            COLLIDES_WITH_P2 = ACTION_STATE_COLLIDES_WITH_P2,
            COLLIDES_WITH_P1 = ACTION_STATE_COLLIDES_WITH_P1,
            DRAW_SPRITE = ACTION_STATE_DRAW_SPRITE,
            IS_AIRBORNE = ACTION_STATE_IS_AIRBORNE,
            IS_IN_HITSTUN = ACTION_STATE_IS_IN_HITSTUN,
            DISABLE_HITBOXES = ACTION_STATE_DISABLE_HITBOXES,
            DISABLE_HURTBOXES = ACTION_STATE_DISABLE_HURTBOXES, /* Essentially identical to StrikeInvuln but some mechanics/moves use one over the other. */
            KNOCKED_DOWN = ACTION_STATE_KNOCKED_DOWN,
            IS_IN_BLOCKSTUN = ACTION_STATE_IS_IN_BLOCKSTUN,
            IS_CROUCHING = ACTION_STATE_IS_CROUCHING,
            IS_CORNERED = ACTION_STATE_IS_CORNERED,
            LANDING_FLAG = ACTION_STATE_LANDING_FLAG,
            IS_AT_SCREEN_LIMIT = ACTION_STATE_IS_AT_SCREEN_LIMIT, /* Not necessarily cornered */
            PROJECTILE_INVULN = ACTION_STATE_PROJECTILE_INVULN, /* A legacy value, that's mainly used in throw animations as a sort of invlun flag. */
            WAKEUP = ACTION_STATE_WAKEUP,
            DISABLE_WAKEUP = ACTION_STATE_DISABLE_WAKEUP,  /* Set at round end for the KO'd player. */
            STRIKE_INVLUN = ACTION_STATE_STRIKE_INVLUN,
            IS_IDLE = ACTION_STATE_IS_IDLE,
            SUPER_FLASH = ACTION_STATE_SUPER_FLASH, /* Set only when the entity is frozen in super flash. */
            NO_COLLISION = ACTION_STATE_NO_COLLISION,
            GRAVITY = ACTION_STATE_GRAVITY,
            UNKNOWN_BIT_22 = ACTION_STATE_UNKNOWN_BIT_22,
            THROW_INVULN = ACTION_STATE_THROW_INVULN,
            UNKNOWN_BIT_24 = ACTION_STATE_UNKNOWN_BIT_24,
            UNKNOWN_BIT_25 = ACTION_STATE_UNKNOWN_BIT_25,
            UNKNOWN_BIT_26 = ACTION_STATE_UNKNOWN_BIT_26,
            UNKNOWN_BIT_27 = ACTION_STATE_UNKNOWN_BIT_27,
            UNKNOWN_BIT_28 = ACTION_STATE_UNKNOWN_BIT_28,
            UNKNOWN_BIT_29 = ACTION_STATE_UNKNOWN_BIT_29,
            IGNORE_RECEIVED_HIT_EFFECTS = ACTION_STATE_IGNORE_RECEIVED_HIT_EFFECTS, /* Used for Dizzy bubble. Essentially disables hitstop. */
            DESPAWN = ACTION_STATE_DESPAWN, /* Entity is marked for despawn */
    };

    enum class AttackState : uint32_t {
        NONE = ATTACK_STATE_NONE,
        IS_ATTACK = ATTACK_STATE_IS_ATTACK,
        UNKNOWN_BIT_1 = ATTACK_STATE_UNKNOWN_BIT_1,
        UNKNOWN_BIT_2 = ATTACK_STATE_UNKNOWN_BIT_2,
        UNKNOWN_BIT_3 = ATTACK_STATE_UNKNOWN_BIT_3,
        IN_GATLING_WINDOW = ATTACK_STATE_IN_GATLING_WINDOW,
        SPECIAL_CANCEL_OKAY = ATTACK_STATE_SPECIAL_CANCEL_OKAY,
        UNKNOWN_BIT_6 = ATTACK_STATE_UNKNOWN_BIT_6,
        UNKNOWN_BIT_7 = ATTACK_STATE_UNKNOWN_BIT_7,
        HOMING_JUMP_OKAY = ATTACK_STATE_HOMING_JUMP_OKAY,
        KARA_FD_OKAY = ATTACK_STATE_KARA_FD_OKAY,
        NO_SPECIAL_CANCEL = ATTACK_STATE_NO_SPECIAL_CANCEL,
        IN_RECOVERY = ATTACK_STATE_IN_RECOVERY,
        HAS_CONNECTED = ATTACK_STATE_HAS_CONNECTED,
        UNKNOWN_BIT_13 = ATTACK_STATE_UNKNOWN_BIT_13,
        UNKNOWN_BIT_14 = ATTACK_STATE_UNKNOWN_BIT_14,
        UNKNOWN_BIT_15 = ATTACK_STATE_UNKNOWN_BIT_15,
        JUMP_CANCELABLE = ATTACK_STATE_JUMP_CANCELABLE,
        HAS_HIT_OPPONENT = ATTACK_STATE_HAS_HIT_OPPONENT,
        UNKNOWN_BIT_18 = ATTACK_STATE_UNKNOWN_BIT_18,
        UNKNOWN_BIT_19 = ATTACK_STATE_UNKNOWN_BIT_19,
        UNKNOWN_BIT_20 = ATTACK_STATE_UNKNOWN_BIT_20, /* maybe related to guardpoint */
        UNKNOWN_BIT_21 = ATTACK_STATE_UNKNOWN_BIT_21,
        UNKNOWN_BIT_22 = ATTACK_STATE_UNKNOWN_BIT_22,
        UNKNOWN_BIT_23 = ATTACK_STATE_UNKNOWN_BIT_23,
    };

    enum class CommandState : uint32_t {
        NONE = COMMAND_STATE_NONE,
        NO_NEUTRAL = COMMAND_STATE_NO_NEUTRAL,
        NO_FORWARD = COMMAND_STATE_NO_FORWARD,
        NO_BACKWARD = COMMAND_STATE_NO_BACKWARD,
        NO_CROUCH = COMMAND_STATE_NO_CROUCH,
        UNKNOWN_BIT_4 = COMMAND_STATE_UNKNOWN_BIT_4,
        UNKNOWN_BIT_5 = COMMAND_STATE_UNKNOWN_BIT_5,
        NO_ATTACK = COMMAND_STATE_NO_ATTACK,
        UNKNOWN_BIT_7 = COMMAND_STATE_UNKNOWN_BIT_7,
        UNKNOWN_BIT_8 = COMMAND_STATE_UNKNOWN_BIT_8,
        UNKNOWN_BIT_9 = COMMAND_STATE_UNKNOWN_BIT_9,
        AIR_DASH = COMMAND_STATE_AIR_DASH,
        UKEMI = COMMAND_STATE_UKEMI,
        PREJUMP = COMMAND_STATE_PREJUMP,
        DISABLE_TRHOW = COMMAND_STATE_DISABLE_TRHOW,
        FAUST_CRAWL_FORWARD = COMMAND_STATE_FAUST_CRAWL_FORWARD,
        FAUST_CRAWL_BACKWARD = COMMAND_STATE_FAUST_CRAWL_BACKWARD,
    };

    enum class GuardState : uint16_t {
        NONE = GUARD_STATE_NONE,
        STAND_BLOCKING = GUARD_STATE_STAND_BLOCKING,
        CROUCH_BLOCKING = GUARD_STATE_CROUCH_BLOCKING,
        AIR_BLOCKING = GUARD_STATE_AIR_BLOCKING,
        FAULTLESS_DEFENSE = GUARD_STATE_FAULTLESS_DEFENSE,
        UNKNOWN_BIT_4 = GUARD_STATE_UNKNOWN_BIT_4,
        UNKNOWN_BIT_5 = GUARD_STATE_UNKNOWN_BIT_5,
        IN_BLOCKSTUN = GUARD_STATE_IN_BLOCKSTUN,
        UNKNOWN_BIT_7 = GUARD_STATE_UNKNOWN_BIT_7,
        UNKNOWN_BIT_8 = GUARD_STATE_UNKNOWN_BIT_8,
        GUARD_POINT = GUARD_STATE_GUARD_POINT,
        ARMOR = GUARD_STATE_ARMOR,
        PARRY_1 = GUARD_STATE_PARRY_1,
        PARRY_2 = GUARD_STATE_PARRY_2,
        UNKNOWN_BIT_13 = GUARD_STATE_UNKNOWN_BIT_13,
        UNKNOWN_BIT_14 = GUARD_STATE_UNKNOWN_BIT_14,
        UNKNOWN_BIT_15 = GUARD_STATE_UNKNOWN_BIT_15,
    };

    enum class BackgroundState : uint32_t {
        NONE = BACKGROUND_STATE_NONE,
        UNKNOWN_BIT_0 = BACKGROUND_STATE_UNKNOWN_BIT_0,
        HUD_OFF = BACKGROUND_STATE_HUD_OFF,
        DEFAULT = BACKGROUND_STATE_DEFAULT,
        BLACK_BACKGROUND = BACKGROUND_STATE_BLACK_BACKGROUND,
        POST_FLASH_DIM = BACKGROUND_STATE_POST_FLASH_DIM,
        UNKNOWN_BIT_5 = BACKGROUND_STATE_UNKNOWN_BIT_5,
        UNKNOWN_BIT_6 = BACKGROUND_STATE_UNKNOWN_BIT_6,
        UNKNOWN_BIT_7 = BACKGROUND_STATE_UNKNOWN_BIT_7,
        UNKNOWN_BIT_8 = BACKGROUND_STATE_UNKNOWN_BIT_8,
        UNKNOWN_BIT_9 = BACKGROUND_STATE_UNKNOWN_BIT_9,
        UNKNOWN_BIT_10 = BACKGROUND_STATE_UNKNOWN_BIT_10,
        SUPER_FLASH = BACKGROUND_STATE_SUPER_FLASH,
        UNKNOWN_BIT_12 = BACKGROUND_STATE_UNKNOWN_BIT_12,
        UNKNOWN_BIT_13 = BACKGROUND_STATE_UNKNOWN_BIT_13,
        FORCE_BREAK_SUPER_FLASH = BACKGROUND_STATE_FORCE_BREAK_SUPER_FLASH,
        LIGHTNING = BACKGROUND_STATE_LIGHTNING,
    };

    enum class SpriteRenderState : uint32_t {
        NONE = SPRITE_RENDER_STATE_NONE,
        IGNORE_UNIVERSAL_Y_OFFSET = SPRITE_RENDER_STATE_IGNORE_UNIVERSAL_Y_OFFSET,
        DEAD_ANGLE_OR_FRC_FLASH = SPRITE_RENDER_STATE_DEAD_ANGLE_OR_FRC_FLASH,
        STROBE = SPRITE_RENDER_STATE_STROBE, /* Usually paired with `FIRE` */
        FIRE = SPRITE_RENDER_STATE_FIRE,
        INSTALL_FLASH = SPRITE_RENDER_STATE_INSTALL_FLASH,
        INVISIBLE = SPRITE_RENDER_STATE_INVISIBLE,
        SILHOUETTE = SPRITE_RENDER_STATE_SILHOUETTE,
        MOROHA_MODE_FLASH = SPRITE_RENDER_STATE_MOROHA_MODE_FLASH, /* Also used for Justice's install */
        POISON_FLASH = SPRITE_RENDER_STATE_POISON_FLASH,
        INSTANT_BLOCK_FLASH = SPRITE_RENDER_STATE_INSTANT_BLOCK_FLASH,
        COUNTER_HIT_FLASH = SPRITE_RENDER_STATE_COUNTER_HIT_FLASH,
        UNKNOWN_0x4000000 = SPRITE_RENDER_STATE_UNKNOWN_0x4000000, /* FRC flash maybe? */
        LIGHT_GREEN_FLASH = SPRITE_RENDER_STATE_LIGHT_GREEN_FLASH,
        LIGHT_YELLOW_FLASH = SPRITE_RENDER_STATE_LIGHT_YELLOW_FLASH,
        LIGHT_BLUE_FLASH = SPRITE_RENDER_STATE_LIGHT_BLUE_FLASH,
    };

    // bitflag operators
    template<typename E>
    struct bitflags : std::false_type {};
    template<> struct bitflags<ActionState> : std::true_type {};
    template<> struct bitflags<AttackState> : std::true_type {};
    template<> struct bitflags<CommandState> : std::true_type {};
    template<> struct bitflags<GuardState> : std::true_type {};
    template<> struct bitflags<BackgroundState> : std::true_type {};
    template<> struct bitflags<SpriteRenderState> : std::true_type {};

    template<typename E>
    constexpr std::enable_if_t<bitflags<E>::value, E>
    operator|(E a, E b) noexcept {
        using U = std::underlying_type_t<E>;
        return static_cast<E>(
            static_cast<U>(a) | static_cast<U>(b));
    }
    template <typename E>
    constexpr std::enable_if_t<bitflags<E>::value, E>
    operator&(E a, E b) noexcept {
        using U = std::underlying_type_t<E>;
        return static_cast<E>(
            static_cast<U>(a) & static_cast<U>(b));
    }
    template <typename E>
    constexpr std::enable_if_t<bitflags<E>::value, E>
    operator~(E flags) noexcept {
        using U = std::underlying_type_t<E>;
        return static_cast<E>(~static_cast<U>(flags));
    }
    template <typename E>
    constexpr std::enable_if_t<bitflags<E>::value, E>
    operator|=(E& a, E b) noexcept {
        a = a | b;
        return a;
    }
    template <typename E>
    constexpr std::enable_if_t<bitflags<E>::value, E>
    operator&=(E& a, E b) noexcept {
        a = a & b;
        return a;
    }


    /* ========== #VIEWS ========== */

    /** 
     *  \brief A point in the world.
     * 
     *  Stages walls are at x coordinates -74000 and +74000. Stage floor is at
     *      y coordinate 0. Apparent height has a negative coorelation with the
     *      world coordinate y-axis, so airborne entities have negative y coordinates.
     */
    struct WorldCoordinate { int x; int y; };
    /**
     *  \brief A point in model space.
     * 
     *  Model space is always relative to the model in question. For example, hitbox
     *      offsets and dimensions are given in model coordinates with the model
     *      being the player that owns them.  The player's position is always
     *      coorindate [0,0] in model coordinates. Model coordinates typically
     *      translate to world coordinates at a ratio of 1:100 subject to the model's
     *      scale property.
     */
    struct ModelCoordinate { int x; int y; };
    struct Scale { float x; float y; };

    class PlayerData {
    public:
        PlayerData(GGXXACPR_PlayerData* ref = nullptr)
            : raw(ref) {};

        const uint16_t &tension = raw->tension;
    private:
        GGXXACPR_PlayerData* raw;
    };
    /**
     *  \brief Wraps a `GGXXACPR_Entity` struct pointer and interprets its data
     *      as a player entity.
     * 
     *  This wrapper only exposes data that has a known use and only provides
     *      setters for data that has a known effect when changed. For full,
     *      direct data access, call `Player::getRaw()` and use the
     *      `GGXXACPR_Entity` interface.
     */
    class Player {
    public:
        Player(GGXXACPR_Entity* ref = nullptr)
            : raw(ref) {};
        /** 
         *  \brief Checks if the underlying reference is valid.
         * 
         *  +R's entity data structures are not initialized immediately,
         *  so this method should be used as a null check.
         */
        bool isValid() { return raw != nullptr; }
        /**
         *  \brief Returns the underlying `GGXXACPR_Entity` struct pointer
         */
        GGXXACPR_Entity* const getRaw() { return raw; }


        EntityId id() {
            return static_cast<EntityId>(raw->id);
        }
        bool isFacingRight() {
            return static_cast<bool>(raw->bIsFacingRight);
        }
        bool isLeftSide() {
            return static_cast<bool>(raw->bIsLeftSide);
        }
        ActionState actionState() {
            return static_cast<ActionState>(raw->actionState);
        }
        auto actId() { return raw->actId; }
        auto actTimer() { return raw->actTimer; }
        auto health() { return raw->health; }
        auto playerIndex() { return raw->playerIndex; }
        GuardState guardState() {
            return static_cast<GuardState>(raw->guardState);
        }
        PlayerData playerData() { return raw->playerEntityDataPtr; }
        AttackState attackState() {
            return static_cast<AttackState>(raw->attackState);
        }
        /**
         *  \brief The position of the player's center point.
         * 
         *  Combines `GGXXACPR_Entity::coreX` and `GGXXACPR_Entity::coreY`.
         */
        WorldCoordinate corePos() { return {raw->coreX, raw->coreY}; }
        /**
         *  \brief The scaling factor that should be applied to this player's
         *      colliders.
         * 
         *  An interpretation of `GGXXACPR_Entity::scale` and `GGXXACPR_Entity::scaleY`.
         */
        Scale scale() {
            int16_t rawX = raw->scale;
            int16_t rawY = raw->scaleY;

            if (rawX < 0 && rawY < 0) return {1.0F, 1.0F};

            return {
                (rawX < 0 ? rawY : rawX) / 1000.0F,
                (rawY < 0 ? rawX : rawY) / 1000.0F
            };
        }

        #ifdef __cpp_lib_span
        std::span<GGXXACPR_Collider> colliders() {
            return std::span<GGXXACPR_Collider>(raw->colliderSetPtr, raw->boxCount);
        }
        std::span<GGXXACPR_Collider> extraColliders() {
            return std::span<GGXXACPR_Collider>(raw->extraColliderSetPtr, raw->boxCount);
        }
        #else
        GGXXACPR_Collider* colliders() {
            return raw->colliderSetPtr;
        }
        GGXXACPR_Collider* extraColliders() {
            return raw->extraColliderSetPtr;
        }
        #endif

        /**
         *  \brief The position of the player.
         * 
         *  Sometimes called the "origin point" or "pivot".
         */
        WorldCoordinate position() { return {raw->xPos, raw->yPos}; }
        /**
         *  \brief Player velocity given in world coordinates per frame.
         */
        WorldCoordinate velocity() { return {raw->xVelocity, raw->yVelocity}; }
        /**
         *  \brief Determines which sprites draw in which order.
         * 
         *  Higher values draw first, lower values draw over higher values.
         *  Essentially a position on the z-axis / depth buffer.
         */
        int16_t drawPriority() { return raw->drawPriority; }
        /**
         *  \brief A counter for hitstop.
         */
        uint8_t hitstopTime() { return raw->hitstopTime; }

    private:
        GGXXACPR_Entity *raw;
    };
    /**
     *  \brief Wraps a `GGXXACPR_Entity` struct pointer and interprets its
     *      data as a non-player entity.
     * 
     *  This wrapper only exposes data that has a known use and only provides
     *      setters for data that has a known effect when changed. For full,
     *      direct data access, call `Entity::getRaw()` and use the
     *      `GGXXACPR_Entity` interface.
     */
    class Entity {
    public:
        Entity(GGXXACPR_Entity* ref = nullptr)
            : raw(ref) {}
        /** 
         *  \brief Checks if the underlying reference is valid
         */
        bool isValid() {
            return raw != nullptr && raw->id != 0;
        }
        /**
         *  \brief Returns the underlying `GGXXACPR_Entity` struct pointer
         */
        GGXXACPR_Entity* const getRaw() {
            return raw;
        }
        
        
    private:
        GGXXACPR_Entity *raw;
    };

}

#endif
