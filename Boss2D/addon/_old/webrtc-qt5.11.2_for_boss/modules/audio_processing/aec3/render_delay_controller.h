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

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__optional_h //original-code:"api/optional.h"
#include "modules/audio_processing/aec3/delay_estimate.h"
#include "modules/audio_processing/aec3/downsampled_render_buffer.h"
#include "modules/audio_processing/aec3/render_delay_buffer.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include "modules/audio_processing/logging/apm_data_dumper.h"

namespace webrtc {

// Class for aligning the render and capture signal using a RenderDelayBuffer.
class RenderDelayController {
 public:
  static RenderDelayController* Create(const EchoCanceller3Config& config,
                                       int non_causal_offset,
                                       int sample_rate_hz);
  virtual ~RenderDelayController() = default;

  // Resets the delay controller.
  virtual void Reset() = 0;

  // Logs a render call.
  virtual void LogRenderCall() = 0;

  // Aligns the render buffer content with the capture signal.
  virtual rtc::Optional<DelayEstimate> GetDelay(
      const DownsampledRenderBuffer& render_buffer,
      rtc::ArrayView<const float> capture) = 0;
};
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_RENDER_DELAY_CONTROLLER_H_
