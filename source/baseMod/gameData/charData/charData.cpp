#include "charData.h"
#include "baseMod_p.h"
#include "offsets.h"


uint16_t* __stdcall GetPushboxDimensionArray(int32_t type) {
    switch (type) {
        case BM_PD_STANDING_WIDTH:
            return reinterpret_cast<uint16_t*>(getBaseAddress() + offsets::PUSHBOX_STANDING_WIDTH_ARRAY);
        case BM_PD_STANDING_HEIGHT_AC:
            return reinterpret_cast<uint16_t*>(getBaseAddress() + offsets::PUSHBOX_STANDING_HEIGHT_ARRAY_AC);
        case BM_PD_STANDING_HEIGHT_PR:
            return reinterpret_cast<uint16_t*>(getBaseAddress() + offsets::PUSHBOX_STANDING_HEIGHT_ARRAY_PR);
        case BM_PD_CROUCHING_WIDTH:
            return reinterpret_cast<uint16_t*>(getBaseAddress() + offsets::PUSHBOX_CROUCHING_WIDTH_ARRAY);
        case BM_PD_CROUCHING_HEIGHT:
            return reinterpret_cast<uint16_t*>(getBaseAddress() + offsets::PUSHBOX_CROUCHING_HEIGHT_ARRAY);
        case BM_PD_AIRBORNE_WIDTH:
            return reinterpret_cast<uint16_t*>(getBaseAddress() + offsets::PUSHBOX_AIR_WIDTH_ARRAY);
        case BM_PD_AIRBORNE_HEIGHT:
            return reinterpret_cast<uint16_t*>(getBaseAddress() + offsets::PUSHBOX_AIR_HEIGHT_ARRAY);
        default:
            return nullptr;
    }
}
int16_t* __stdcall GetPushboxAirborneOffsetArray(int32_t gameVer) {
    if (gameVer == GGXXACPR_GameVersion::GAME_VERSION_ACCENT_CORE) {
        return reinterpret_cast<int16_t*>(getBaseAddress() + offsets::PUSHBOX_AIR_OFFSET_ARRAY_AC);
    } else if (gameVer == GGXXACPR_GameVersion::GAME_VERSION_PLUS_R) {
        return reinterpret_cast<int16_t*>(getBaseAddress() + offsets::PUSHBOX_AIR_OFFSET_ARRAY_PR);
    } else {
        return nullptr;
    }
}
int16_t* __stdcall GetThrowRangeArray(int32_t type) {
    switch (type) {
        case BM_TR_GROUND_AC:
            return reinterpret_cast<int16_t*>(getBaseAddress() + offsets::AC_GROUND_THROW_RANGE_ARRAY);
        case BM_TR_GROUND_PR:
            return reinterpret_cast<int16_t*>(getBaseAddress() + offsets::PLUSR_GROUND_THROW_RANGE_ARRAY);
        case BM_TR_AIR_HORIZONTAL_AC:
            return reinterpret_cast<int16_t*>(getBaseAddress() + offsets::AC_AIR_THROW_HORIZONTAL_RANGE_ARRAY);
        case BM_TR_AIR_HORIZONTAL_PR:
            return reinterpret_cast<int16_t*>(getBaseAddress() + offsets::PLUSR_AIR_THROW_HORIZONTAL_RANGE_ARRAY);
        case BM_TR_AIR_UPPER:
            return reinterpret_cast<int16_t*>(getBaseAddress() + offsets::AIR_THROW_UPPER_RANGE_ARRAY);
        case BM_TR_AIR_LOWER:
            return reinterpret_cast<int16_t*>(getBaseAddress() + offsets::AIR_THROW_LOWER_RANGE_ARRAY);
        default:
            return nullptr;
    }
}
int16_t* __stdcall GetThrowRangeArrayAlt(int32_t type) {
    static const uint32_t offsetMap[] = {
        offsets::AC_GROUND_THROW_RANGE_ARRAY,
        offsets::PLUSR_GROUND_THROW_RANGE_ARRAY,
        offsets::AC_AIR_THROW_HORIZONTAL_RANGE_ARRAY,
        offsets::PLUSR_AIR_THROW_HORIZONTAL_RANGE_ARRAY,
        offsets::AIR_THROW_UPPER_RANGE_ARRAY,
        offsets::AIR_THROW_LOWER_RANGE_ARRAY,
    };
    if (0 > type || type > 5) return nullptr;
    return reinterpret_cast<int16_t*>(getBaseAddress() + offsetMap[type]);
}
uint16_t* __stdcall GetCommandGrabRangeArray() {
    return reinterpret_cast<uint16_t*>(getBaseAddress() + offsets::COMMAND_GRAB_RANGE_LOOKUP_TABLE);
}


const BaseMod_CharDataApi* GetCharDataApi() {
    static const BaseMod_CharDataApi _charDataApi = {
        size: sizeof(BaseMod_CharDataApi),
        version: BASEMOD_API_VERSION_NUM,
        
        GetPushboxDimensionArray: GetPushboxDimensionArray,
        GetPushboxAirborneOffsetArray: GetPushboxAirborneOffsetArray,
        GetThrowRangeArray: GetThrowRangeArray,
        GetCommandGrabRangeArray: GetCommandGrabRangeArray,
    };

    return &_charDataApi;
}