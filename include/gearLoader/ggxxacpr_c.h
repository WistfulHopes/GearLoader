/*-------------------------------------------------------------------------------------------------
* This header serves as the GGXXACPR ABI which defines its native data types and structures.
*
* Mod Developers:
*  This header is included by `baseMod_c.h`. If you're forgoing the basemod
*  include this header instead to interface with the game directly.
*
* GearLoader Developers:
*  Generally, feature additions to the API should be added to `baseMod.h`.
*  This file should only be changed to address compiler compatibility, breaking
*  game updates, indentify unknown fields, and refine field names.
*--------------------------------------------------------------------------------------------------*/

#ifndef GGXXACPR_H
#define GGXXACPR_H

#define GGXXACPR_ABI_VERSION "0.1.0"

#if __cplusplus
    #include <cstdint>
#else
    #include <stdint.h>
#endif

// Used for fields that have a known data type, but unknown use
#define UNKNOWN_FIELD(label) _unknown_##label
// Used for stretchs of struct data that are either unknown or unused
#define PAD(num_bytes, label) uint8_t _padding_##label[num_bytes]

static_assert(sizeof(float) == 4, "float size must be 32-bit");
static_assert(sizeof(void*) == 4, "pointer size must be 32-bit");


/* ========== #ENUMS ========== */

enum GGXXACPR_GameVersion {
    GAME_VERSION_ACCENT_CORE = 0,
    GAME_VERSION_PLUS_R
};

enum GGXXACPR_GameMode {
    GAME_MODE_ARCADE = 0,
    GAME_MODE_MOM,
    GAME_MODE_VERSUS,
    GAME_MODE_ONLINE,
    GAME_MODE_VS_CPU,
    GAME_MODE_TEAM_VERSUS,
    GAME_MODE_TEAM_VS_CPU,
    GAME_MODE_TRAINING,
    GAME_MODE_SURVIVAL,
    GAME_MODE_MISSION,
    GAME_MODE_STORY,
    GAME_MODE_GALLERY,
    GAME_MODE_REPLAY_THEATER,
    GAME_MODE_SOUND_TEST
};

