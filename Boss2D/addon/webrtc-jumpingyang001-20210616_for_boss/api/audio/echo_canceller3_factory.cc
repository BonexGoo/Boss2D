/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_api__audio__echo_canceller3_factory_h //original-code:"api/audio/echo_canceller3_factory.h"

#include <memory>

#include BOSS_WEBRTC_U_modules__audio_processing__aec3__echo_canceller3_h //original-code:"modules/audio_processing/aec3/echo_canceller3.h"

namespace webrtc {

EchoCanceller3Factory::EchoCanceller3Factory() {}

EchoCanceller3Factory::EchoCanceller3Factory(const EchoCanceller3Config& config)
    : config_(config) {}

std::unique_ptr<EchoControl> EchoCanceller3Factory::Create(
    int sample_rate_hz,
    int num_render_channels,
    int num_capture_channels) {
  return std::make_unique<EchoCanceller3>(
      config_, sample_rate_hz, num_render_channels, num_capture_channels);
}

}  // namespace webrtc
