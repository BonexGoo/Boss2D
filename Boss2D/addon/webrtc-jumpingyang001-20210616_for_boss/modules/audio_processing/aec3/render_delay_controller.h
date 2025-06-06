/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_RENDER_DELAY_CONTROLLER_H_
#define MODULES_AUDIO_PROCESSING_AEC3_RENDER_DELAY_CONTROLLER_H_

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__delay_estimate_h //original-code:"modules/audio_processing/aec3/delay_estimate.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__downsampled_render_buffer_h //original-code:"modules/audio_processing/aec3/downsampled_render_buffer.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__render_delay_buffer_h //original-code:"modules/audio_processing/aec3/render_delay_buffer.h"
#include BOSS_WEBRTC_U_modules__audio_processing__logging__apm_data_dumper_h //original-code:"modules/audio_processing/logging/apm_data_dumper.h"

namespace webrtc {

// Class for aligning the render and capture signal using a RenderDelayBuffer.
class RenderDelayController {
 public:
  static RenderDelayController* Create(const EchoCanceller3Config& config,
                                       int sample_rate_hz,
                                       size_t num_capture_channels);
  virtual ~RenderDelayController() = default;

  // Resets the delay controller. If the delay confidence is reset, the reset
  // behavior is as if the call is restarted.
  virtual void Reset(bool reset_delay_confidence) = 0;

  // Logs a render call.
  virtual void LogRenderCall() = 0;

  // Aligns the render buffer content with the capture signal.
  virtual absl::optional<DelayEstimate> GetDelay(
      const DownsampledRenderBuffer& render_buffer,
      size_t render_delay_buffer_delay,
      const std::vector<std::vector<float>>& capture) = 0;

  // Returns true if clockdrift has been detected.
  virtual bool HasClockdrift() const = 0;
};
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_RENDER_DELAY_CONTROLLER_H_
