/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_BLOCK_PROCESSOR_H_
#define MODULES_AUDIO_PROCESSING_AEC3_BLOCK_PROCESSOR_H_

#include <stddef.h>

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"
#include BOSS_WEBRTC_U_api__audio__echo_control_h //original-code:"api/audio/echo_control.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__echo_remover_h //original-code:"modules/audio_processing/aec3/echo_remover.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__render_delay_buffer_h //original-code:"modules/audio_processing/aec3/render_delay_buffer.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__render_delay_controller_h //original-code:"modules/audio_processing/aec3/render_delay_controller.h"

namespace webrtc {

// Class for performing echo cancellation on 64 sample blocks of audio data.
class BlockProcessor {
 public:
  static BlockProcessor* Create(const EchoCanceller3Config& config,
                                int sample_rate_hz,
                                size_t num_render_channels,
                                size_t num_capture_channels);
  // Only used for testing purposes.
  static BlockProcessor* Create(
      const EchoCanceller3Config& config,
      int sample_rate_hz,
      size_t num_render_channels,
      size_t num_capture_channels,
      std::unique_ptr<RenderDelayBuffer> render_buffer);
  static BlockProcessor* Create(
      const EchoCanceller3Config& config,
      int sample_rate_hz,
      size_t num_render_channels,
      size_t num_capture_channels,
      std::unique_ptr<RenderDelayBuffer> render_buffer,
      std::unique_ptr<RenderDelayController> delay_controller,
      std::unique_ptr<EchoRemover> echo_remover);

  virtual ~BlockProcessor() = default;

  // Get current metrics.
  virtual void GetMetrics(EchoControl::Metrics* metrics) const = 0;

  // Provides an optional external estimate of the audio buffer delay.
  virtual void SetAudioBufferDelay(int delay_ms) = 0;

  // Processes a block of capture data.
  virtual void ProcessCapture(
      bool echo_path_gain_change,
      bool capture_signal_saturation,
      std::vector<std::vector<std::vector<float>>>* linear_output,
      std::vector<std::vector<std::vector<float>>>* capture_block) = 0;

  // Buffers a block of render data supplied by a FrameBlocker object.
  virtual void BufferRender(
      const std::vector<std::vector<std::vector<float>>>& render_block) = 0;

  // Reports whether echo leakage has been detected in the echo canceller
  // output.
  virtual void UpdateEchoLeakageStatus(bool leakage_detected) = 0;

  // Specifies whether the capture output will be used. The purpose of this is
  // to allow the block processor to deactivate some of the processing when the
  // resulting output is anyway not used, for instance when the endpoint is
  // muted.
  virtual void SetCaptureOutputUsage(bool capture_output_used) = 0;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_BLOCK_PROCESSOR_H_