// Character / entity group ids
enum GGXXACPR_EntityId {
    ENTITY_ID_NONE = 0,
    ENTITY_ID_SOL = 1,
    ENTITY_ID_KY = 2,
    ENTITY_ID_MAY = 3,
    ENTITY_ID_MILLIA = 4,
    ENTITY_ID_AXL = 5,
    ENTITY_ID_POTEMKIN = 6,
    ENTITY_ID_CHIPP = 7,
    ENTITY_ID_EDDIE = 8,
    ENTITY_ID_BAIKEN = 9,
    ENTITY_ID_FAUST = 10,
    ENTITY_ID_TESTAMENT = 11,
    ENTITY_ID_JAM = 12,
    ENTITY_ID_ANJI = 13,
    ENTITY_ID_JOHNNY = 14,
    ENTITY_ID_VENOM = 15,
    ENTITY_ID_DIZZY = 16,
    ENTITY_ID_SLAYER = 17,
    ENTITY_ID_I_NO = 18,
    ENTITY_ID_ZAPPA = 19,
    ENTITY_ID_BRIDGET = 20,
    ENTITY_ID_ROBO_KY = 21,
    ENTITY_ID_ABA = 22,
    ENTITY_ID_ORDER_SOL = 23,
    ENTITY_ID_KLIFF = 24,
    ENTITY_ID_JUSTICE = 25,
    ENTITY_ID_DIZZY_ENTITY_1 = 0x20, // Scythe / Ice Spike
    ENTITY_ID_MILLIA_ENTITY_1 = 0x21, // Everything but hair pin and IK
    ENTITY_ID_VENOM_ENTITY_1 = 0x22, // Normal balls
    ENTITY_ID_EDDIE_ENTITY_SHADOW = 0x23,
    ENTITY_ID_KY_ENTITY = 0x29,
    ENTITY_ID_FAUST_ENTITY_1 = 0x2B, // Items except for the minis
    ENTITY_ID_SOL_ENTITY = 0x2C,
    ENTITY_ID_MILLIA_ENTITY_2 = 0x2E, // Hair pin / IK
    ENTITY_ID_JAM_ENTITY = 0x2F,
    ENTITY_ID_VENOM_ENTITY_2 = 0x31, // Charged ball
    ENTITY_ID_VENOM_ENTITY_3 = 0x32, // Dark Angel / IK
    ENTITY_ID_MILLIA_ENTITY_3 = 0x34, // Main IK entity
    ENTITY_ID_MAY_ENTITY_INSTANT_KILL_1 = 0x37,  // Jellyfish pirates
    ENTITY_ID_BAIKEN_ENTITY = 0x39,
    ENTITY_ID_BAIKEN_ENTITY_INSTANT_KILL = 0x3A,
    ENTITY_ID_CHIPP_ENTITY = 0x3B,
    ENTITY_ID_POTEMKIN_ENTITY = 0x3D,
    ENTITY_ID_JOHNNY_ENTITY = 0x3F,
    ENTITY_ID_FAUST_ENTITY_2 = 0x40, // Love explosion / IK
    ENTITY_ID_EDDIE_ENTITY = 0x41,
    ENTITY_ID_DIZZY_ENTITY_2 = 0x43, // Fish / Knife / Bubble / Supers
    ENTITY_ID_MAY_ENTITY_1 = 0x44, // Dolphins
    ENTITY_ID_MAY_ENTITY_2 = 0x46, // Supers
    ENTITY_ID_MAY_ENTITY_3 = 0x48, // Hoop
    ENTITY_ID_FAUST_ENTITY_3 = 0x4A,   // Minis / Love
    ENTITY_ID_MAY_ENTITY_INSTANT_KILL_2 = 0x4B,  // Granny / Mech girl pirates
    ENTITY_ID_ANJI_ENTITY = 0x4C,
    ENTITY_ID_AXL_ENTITY_1 = 0x4D, // Rensen / Kokuu Geki / Super / Counter bandana visual / IK
    ENTITY_ID_ANJI_ENTITY_BUTTERFLY = 0x4E,
    ENTITY_ID_TESTAMENT_ENTITY_1 = 0x4F,   // Curse / Net / Nightmare Circular
    ENTITY_ID_TESTAMENT_ENTITY_2 = 0x50,   // Tree / EXE Beast
    ENTITY_ID_AXL_ENTITY_2 = 0x51, // Unblockable / Counter visual / Shiranami no Homura explosion
    ENTITY_ID_TESTAMENT_ENTITY_CROW = 0x52,
    ENTITY_ID_JOHNNY_ENTITY_COIN = 0x55,
    ENTITY_ID_I_NO_ENTITY = 0x56,
    ENTITY_ID_ZAPPA_ENTITY_RAOU = 0x57,
    ENTITY_ID_SLAYER_ENTITY = 0x58,
    ENTITY_ID_ZAPPA_ENTITY = 0x59,
    ENTITY_ID_ZAPPA_ENTITY_TRIPLETS = 0x5B,
    ENTITY_ID_BRIDGET_ENTITY_1 = 0x5C,
    ENTITY_ID_ZAPPA_ENTITY_SWORD = 0x5D,
    ENTITY_ID_BRIDGET_ENTITY_2 = 0x5E, // KSMH~Stop / Maintenance Disaster / IK
    ENTITY_ID_ZAPPA_ENTITY_DOG = 0x60,
    ENTITY_ID_ROBO_KY_ENTITY_1 = 0x62,  // 6P / 6H / 2D / Rocket / Install explosion
    ENTITY_ID_ROBO_KY_ENTITY_2 = 0x63,  // 2H / Air Rocket Punch / Mini-Robo-Kys in IK
    ENTITY_ID_ABA_ENTITY = 0x64,
    ENTITY_ID_ORDER_SOL_ENTITY_1 = 0x67,
    ENTITY_ID_ORDER_SOL_ENTITY_2 = 0x68,    // Little fire visual on Tyrant Rave (maybe others)
    ENTITY_ID_KLIFF_ENTITY = 0x6C,
    ENTITY_ID_JUSTICE_ENTITY_1 = 0x6E, // Everything but nukes
    ENTITY_ID_JUSTICE_ENTITY_2 = 0x6F, // Nukes
    ENTITY_ID_BURST_ENTITY = 0x84,
};

enum GGXXACPR_StageId {
    STAGE_ID_GREY = 0,
    STAGE_ID_LONDON = 1,
    STAGE_ID_COLONY = 2,
    STAGE_ID_RUSSIA = 3,
    STAGE_ID_CHINA = 4,
    STAGE_ID_MAY_SHIP = 5,
    STAGE_ID_ZEPP = 6,
    STAGE_ID_NIRVANA = 7,
    STAGE_ID_PARIS = 8,
    STAGE_ID_HELL = 9,
    STAGE_ID_GROVE = 10,
    STAGE_ID_VERDANT = 11,
    STAGE_ID_CASTLE = 12,
    STAGE_ID_BABYLON = 13,
    STAGE_ID_PHANTOM_CITY = 14,
    STAGE_ID_UNKNOWN = 15,
    STAGE_ID_FRASCO = 16,
    STAGE_ID_AD2172 = 17,
    STAGE_ID_GRAVE = 18,
    STAGE_ID_HEAVEN = 19,
    STAGE_ID_KOREA = 20
};

