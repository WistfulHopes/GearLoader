#ifndef RESCRIPT_H
#define RESCRIPT_H

#define RESCRIPT_NAME "Character Framework"
#define RESCRIPT_API_VERSION "1.0.0"
#define RESCRIPT_API_VERSION_NUM 0x010000
#define RESCRIPT_CALL __stdcall

#ifdef __cplusplus
    #include <cstdint>
#else
    #include <stdint.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct ReScript_Api {
    uint32_t size;
    uint32_t version;
};

#ifdef __cplusplus
}   // extern "C"
#endif

#endif // RESCRIPT_H