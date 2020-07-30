#include <boss.h>
#if BOSS_NEED_ADDON_ZIPA

#include "boss_integration_ziparchive-4.6.7.h"

#include <addon/ziparchive-4.6.7_for_boss/ZipArchive/zlib/adler32.c>
#include <addon/ziparchive-4.6.7_for_boss/ZipArchive/zlib/compress.c>
#include <addon/ziparchive-4.6.7_for_boss/ZipArchive/zlib/crc32.c>
#include <addon/ziparchive-4.6.7_for_boss/ZipArchive/zlib/deflate.c>
#include <addon/ziparchive-4.6.7_for_boss/ZipArchive/zlib/infback.c>
#include <addon/ziparchive-4.6.7_for_boss/ZipArchive/zlib/inffast.c>
#define fixedtables fixedtables_inflate_BOSS
#include <addon/ziparchive-4.6.7_for_boss/ZipArchive/zlib/inflate.c>
#undef fixedtables
#include <addon/ziparchive-4.6.7_for_boss/ZipArchive/zlib/inftrees.c>
#include <addon/ziparchive-4.6.7_for_boss/ZipArchive/zlib/trees.c>
#include <addon/ziparchive-4.6.7_for_boss/ZipArchive/zlib/uncompr.c>
#include <addon/ziparchive-4.6.7_for_boss/ZipArchive/zlib/zutil.c>

#endif
