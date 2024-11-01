/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_DECIMATOR_H_
#define MODULES_AUDIO_PROCESSING_AEC3_DECIMATOR_H_

#include <array>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include "modules/audio_processing/aec3/aec3_common.h"
#include "modules/audio_processing/aec3/cascaded_biquad_filter.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

// Provides functionality for decimating a signal.
class Decimator {
 public:
  explicit Decimator(size_t down_sampling_factor);

  // Downsamples the signal.
  void Decimate(rtc::ArrayView<const float> in, rtc::ArrayView<float> out);

 private:
  const size_t down_sampling_factor_;
  CascadedBiQuadFilter low_pass_filter_;

  RTC_DISALLOW_COPY_AND_ASSIGN(Decimator);
};
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_DECIMATOR_H_
