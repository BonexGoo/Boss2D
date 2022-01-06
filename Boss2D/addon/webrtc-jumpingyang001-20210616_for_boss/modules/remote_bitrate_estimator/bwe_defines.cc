/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__include__bwe_defines_h //original-code:"modules/remote_bitrate_estimator/include/bwe_defines.h"

#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"

namespace webrtc {

const char kBweTypeHistogram[] = "WebRTC.BWE.Types";

namespace congestion_controller {
int GetMinBitrateBps() {
  constexpr int kMinBitrateBps = 5000;
  return kMinBitrateBps;
}

DataRate GetMinBitrate() {
  return DataRate::BitsPerSec(GetMinBitrateBps());
}

}  // namespace congestion_controller

RateControlInput::RateControlInput(
    BandwidthUsage bw_state,
    const absl::optional<DataRate>& estimated_throughput)
    : bw_state(bw_state), estimated_throughput(estimated_throughput) {}

RateControlInput::~RateControlInput() = default;

}  // namespace webrtc