enum GGXXACPR_ActionState {
    ACTION_STATE_NONE = 0x0,
    ACTION_STATE_IS_ENTITY = 0x1, /* Typically always set unless the enity  */
    ACTION_STATE_COLLIDES_WITH_P2 = 0x2,
    ACTION_STATE_COLLIDES_WITH_P1 = 0x4,
    ACTION_STATE_DRAW_SPRITE = 0x8,
    ACTION_STATE_IS_AIRBORNE = 0x10,
    ACTION_STATE_IS_IN_HITSTUN = 0x20,
    ACTION_STATE_DISABLE_HITBOXES = 0x40,
    ACTION_STATE_DISABLE_HURTBOXES = 0x80, /* Essentially identical to StrikeInvuln but some mechanics/moves use one over the other. */
    ACTION_STATE_KNOCKED_DOWN = 0x100,
    ACTION_STATE_IS_IN_BLOCKSTUN = 0x200,
    ACTION_STATE_IS_CROUCHING = 0x400,
    ACTION_STATE_IS_CORNERED = 0x800,
    ACTION_STATE_LANDING_FLAG = 0x1000,
    ACTION_STATE_IS_AT_SCREEN_LIMIT = 0x2000, /* Not necessarily cornered */
    ACTION_STATE_PROJECTILE_INVULN = 0x4000, /* A legacy value, that's mainly used in throw animations as a sort of invlun flag. */
    ACTION_STATE_WAKEUP = 0x8000,
    ACTION_STATE_DISABLE_WAKEUP = 0x10000,  /* Set at round end for the KO'd player. */
    ACTION_STATE_STRIKE_INVLUN = 0x20000,
    ACTION_STATE_IS_IDLE = 0x40000,
    ACTION_STATE_SUPER_FLASH = 0x80000, /* Set only when the entity is frozen in super flash. */
    ACTION_STATE_NO_COLLISION = 0x100000,
    ACTION_STATE_GRAVITY = 0x200000,
    ACTION_STATE_UNKNOWN_BIT_22 = 0x400000,
    ACTION_STATE_THROW_INVULN = 0x800000,
    ACTION_STATE_UNKNOWN_BIT_24 = 0x1000000,
    ACTION_STATE_UNKNOWN_BIT_25 = 0x2000000,
    ACTION_STATE_UNKNOWN_BIT_26 = 0x4000000,
    ACTION_STATE_UNKNOWN_BIT_27 = 0x8000000,
    ACTION_STATE_UNKNOWN_BIT_28 = 0x10000000,
    ACTION_STATE_UNKNOWN_BIT_29 = 0x20000000,
    ACTION_STATE_IGNORE_RECEIVED_HIT_EFFECTS = 0x40000000, /* Used for Dizzy bubble. Essentially disables hitstop. */
    ACTION_STATE_DESPAWN = 0x80000000, /* Entity is marked for despawn */
};

enum GGXXACPR_AttackState {
    ATTACK_STATE_NONE = 0x0,
    ATTACK_STATE_IS_ATTACK = 0x1,
    ATTACK_STATE_UNKNOWN_BIT_1 = 0x2,
    ATTACK_STATE_UNKNOWN_BIT_2 = 0x4,
    ATTACK_STATE_UNKNOWN_BIT_3 = 0x8,
    ATTACK_STATE_IN_GATLING_WINDOW = 0x10,
    ATTACK_STATE_SPECIAL_CANCEL_OKAY = 0x20,
    ATTACK_STATE_UNKNOWN_BIT_6 = 0x40,
    ATTACK_STATE_UNKNOWN_BIT_7 = 0x80,
    ATTACK_STATE_HOMING_JUMP_OKAY = 0x100,
    ATTACK_STATE_KARA_FD_OKAY = 0x200,
    ATTACK_STATE_NO_SPECIAL_CANCEL = 0x400,
    ATTACK_STATE_IN_RECOVERY = 0x800,
    ATTACK_STATE_HAS_CONNECTED = 0x1000,
    ATTACK_STATE_UNKNOWN_BIT_13 = 0x2000,
    ATTACK_STATE_UNKNOWN_BIT_14 = 0x4000,
    ATTACK_STATE_UNKNOWN_BIT_15 = 0x8000,
    ATTACK_STATE_JUMP_CANCELABLE = 0x10000,
    ATTACK_STATE_HAS_HIT_OPPONENT = 0x20000,
    ATTACK_STATE_UNKNOWN_BIT_18 = 0x40000,
    ATTACK_STATE_UNKNOWN_BIT_19 = 0x80000,
    ATTACK_STATE_UNKNOWN_BIT_20 = 0x100000, /* maybe related to guardpoint */
    ATTACK_STATE_UNKNOWN_BIT_21 = 0x200000,
    ATTACK_STATE_UNKNOWN_BIT_22 = 0x400000,
    ATTACK_STATE_UNKNOWN_BIT_23 = 0x800000
};

