/*
 *  Copyright 2011 The LibYuv Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS. All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_LIBYUV_U_libyuv__scale_h //original-code:"libyuv/scale.h"

#include <assert.h>
#include <string.h>

#include BOSS_LIBYUV_U_libyuv__cpu_id_h //original-code:"libyuv/cpu_id.h"
#include BOSS_LIBYUV_U_libyuv__planar_functions_h //original-code:"libyuv/planar_functions.h"  // For CopyPlane
#include BOSS_LIBYUV_U_libyuv__row_h //original-code:"libyuv/row.h"
#include BOSS_LIBYUV_U_libyuv__scale_row_h //original-code:"libyuv/scale_row.h"
#include BOSS_LIBYUV_U_libyuv__scale_uv_h //original-code:"libyuv/scale_uv.h"  // For UVScale

#ifdef __cplusplus
namespace libyuv {
extern "C" {
#endif

// removed by BOSS:static __inline int Abs(int v) {
// removed by BOSS:  return v >= 0 ? v : -v;
// removed by BOSS:}

#define SUBSAMPLE(v, a, s) (v < 0) ? (-((-v + a) >> s)) : ((v + a) >> s)

// Scale plane, 1/2
// This is an optimized version for scaling down a plane to 1/2 of
// its original size.

static void ScalePlaneDown2(int src_width,
                            int src_height,
                            int dst_width,
                            int dst_height,
                            int src_stride,
                            int dst_stride,
                            const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            enum FilterMode filtering) {
  int y;
  void (*ScaleRowDown2)(const uint8_t* src_ptr, ptrdiff_t src_stride,
                        uint8_t* dst_ptr, int dst_width) =
      filtering == kFilterNone
          ? ScaleRowDown2_C
          : (filtering == kFilterLinear ? ScaleRowDown2Linear_C
                                        : ScaleRowDown2Box_C);
  int row_stride = src_stride * 2;
  (void)src_width;
  (void)src_height;
  if (!filtering) {
    src_ptr += src_stride;  // Point to odd rows.
    src_stride = 0;
  }

#if defined(HAS_SCALEROWDOWN2_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ScaleRowDown2 =
        filtering == kFilterNone
            ? ScaleRowDown2_Any_NEON
            : (filtering == kFilterLinear ? ScaleRowDown2Linear_Any_NEON
                                          : ScaleRowDown2Box_Any_NEON);
    if (IS_ALIGNED(dst_width, 16)) {
      ScaleRowDown2 = filtering == kFilterNone ? ScaleRowDown2_NEON
                                               : (filtering == kFilterLinear
                                                      ? ScaleRowDown2Linear_NEON
                                                      : ScaleRowDown2Box_NEON);
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN2_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ScaleRowDown2 =
        filtering == kFilterNone
            ? ScaleRowDown2_Any_SSSE3
            : (filtering == kFilterLinear ? ScaleRowDown2Linear_Any_SSSE3
                                          : ScaleRowDown2Box_Any_SSSE3);
    if (IS_ALIGNED(dst_width, 16)) {
      ScaleRowDown2 =
          filtering == kFilterNone
              ? ScaleRowDown2_SSSE3
              : (filtering == kFilterLinear ? ScaleRowDown2Linear_SSSE3
                                            : ScaleRowDown2Box_SSSE3);
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN2_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ScaleRowDown2 =
        filtering == kFilterNone
            ? ScaleRowDown2_Any_AVX2
            : (filtering == kFilterLinear ? ScaleRowDown2Linear_Any_AVX2
                                          : ScaleRowDown2Box_Any_AVX2);
    if (IS_ALIGNED(dst_width, 32)) {
      ScaleRowDown2 = filtering == kFilterNone ? ScaleRowDown2_AVX2
                                               : (filtering == kFilterLinear
                                                      ? ScaleRowDown2Linear_AVX2
                                                      : ScaleRowDown2Box_AVX2);
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN2_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    ScaleRowDown2 =
        filtering == kFilterNone
            ? ScaleRowDown2_Any_MMI
            : (filtering == kFilterLinear ? ScaleRowDown2Linear_Any_MMI
                                          : ScaleRowDown2Box_Any_MMI);
    if (IS_ALIGNED(dst_width, 8)) {
      ScaleRowDown2 = filtering == kFilterNone ? ScaleRowDown2_MMI
                                               : (filtering == kFilterLinear
                                                      ? ScaleRowDown2Linear_MMI
                                                      : ScaleRowDown2Box_MMI);
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN2_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    ScaleRowDown2 =
        filtering == kFilterNone
            ? ScaleRowDown2_Any_MSA
            : (filtering == kFilterLinear ? ScaleRowDown2Linear_Any_MSA
                                          : ScaleRowDown2Box_Any_MSA);
    if (IS_ALIGNED(dst_width, 32)) {
      ScaleRowDown2 = filtering == kFilterNone ? ScaleRowDown2_MSA
                                               : (filtering == kFilterLinear
                                                      ? ScaleRowDown2Linear_MSA
                                                      : ScaleRowDown2Box_MSA);
    }
  }
#endif

  if (filtering == kFilterLinear) {
    src_stride = 0;
  }
  // TODO(fbarchard): Loop through source height to allow odd height.
  for (y = 0; y < dst_height; ++y) {
    ScaleRowDown2(src_ptr, src_stride, dst_ptr, dst_width);
    src_ptr += row_stride;
    dst_ptr += dst_stride;
  }
}

static void ScalePlaneDown2_16(int src_width,
                               int src_height,
                               int dst_width,
                               int dst_height,
                               int src_stride,
                               int dst_stride,
                               const uint16_t* src_ptr,
                               uint16_t* dst_ptr,
                               enum FilterMode filtering) {
  int y;
  void (*ScaleRowDown2)(const uint16_t* src_ptr, ptrdiff_t src_stride,
                        uint16_t* dst_ptr, int dst_width) =
      filtering == kFilterNone
          ? ScaleRowDown2_16_C
          : (filtering == kFilterLinear ? ScaleRowDown2Linear_16_C
                                        : ScaleRowDown2Box_16_C);
  int row_stride = src_stride * 2;
  (void)src_width;
  (void)src_height;
  if (!filtering) {
    src_ptr += src_stride;  // Point to odd rows.
    src_stride = 0;
  }

#if defined(HAS_SCALEROWDOWN2_16_NEON)
  if (TestCpuFlag(kCpuHasNEON) && IS_ALIGNED(dst_width, 16)) {
    ScaleRowDown2 =
        filtering ? ScaleRowDown2Box_16_NEON : ScaleRowDown2_16_NEON;
  }
#endif
#if defined(HAS_SCALEROWDOWN2_16_SSE2)
  if (TestCpuFlag(kCpuHasSSE2) && IS_ALIGNED(dst_width, 16)) {
    ScaleRowDown2 =
        filtering == kFilterNone
            ? ScaleRowDown2_16_SSE2
            : (filtering == kFilterLinear ? ScaleRowDown2Linear_16_SSE2
                                          : ScaleRowDown2Box_16_SSE2);
  }
#endif
#if defined(HAS_SCALEROWDOWN2_16_MMI)
  if (TestCpuFlag(kCpuHasMMI) && IS_ALIGNED(dst_width, 4)) {
    ScaleRowDown2 = filtering == kFilterNone ? ScaleRowDown2_16_MMI
                                             : (filtering == kFilterLinear
                                                    ? ScaleRowDown2Linear_16_MMI
                                                    : ScaleRowDown2Box_16_MMI);
  }
#endif

  if (filtering == kFilterLinear) {
    src_stride = 0;
  }
  // TODO(fbarchard): Loop through source height to allow odd height.
  for (y = 0; y < dst_height; ++y) {
    ScaleRowDown2(src_ptr, src_stride, dst_ptr, dst_width);
    src_ptr += row_stride;
    dst_ptr += dst_stride;
  }
}

// Scale plane, 1/4
// This is an optimized version for scaling down a plane to 1/4 of
// its original size.

static void ScalePlaneDown4(int src_width,
                            int src_height,
                            int dst_width,
                            int dst_height,
                            int src_stride,
                            int dst_stride,
                            const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            enum FilterMode filtering) {
  int y;
  void (*ScaleRowDown4)(const uint8_t* src_ptr, ptrdiff_t src_stride,
                        uint8_t* dst_ptr, int dst_width) =
      filtering ? ScaleRowDown4Box_C : ScaleRowDown4_C;
  int row_stride = src_stride * 4;
  (void)src_width;
  (void)src_height;
  if (!filtering) {
    src_ptr += src_stride * 2;  // Point to row 2.
    src_stride = 0;
  }
#if defined(HAS_SCALEROWDOWN4_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    ScaleRowDown4 =
        filtering ? ScaleRowDown4Box_Any_NEON : ScaleRowDown4_Any_NEON;
    if (IS_ALIGNED(dst_width, 8)) {
      ScaleRowDown4 = filtering ? ScaleRowDown4Box_NEON : ScaleRowDown4_NEON;
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN4_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ScaleRowDown4 =
        filtering ? ScaleRowDown4Box_Any_SSSE3 : ScaleRowDown4_Any_SSSE3;
    if (IS_ALIGNED(dst_width, 8)) {
      ScaleRowDown4 = filtering ? ScaleRowDown4Box_SSSE3 : ScaleRowDown4_SSSE3;
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN4_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    ScaleRowDown4 =
        filtering ? ScaleRowDown4Box_Any_AVX2 : ScaleRowDown4_Any_AVX2;
    if (IS_ALIGNED(dst_width, 16)) {
      ScaleRowDown4 = filtering ? ScaleRowDown4Box_AVX2 : ScaleRowDown4_AVX2;
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN4_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    ScaleRowDown4 =
        filtering ? ScaleRowDown4Box_Any_MMI : ScaleRowDown4_Any_MMI;
    if (IS_ALIGNED(dst_width, 8)) {
      ScaleRowDown4 = filtering ? ScaleRowDown4Box_MMI : ScaleRowDown4_MMI;
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN4_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    ScaleRowDown4 =
        filtering ? ScaleRowDown4Box_Any_MSA : ScaleRowDown4_Any_MSA;
    if (IS_ALIGNED(dst_width, 16)) {
      ScaleRowDown4 = filtering ? ScaleRowDown4Box_MSA : ScaleRowDown4_MSA;
    }
  }
#endif

  if (filtering == kFilterLinear) {
    src_stride = 0;
  }
  for (y = 0; y < dst_height; ++y) {
    ScaleRowDown4(src_ptr, src_stride, dst_ptr, dst_width);
    src_ptr += row_stride;
    dst_ptr += dst_stride;
  }
}

static void ScalePlaneDown4_16(int src_width,
                               int src_height,
                               int dst_width,
                               int dst_height,
                               int src_stride,
                               int dst_stride,
                               const uint16_t* src_ptr,
                               uint16_t* dst_ptr,
                               enum FilterMode filtering) {
  int y;
  void (*ScaleRowDown4)(const uint16_t* src_ptr, ptrdiff_t src_stride,
                        uint16_t* dst_ptr, int dst_width) =
      filtering ? ScaleRowDown4Box_16_C : ScaleRowDown4_16_C;
  int row_stride = src_stride * 4;
  (void)src_width;
  (void)src_height;
  if (!filtering) {
    src_ptr += src_stride * 2;  // Point to row 2.
    src_stride = 0;
  }
#if defined(HAS_SCALEROWDOWN4_16_NEON)
  if (TestCpuFlag(kCpuHasNEON) && IS_ALIGNED(dst_width, 8)) {
    ScaleRowDown4 =
        filtering ? ScaleRowDown4Box_16_NEON : ScaleRowDown4_16_NEON;
  }
#endif
#if defined(HAS_SCALEROWDOWN4_16_SSE2)
  if (TestCpuFlag(kCpuHasSSE2) && IS_ALIGNED(dst_width, 8)) {
    ScaleRowDown4 =
        filtering ? ScaleRowDown4Box_16_SSE2 : ScaleRowDown4_16_SSE2;
  }
#endif
#if defined(HAS_SCALEROWDOWN4_16_MMI)
  if (TestCpuFlag(kCpuHasMMI) && IS_ALIGNED(dst_width, 8)) {
    ScaleRowDown4 = filtering ? ScaleRowDown4Box_16_MMI : ScaleRowDown4_16_MMI;
  }
#endif

  if (filtering == kFilterLinear) {
    src_stride = 0;
  }
  for (y = 0; y < dst_height; ++y) {
    ScaleRowDown4(src_ptr, src_stride, dst_ptr, dst_width);
    src_ptr += row_stride;
    dst_ptr += dst_stride;
  }
}

// Scale plane down, 3/4
static void ScalePlaneDown34(int src_width,
                             int src_height,
                             int dst_width,
                             int dst_height,
                             int src_stride,
                             int dst_stride,
                             const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             enum FilterMode filtering) {
  int y;
  void (*ScaleRowDown34_0)(const uint8_t* src_ptr, ptrdiff_t src_stride,
                           uint8_t* dst_ptr, int dst_width);
  void (*ScaleRowDown34_1)(const uint8_t* src_ptr, ptrdiff_t src_stride,
                           uint8_t* dst_ptr, int dst_width);
  const int filter_stride = (filtering == kFilterLinear) ? 0 : src_stride;
  (void)src_width;
  (void)src_height;
  assert(dst_width % 3 == 0);
  if (!filtering) {
    ScaleRowDown34_0 = ScaleRowDown34_C;
    ScaleRowDown34_1 = ScaleRowDown34_C;
  } else {
    ScaleRowDown34_0 = ScaleRowDown34_0_Box_C;
    ScaleRowDown34_1 = ScaleRowDown34_1_Box_C;
  }
#if defined(HAS_SCALEROWDOWN34_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    if (!filtering) {
      ScaleRowDown34_0 = ScaleRowDown34_Any_NEON;
      ScaleRowDown34_1 = ScaleRowDown34_Any_NEON;
    } else {
      ScaleRowDown34_0 = ScaleRowDown34_0_Box_Any_NEON;
      ScaleRowDown34_1 = ScaleRowDown34_1_Box_Any_NEON;
    }
    if (dst_width % 24 == 0) {
      if (!filtering) {
        ScaleRowDown34_0 = ScaleRowDown34_NEON;
        ScaleRowDown34_1 = ScaleRowDown34_NEON;
      } else {
        ScaleRowDown34_0 = ScaleRowDown34_0_Box_NEON;
        ScaleRowDown34_1 = ScaleRowDown34_1_Box_NEON;
      }
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN34_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    if (!filtering) {
      ScaleRowDown34_0 = ScaleRowDown34_Any_MMI;
      ScaleRowDown34_1 = ScaleRowDown34_Any_MMI;
      if (dst_width % 24 == 0) {
        ScaleRowDown34_0 = ScaleRowDown34_MMI;
        ScaleRowDown34_1 = ScaleRowDown34_MMI;
      }
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN34_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    if (!filtering) {
      ScaleRowDown34_0 = ScaleRowDown34_Any_MSA;
      ScaleRowDown34_1 = ScaleRowDown34_Any_MSA;
    } else {
      ScaleRowDown34_0 = ScaleRowDown34_0_Box_Any_MSA;
      ScaleRowDown34_1 = ScaleRowDown34_1_Box_Any_MSA;
    }
    if (dst_width % 48 == 0) {
      if (!filtering) {
        ScaleRowDown34_0 = ScaleRowDown34_MSA;
        ScaleRowDown34_1 = ScaleRowDown34_MSA;
      } else {
        ScaleRowDown34_0 = ScaleRowDown34_0_Box_MSA;
        ScaleRowDown34_1 = ScaleRowDown34_1_Box_MSA;
      }
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN34_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    if (!filtering) {
      ScaleRowDown34_0 = ScaleRowDown34_Any_SSSE3;
      ScaleRowDown34_1 = ScaleRowDown34_Any_SSSE3;
    } else {
      ScaleRowDown34_0 = ScaleRowDown34_0_Box_Any_SSSE3;
      ScaleRowDown34_1 = ScaleRowDown34_1_Box_Any_SSSE3;
    }
    if (dst_width % 24 == 0) {
      if (!filtering) {
        ScaleRowDown34_0 = ScaleRowDown34_SSSE3;
        ScaleRowDown34_1 = ScaleRowDown34_SSSE3;
      } else {
        ScaleRowDown34_0 = ScaleRowDown34_0_Box_SSSE3;
        ScaleRowDown34_1 = ScaleRowDown34_1_Box_SSSE3;
      }
    }
  }
#endif

  for (y = 0; y < dst_height - 2; y += 3) {
    ScaleRowDown34_0(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride;
    dst_ptr += dst_stride;
    ScaleRowDown34_1(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride;
    dst_ptr += dst_stride;
    ScaleRowDown34_0(src_ptr + src_stride, -filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride * 2;
    dst_ptr += dst_stride;
  }

  // Remainder 1 or 2 rows with last row vertically unfiltered
  if ((dst_height % 3) == 2) {
    ScaleRowDown34_0(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride;
    dst_ptr += dst_stride;
    ScaleRowDown34_1(src_ptr, 0, dst_ptr, dst_width);
  } else if ((dst_height % 3) == 1) {
    ScaleRowDown34_0(src_ptr, 0, dst_ptr, dst_width);
  }
}

static void ScalePlaneDown34_16(int src_width,
                                int src_height,
                                int dst_width,
                                int dst_height,
                                int src_stride,
                                int dst_stride,
                                const uint16_t* src_ptr,
                                uint16_t* dst_ptr,
                                enum FilterMode filtering) {
  int y;
  void (*ScaleRowDown34_0)(const uint16_t* src_ptr, ptrdiff_t src_stride,
                           uint16_t* dst_ptr, int dst_width);
  void (*ScaleRowDown34_1)(const uint16_t* src_ptr, ptrdiff_t src_stride,
                           uint16_t* dst_ptr, int dst_width);
  const int filter_stride = (filtering == kFilterLinear) ? 0 : src_stride;
  (void)src_width;
  (void)src_height;
  assert(dst_width % 3 == 0);
  if (!filtering) {
    ScaleRowDown34_0 = ScaleRowDown34_16_C;
    ScaleRowDown34_1 = ScaleRowDown34_16_C;
  } else {
    ScaleRowDown34_0 = ScaleRowDown34_0_Box_16_C;
    ScaleRowDown34_1 = ScaleRowDown34_1_Box_16_C;
  }
#if defined(HAS_SCALEROWDOWN34_16_NEON)
  if (TestCpuFlag(kCpuHasNEON) && (dst_width % 24 == 0)) {
    if (!filtering) {
      ScaleRowDown34_0 = ScaleRowDown34_16_NEON;
      ScaleRowDown34_1 = ScaleRowDown34_16_NEON;
    } else {
      ScaleRowDown34_0 = ScaleRowDown34_0_Box_16_NEON;
      ScaleRowDown34_1 = ScaleRowDown34_1_Box_16_NEON;
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN34_16_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && (dst_width % 24 == 0)) {
    if (!filtering) {
      ScaleRowDown34_0 = ScaleRowDown34_16_SSSE3;
      ScaleRowDown34_1 = ScaleRowDown34_16_SSSE3;
    } else {
      ScaleRowDown34_0 = ScaleRowDown34_0_Box_16_SSSE3;
      ScaleRowDown34_1 = ScaleRowDown34_1_Box_16_SSSE3;
    }
  }
#endif

  for (y = 0; y < dst_height - 2; y += 3) {
    ScaleRowDown34_0(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride;
    dst_ptr += dst_stride;
    ScaleRowDown34_1(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride;
    dst_ptr += dst_stride;
    ScaleRowDown34_0(src_ptr + src_stride, -filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride * 2;
    dst_ptr += dst_stride;
  }

  // Remainder 1 or 2 rows with last row vertically unfiltered
  if ((dst_height % 3) == 2) {
    ScaleRowDown34_0(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride;
    dst_ptr += dst_stride;
    ScaleRowDown34_1(src_ptr, 0, dst_ptr, dst_width);
  } else if ((dst_height % 3) == 1) {
    ScaleRowDown34_0(src_ptr, 0, dst_ptr, dst_width);
  }
}

// Scale plane, 3/8
// This is an optimized version for scaling down a plane to 3/8
// of its original size.
//
// Uses box filter arranges like this
// aaabbbcc -> abc
// aaabbbcc    def
// aaabbbcc    ghi
// dddeeeff
// dddeeeff
// dddeeeff
// ggghhhii
// ggghhhii
// Boxes are 3x3, 2x3, 3x2 and 2x2

static void ScalePlaneDown38(int src_width,
                             int src_height,
                             int dst_width,
                             int dst_height,
                             int src_stride,
                             int dst_stride,
                             const uint8_t* src_ptr,
                             uint8_t* dst_ptr,
                             enum FilterMode filtering) {
  int y;
  void (*ScaleRowDown38_3)(const uint8_t* src_ptr, ptrdiff_t src_stride,
                           uint8_t* dst_ptr, int dst_width);
  void (*ScaleRowDown38_2)(const uint8_t* src_ptr, ptrdiff_t src_stride,
                           uint8_t* dst_ptr, int dst_width);
  const int filter_stride = (filtering == kFilterLinear) ? 0 : src_stride;
  assert(dst_width % 3 == 0);
  (void)src_width;
  (void)src_height;
  if (!filtering) {
    ScaleRowDown38_3 = ScaleRowDown38_C;
    ScaleRowDown38_2 = ScaleRowDown38_C;
  } else {
    ScaleRowDown38_3 = ScaleRowDown38_3_Box_C;
    ScaleRowDown38_2 = ScaleRowDown38_2_Box_C;
  }

#if defined(HAS_SCALEROWDOWN38_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    if (!filtering) {
      ScaleRowDown38_3 = ScaleRowDown38_Any_NEON;
      ScaleRowDown38_2 = ScaleRowDown38_Any_NEON;
    } else {
      ScaleRowDown38_3 = ScaleRowDown38_3_Box_Any_NEON;
      ScaleRowDown38_2 = ScaleRowDown38_2_Box_Any_NEON;
    }
    if (dst_width % 12 == 0) {
      if (!filtering) {
        ScaleRowDown38_3 = ScaleRowDown38_NEON;
        ScaleRowDown38_2 = ScaleRowDown38_NEON;
      } else {
        ScaleRowDown38_3 = ScaleRowDown38_3_Box_NEON;
        ScaleRowDown38_2 = ScaleRowDown38_2_Box_NEON;
      }
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN38_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    if (!filtering) {
      ScaleRowDown38_3 = ScaleRowDown38_Any_SSSE3;
      ScaleRowDown38_2 = ScaleRowDown38_Any_SSSE3;
    } else {
      ScaleRowDown38_3 = ScaleRowDown38_3_Box_Any_SSSE3;
      ScaleRowDown38_2 = ScaleRowDown38_2_Box_Any_SSSE3;
    }
    if (dst_width % 12 == 0 && !filtering) {
      ScaleRowDown38_3 = ScaleRowDown38_SSSE3;
      ScaleRowDown38_2 = ScaleRowDown38_SSSE3;
    }
    if (dst_width % 6 == 0 && filtering) {
      ScaleRowDown38_3 = ScaleRowDown38_3_Box_SSSE3;
      ScaleRowDown38_2 = ScaleRowDown38_2_Box_SSSE3;
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN38_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    if (!filtering) {
      ScaleRowDown38_3 = ScaleRowDown38_Any_MSA;
      ScaleRowDown38_2 = ScaleRowDown38_Any_MSA;
    } else {
      ScaleRowDown38_3 = ScaleRowDown38_3_Box_Any_MSA;
      ScaleRowDown38_2 = ScaleRowDown38_2_Box_Any_MSA;
    }
    if (dst_width % 12 == 0) {
      if (!filtering) {
        ScaleRowDown38_3 = ScaleRowDown38_MSA;
        ScaleRowDown38_2 = ScaleRowDown38_MSA;
      } else {
        ScaleRowDown38_3 = ScaleRowDown38_3_Box_MSA;
        ScaleRowDown38_2 = ScaleRowDown38_2_Box_MSA;
      }
    }
  }
#endif

  for (y = 0; y < dst_height - 2; y += 3) {
    ScaleRowDown38_3(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride * 3;
    dst_ptr += dst_stride;
    ScaleRowDown38_3(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride * 3;
    dst_ptr += dst_stride;
    ScaleRowDown38_2(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride * 2;
    dst_ptr += dst_stride;
  }

  // Remainder 1 or 2 rows with last row vertically unfiltered
  if ((dst_height % 3) == 2) {
    ScaleRowDown38_3(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride * 3;
    dst_ptr += dst_stride;
    ScaleRowDown38_3(src_ptr, 0, dst_ptr, dst_width);
  } else if ((dst_height % 3) == 1) {
    ScaleRowDown38_3(src_ptr, 0, dst_ptr, dst_width);
  }
}

static void ScalePlaneDown38_16(int src_width,
                                int src_height,
                                int dst_width,
                                int dst_height,
                                int src_stride,
                                int dst_stride,
                                const uint16_t* src_ptr,
                                uint16_t* dst_ptr,
                                enum FilterMode filtering) {
  int y;
  void (*ScaleRowDown38_3)(const uint16_t* src_ptr, ptrdiff_t src_stride,
                           uint16_t* dst_ptr, int dst_width);
  void (*ScaleRowDown38_2)(const uint16_t* src_ptr, ptrdiff_t src_stride,
                           uint16_t* dst_ptr, int dst_width);
  const int filter_stride = (filtering == kFilterLinear) ? 0 : src_stride;
  (void)src_width;
  (void)src_height;
  assert(dst_width % 3 == 0);
  if (!filtering) {
    ScaleRowDown38_3 = ScaleRowDown38_16_C;
    ScaleRowDown38_2 = ScaleRowDown38_16_C;
  } else {
    ScaleRowDown38_3 = ScaleRowDown38_3_Box_16_C;
    ScaleRowDown38_2 = ScaleRowDown38_2_Box_16_C;
  }
#if defined(HAS_SCALEROWDOWN38_16_NEON)
  if (TestCpuFlag(kCpuHasNEON) && (dst_width % 12 == 0)) {
    if (!filtering) {
      ScaleRowDown38_3 = ScaleRowDown38_16_NEON;
      ScaleRowDown38_2 = ScaleRowDown38_16_NEON;
    } else {
      ScaleRowDown38_3 = ScaleRowDown38_3_Box_16_NEON;
      ScaleRowDown38_2 = ScaleRowDown38_2_Box_16_NEON;
    }
  }
#endif
#if defined(HAS_SCALEROWDOWN38_16_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && (dst_width % 24 == 0)) {
    if (!filtering) {
      ScaleRowDown38_3 = ScaleRowDown38_16_SSSE3;
      ScaleRowDown38_2 = ScaleRowDown38_16_SSSE3;
    } else {
      ScaleRowDown38_3 = ScaleRowDown38_3_Box_16_SSSE3;
      ScaleRowDown38_2 = ScaleRowDown38_2_Box_16_SSSE3;
    }
  }
#endif

  for (y = 0; y < dst_height - 2; y += 3) {
    ScaleRowDown38_3(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride * 3;
    dst_ptr += dst_stride;
    ScaleRowDown38_3(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride * 3;
    dst_ptr += dst_stride;
    ScaleRowDown38_2(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride * 2;
    dst_ptr += dst_stride;
  }

  // Remainder 1 or 2 rows with last row vertically unfiltered
  if ((dst_height % 3) == 2) {
    ScaleRowDown38_3(src_ptr, filter_stride, dst_ptr, dst_width);
    src_ptr += src_stride * 3;
    dst_ptr += dst_stride;
    ScaleRowDown38_3(src_ptr, 0, dst_ptr, dst_width);
  } else if ((dst_height % 3) == 1) {
    ScaleRowDown38_3(src_ptr, 0, dst_ptr, dst_width);
  }
}

#define MIN1(x) ((x) < 1 ? 1 : (x))

static __inline uint32_t SumPixels(int iboxwidth, const uint16_t* src_ptr) {
  uint32_t sum = 0u;
  int x;
  assert(iboxwidth > 0);
  for (x = 0; x < iboxwidth; ++x) {
    sum += src_ptr[x];
  }
  return sum;
}

static __inline uint32_t SumPixels_16(int iboxwidth, const uint32_t* src_ptr) {
  uint32_t sum = 0u;
  int x;
  assert(iboxwidth > 0);
  for (x = 0; x < iboxwidth; ++x) {
    sum += src_ptr[x];
  }
  return sum;
}

static void ScaleAddCols2_C(int dst_width,
                            int boxheight,
                            int x,
                            int dx,
                            const uint16_t* src_ptr,
                            uint8_t* dst_ptr) {
  int i;
  int scaletbl[2];
  int minboxwidth = dx >> 16;
  int boxwidth;
  scaletbl[0] = 65536 / (MIN1(minboxwidth) * boxheight);
  scaletbl[1] = 65536 / (MIN1(minboxwidth + 1) * boxheight);
  for (i = 0; i < dst_width; ++i) {
    int ix = x >> 16;
    x += dx;
    boxwidth = MIN1((x >> 16) - ix);
    *dst_ptr++ =
        SumPixels(boxwidth, src_ptr + ix) * scaletbl[boxwidth - minboxwidth] >>
        16;
  }
}

static void ScaleAddCols2_16_C(int dst_width,
                               int boxheight,
                               int x,
                               int dx,
                               const uint32_t* src_ptr,
                               uint16_t* dst_ptr) {
  int i;
  int scaletbl[2];
  int minboxwidth = dx >> 16;
  int boxwidth;
  scaletbl[0] = 65536 / (MIN1(minboxwidth) * boxheight);
  scaletbl[1] = 65536 / (MIN1(minboxwidth + 1) * boxheight);
  for (i = 0; i < dst_width; ++i) {
    int ix = x >> 16;
    x += dx;
    boxwidth = MIN1((x >> 16) - ix);
    *dst_ptr++ = SumPixels_16(boxwidth, src_ptr + ix) *
                     scaletbl[boxwidth - minboxwidth] >>
                 16;
  }
}

static void ScaleAddCols0_C(int dst_width,
                            int boxheight,
                            int x,
                            int dx,
                            const uint16_t* src_ptr,
                            uint8_t* dst_ptr) {
  int scaleval = 65536 / boxheight;
  int i;
  (void)dx;
  src_ptr += (x >> 16);
  for (i = 0; i < dst_width; ++i) {
    *dst_ptr++ = src_ptr[i] * scaleval >> 16;
  }
}

static void ScaleAddCols1_C(int dst_width,
                            int boxheight,
                            int x,
                            int dx,
                            const uint16_t* src_ptr,
                            uint8_t* dst_ptr) {
  int boxwidth = MIN1(dx >> 16);
  int scaleval = 65536 / (boxwidth * boxheight);
  int i;
  x >>= 16;
  for (i = 0; i < dst_width; ++i) {
    *dst_ptr++ = SumPixels(boxwidth, src_ptr + x) * scaleval >> 16;
    x += boxwidth;
  }
}

static void ScaleAddCols1_16_C(int dst_width,
                               int boxheight,
                               int x,
                               int dx,
                               const uint32_t* src_ptr,
                               uint16_t* dst_ptr) {
  int boxwidth = MIN1(dx >> 16);
  int scaleval = 65536 / (boxwidth * boxheight);
  int i;
  for (i = 0; i < dst_width; ++i) {
    *dst_ptr++ = SumPixels_16(boxwidth, src_ptr + x) * scaleval >> 16;
    x += boxwidth;
  }
}

// Scale plane down to any dimensions, with interpolation.
// (boxfilter).
//
// Same method as SimpleScale, which is fixed point, outputting
// one pixel of destination using fixed point (16.16) to step
// through source, sampling a box of pixel with simple
// averaging.
static void ScalePlaneBox(int src_width,
                          int src_height,
                          int dst_width,
                          int dst_height,
                          int src_stride,
                          int dst_stride,
                          const uint8_t* src_ptr,
                          uint8_t* dst_ptr) {
  int j, k;
  // Initial source x/y coordinate and step values as 16.16 fixed point.
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = 0;
  const int max_y = (src_height << 16);
  ScaleSlope(src_width, src_height, dst_width, dst_height, kFilterBox, &x, &y,
             &dx, &dy);
  src_width = Abs(src_width);
  {
    // Allocate a row buffer of uint16_t.
    align_buffer_64(row16, src_width * 2);
    void (*ScaleAddCols)(int dst_width, int boxheight, int x, int dx,
                         const uint16_t* src_ptr, uint8_t* dst_ptr) =
        (dx & 0xffff) ? ScaleAddCols2_C
                      : ((dx != 0x10000) ? ScaleAddCols1_C : ScaleAddCols0_C);
    void (*ScaleAddRow)(const uint8_t* src_ptr, uint16_t* dst_ptr,
                        int src_width) = ScaleAddRow_C;
#if defined(HAS_SCALEADDROW_SSE2)
    if (TestCpuFlag(kCpuHasSSE2)) {
      ScaleAddRow = ScaleAddRow_Any_SSE2;
      if (IS_ALIGNED(src_width, 16)) {
        ScaleAddRow = ScaleAddRow_SSE2;
      }
    }
#endif
#if defined(HAS_SCALEADDROW_AVX2)
    if (TestCpuFlag(kCpuHasAVX2)) {
      ScaleAddRow = ScaleAddRow_Any_AVX2;
      if (IS_ALIGNED(src_width, 32)) {
        ScaleAddRow = ScaleAddRow_AVX2;
      }
    }
#endif
#if defined(HAS_SCALEADDROW_NEON)
    if (TestCpuFlag(kCpuHasNEON)) {
      ScaleAddRow = ScaleAddRow_Any_NEON;
      if (IS_ALIGNED(src_width, 16)) {
        ScaleAddRow = ScaleAddRow_NEON;
      }
    }
#endif
#if defined(HAS_SCALEADDROW_MMI)
    if (TestCpuFlag(kCpuHasMMI)) {
      ScaleAddRow = ScaleAddRow_Any_MMI;
      if (IS_ALIGNED(src_width, 8)) {
        ScaleAddRow = ScaleAddRow_MMI;
      }
    }
#endif
#if defined(HAS_SCALEADDROW_MSA)
    if (TestCpuFlag(kCpuHasMSA)) {
      ScaleAddRow = ScaleAddRow_Any_MSA;
      if (IS_ALIGNED(src_width, 16)) {
        ScaleAddRow = ScaleAddRow_MSA;
      }
    }
#endif

    for (j = 0; j < dst_height; ++j) {
      int boxheight;
      int iy = y >> 16;
      const uint8_t* src = src_ptr + iy * (int64_t)src_stride;
      y += dy;
      if (y > max_y) {
        y = max_y;
      }
      boxheight = MIN1((y >> 16) - iy);
      memset(row16, 0, src_width * 2);
      for (k = 0; k < boxheight; ++k) {
        ScaleAddRow(src, (uint16_t*)(row16), src_width);
        src += src_stride;
      }
      ScaleAddCols(dst_width, boxheight, x, dx, (uint16_t*)(row16), dst_ptr);
      dst_ptr += dst_stride;
    }
    free_aligned_buffer_64(row16);
  }
}

static void ScalePlaneBox_16(int src_width,
                             int src_height,
                             int dst_width,
                             int dst_height,
                             int src_stride,
                             int dst_stride,
                             const uint16_t* src_ptr,
                             uint16_t* dst_ptr) {
  int j, k;
  // Initial source x/y coordinate and step values as 16.16 fixed point.
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = 0;
  const int max_y = (src_height << 16);
  ScaleSlope(src_width, src_height, dst_width, dst_height, kFilterBox, &x, &y,
             &dx, &dy);
  src_width = Abs(src_width);
  {
    // Allocate a row buffer of uint32_t.
    align_buffer_64(row32, src_width * 4);
    void (*ScaleAddCols)(int dst_width, int boxheight, int x, int dx,
                         const uint32_t* src_ptr, uint16_t* dst_ptr) =
        (dx & 0xffff) ? ScaleAddCols2_16_C : ScaleAddCols1_16_C;
    void (*ScaleAddRow)(const uint16_t* src_ptr, uint32_t* dst_ptr,
                        int src_width) = ScaleAddRow_16_C;

#if defined(HAS_SCALEADDROW_16_SSE2)
    if (TestCpuFlag(kCpuHasSSE2) && IS_ALIGNED(src_width, 16)) {
      ScaleAddRow = ScaleAddRow_16_SSE2;
    }
#endif

#if defined(HAS_SCALEADDROW_16_MMI)
    if (TestCpuFlag(kCpuHasMMI) && IS_ALIGNED(src_width, 4)) {
      ScaleAddRow = ScaleAddRow_16_MMI;
    }
#endif
    for (j = 0; j < dst_height; ++j) {
      int boxheight;
      int iy = y >> 16;
      const uint16_t* src = src_ptr + iy * (int64_t)src_stride;
      y += dy;
      if (y > max_y) {
        y = max_y;
      }
      boxheight = MIN1((y >> 16) - iy);
      memset(row32, 0, src_width * 4);
      for (k = 0; k < boxheight; ++k) {
        ScaleAddRow(src, (uint32_t*)(row32), src_width);
        src += src_stride;
      }
      ScaleAddCols(dst_width, boxheight, x, dx, (uint32_t*)(row32), dst_ptr);
      dst_ptr += dst_stride;
    }
    free_aligned_buffer_64(row32);
  }
}

// Scale plane down with bilinear interpolation.
void ScalePlaneBilinearDown(int src_width,
                            int src_height,
                            int dst_width,
                            int dst_height,
                            int src_stride,
                            int dst_stride,
                            const uint8_t* src_ptr,
                            uint8_t* dst_ptr,
                            enum FilterMode filtering) {
  // Initial source x/y coordinate and step values as 16.16 fixed point.
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = 0;
  // TODO(fbarchard): Consider not allocating row buffer for kFilterLinear.
  // Allocate a row buffer.
  align_buffer_64(row, src_width);

  const int max_y = (src_height - 1) << 16;
  int j;
  void (*ScaleFilterCols)(uint8_t * dst_ptr, const uint8_t* src_ptr,
                          int dst_width, int x, int dx) =
      (src_width >= 32768) ? ScaleFilterCols64_C : ScaleFilterCols_C;
  void (*InterpolateRow)(uint8_t * dst_ptr, const uint8_t* src_ptr,
                         ptrdiff_t src_stride, int dst_width,
                         int source_y_fraction) = InterpolateRow_C;
  ScaleSlope(src_width, src_height, dst_width, dst_height, filtering, &x, &y,
             &dx, &dy);
  src_width = Abs(src_width);

#if defined(HAS_INTERPOLATEROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    InterpolateRow = InterpolateRow_Any_SSSE3;
    if (IS_ALIGNED(src_width, 16)) {
      InterpolateRow = InterpolateRow_SSSE3;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    InterpolateRow = InterpolateRow_Any_AVX2;
    if (IS_ALIGNED(src_width, 32)) {
      InterpolateRow = InterpolateRow_AVX2;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    InterpolateRow = InterpolateRow_Any_NEON;
    if (IS_ALIGNED(src_width, 16)) {
      InterpolateRow = InterpolateRow_NEON;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_MMI)
  if (TestCpuFlag(kCpuHasMMI)) {
    InterpolateRow = InterpolateRow_Any_MMI;
    if (IS_ALIGNED(src_width, 16)) {
      InterpolateRow = InterpolateRow_MMI;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_MSA)
  if (TestCpuFlag(kCpuHasMSA)) {
    InterpolateRow = InterpolateRow_Any_MSA;
    if (IS_ALIGNED(src_width, 32)) {
      InterpolateRow = InterpolateRow_MSA;
    }
  }
#endif

#if defined(HAS_SCALEFILTERCOLS_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && src_width < 32768) {
    ScaleFilterCols = ScaleFilterCols_SSSE3;
  }
#endif
#if defined(HAS_SCALEFILTERCOLS_NEON)
  if (TestCpuFlag(kCpuHasNEON) && src_width < 32768) {
    ScaleFilterCols = ScaleFilterCols_Any_NEON;
    if (IS_ALIGNED(dst_width, 8)) {
      ScaleFilterCols = ScaleFilterCols_NEON;
    }
  }
#endif
#if defined(HAS_SCALEFILTERCOLS_MSA)
  if (TestCpuFlag(kCpuHasMSA) && src_width < 32768) {
    ScaleFilterCols = ScaleFilterCols_Any_MSA;
    if (IS_ALIGNED(dst_width, 16)) {
      ScaleFilterCols = ScaleFilterCols_MSA;
    }
  }
#endif
  if (y > max_y) {
    y = max_y;
  }

  for (j = 0; j < dst_height; ++j) {
    int yi = y >> 16;
    const uint8_t* src = src_ptr + yi * (int64_t)src_stride;
    if (filtering == kFilterLinear) {
      ScaleFilterCols(dst_ptr, src, dst_width, x, dx);
    } else {
      int yf = (y >> 8) & 255;
      InterpolateRow(row, src, src_stride, src_width, yf);
      ScaleFilterCols(dst_ptr, row, dst_width, x, dx);
    }
    dst_ptr += dst_stride;
    y += dy;
    if (y > max_y) {
      y = max_y;
    }
  }
  free_aligned_buffer_64(row);
}

void ScalePlaneBilinearDown_16(int src_width,
                               int src_height,
                               int dst_width,
                               int dst_height,
                               int src_stride,
                               int dst_stride,
                               const uint16_t* src_ptr,
                               uint16_t* dst_ptr,
                               enum FilterMode filtering) {
  // Initial source x/y coordinate and step values as 16.16 fixed point.
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = 0;
  // TODO(fbarchard): Consider not allocating row buffer for kFilterLinear.
  // Allocate a row buffer.
  align_buffer_64(row, src_width * 2);

  const int max_y = (src_height - 1) << 16;
  int j;
  void (*ScaleFilterCols)(uint16_t * dst_ptr, const uint16_t* src_ptr,
                          int dst_width, int x, int dx) =
      (src_width >= 32768) ? ScaleFilterCols64_16_C : ScaleFilterCols_16_C;
  void (*InterpolateRow)(uint16_t * dst_ptr, const uint16_t* src_ptr,
                         ptrdiff_t src_stride, int dst_width,
                         int source_y_fraction) = InterpolateRow_16_C;
  ScaleSlope(src_width, src_height, dst_width, dst_height, filtering, &x, &y,
             &dx, &dy);
  src_width = Abs(src_width);

#if defined(HAS_INTERPOLATEROW_16_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    InterpolateRow = InterpolateRow_Any_16_SSE2;
    if (IS_ALIGNED(src_width, 16)) {
      InterpolateRow = InterpolateRow_16_SSE2;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_16_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    InterpolateRow = InterpolateRow_Any_16_SSSE3;
    if (IS_ALIGNED(src_width, 16)) {
      InterpolateRow = InterpolateRow_16_SSSE3;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_16_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    InterpolateRow = InterpolateRow_Any_16_AVX2;
    if (IS_ALIGNED(src_width, 32)) {
      InterpolateRow = InterpolateRow_16_AVX2;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_16_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    InterpolateRow = InterpolateRow_Any_16_NEON;
    if (IS_ALIGNED(src_width, 16)) {
      InterpolateRow = InterpolateRow_16_NEON;
    }
  }
#endif

#if defined(HAS_SCALEFILTERCOLS_16_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3) && src_width < 32768) {
    ScaleFilterCols = ScaleFilterCols_16_SSSE3;
  }
#endif
  if (y > max_y) {
    y = max_y;
  }

  for (j = 0; j < dst_height; ++j) {
    int yi = y >> 16;
    const uint16_t* src = src_ptr + yi * (int64_t)src_stride;
    if (filtering == kFilterLinear) {
      ScaleFilterCols(dst_ptr, src, dst_width, x, dx);
    } else {
      int yf = (y >> 8) & 255;
      InterpolateRow((uint16_t*)row, src, src_stride, src_width, yf);
      ScaleFilterCols(dst_ptr, (uint16_t*)row, dst_width, x, dx);
    }
    dst_ptr += dst_stride;
    y += dy;
    if (y > max_y) {
      y = max_y;
    }
  }
  free_aligned_buffer_64(row);
}

// Scale up down with bilinear interpolation.
void ScalePlaneBilinearUp(int src_width,
                          int src_height,
                          int dst_width,
                          int dst_height,
                          int src_stride,
                          int dst_stride,
                          const uint8_t* src_ptr,
                          uint8_t* dst_ptr,
                          enum FilterMode filtering) {
  int j;
  // Initial source x/y coordinate and step values as 16.16 fixed point.
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = 0;
  const int max_y = (src_height - 1) << 16;
  void (*InterpolateRow)(uint8_t * dst_ptr, const uint8_t* src_ptr,
                         ptrdiff_t src_stride, int dst_width,
                         int source_y_fraction) = InterpolateRow_C;
  void (*ScaleFilterCols)(uint8_t * dst_ptr, const uint8_t* src_ptr,
                          int dst_width, int x, int dx) =
      filtering ? ScaleFilterCols_C : ScaleCols_C;
  ScaleSlope(src_width, src_height, dst_width, dst_height, filtering, &x, &y,
             &dx, &dy);
  src_width = Abs(src_width);

#if defined(HAS_INTERPOLATEROW_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    InterpolateRow = InterpolateRow_Any_SSSE3;
    if (IS_ALIGNED(dst_width, 16)) {
      InterpolateRow = InterpolateRow_SSSE3;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    InterpolateRow = InterpolateRow_Any_AVX2;
    if (IS_ALIGNED(dst_width, 32)) {
      InterpolateRow = InterpolateRow_AVX2;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    InterpolateRow = InterpolateRow_Any_NEON;
    if (IS_ALIGNED(dst_width, 16)) {
      InterpolateRow = InterpolateRow_NEON;
    }
  }
#endif

  if (filtering && src_width >= 32768) {
    ScaleFilterCols = ScaleFilterCols64_C;
  }
#if defined(HAS_SCALEFILTERCOLS_SSSE3)
  if (filtering && TestCpuFlag(kCpuHasSSSE3) && src_width < 32768) {
    ScaleFilterCols = ScaleFilterCols_SSSE3;
  }
#endif
#if defined(HAS_SCALEFILTERCOLS_NEON)
  if (filtering && TestCpuFlag(kCpuHasNEON) && src_width < 32768) {
    ScaleFilterCols = ScaleFilterCols_Any_NEON;
    if (IS_ALIGNED(dst_width, 8)) {
      ScaleFilterCols = ScaleFilterCols_NEON;
    }
  }
#endif
#if defined(HAS_SCALEFILTERCOLS_MSA)
  if (filtering && TestCpuFlag(kCpuHasMSA) && src_width < 32768) {
    ScaleFilterCols = ScaleFilterCols_Any_MSA;
    if (IS_ALIGNED(dst_width, 16)) {
      ScaleFilterCols = ScaleFilterCols_MSA;
    }
  }
#endif
  if (!filtering && src_width * 2 == dst_width && x < 0x8000) {
    ScaleFilterCols = ScaleColsUp2_C;
#if defined(HAS_SCALECOLS_SSE2)
    if (TestCpuFlag(kCpuHasSSE2) && IS_ALIGNED(dst_width, 8)) {
      ScaleFilterCols = ScaleColsUp2_SSE2;
    }
#endif
#if defined(HAS_SCALECOLS_MMI)
    if (TestCpuFlag(kCpuHasMMI) && IS_ALIGNED(dst_width, 8)) {
      ScaleFilterCols = ScaleColsUp2_MMI;
    }
#endif
  }

  if (y > max_y) {
    y = max_y;
  }
  {
    int yi = y >> 16;
    const uint8_t* src = src_ptr + yi * (int64_t)src_stride;

    // Allocate 2 row buffers.
    const int kRowSize = (dst_width + 31) & ~31;
    align_buffer_64(row, kRowSize * 2);

    uint8_t* rowptr = row;
    int rowstride = kRowSize;
    int lasty = yi;

    ScaleFilterCols(rowptr, src, dst_width, x, dx);
    if (src_height > 1) {
      src += src_stride;
    }
    ScaleFilterCols(rowptr + rowstride, src, dst_width, x, dx);
    src += src_stride;

    for (j = 0; j < dst_height; ++j) {
      yi = y >> 16;
      if (yi != lasty) {
        if (y > max_y) {
          y = max_y;
          yi = y >> 16;
          src = src_ptr + yi * (int64_t)src_stride;
        }
        if (yi != lasty) {
          ScaleFilterCols(rowptr, src, dst_width, x, dx);
          rowptr += rowstride;
          rowstride = -rowstride;
          lasty = yi;
          src += src_stride;
        }
      }
      if (filtering == kFilterLinear) {
        InterpolateRow(dst_ptr, rowptr, 0, dst_width, 0);
      } else {
        int yf = (y >> 8) & 255;
        InterpolateRow(dst_ptr, rowptr, rowstride, dst_width, yf);
      }
      dst_ptr += dst_stride;
      y += dy;
    }
    free_aligned_buffer_64(row);
  }
}

// Scale plane, horizontally up by 2 times.
// Uses linear filter horizontally, nearest vertically.
// This is an optimized version for scaling up a plane to 2 times of
// its original width, using linear interpolation.
// This is used to scale U and V planes of I422 to I444.
void ScalePlaneUp2_Linear(int src_width,
                          int src_height,
                          int dst_width,
                          int dst_height,
                          int src_stride,
                          int dst_stride,
                          const uint8_t* src_ptr,
                          uint8_t* dst_ptr) {
  void (*ScaleRowUp)(const uint8_t* src_ptr, uint8_t* dst_ptr, int dst_width) =
      ScaleRowUp2_Linear_Any_C;
  int i;
  int y;
  int dy;

  // This function can only scale up by 2 times horizontally.
  assert(src_width == ((dst_width + 1) / 2));

#ifdef HAS_SCALEROWUP2LINEAR_SSE2
  if (TestCpuFlag(kCpuHasSSE2)) {
    ScaleRowUp = ScaleRowUp2_Linear_Any_SSE2;
  }
#endif

#ifdef HAS_SCALEROWUP2LINEAR_SSSE3
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ScaleRowUp = ScaleRowUp2_Linear_Any_SSSE3;
  }
#endif

#ifdef HAS_SCALEROWUP2LINEAR_AVX2
  if (TestCpuFlag(kCpuHasAVX2)) {
    ScaleRowUp = ScaleRowUp2_Linear_Any_AVX2;
  }
#endif

#ifdef HAS_SCALEROWUP2LINEAR_NEON
  if (TestCpuFlag(kCpuHasNEON)) {
    ScaleRowUp = ScaleRowUp2_Linear_Any_NEON;
  }
#endif

  if (dst_height == 1) {
    ScaleRowUp(src_ptr + ((src_height - 1) / 2) * (int64_t)src_stride, dst_ptr,
               dst_width);
  } else {
    dy = FixedDiv(src_height - 1, dst_height - 1);
    y = (1 << 15) - 1;
    for (i = 0; i < dst_height; ++i) {
      ScaleRowUp(src_ptr + (y >> 16) * (int64_t)src_stride, dst_ptr, dst_width);
      dst_ptr += dst_stride;
      y += dy;
    }
  }
}

// Scale plane, up by 2 times.
// This is an optimized version for scaling up a plane to 2 times of
// its original size, using bilinear interpolation.
// This is used to scale U and V planes of I420 to I444.
void ScalePlaneUp2_Bilinear(int src_width,
                            int src_height,
                            int dst_width,
                            int dst_height,
                            int src_stride,
                            int dst_stride,
                            const uint8_t* src_ptr,
                            uint8_t* dst_ptr) {
  void (*Scale2RowUp)(const uint8_t* src_ptr, ptrdiff_t src_stride,
                      uint8_t* dst_ptr, ptrdiff_t dst_stride, int dst_width) =
      ScaleRowUp2_Bilinear_Any_C;
  int x;

  // This function can only scale up by 2 times.
  assert(src_width == ((dst_width + 1) / 2));
  assert(src_height == ((dst_height + 1) / 2));

#ifdef HAS_SCALEROWUP2BILINEAR_SSE2
  if (TestCpuFlag(kCpuHasSSE2)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_Any_SSE2;
  }
#endif

#ifdef HAS_SCALEROWUP2BILINEAR_SSSE3
  if (TestCpuFlag(kCpuHasSSSE3)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_Any_SSSE3;
  }
#endif

#ifdef HAS_SCALEROWUP2BILINEAR_AVX2
  if (TestCpuFlag(kCpuHasAVX2)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_Any_AVX2;
  }
#endif

#ifdef HAS_SCALEROWUP2BILINEAR_NEON
  if (TestCpuFlag(kCpuHasNEON)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_Any_NEON;
  }
#endif

  Scale2RowUp(src_ptr, 0, dst_ptr, 0, dst_width);
  dst_ptr += dst_stride;
  for (x = 0; x < src_height - 1; ++x) {
    Scale2RowUp(src_ptr, src_stride, dst_ptr, dst_stride, dst_width);
    src_ptr += src_stride;
    // TODO(fbarchard): Test performance of writing one row of destination at a
    // time.
    dst_ptr += 2 * dst_stride;
  }
  if (!(dst_height & 1)) {
    Scale2RowUp(src_ptr, 0, dst_ptr, 0, dst_width);
  }
}

// Scale at most 14 bit plane, horizontally up by 2 times.
// This is an optimized version for scaling up a plane to 2 times of
// its original width, using linear interpolation.
// stride is in count of uint16_t.
// This is used to scale U and V planes of I210 to I410 and I212 to I412.
void ScalePlaneUp2_12_Linear(int src_width,
                             int src_height,
                             int dst_width,
                             int dst_height,
                             int src_stride,
                             int dst_stride,
                             const uint16_t* src_ptr,
                             uint16_t* dst_ptr) {
  void (*ScaleRowUp)(const uint16_t* src_ptr, uint16_t* dst_ptr,
                     int dst_width) = ScaleRowUp2_Linear_16_Any_C;
  int i;
  int y;
  int dy;

  // This function can only scale up by 2 times horizontally.
  assert(src_width == ((dst_width + 1) / 2));

#ifdef HAS_SCALEROWUP2LINEAR_12_SSSE3
  if (TestCpuFlag(kCpuHasSSSE3)) {
    ScaleRowUp = ScaleRowUp2_Linear_12_Any_SSSE3;
  }
#endif

#ifdef HAS_SCALEROWUP2LINEAR_12_AVX2
  if (TestCpuFlag(kCpuHasAVX2)) {
    ScaleRowUp = ScaleRowUp2_Linear_12_Any_AVX2;
  }
#endif

#ifdef HAS_SCALEROWUP2LINEAR_12_NEON
  if (TestCpuFlag(kCpuHasNEON)) {
    ScaleRowUp = ScaleRowUp2_Linear_12_Any_NEON;
  }
#endif

  if (dst_height == 1) {
    ScaleRowUp(src_ptr + ((src_height - 1) / 2) * (int64_t)src_stride, dst_ptr,
               dst_width);
  } else {
    dy = FixedDiv(src_height - 1, dst_height - 1);
    y = (1 << 15) - 1;
    for (i = 0; i < dst_height; ++i) {
      ScaleRowUp(src_ptr + (y >> 16) * (int64_t)src_stride, dst_ptr, dst_width);
      dst_ptr += dst_stride;
      y += dy;
    }
  }
}

// Scale at most 12 bit plane, up by 2 times.
// This is an optimized version for scaling up a plane to 2 times of
// its original size, using bilinear interpolation.
// stride is in count of uint16_t.
// This is used to scale U and V planes of I010 to I410 and I012 to I412.
void ScalePlaneUp2_12_Bilinear(int src_width,
                               int src_height,
                               int dst_width,
                               int dst_height,
                               int src_stride,
                               int dst_stride,
                               const uint16_t* src_ptr,
                               uint16_t* dst_ptr) {
  void (*Scale2RowUp)(const uint16_t* src_ptr, ptrdiff_t src_stride,
                      uint16_t* dst_ptr, ptrdiff_t dst_stride, int dst_width) =
      ScaleRowUp2_Bilinear_16_Any_C;
  int x;

  // This function can only scale up by 2 times.
  assert(src_width == ((dst_width + 1) / 2));
  assert(src_height == ((dst_height + 1) / 2));

#ifdef HAS_SCALEROWUP2BILINEAR_12_SSSE3
  if (TestCpuFlag(kCpuHasSSSE3)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_12_Any_SSSE3;
  }
#endif

#ifdef HAS_SCALEROWUP2BILINEAR_12_AVX2
  if (TestCpuFlag(kCpuHasAVX2)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_12_Any_AVX2;
  }
#endif

#ifdef HAS_SCALEROWUP2BILINEAR_12_NEON
  if (TestCpuFlag(kCpuHasNEON)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_12_Any_NEON;
  }
#endif

  Scale2RowUp(src_ptr, 0, dst_ptr, 0, dst_width);
  dst_ptr += dst_stride;
  for (x = 0; x < src_height - 1; ++x) {
    Scale2RowUp(src_ptr, src_stride, dst_ptr, dst_stride, dst_width);
    src_ptr += src_stride;
    dst_ptr += 2 * dst_stride;
  }
  if (!(dst_height & 1)) {
    Scale2RowUp(src_ptr, 0, dst_ptr, 0, dst_width);
  }
}

void ScalePlaneUp2_16_Linear(int src_width,
                             int src_height,
                             int dst_width,
                             int dst_height,
                             int src_stride,
                             int dst_stride,
                             const uint16_t* src_ptr,
                             uint16_t* dst_ptr) {
  void (*ScaleRowUp)(const uint16_t* src_ptr, uint16_t* dst_ptr,
                     int dst_width) = ScaleRowUp2_Linear_16_Any_C;
  int i;
  int y;
  int dy;

  // This function can only scale up by 2 times horizontally.
  assert(src_width == ((dst_width + 1) / 2));

#ifdef HAS_SCALEROWUP2LINEAR_16_SSE2
  if (TestCpuFlag(kCpuHasSSE2)) {
    ScaleRowUp = ScaleRowUp2_Linear_16_Any_SSE2;
  }
#endif

#ifdef HAS_SCALEROWUP2LINEAR_16_AVX2
  if (TestCpuFlag(kCpuHasAVX2)) {
    ScaleRowUp = ScaleRowUp2_Linear_16_Any_AVX2;
  }
#endif

#ifdef HAS_SCALEROWUP2LINEAR_16_NEON
  if (TestCpuFlag(kCpuHasNEON)) {
    ScaleRowUp = ScaleRowUp2_Linear_16_Any_NEON;
  }
#endif

  if (dst_height == 1) {
    ScaleRowUp(src_ptr + ((src_height - 1) / 2) * (int64_t)src_stride, dst_ptr,
               dst_width);
  } else {
    dy = FixedDiv(src_height - 1, dst_height - 1);
    y = (1 << 15) - 1;
    for (i = 0; i < dst_height; ++i) {
      ScaleRowUp(src_ptr + (y >> 16) * (int64_t)src_stride, dst_ptr, dst_width);
      dst_ptr += dst_stride;
      y += dy;
    }
  }
}

void ScalePlaneUp2_16_Bilinear(int src_width,
                               int src_height,
                               int dst_width,
                               int dst_height,
                               int src_stride,
                               int dst_stride,
                               const uint16_t* src_ptr,
                               uint16_t* dst_ptr) {
  void (*Scale2RowUp)(const uint16_t* src_ptr, ptrdiff_t src_stride,
                      uint16_t* dst_ptr, ptrdiff_t dst_stride, int dst_width) =
      ScaleRowUp2_Bilinear_16_Any_C;
  int x;

  // This function can only scale up by 2 times.
  assert(src_width == ((dst_width + 1) / 2));
  assert(src_height == ((dst_height + 1) / 2));

#ifdef HAS_SCALEROWUP2BILINEAR_16_SSE2
  if (TestCpuFlag(kCpuHasSSSE3)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_16_Any_SSSE3;
  }
#endif

#ifdef HAS_SCALEROWUP2BILINEAR_16_AVX2
  if (TestCpuFlag(kCpuHasAVX2)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_16_Any_AVX2;
  }
#endif

#ifdef HAS_SCALEROWUP2BILINEAR_16_NEON
  if (TestCpuFlag(kCpuHasNEON)) {
    Scale2RowUp = ScaleRowUp2_Bilinear_16_Any_NEON;
  }
#endif

  Scale2RowUp(src_ptr, 0, dst_ptr, 0, dst_width);
  dst_ptr += dst_stride;
  for (x = 0; x < src_height - 1; ++x) {
    Scale2RowUp(src_ptr, src_stride, dst_ptr, dst_stride, dst_width);
    src_ptr += src_stride;
    dst_ptr += 2 * dst_stride;
  }
  if (!(dst_height & 1)) {
    Scale2RowUp(src_ptr, 0, dst_ptr, 0, dst_width);
  }
}

void ScalePlaneBilinearUp_16(int src_width,
                             int src_height,
                             int dst_width,
                             int dst_height,
                             int src_stride,
                             int dst_stride,
                             const uint16_t* src_ptr,
                             uint16_t* dst_ptr,
                             enum FilterMode filtering) {
  int j;
  // Initial source x/y coordinate and step values as 16.16 fixed point.
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = 0;
  const int max_y = (src_height - 1) << 16;
  void (*InterpolateRow)(uint16_t * dst_ptr, const uint16_t* src_ptr,
                         ptrdiff_t src_stride, int dst_width,
                         int source_y_fraction) = InterpolateRow_16_C;
  void (*ScaleFilterCols)(uint16_t * dst_ptr, const uint16_t* src_ptr,
                          int dst_width, int x, int dx) =
      filtering ? ScaleFilterCols_16_C : ScaleCols_16_C;
  ScaleSlope(src_width, src_height, dst_width, dst_height, filtering, &x, &y,
             &dx, &dy);
  src_width = Abs(src_width);

#if defined(HAS_INTERPOLATEROW_16_SSE2)
  if (TestCpuFlag(kCpuHasSSE2)) {
    InterpolateRow = InterpolateRow_Any_16_SSE2;
    if (IS_ALIGNED(dst_width, 16)) {
      InterpolateRow = InterpolateRow_16_SSE2;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_16_SSSE3)
  if (TestCpuFlag(kCpuHasSSSE3)) {
    InterpolateRow = InterpolateRow_Any_16_SSSE3;
    if (IS_ALIGNED(dst_width, 16)) {
      InterpolateRow = InterpolateRow_16_SSSE3;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_16_AVX2)
  if (TestCpuFlag(kCpuHasAVX2)) {
    InterpolateRow = InterpolateRow_Any_16_AVX2;
    if (IS_ALIGNED(dst_width, 32)) {
      InterpolateRow = InterpolateRow_16_AVX2;
    }
  }
#endif
#if defined(HAS_INTERPOLATEROW_16_NEON)
  if (TestCpuFlag(kCpuHasNEON)) {
    InterpolateRow = InterpolateRow_Any_16_NEON;
    if (IS_ALIGNED(dst_width, 16)) {
      InterpolateRow = InterpolateRow_16_NEON;
    }
  }
#endif

  if (filtering && src_width >= 32768) {
    ScaleFilterCols = ScaleFilterCols64_16_C;
  }
#if defined(HAS_SCALEFILTERCOLS_16_SSSE3)
  if (filtering && TestCpuFlag(kCpuHasSSSE3) && src_width < 32768) {
    ScaleFilterCols = ScaleFilterCols_16_SSSE3;
  }
#endif
  if (!filtering && src_width * 2 == dst_width && x < 0x8000) {
    ScaleFilterCols = ScaleColsUp2_16_C;
#if defined(HAS_SCALECOLS_16_SSE2)
    if (TestCpuFlag(kCpuHasSSE2) && IS_ALIGNED(dst_width, 8)) {
      ScaleFilterCols = ScaleColsUp2_16_SSE2;
    }
#endif
#if defined(HAS_SCALECOLS_16_MMI)
    if (TestCpuFlag(kCpuHasMMI) && IS_ALIGNED(dst_width, 8)) {
      ScaleFilterCols = ScaleColsUp2_16_MMI;
    }
#endif
  }

  if (y > max_y) {
    y = max_y;
  }
  {
    int yi = y >> 16;
    const uint16_t* src = src_ptr + yi * (int64_t)src_stride;

    // Allocate 2 row buffers.
    const int kRowSize = (dst_width + 31) & ~31;
    align_buffer_64(row, kRowSize * 4);

    uint16_t* rowptr = (uint16_t*)row;
    int rowstride = kRowSize;
    int lasty = yi;

    ScaleFilterCols(rowptr, src, dst_width, x, dx);
    if (src_height > 1) {
      src += src_stride;
    }
    ScaleFilterCols(rowptr + rowstride, src, dst_width, x, dx);
    src += src_stride;

    for (j = 0; j < dst_height; ++j) {
      yi = y >> 16;
      if (yi != lasty) {
        if (y > max_y) {
          y = max_y;
          yi = y >> 16;
          src = src_ptr + yi * (int64_t)src_stride;
        }
        if (yi != lasty) {
          ScaleFilterCols(rowptr, src, dst_width, x, dx);
          rowptr += rowstride;
          rowstride = -rowstride;
          lasty = yi;
          src += src_stride;
        }
      }
      if (filtering == kFilterLinear) {
        InterpolateRow(dst_ptr, rowptr, 0, dst_width, 0);
      } else {
        int yf = (y >> 8) & 255;
        InterpolateRow(dst_ptr, rowptr, rowstride, dst_width, yf);
      }
      dst_ptr += dst_stride;
      y += dy;
    }
    free_aligned_buffer_64(row);
  }
}

// Scale Plane to/from any dimensions, without interpolation.
// Fixed point math is used for performance: The upper 16 bits
// of x and dx is the integer part of the source position and
// the lower 16 bits are the fixed decimal part.

static void ScalePlaneSimple(int src_width,
                             int src_height,
                             int dst_width,
                             int dst_height,
                             int src_stride,
                             int dst_stride,
                             const uint8_t* src_ptr,
                             uint8_t* dst_ptr) {
  int i;
  void (*ScaleCols)(uint8_t * dst_ptr, const uint8_t* src_ptr, int dst_width,
                    int x, int dx) = ScaleCols_C;
  // Initial source x/y coordinate and step values as 16.16 fixed point.
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = 0;
  ScaleSlope(src_width, src_height, dst_width, dst_height, kFilterNone, &x, &y,
             &dx, &dy);
  src_width = Abs(src_width);

  if (src_width * 2 == dst_width && x < 0x8000) {
    ScaleCols = ScaleColsUp2_C;
#if defined(HAS_SCALECOLS_SSE2)
    if (TestCpuFlag(kCpuHasSSE2) && IS_ALIGNED(dst_width, 8)) {
      ScaleCols = ScaleColsUp2_SSE2;
    }
#endif
#if defined(HAS_SCALECOLS_MMI)
    if (TestCpuFlag(kCpuHasMMI) && IS_ALIGNED(dst_width, 8)) {
      ScaleCols = ScaleColsUp2_MMI;
    }
#endif
  }

  for (i = 0; i < dst_height; ++i) {
    ScaleCols(dst_ptr, src_ptr + (y >> 16) * (int64_t)src_stride, dst_width, x,
              dx);
    dst_ptr += dst_stride;
    y += dy;
  }
}

static void ScalePlaneSimple_16(int src_width,
                                int src_height,
                                int dst_width,
                                int dst_height,
                                int src_stride,
                                int dst_stride,
                                const uint16_t* src_ptr,
                                uint16_t* dst_ptr) {
  int i;
  void (*ScaleCols)(uint16_t * dst_ptr, const uint16_t* src_ptr, int dst_width,
                    int x, int dx) = ScaleCols_16_C;
  // Initial source x/y coordinate and step values as 16.16 fixed point.
  int x = 0;
  int y = 0;
  int dx = 0;
  int dy = 0;
  ScaleSlope(src_width, src_height, dst_width, dst_height, kFilterNone, &x, &y,
             &dx, &dy);
  src_width = Abs(src_width);

  if (src_width * 2 == dst_width && x < 0x8000) {
    ScaleCols = ScaleColsUp2_16_C;
#if defined(HAS_SCALECOLS_16_SSE2)
    if (TestCpuFlag(kCpuHasSSE2) && IS_ALIGNED(dst_width, 8)) {
      ScaleCols = ScaleColsUp2_16_SSE2;
    }
#endif
#if defined(HAS_SCALECOLS_16_MMI)
    if (TestCpuFlag(kCpuHasMMI) && IS_ALIGNED(dst_width, 8)) {
      ScaleCols = ScaleColsUp2_16_MMI;
    }
#endif
  }

  for (i = 0; i < dst_height; ++i) {
    ScaleCols(dst_ptr, src_ptr + (y >> 16) * (int64_t)src_stride, dst_width, x,
              dx);
    dst_ptr += dst_stride;
    y += dy;
  }
}

// Scale a plane.
// This function dispatches to a specialized scaler based on scale factor.

LIBYUV_API
void ScalePlane(const uint8_t* src,
                int src_stride,
                int src_width,
                int src_height,
                uint8_t* dst,
                int dst_stride,
                int dst_width,
                int dst_height,
                enum FilterMode filtering) {
  // Simplify filtering when possible.
  filtering = ScaleFilterReduce(src_width, src_height, dst_width, dst_height,
                                filtering);

  // Negative height means invert the image.
  if (src_height < 0) {
    src_height = -src_height;
    src = src + (src_height - 1) * (int64_t)src_stride;
    src_stride = -src_stride;
  }

  // Use specialized scales to improve performance for common resolutions.
  // For example, all the 1/2 scalings will use ScalePlaneDown2()
  if (dst_width == src_width && dst_height == src_height) {
    // Straight copy.
    CopyPlane(src, src_stride, dst, dst_stride, dst_width, dst_height);
    return;
  }
  if (dst_width == src_width && filtering != kFilterBox) {
    int dy = FixedDiv(src_height, dst_height);
    // Arbitrary scale vertically, but unscaled horizontally.
    ScalePlaneVertical(src_height, dst_width, dst_height, src_stride,
                       dst_stride, src, dst, 0, 0, dy, 1, filtering);
    return;
  }
  if (dst_width <= Abs(src_width) && dst_height <= src_height) {
    // Scale down.
    if (4 * dst_width == 3 * src_width && 4 * dst_height == 3 * src_height) {
      // optimized, 3/4
      ScalePlaneDown34(src_width, src_height, dst_width, dst_height, src_stride,
                       dst_stride, src, dst, filtering);
      return;
    }
    if (2 * dst_width == src_width && 2 * dst_height == src_height) {
      // optimized, 1/2
      ScalePlaneDown2(src_width, src_height, dst_width, dst_height, src_stride,
                      dst_stride, src, dst, filtering);
      return;
    }
    // 3/8 rounded up for odd sized chroma height.
    if (8 * dst_width == 3 * src_width && 8 * dst_height == 3 * src_height) {
      // optimized, 3/8
      ScalePlaneDown38(src_width, src_height, dst_width, dst_height, src_stride,
                       dst_stride, src, dst, filtering);
      return;
    }
    if (4 * dst_width == src_width && 4 * dst_height == src_height &&
        (filtering == kFilterBox || filtering == kFilterNone)) {
      // optimized, 1/4
      ScalePlaneDown4(src_width, src_height, dst_width, dst_height, src_stride,
                      dst_stride, src, dst, filtering);
      return;
    }
  }
  if (filtering == kFilterBox && dst_height * 2 < src_height) {
    ScalePlaneBox(src_width, src_height, dst_width, dst_height, src_stride,
                  dst_stride, src, dst);
    return;
  }
  if ((dst_width + 1) / 2 == src_width && filtering == kFilterLinear) {
    ScalePlaneUp2_Linear(src_width, src_height, dst_width, dst_height,
                         src_stride, dst_stride, src, dst);
    return;
  }
  if ((dst_height + 1) / 2 == src_height && (dst_width + 1) / 2 == src_width &&
      (filtering == kFilterBilinear || filtering == kFilterBox)) {
    ScalePlaneUp2_Bilinear(src_width, src_height, dst_width, dst_height,
                           src_stride, dst_stride, src, dst);
    return;
  }
  if (filtering && dst_height > src_height) {
    ScalePlaneBilinearUp(src_width, src_height, dst_width, dst_height,
                         src_stride, dst_stride, src, dst, filtering);
    return;
  }
  if (filtering) {
    ScalePlaneBilinearDown(src_width, src_height, dst_width, dst_height,
                           src_stride, dst_stride, src, dst, filtering);
    return;
  }
  ScalePlaneSimple(src_width, src_height, dst_width, dst_height, src_stride,
                   dst_stride, src, dst);
}

LIBYUV_API
void ScalePlane_16(const uint16_t* src,
                   int src_stride,
                   int src_width,
                   int src_height,
                   uint16_t* dst,
                   int dst_stride,
                   int dst_width,
                   int dst_height,
                   enum FilterMode filtering) {
  // Simplify filtering when possible.
  filtering = ScaleFilterReduce(src_width, src_height, dst_width, dst_height,
                                filtering);

  // Negative height means invert the image.
  if (src_height < 0) {
    src_height = -src_height;
    src = src + (src_height - 1) * (int64_t)src_stride;
    src_stride = -src_stride;
  }

  // Use specialized scales to improve performance for common resolutions.
  // For example, all the 1/2 scalings will use ScalePlaneDown2()
  if (dst_width == src_width && dst_height == src_height) {
    // Straight copy.
    CopyPlane_16(src, src_stride, dst, dst_stride, dst_width, dst_height);
    return;
  }
  if (dst_width == src_width && filtering != kFilterBox) {
    int dy = FixedDiv(src_height, dst_height);
    // Arbitrary scale vertically, but unscaled horizontally.
    ScalePlaneVertical_16(src_height, dst_width, dst_height, src_stride,
                          dst_stride, src, dst, 0, 0, dy, 1, filtering);
    return;
  }
  if (dst_width <= Abs(src_width) && dst_height <= src_height) {
    // Scale down.
    if (4 * dst_width == 3 * src_width && 4 * dst_height == 3 * src_height) {
      // optimized, 3/4
      ScalePlaneDown34_16(src_width, src_height, dst_width, dst_height,
                          src_stride, dst_stride, src, dst, filtering);
      return;
    }
    if (2 * dst_width == src_width && 2 * dst_height == src_height) {
      // optimized, 1/2
      ScalePlaneDown2_16(src_width, src_height, dst_width, dst_height,
                         src_stride, dst_stride, src, dst, filtering);
      return;
    }
    // 3/8 rounded up for odd sized chroma height.
    if (8 * dst_width == 3 * src_width && 8 * dst_height == 3 * src_height) {
      // optimized, 3/8
      ScalePlaneDown38_16(src_width, src_height, dst_width, dst_height,
                          src_stride, dst_stride, src, dst, filtering);
      return;
    }
    if (4 * dst_width == src_width && 4 * dst_height == src_height &&
        (filtering == kFilterBox || filtering == kFilterNone)) {
      // optimized, 1/4
      ScalePlaneDown4_16(src_width, src_height, dst_width, dst_height,
                         src_stride, dst_stride, src, dst, filtering);
      return;
    }
  }
  if (filtering == kFilterBox && dst_height * 2 < src_height) {
    ScalePlaneBox_16(src_width, src_height, dst_width, dst_height, src_stride,
                     dst_stride, src, dst);
    return;
  }
  if ((dst_width + 1) / 2 == src_width && filtering == kFilterLinear) {
    ScalePlaneUp2_16_Linear(src_width, src_height, dst_width, dst_height,
                            src_stride, dst_stride, src, dst);
    return;
  }
  if ((dst_height + 1) / 2 == src_height && (dst_width + 1) / 2 == src_width &&
      (filtering == kFilterBilinear || filtering == kFilterBox)) {
    ScalePlaneUp2_16_Bilinear(src_width, src_height, dst_width, dst_height,
                              src_stride, dst_stride, src, dst);
    return;
  }
  if (filtering && dst_height > src_height) {
    ScalePlaneBilinearUp_16(src_width, src_height, dst_width, dst_height,
                            src_stride, dst_stride, src, dst, filtering);
    return;
  }
  if (filtering) {
    ScalePlaneBilinearDown_16(src_width, src_height, dst_width, dst_height,
                              src_stride, dst_stride, src, dst, filtering);
    return;
  }
  ScalePlaneSimple_16(src_width, src_height, dst_width, dst_height, src_stride,
                      dst_stride, src, dst);
}

LIBYUV_API
void ScalePlane_12(const uint16_t* src,
                   int src_stride,
                   int src_width,
                   int src_height,
                   uint16_t* dst,
                   int dst_stride,
                   int dst_width,
                   int dst_height,
                   enum FilterMode filtering) {
  // Simplify filtering when possible.
  filtering = ScaleFilterReduce(src_width, src_height, dst_width, dst_height,
                                filtering);

  // Negative height means invert the image.
  if (src_height < 0) {
    src_height = -src_height;
    src = src + (src_height - 1) * (int64_t)src_stride;
    src_stride = -src_stride;
  }

  if ((dst_width + 1) / 2 == src_width && filtering == kFilterLinear) {
    ScalePlaneUp2_12_Linear(src_width, src_height, dst_width, dst_height,
                            src_stride, dst_stride, src, dst);
    return;
  }
  if ((dst_height + 1) / 2 == src_height && (dst_width + 1) / 2 == src_width &&
      (filtering == kFilterBilinear || filtering == kFilterBox)) {
    ScalePlaneUp2_12_Bilinear(src_width, src_height, dst_width, dst_height,
                              src_stride, dst_stride, src, dst);
    return;
  }

  ScalePlane_16(src, src_stride, src_width, src_height, dst, dst_stride,
                dst_width, dst_height, filtering);
}

// Scale an I420 image.
// This function in turn calls a scaling function for each plane.

LIBYUV_API
int I420Scale(const uint8_t* src_y,
              int src_stride_y,
              const uint8_t* src_u,
              int src_stride_u,
              const uint8_t* src_v,
              int src_stride_v,
              int src_width,
              int src_height,
              uint8_t* dst_y,
              int dst_stride_y,
              uint8_t* dst_u,
              int dst_stride_u,
              uint8_t* dst_v,
              int dst_stride_v,
              int dst_width,
              int dst_height,
              enum FilterMode filtering) {
  int src_halfwidth = SUBSAMPLE(src_width, 1, 1);
  int src_halfheight = SUBSAMPLE(src_height, 1, 1);
  int dst_halfwidth = SUBSAMPLE(dst_width, 1, 1);
  int dst_halfheight = SUBSAMPLE(dst_height, 1, 1);
  if (!src_y || !src_u || !src_v || src_width <= 0 || src_height == 0 ||
      src_width > 32768 || src_height > 32768 || !dst_y || !dst_u || !dst_v ||
      dst_width <= 0 || dst_height <= 0) {
    return -1;
  }

  ScalePlane(src_y, src_stride_y, src_width, src_height, dst_y, dst_stride_y,
             dst_width, dst_height, filtering);
  ScalePlane(src_u, src_stride_u, src_halfwidth, src_halfheight, dst_u,
             dst_stride_u, dst_halfwidth, dst_halfheight, filtering);
  ScalePlane(src_v, src_stride_v, src_halfwidth, src_halfheight, dst_v,
             dst_stride_v, dst_halfwidth, dst_halfheight, filtering);
  return 0;
}

LIBYUV_API
int I420Scale_16(const uint16_t* src_y,
                 int src_stride_y,
                 const uint16_t* src_u,
                 int src_stride_u,
                 const uint16_t* src_v,
                 int src_stride_v,
                 int src_width,
                 int src_height,
                 uint16_t* dst_y,
                 int dst_stride_y,
                 uint16_t* dst_u,
                 int dst_stride_u,
                 uint16_t* dst_v,
                 int dst_stride_v,
                 int dst_width,
                 int dst_height,
                 enum FilterMode filtering) {
  int src_halfwidth = SUBSAMPLE(src_width, 1, 1);
  int src_halfheight = SUBSAMPLE(src_height, 1, 1);
  int dst_halfwidth = SUBSAMPLE(dst_width, 1, 1);
  int dst_halfheight = SUBSAMPLE(dst_height, 1, 1);
  if (!src_y || !src_u || !src_v || src_width <= 0 || src_height == 0 ||
      src_width > 32768 || src_height > 32768 || !dst_y || !dst_u || !dst_v ||
      dst_width <= 0 || dst_height <= 0) {
    return -1;
  }

  ScalePlane_16(src_y, src_stride_y, src_width, src_height, dst_y, dst_stride_y,
                dst_width, dst_height, filtering);
  ScalePlane_16(src_u, src_stride_u, src_halfwidth, src_halfheight, dst_u,
                dst_stride_u, dst_halfwidth, dst_halfheight, filtering);
  ScalePlane_16(src_v, src_stride_v, src_halfwidth, src_halfheight, dst_v,
                dst_stride_v, dst_halfwidth, dst_halfheight, filtering);
  return 0;
}

LIBYUV_API
int I420Scale_12(const uint16_t* src_y,
                 int src_stride_y,
                 const uint16_t* src_u,
                 int src_stride_u,
                 const uint16_t* src_v,
                 int src_stride_v,
                 int src_width,
                 int src_height,
                 uint16_t* dst_y,
                 int dst_stride_y,
                 uint16_t* dst_u,
                 int dst_stride_u,
                 uint16_t* dst_v,
                 int dst_stride_v,
                 int dst_width,
                 int dst_height,
                 enum FilterMode filtering) {
  int src_halfwidth = SUBSAMPLE(src_width, 1, 1);
  int src_halfheight = SUBSAMPLE(src_height, 1, 1);
  int dst_halfwidth = SUBSAMPLE(dst_width, 1, 1);
  int dst_halfheight = SUBSAMPLE(dst_height, 1, 1);
  if (!src_y || !src_u || !src_v || src_width <= 0 || src_height == 0 ||
      src_width > 32768 || src_height > 32768 || !dst_y || !dst_u || !dst_v ||
      dst_width <= 0 || dst_height <= 0) {
    return -1;
  }

  ScalePlane_12(src_y, src_stride_y, src_width, src_height, dst_y, dst_stride_y,
                dst_width, dst_height, filtering);
  ScalePlane_12(src_u, src_stride_u, src_halfwidth, src_halfheight, dst_u,
                dst_stride_u, dst_halfwidth, dst_halfheight, filtering);
  ScalePlane_12(src_v, src_stride_v, src_halfwidth, src_halfheight, dst_v,
                dst_stride_v, dst_halfwidth, dst_halfheight, filtering);
  return 0;
}

// Scale an I444 image.
// This function in turn calls a scaling function for each plane.

LIBYUV_API
int I444Scale(const uint8_t* src_y,
              int src_stride_y,
              const uint8_t* src_u,
              int src_stride_u,
              const uint8_t* src_v,
              int src_stride_v,
              int src_width,
              int src_height,
              uint8_t* dst_y,
              int dst_stride_y,
              uint8_t* dst_u,
              int dst_stride_u,
              uint8_t* dst_v,
              int dst_stride_v,
              int dst_width,
              int dst_height,
              enum FilterMode filtering) {
  if (!src_y || !src_u || !src_v || src_width <= 0 || src_height == 0 ||
      src_width > 32768 || src_height > 32768 || !dst_y || !dst_u || !dst_v ||
      dst_width <= 0 || dst_height <= 0) {
    return -1;
  }

  ScalePlane(src_y, src_stride_y, src_width, src_height, dst_y, dst_stride_y,
             dst_width, dst_height, filtering);
  ScalePlane(src_u, src_stride_u, src_width, src_height, dst_u, dst_stride_u,
             dst_width, dst_height, filtering);
  ScalePlane(src_v, src_stride_v, src_width, src_height, dst_v, dst_stride_v,
             dst_width, dst_height, filtering);
  return 0;
}

LIBYUV_API
int I444Scale_16(const uint16_t* src_y,
                 int src_stride_y,
                 const uint16_t* src_u,
                 int src_stride_u,
                 const uint16_t* src_v,
                 int src_stride_v,
                 int src_width,
                 int src_height,
                 uint16_t* dst_y,
                 int dst_stride_y,
                 uint16_t* dst_u,
                 int dst_stride_u,
                 uint16_t* dst_v,
                 int dst_stride_v,
                 int dst_width,
                 int dst_height,
                 enum FilterMode filtering) {
  if (!src_y || !src_u || !src_v || src_width <= 0 || src_height == 0 ||
      src_width > 32768 || src_height > 32768 || !dst_y || !dst_u || !dst_v ||
      dst_width <= 0 || dst_height <= 0) {
    return -1;
  }

  ScalePlane_16(src_y, src_stride_y, src_width, src_height, dst_y, dst_stride_y,
                dst_width, dst_height, filtering);
  ScalePlane_16(src_u, src_stride_u, src_width, src_height, dst_u, dst_stride_u,
                dst_width, dst_height, filtering);
  ScalePlane_16(src_v, src_stride_v, src_width, src_height, dst_v, dst_stride_v,
                dst_width, dst_height, filtering);
  return 0;
}

LIBYUV_API
int I444Scale_12(const uint16_t* src_y,
                 int src_stride_y,
                 const uint16_t* src_u,
                 int src_stride_u,
                 const uint16_t* src_v,
                 int src_stride_v,
                 int src_width,
                 int src_height,
                 uint16_t* dst_y,
                 int dst_stride_y,
                 uint16_t* dst_u,
                 int dst_stride_u,
                 uint16_t* dst_v,
                 int dst_stride_v,
                 int dst_width,
                 int dst_height,
                 enum FilterMode filtering) {
  if (!src_y || !src_u || !src_v || src_width <= 0 || src_height == 0 ||
      src_width > 32768 || src_height > 32768 || !dst_y || !dst_u || !dst_v ||
      dst_width <= 0 || dst_height <= 0) {
    return -1;
  }

  ScalePlane_12(src_y, src_stride_y, src_width, src_height, dst_y, dst_stride_y,
                dst_width, dst_height, filtering);
  ScalePlane_12(src_u, src_stride_u, src_width, src_height, dst_u, dst_stride_u,
                dst_width, dst_height, filtering);
  ScalePlane_12(src_v, src_stride_v, src_width, src_height, dst_v, dst_stride_v,
                dst_width, dst_height, filtering);
  return 0;
}

// Scale an NV12 image.
// This function in turn calls a scaling function for each plane.

LIBYUV_API
int NV12Scale(const uint8_t* src_y,
              int src_stride_y,
              const uint8_t* src_uv,
              int src_stride_uv,
              int src_width,
              int src_height,
              uint8_t* dst_y,
              int dst_stride_y,
              uint8_t* dst_uv,
              int dst_stride_uv,
              int dst_width,
              int dst_height,
              enum FilterMode filtering) {
  int src_halfwidth = SUBSAMPLE(src_width, 1, 1);
  int src_halfheight = SUBSAMPLE(src_height, 1, 1);
  int dst_halfwidth = SUBSAMPLE(dst_width, 1, 1);
  int dst_halfheight = SUBSAMPLE(dst_height, 1, 1);
  if (!src_y || !src_uv || src_width <= 0 || src_height == 0 ||
      src_width > 32768 || src_height > 32768 || !dst_y || !dst_uv ||
      dst_width <= 0 || dst_height <= 0) {
    return -1;
  }

  ScalePlane(src_y, src_stride_y, src_width, src_height, dst_y, dst_stride_y,
             dst_width, dst_height, filtering);
  UVScale(src_uv, src_stride_uv, src_halfwidth, src_halfheight, dst_uv,
          dst_stride_uv, dst_halfwidth, dst_halfheight, filtering);
  return 0;
}

// Deprecated api
LIBYUV_API
int Scale(const uint8_t* src_y,
          const uint8_t* src_u,
          const uint8_t* src_v,
          int src_stride_y,
          int src_stride_u,
          int src_stride_v,
          int src_width,
          int src_height,
          uint8_t* dst_y,
          uint8_t* dst_u,
          uint8_t* dst_v,
          int dst_stride_y,
          int dst_stride_u,
          int dst_stride_v,
          int dst_width,
          int dst_height,
          LIBYUV_BOOL interpolate) {
  return I420Scale(src_y, src_stride_y, src_u, src_stride_u, src_v,
                   src_stride_v, src_width, src_height, dst_y, dst_stride_y,
                   dst_u, dst_stride_u, dst_v, dst_stride_v, dst_width,
                   dst_height, interpolate ? kFilterBox : kFilterNone);
}

#ifdef __cplusplus
}  // extern "C"
}  // namespace libyuv
#endif
