#pragma once
#include <boss.h>

#ifdef __cplusplus
    extern "C" {
#endif

extern void* malloc(size_t size);
#if BOSS_WINDOWS
    extern void free(void* ptr);
#else
    extern int free(void* ptr);
#endif

#ifdef __cplusplus
    }
#endif