// Command state flags. Still unsure on exact usage of these values, so
//  take their names and descriptions with a grain of salt.
//  Command state flags relate to available state transitions excluding
//  gatlings, cancels, and kara cancels.
enum GGXXACPR_CommandState {
    COMMAND_STATE_NONE = 0x0,
    COMMAND_STATE_NO_NEUTRAL = 0x1,
    COMMAND_STATE_NO_FORWARD = 0x2,
    COMMAND_STATE_NO_BACKWARD = 0x4,
    COMMAND_STATE_NO_CROUCH = 0x8,
    COMMAND_STATE_UNKNOWN_BIT_4 = 0x10,
    COMMAND_STATE_UNKNOWN_BIT_5 = 0x20,
    COMMAND_STATE_NO_ATTACK = 0x40,
    COMMAND_STATE_UNKNOWN_BIT_7 = 0x80,
    COMMAND_STATE_UNKNOWN_BIT_8 = 0x100,
    COMMAND_STATE_UNKNOWN_BIT_9 = 0x200,
    COMMAND_STATE_AIR_DASH = 0x400,
    COMMAND_STATE_UKEMI = 0x800,
    COMMAND_STATE_PREJUMP = 0x1000,
    COMMAND_STATE_DISABLE_TRHOW = 0x2000,
    COMMAND_STATE_FAUST_CRAWL_FORWARD = 0x4000,
    COMMAND_STATE_FAUST_CRAWL_BACKWARD = 0x8000,
};

enum GGXXACPR_GuardState {
    GUARD_STATE_NONE = 0,
    GUARD_STATE_STAND_BLOCKING = 0x1,
    GUARD_STATE_CROUCH_BLOCKING = 0x2,
    GUARD_STATE_AIR_BLOCKING = 0x4,
    GUARD_STATE_FAULTLESS_DEFENSE = 0x8,
    GUARD_STATE_UNKNOWN_BIT_4 = 0x10,
    GUARD_STATE_UNKNOWN_BIT_5 = 0x20,
    GUARD_STATE_IN_BLOCKSTUN = 0x40,
    GUARD_STATE_UNKNOWN_BIT_7 = 0x80,
    GUARD_STATE_UNKNOWN_BIT_8 = 0x100,
    GUARD_STATE_GUARD_POINT = 0x200,
    GUARD_STATE_ARMOR = 0x400,
    GUARD_STATE_PARRY_1 = 0x800,
    GUARD_STATE_PARRY_2 = 0x1000,
    GUARD_STATE_UNKNOWN_BIT_13 = 0x2000,
    GUARD_STATE_UNKNOWN_BIT_14 = 0x4000,
    GUARD_STATE_UNKNOWN_BIT_15 = 0x8000,
};

enum GGXXACPR_BackgroundState {
    BACKGROUND_STATE_NONE = 0,
    BACKGROUND_STATE_UNKNOWN_BIT_0 = 0x1,
    BACKGROUND_STATE_HUD_OFF = 0x2,
    BACKGROUND_STATE_DEFAULT = 0x4,
    BACKGROUND_STATE_BLACK_BACKGROUND = 0x8,
    BACKGROUND_STATE_POST_FLASH_DIM = 0x10,
    BACKGROUND_STATE_UNKNOWN_BIT_5 = 0x20,
    BACKGROUND_STATE_UNKNOWN_BIT_6 = 0x40,
    BACKGROUND_STATE_UNKNOWN_BIT_7 = 0x80,
    BACKGROUND_STATE_UNKNOWN_BIT_8 = 0x100,
    BACKGROUND_STATE_UNKNOWN_BIT_9 = 0x200,
    BACKGROUND_STATE_UNKNOWN_BIT_10 = 0x400,
    BACKGROUND_STATE_SUPER_FLASH = 0x800,
    BACKGROUND_STATE_UNKNOWN_BIT_12 = 0x1000,
    BACKGROUND_STATE_UNKNOWN_BIT_13 = 0x2000,
    BACKGROUND_STATE_FORCE_BREAK_SUPER_FLASH = 0x4000,
    BACKGROUND_STATE_LIGHTNING = 0x8000,
};

enum GGXXACPR_SpriteRenderState {
    SPRITE_RENDER_STATE_NONE = 0x0,
    SPRITE_RENDER_STATE_IGNORE_UNIVERSAL_Y_OFFSET = 0x8,
    SPRITE_RENDER_STATE_DEAD_ANGLE_OR_FRC_FLASH = 0x200,
    SPRITE_RENDER_STATE_STROBE = 0x400, /* Usually paired with `FIRE` */
    SPRITE_RENDER_STATE_FIRE = 0x1000,
    SPRITE_RENDER_STATE_INSTALL_FLASH = 0x4000,
    SPRITE_RENDER_STATE_INVISIBLE = 0x8000,
    SPRITE_RENDER_STATE_SILHOUETTE = 0x10000,
    SPRITE_RENDER_STATE_MOROHA_MODE_FLASH = 0x80000, /* Also used for Justice's install */
    SPRITE_RENDER_STATE_POISON_FLASH = 0x100000,
    SPRITE_RENDER_STATE_INSTANT_BLOCK_FLASH = 0x800000,
    SPRITE_RENDER_STATE_COUNTER_HIT_FLASH = 0x1000000,
    SPRITE_RENDER_STATE_UNKNOWN_0x4000000 = 0x4000000, /* FRC flash maybe? */
    SPRITE_RENDER_STATE_LIGHT_GREEN_FLASH = 0x8000000,
    SPRITE_RENDER_STATE_LIGHT_YELLOW_FLASH = 0x10000000,
    SPRITE_RENDER_STATE_LIGHT_BLUE_FLASH = 0x20000000
};

