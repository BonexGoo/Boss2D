/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_ADAPTIVE_FIR_FILTER_ERL_H_
#define MODULES_AUDIO_PROCESSING_AEC3_ADAPTIVE_FIR_FILTER_ERL_H_

#include <stddef.h>

#include <array>
#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__aec3_common_h //original-code:"modules/audio_processing/aec3/aec3_common.h"
#include BOSS_WEBRTC_U_rtc_base__system__arch_h //original-code:"rtc_base/system/arch.h"

namespace webrtc {
namespace aec3 {

// Computes and stores the echo return loss estimate of the filter, which is the
// sum of the partition frequency responses.
void ErlComputer(const std::vector<std::array<float, kFftLengthBy2Plus1>>& H2,
                 rtc::ArrayView<float> erl);
#if defined(WEBRTC_HAS_NEON)
void ErlComputer_NEON(
    const std::vector<std::array<float, kFftLengthBy2Plus1>>& H2,
    rtc::ArrayView<float> erl);
#endif
#if defined(WEBRTC_ARCH_X86_FAMILY)
void ErlComputer_SSE2(
    const std::vector<std::array<float, kFftLengthBy2Plus1>>& H2,
    rtc::ArrayView<float> erl);

void ErlComputer_AVX2(
    const std::vector<std::array<float, kFftLengthBy2Plus1>>& H2,
    rtc::ArrayView<float> erl);
#endif

}  // namespace aec3

// Computes the echo return loss based on a frequency response.
void ComputeErl(const Aec3Optimization& optimization,
                const std::vector<std::array<float, kFftLengthBy2Plus1>>& H2,
                rtc::ArrayView<float> erl);

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_ADAPTIVE_FIR_FILTER_ERL_H_
