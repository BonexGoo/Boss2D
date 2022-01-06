#include <boss.h>
#if BOSS_NEED_ADDON_WEBRTC

#include "boss_integration_libyuv-20211215.h"

static __inline int Abs(int v) {
  return v >= 0 ? v : -v;
}

#include <addon/libyuv-20211215_for_boss/source/compare.cc>
#include <addon/libyuv-20211215_for_boss/source/compare_common.cc>
#include <addon/libyuv-20211215_for_boss/source/compare_gcc.cc>
#include <addon/libyuv-20211215_for_boss/source/compare_mmi.cc>
#include <addon/libyuv-20211215_for_boss/source/compare_msa.cc>
#include <addon/libyuv-20211215_for_boss/source/compare_neon.cc>
#include <addon/libyuv-20211215_for_boss/source/compare_neon64.cc>
#include <addon/libyuv-20211215_for_boss/source/compare_win.cc>
#include <addon/libyuv-20211215_for_boss/source/convert.cc>

#define kDither565_4x4 kDither565_4x4_convert_argb_BOSS
#include <addon/libyuv-20211215_for_boss/source/convert_argb.cc>
#undef kDither565_4x4

#include <addon/libyuv-20211215_for_boss/source/convert_from.cc>

#define kDither565_4x4 kDither565_4x4_convert_from_argb_BOSS
#include <addon/libyuv-20211215_for_boss/source/convert_from_argb.cc>
#undef kDither565_4x4

#include <addon/libyuv-20211215_for_boss/source/convert_jpeg.cc>
#include <addon/libyuv-20211215_for_boss/source/convert_to_argb.cc>
#include <addon/libyuv-20211215_for_boss/source/convert_to_i420.cc>
#include <addon/libyuv-20211215_for_boss/source/cpu_id.cc>
#include <addon/libyuv-20211215_for_boss/source/mjpeg_decoder.cc>
#include <addon/libyuv-20211215_for_boss/source/mjpeg_validate.cc>
#include <addon/libyuv-20211215_for_boss/source/planar_functions.cc>
#include <addon/libyuv-20211215_for_boss/source/rotate.cc>
#include <addon/libyuv-20211215_for_boss/source/rotate_any.cc>
#include <addon/libyuv-20211215_for_boss/source/rotate_argb.cc>
#include <addon/libyuv-20211215_for_boss/source/rotate_common.cc>
#include <addon/libyuv-20211215_for_boss/source/rotate_gcc.cc>
#include <addon/libyuv-20211215_for_boss/source/rotate_mmi.cc>
#include <addon/libyuv-20211215_for_boss/source/rotate_msa.cc>
#include <addon/libyuv-20211215_for_boss/source/rotate_neon.cc>
#include <addon/libyuv-20211215_for_boss/source/rotate_neon64.cc>
#include <addon/libyuv-20211215_for_boss/source/rotate_win.cc>
#include <addon/libyuv-20211215_for_boss/source/row_any.cc>
#include <addon/libyuv-20211215_for_boss/source/row_common.cc>
#include <addon/libyuv-20211215_for_boss/source/row_gcc.cc>
#include <addon/libyuv-20211215_for_boss/source/row_mmi.cc>
#include <addon/libyuv-20211215_for_boss/source/row_msa.cc>
#include <addon/libyuv-20211215_for_boss/source/row_neon.cc>
#include <addon/libyuv-20211215_for_boss/source/row_neon64.cc>
#include <addon/libyuv-20211215_for_boss/source/row_win.cc>
#include <addon/libyuv-20211215_for_boss/source/scale.cc>
#include <addon/libyuv-20211215_for_boss/source/scale_any.cc>
#include <addon/libyuv-20211215_for_boss/source/scale_argb.cc>
#include <addon/libyuv-20211215_for_boss/source/scale_common.cc>
#include <addon/libyuv-20211215_for_boss/source/scale_gcc.cc>
#include <addon/libyuv-20211215_for_boss/source/scale_mmi.cc>
#include <addon/libyuv-20211215_for_boss/source/scale_msa.cc>
#include <addon/libyuv-20211215_for_boss/source/scale_neon.cc>
#include <addon/libyuv-20211215_for_boss/source/scale_neon64.cc>
#include <addon/libyuv-20211215_for_boss/source/scale_uv.cc>
#include <addon/libyuv-20211215_for_boss/source/scale_win.cc>
#include <addon/libyuv-20211215_for_boss/source/video_common.cc>

#endif
