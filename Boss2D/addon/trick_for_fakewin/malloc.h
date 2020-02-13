#pragma once
#include <boss.h>

#ifdef __cplusplus
    extern "C" {
#endif

extern void* malloc(boss_size_t size);
#if BOSS_WINDOWS | BOSS_LINUX
    extern void free(void* ptr);
#else
    extern int free(void* ptr);
#endif

#ifdef __cplusplus
    }
#endif