enum GGXXACPR_ColliderId {
    COLLIDER_ID_DUMMY,
    COLLIDER_ID_HIT_BOX,
    COLLIDER_ID_HURT_BOX,
    // Signals the game to use the hitbox of the same array index in `Entity::extraHitboxSet`
    //  instead. This behavior can be seen on Dizzy's knife projectile (421S).
    COLLIDER_ID_USE_EXTRA,
    // Rather than acting like a collider, contains dimensions for
    //  a push box adjustment. Can be seen on Sol's 6H.
    COLLIDER_ID_ADJUST_PUSH,
    COLLIDER_ID_UNKNOWN_5,
    COLLIDER_ID_UNKNOWN_6,
};


/* ========== #STRUCTS ========== */

typedef struct GGXXACPR_Collider {
    int16_t xOffset;
    int16_t yOffset;
    int16_t width;
    int16_t height;
    uint16_t boxTypeId; /* enum COLLIDER_ID */
    uint16_t boxFlags; /* unknown */
} GGXXACPR_Collider;
static_assert(sizeof(GGXXACPR_Collider) == 0xC );

typedef struct GGXXACPR_ActionHeader {
    uint32_t ActHeaderFlags;
    int16_t lvlFlag;
    uint8_t damage;
    uint8_t flag2;
} GGXXACPR_ActionHeader;

typedef struct GGXXACPR_HitParam {
    int16_t launchVelocityX;
    int16_t launchVelocityY;
    PAD(40, 0x05);
    int16_t counterHitLaunchVelocityX;
    int16_t counterHitLaunchVelocityY;
    PAD(24, 0x30);
    int16_t cleanHitCheckXDist;
    int16_t cleanHitCheckYDist;
    int16_t cleanHitCheckHalfWidth;
    int16_t cleanHitCheckHalfHeight;
    int16_t cleanHitCheckScale;
    uint8_t forceCleanHit;
    PAD(9, 0x53);
} GGXXACPR_HitParam;
/* Struct layout sanity checks */
static_assert(offsetof(GGXXACPR_HitParam, counterHitLaunchVelocityX) == 0x2C);
static_assert(offsetof(GGXXACPR_HitParam, cleanHitCheckXDist) == 0x48);
static_assert(sizeof(GGXXACPR_HitParam) == 0x5C);

typedef struct GGXXACPR_DamageParam {
    uint16_t downX;
    uint16_t downY;
    uint16_t downGravity;
    uint16_t downUkemiTime;
    uint16_t faintPoint;
    uint16_t UNKNOWN_FIELD(0xA);
    uint32_t downFlag;
    uint16_t kezuri; /* chip damage? */
    uint16_t damageHosei; /* some scaling thing */
    uint16_t hitSetParamNo;
    uint16_t hitSetParamFlag;
    uint16_t GuardSetParamNo;
    uint16_t GuardSetParamFlag;
    uint8_t dno; // used for hit level, but one less than on wiki
    uint8_t dprob;
    uint8_t gno; // used for hit level, but one less than on wiki
    uint8_t gprob;
    PAD(12, 0x20);
} GGXXACPR_DamageParam;
static_assert(sizeof(GGXXACPR_DamageParam) == 0x2C);

// Covers a region of static game memory that holds camera data
//  as well as some cached calculations related to camera behavior.
typedef struct GGXXACPR_Camera {
    float playerXDist;
    float playerYDist;
    PAD(8, 0x8);
    int32_t anchorXPos; // anchor point is the X middle of the camera view and 
    int32_t anchorYPos; // anchor point is the X middle of the camera view and 
    uint32_t cameraWidth_2; // Duplicated for unknown reason
    PAD(4, 0x1C);
    int32_t leftEdgePos;
    int32_t topEdgePos;
    uint32_t cameraWidth_3; // Duplicated for unknown reason
    uint32_t cameraHeight;
    float minPlayerYPosFloat;
    PAD(8, 0x34);
    uint32_t cameraWidth; // direct manipulation affects camera size
    int32_t zoomTimer; // Timer that delays rezoom to ideal camera size
    float zoom;
    float zoomBackgroundScale; // direct manipulation affects background render scale
    float zoomSpriteScale; // direct manipulation affects player sprite render scale
    uint32_t cameraFlags;
    PAD(4, 0x54);
    int32_t player1XPos;
    int32_t player2XPos;
    int32_t minPlayerYPos;
    int32_t UNKNOWN_FIELD(0x64); // Something like player 1 screen-space velocity
    int32_t UNKNOWN_FIELD(0x68); // Something like player 2 screen-space velocity
    PAD(12, 0x6C); // Camera shake flags here
    float centerPointBetweenPlayersXPos;
    int32_t cameraCenterPointXPos;
    int32_t cameraYPos_2; // Duplicated for unknown reason
    PAD(8, 0x84);
    int32_t cameraCenterPointXPos_2; // Duplicated for unknown reason
    int32_t cameraYPos_3; // Duplicated for unknown reason
    uint32_t cameraWidth_4; // Duplicated for unknown reason

} GGXXACPR_Camera;

