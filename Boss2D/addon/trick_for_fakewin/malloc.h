#pragma once
#include <boss.h>

#ifdef __cplusplus
    extern "C" {
#endif

extern void* malloc(boss_size_t size);
#if BOSS_WINDOWS | BOSS_LINUX | BOSS_WASM
    extern void free(void* ptr);
#elif !BOSS_MAC_OSX & !BOSS_IPHONE
    extern int free(void* ptr);
#endif

#ifdef __cplusplus
    }
#endif
