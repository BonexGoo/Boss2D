#include <boss.h>
#if BOSS_NEED_ADDON_OGG

#if !BOSS_WINDOWS
    #define _M_ARM
#endif

#include "boss_integration_libvorbis-1.3.6.h"

#include <addon/libvorbis-1.3.6_for_boss/lib/analysis.c>
//#include <addon/libvorbis-1.3.6_for_boss/lib/barkmel.c> // main이 포함된 소스
#include <addon/libvorbis-1.3.6_for_boss/lib/bitrate.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/block.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/codebook.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/envelope.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/floor0.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/floor1.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/info.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/lookup.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/lpc.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/lsp.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/mapping0.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/mdct.c>
#define FLOOR1_fromdB_LOOKUP FLOOR1_fromdB_LOOKUP_psy_BOSS
#include <addon/libvorbis-1.3.6_for_boss/lib/psy.c>
//#include <addon/libvorbis-1.3.6_for_boss/lib/psytune.c> // main이 포함된 소스
#include <addon/libvorbis-1.3.6_for_boss/lib/registry.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/res0.c>
#define bitreverse bitreverse_sharedbook_BOSS
#include <addon/libvorbis-1.3.6_for_boss/lib/sharedbook.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/smallft.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/synthesis.c>
//#include <addon/libvorbis-1.3.6_for_boss/lib/tone.c> // main이 포함된 소스
//#include <addon/libvorbis-1.3.6_for_boss/lib/vorbisenc.c> // IOS개발중
#include <addon/libvorbis-1.3.6_for_boss/lib/vorbisfile.c>
#include <addon/libvorbis-1.3.6_for_boss/lib/window.c>

#endif