/* Struct layout sanity checks */
static_assert(offsetof(GGXXACPR_Camera, anchorXPos) == 0x10);
static_assert(offsetof(GGXXACPR_Camera, leftEdgePos) == 0x20);
static_assert(offsetof(GGXXACPR_Camera, cameraWidth) == 0x3C);
static_assert(offsetof(GGXXACPR_Camera, player1XPos) == 0x58);
static_assert(offsetof(GGXXACPR_Camera, centerPointBetweenPlayersXPos) == 0x78);
static_assert(sizeof(GGXXACPR_Camera) == 0x98);

// A sub-struct that contains data exclusive to player entities (as opposed to more generic entitiy data).
typedef struct GGXXACPR_PlayerEntityData {
    // Range: [0, 10000]
    uint16_t tension;
    uint8_t staggerShakeTime;
    uint8_t backStepInvalid;
    uint16_t currentGlobalProration;
    int16_t negativePenaltyTimer;
    int16_t counterRedTime; /* Timer for the red flash effect on players that have just taken a counter hit. */
    uint16_t homingJumpFlag;
    int32_t homingJumpTimer;
    int32_t runInertia;
    int32_t offX;
    int16_t throwProtectionTimer;
    int16_t guardBar;
    int16_t guardTimer;
    int16_t untechTimer;
    uint16_t homingJumpDir;
    uint8_t reqHit;
    uint8_t hitDispTime;
    float hitDispIn;
    uint8_t inertiaFlag;
    uint8_t whiteOutTime;
    uint8_t invulnCounter;
    uint8_t airDashRecoveryTimer;
    uint8_t inertiaTime;
    uint8_t downBoundDamage;
    PAD(2, 0x2E);
    int16_t tensionGainPenaltyTimer;
    uint8_t frcTime;
    uint8_t frcLockoutTimer;
    int32_t UNKNOWN_FIELD(0x34);
    int16_t UNKNOWN_FIELD(0x38);
    int16_t UNKNOWN_FIELD(0x3A);
    uint8_t UNKNOWN_FIELD(0x3C);
    uint8_t instantBlockTimer;
    int16_t instantBlockLockoutTimer;

    /* OFFSET 0x40 -------------------------------------------------- */

    uint32_t getChainDataTableFunc;
    uint32_t enemyPtr;
    int16_t punchKey;
    int16_t kickKey;
    int16_t slashKey;
    int16_t heavySlashKey;
    int16_t dustKey;
    int16_t fuckinKey;
    int16_t posResetKey;
    int16_t pauseKey;
    int16_t recPlayerKey;
    int16_t recEnemyKey;
    int16_t playMemoryKey;
    int16_t switchKey;
    int16_t enemyWalkKey;
    int16_t enemyJumpKey;
    int16_t punchAndKickKey;
    int16_t punchAndDustKey;
    int16_t pksKey;
    int16_t pksshKey;
    int16_t lifeLength;
    int16_t lifeLengthDamage;
    uint8_t jumpCount;
    uint8_t airDashCount;
    int16_t jumpFlags;
    int16_t chainFlag1;
    int16_t chainFlag2;
    uint32_t rendaFlag;
    int16_t dizzyBuildup;
    int16_t dizzyDuration;

    /* OFFSET 0x80 -------------------------------------------------- */

    uint32_t exCommandFunc;
    uint8_t jumpDirection;
    uint8_t cpu;
    uint8_t sousaiTime;
    uint8_t dizzyThreshold; /* AKA FaintMax in debug symbols */
    int16_t charSpecificVar; /* Zappa active summon, ABA install mode, Sol's Dragon Install timer, etc */
    int16_t UNKNOWN_FIELD(0x8A); /* RE note: set to 0 beginning of act_update for Anji; relates to not being airborne */
    int16_t UNKNOWN_FIELD(0x8C); /* RE note: used in Bridget & Robo-Ky update func */
    uint8_t UNKNOWN_FIELD(0x8E); /* RE note: used in Robo-Ky update func as short (extends into JamParry) */
    uint8_t jamParry;
    uint8_t jamParryTime;
    PAD(27, 0x91);
    uint32_t hitstunFlags; /* SlidingKD, wallbounce, etc */
    uint32_t UNKNOWN_FIELD(0xB0); /* RE note: Child?*/
    uint32_t atkHitInterrupt;
    uint32_t atkGuardInterrupt;
    uint32_t damageInterrupt;

    /* OFFSET 0xC0 -------------------------------------------------- */

    uint32_t landInterrupt;
    uint32_t actChangeInterrupt;
    uint32_t guardFunc;
    uint32_t getUpInterrupt;
    uint32_t guardCheckInterrupt;
    uint32_t posByEnemyAddr;
    uint8_t overKillFlag;
    uint8_t objFlag;
    uint8_t dustTime;
    uint8_t afterImageIndex;
    int32_t keyHook;
    int16_t healthVisual;
    int16_t tensionPulse;
    int16_t tensionSideTime;
    int16_t tensionPassivityCounter;
    int32_t lastPosX;
    int16_t tensionBar;
    uint8_t tShand;
    uint8_t pushTime;
    uint8_t attackDown;
    uint8_t defenceDown;
    uint8_t jumpSeal;
    uint8_t artsSeal;
    uint8_t poison;
    uint8_t mistTime; /* Baccus Sigh */
    int16_t comboTime;
    int16_t burstMeter;
    uint8_t UNKNOWN_FIELD(0xFA); /* RE Notes: something about throw */
    uint8_t throwCounterThing;
    uint8_t UNKNOWN_FIELD(0xFC); /* RE Notes: relates to blue burst */
    uint8_t UNKNOWN_FIELD(0xFD);
    int16_t comboCounter;

    /* OFFSET 0x100 -------------------------------------------------- */

    int16_t comboCounterVisual;
    uint8_t UNKNOWN_FIELD(0x102); /* RE Note: some counter relates to light green flash effect */
    uint8_t UNKNOWN_FIELD(0x103);
    uint8_t allCounter;
    uint8_t punchSeal;
    uint8_t kickSeal;
    uint8_t slashSeal;
    uint8_t heavySlashSeal;
    uint8_t dustSeal;
    uint8_t UNKNOWN_FIELD(0x10A); /* RE Note: relates to IB/SB */
    uint8_t slashBackActiveWindow;
    uint8_t slashBackGuardLockoutTimer;
    uint8_t slashBackGuardstunTimer;
    uint8_t slashBackLockoutOnWakeupTimer;
    uint8_t slashBackFollowupTimer;
    int16_t throwTechFlags;
    uint8_t UNKNOWN_FIELD(0x112); /* RE Note: Resets beginning of act_update */
    uint8_t UNKNOWN_FIELD(0x113); /* RE Note: a count down timer */
    int32_t throwTechColorCounter;
    int32_t UNKNOWN_FIELD(0x118); /* RE Note: Resets beginning of act_update */
    int32_t cleanHitCounter;
    int32_t UNKNOWN_FIELD(0x120);
    int32_t UNKNOWN_FIELD(0x124);
    int32_t UNKNOWN_FIELD(0x128);
    int32_t UNKNOWN_FIELD(0x12C);
    int32_t UNKNOWN_FIELD(0x130);
    int32_t UNKNOWN_FIELD(0x134);
    int32_t tensionCopy;
    int32_t UNKNOWN_FIELD(0x13C); /* RE Note: Tension thing (relates to expenditure) */

    /* OFFSET 0x140 -------------------------------------------------- */

    int32_t tensionRefill; /* Training */
    int32_t UNKNOWN_FIELD(0x144);
} GGXXACPR_PlayerEntityData;

