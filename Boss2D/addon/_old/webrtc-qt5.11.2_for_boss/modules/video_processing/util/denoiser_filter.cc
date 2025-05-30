/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/video_processing/util/denoiser_filter.h"
#include "modules/video_processing/util/denoiser_filter_c.h"
#include "modules/video_processing/util/denoiser_filter_neon.h"
#include "modules/video_processing/util/denoiser_filter_sse2.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_system_wrappers__include__cpu_features_wrapper_h //original-code:"system_wrappers/include/cpu_features_wrapper.h"

namespace webrtc {

const int kMotionMagnitudeThreshold = 8 * 3;
const int kSumDiffThreshold = 96;
const int kSumDiffThresholdHigh = 448;

std::unique_ptr<DenoiserFilter> DenoiserFilter::Create(
    bool runtime_cpu_detection,
    CpuType* cpu_type) {
  std::unique_ptr<DenoiserFilter> filter;

  if (cpu_type != nullptr)
    *cpu_type = CPU_NOT_NEON;
  if (runtime_cpu_detection) {
// If we know the minimum architecture at compile time, avoid CPU detection.
#if defined(WEBRTC_ARCH_X86_FAMILY)
#if defined(__SSE2__)
    filter.reset(new DenoiserFilterSSE2());
#else
    // x86 CPU detection required.
    if (WebRtc_GetCPUInfo(kSSE2)) {
      filter.reset(new DenoiserFilterSSE2());
    } else {
      filter.reset(new DenoiserFilterC());
    }
#endif
#elif defined(WEBRTC_HAS_NEON)
    filter.reset(new DenoiserFilterNEON());
    if (cpu_type != nullptr)
      *cpu_type = CPU_NEON;
#else
    filter.reset(new DenoiserFilterC());
#endif
  } else {
    filter.reset(new DenoiserFilterC());
  }

  RTC_DCHECK(filter.get() != nullptr);
  return filter;
}

}  // namespace webrtc
