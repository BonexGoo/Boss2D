/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/audio_processing/aec3/reverb_model.h"

#include <math.h>

#include <algorithm>
#include <functional>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include "modules/audio_processing/aec3/aec3_common.h"

namespace webrtc {

ReverbModel::ReverbModel() {
  Reset();
}

ReverbModel::~ReverbModel() = default;

void ReverbModel::Reset() {
  reverb_.fill(0.);
}

void ReverbModel::AddReverbNoFreqShaping(
    rtc::ArrayView<const float> power_spectrum,
    float power_spectrum_scaling,
    float reverb_decay,
    rtc::ArrayView<float> reverb_power_spectrum) {
  UpdateReverbContributionsNoFreqShaping(power_spectrum, power_spectrum_scaling,
                                         reverb_decay);

  // Add the power of the echo reverb to the residual echo power.
  std::transform(reverb_power_spectrum.begin(), reverb_power_spectrum.end(),
                 reverb_.begin(), reverb_power_spectrum.begin(),
                 std::plus<float>());
}

void ReverbModel::AddReverb(rtc::ArrayView<const float> power_spectrum,
                            rtc::ArrayView<const float> power_spectrum_scaling,
                            float reverb_decay,
                            rtc::ArrayView<float> reverb_power_spectrum) {
  UpdateReverbContributions(power_spectrum, power_spectrum_scaling,
                            reverb_decay);

  // Add the power of the echo reverb to the residual echo power.
  std::transform(reverb_power_spectrum.begin(), reverb_power_spectrum.end(),
                 reverb_.begin(), reverb_power_spectrum.begin(),
                 std::plus<float>());
}

void ReverbModel::UpdateReverbContributionsNoFreqShaping(
    rtc::ArrayView<const float> power_spectrum,
    float power_spectrum_scaling,
    float reverb_decay) {
  if (reverb_decay > 0) {
    // Update the estimate of the reverberant power.
    for (size_t k = 0; k < power_spectrum.size(); ++k) {
      reverb_[k] = (reverb_[k] + power_spectrum[k] * power_spectrum_scaling) *
                   reverb_decay;
    }
  }
}

void ReverbModel::UpdateReverbContributions(
    rtc::ArrayView<const float>& power_spectrum,
    rtc::ArrayView<const float>& power_spectrum_scaling,
    float reverb_decay) {
  if (reverb_decay > 0) {
    // Update the estimate of the reverberant power.
    for (size_t k = 0; k < power_spectrum.size(); ++k) {
      reverb_[k] =
          (reverb_[k] + power_spectrum[k] * power_spectrum_scaling[k]) *
          reverb_decay;
    }
  }
}

}  // namespace webrtc