/* Struct layout sanity checks */
static_assert(offsetof(GGXXACPR_PlayerEntityData, getChainDataTableFunc) == 0x40);
static_assert(offsetof(GGXXACPR_PlayerEntityData, exCommandFunc) == 0x80);
static_assert(offsetof(GGXXACPR_PlayerEntityData, landInterrupt) == 0xC0);
static_assert(offsetof(GGXXACPR_PlayerEntityData, comboCounterVisual) == 0x100);
static_assert(offsetof(GGXXACPR_PlayerEntityData, tensionRefill) == 0x140);
static_assert(sizeof(GGXXACPR_PlayerEntityData) == 0x148);


/* Represents Players, projectiles, summons, and graphical effects. */
typedef struct GGXXACPR_Entity {
    uint16_t id; /* enum ENTITY_ID*/
    uint8_t bIsFacingRight;
    uint8_t bIsLeftSide;
    // Together with the `Next` pointer forms a doubly-linked list. There is
    //  one linked list for projectiles/summons and another for graphical entities.
    //  Each linked-listed has a dummy root entity (unsure about graphic entities).
    //  These values are nullptr for player entities.
    uint32_t prevPtr;
    // Together with the `Prev` pointer forms a doubly-linked list. There is
    //  one linked list for projectiles/summons and another for graphical entities.
    //  Each linked-listed has a dummy root entity (unsure about graphic entities).
    //  These values are nullptr for player entities.
    uint32_t nextPtr;
    uint32_t actionState; /* enum ACTION_STATE */
    uint16_t bufferedActionId;
    uint16_t bufferFlag; /* bitfield */
    uint16_t bufferedActionIdTemp;
    uint16_t bufferFlagTemp;
    uint16_t actId; /* Current action id. */
    uint16_t localId;
    uint16_t actTimer; /* Frame counter for the current action. */
    uint16_t health; /* [0 - 460] */
    // Pointer to parent entity's playerEntityData struct if applicable.
    GGXXACPR_PlayerEntityData* parentPlayerEntityDataPtr;
    uint16_t actionIdSv;
    PAD(1, 0x26);
    uint8_t playerIndex; /* P1 = 0, P2 = 1 */
    uint16_t parentFlag;
    uint16_t guardState; /* enum GUARD_STATE */
    // Contains additional player entity data. Always nullptr on non-player entities.
    GGXXACPR_PlayerEntityData* playerEntityDataPtr;
    uint32_t graphicFunction;
    uint32_t attackState; /* enum ATTACK_STATE */
    uint32_t commandFlags; /* enum COMMAND_STATE */
    uint32_t cellTop;

    /* OFFSET 0x40 -------------------------------------------------- */

    uint16_t cellNo;
    uint16_t lastCellNo;
    uint16_t imageNo;
    uint16_t lastImageNo;
    uint32_t gclTop;
    int16_t coreX;
    int16_t coreY;
    int16_t scale;
    int16_t scaleY;
    uint32_t hitboxSetPtr; /* pointer to a Collider struct array. */
    uint32_t extraHitboxSetPtr; /* pointer to a Collider struct array. */
    uint8_t boxFlag;
    uint8_t hitboxFlag;
    uint8_t hurtboxFlag;
    PAD(1, 0x5F);
    uint32_t tensionAddByHit;
    uint32_t attackTargetPtr;
    uint32_t hitColliPtr;
    uint32_t damColliPtr;
    int32_t imageW;
    int32_t imageH;
    uint8_t colliCnt;
    uint8_t colliNo;
    int16_t priority2;
    int16_t UNKNOWN_FIELD(0x7C); /* RE note: maybe something to do with the after image trail */
    int16_t UNKNOWN_FIELD(0x7E); /* RE note: maybe something to do with the after image trail */

    /* OFFSET 0x80 -------------------------------------------------- */

    int16_t displayVar1;
    int16_t displayVar2;
    uint8_t boxCount; /* length of `hitboxSetPtr` and `extraHitboxSetPtr` arrays */
    int8_t hitboxSetIterationVar;
    PAD(2, 0x86);
    uint32_t hitParamPtr;
    uint32_t damParamPtr;
    uint32_t colliHitInterrupt;
    uint32_t paletteProcedurePtr;
    uint8_t paletteId;
    uint8_t localPalette;
    uint8_t paletteFlag;
    uint8_t fadVal;
    int16_t spriteIdTop;
    int16_t fbSpriteIdTop;
    int16_t spriteIdMax;
    int16_t spriteId;
    uint32_t sphAddr;
    int16_t transMode;
    uint8_t spriteIdIdx;
    uint8_t texSizeMax;
    uint32_t spriteFlag;
    int32_t xPos; /* In world units */
    int32_t yPos; /* In world units */
    int32_t xVelocity; /* world units per frame */
    int32_t yVelocity; /* world units per frame */

    /* OFFSET 0xC0 -------------------------------------------------- */

    int32_t speedsvX;
    int32_t speedsvY;
    int32_t hitBackX;
    int32_t rHitBackX;
    int32_t adBackX;
    int32_t gravity;
    int16_t parentX;
    int16_t parentY;
    int16_t angle;
    int16_t drawPriority; /* Higher values draw first, lower values draw over higher values (i.e. Z position). */
    uint32_t instTable; /* Pointer to an array of ActionHeader struct pointers */
    GGXXACPR_ActionHeader actHeader;
    uint32_t actHeaderPtr;
    uint32_t instAddr;
    uint16_t instSt;
    uint16_t instVal;
    uint16_t animationNo;
    uint16_t animationTime;
    uint8_t animationRate;
    uint8_t hitstopTime; /* Hitstop duration timer. */
    uint8_t animationTimeMax;
    int8_t mark; /* move-specific variable. Used to denote active windows of command grabs, some counter stance moves, etc. */

    /* OFFSET 0x100 -------------------------------------------------- */

    uint8_t trans;
    uint8_t trans2;
    uint16_t dashFlags;
    uint16_t airDashFlags;
    uint16_t airDashFlags2;
    PAD(24, 0x108);
    uint32_t throwFlags; /* bitfield */
    PAD(12, 0x124);
} GGXXACPR_Entity;

/* Struct layout sanity checks */
static_assert(offsetof(GGXXACPR_Entity, cellNo) == 0x40);
static_assert(offsetof(GGXXACPR_Entity, displayVar1) == 0x80);
static_assert(offsetof(GGXXACPR_Entity, speedsvX) == 0xC0);
static_assert(offsetof(GGXXACPR_Entity, trans) == 0x100);
static_assert(sizeof(GGXXACPR_Entity) == 0x130);



#undef UNKNOWN_FIELD
#undef PAD

#endif
