/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_NEAREND_DETECTOR_H_
#define MODULES_AUDIO_PROCESSING_AEC3_NEAREND_DETECTOR_H_

#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__aec3_common_h //original-code:"modules/audio_processing/aec3/aec3_common.h"

namespace webrtc {
// Class for selecting whether the suppressor is in the nearend or echo state.
class NearendDetector {
 public:
  virtual ~NearendDetector() {}

  // Returns whether the current state is the nearend state.
  virtual bool IsNearendState() const = 0;

  // Updates the state selection based on latest spectral estimates.
  virtual void Update(
      rtc::ArrayView<const std::array<float, kFftLengthBy2Plus1>>
          nearend_spectrum,
      rtc::ArrayView<const std::array<float, kFftLengthBy2Plus1>>
          residual_echo_spectrum,
      rtc::ArrayView<const std::array<float, kFftLengthBy2Plus1>>
          comfort_noise_spectrum,
      bool initial_state) = 0;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_NEAREND_DETECTOR_H_
