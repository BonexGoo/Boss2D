/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/audio_processing/aec3/aec3_common.h"

#include BOSS_WEBRTC_U_typedefs_h //original-code:"typedefs.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_system_wrappers__include__cpu_features_wrapper_h //original-code:"system_wrappers/include/cpu_features_wrapper.h"

namespace webrtc {

Aec3Optimization DetectOptimization() {
#if defined(WEBRTC_ARCH_X86_FAMILY)
  if (WebRtc_GetCPUInfo(kSSE2) != 0) {
    return Aec3Optimization::kSse2;
  }
#endif

#if defined(WEBRTC_HAS_NEON)
  return Aec3Optimization::kNeon;
#endif

  return Aec3Optimization::kNone;
}

}  // namespace webrtc
