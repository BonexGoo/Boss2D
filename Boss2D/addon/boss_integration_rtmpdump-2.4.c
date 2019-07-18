#include <boss.h>
#if BOSS_NEED_ADDON_CURL

#if BOSS_WINDOWS
    #pragma comment(lib, "winmm.lib")
#endif

#pragma warning(disable : 4090)

#include "boss_integration_rtmpdump-2.4.h"

#include <addon/rtmpdump-2.4_for_boss/librtmp/amf.c>
#include <addon/rtmpdump-2.4_for_boss/librtmp/hashswf.c>
#include <addon/rtmpdump-2.4_for_boss/librtmp/log.c>
#include <addon/rtmpdump-2.4_for_boss/librtmp/parseurl.c>
#include <addon/rtmpdump-2.4_for_boss/librtmp/rtmp.c>

#endif
