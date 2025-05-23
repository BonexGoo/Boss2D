/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AEC3_ECHO_CANCELLER3_H_
#define MODULES_AUDIO_PROCESSING_AEC3_ECHO_CANCELLER3_H_

#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"
#include "modules/audio_processing/aec3/block_framer.h"
#include "modules/audio_processing/aec3/block_processor.h"
#include "modules/audio_processing/aec3/cascaded_biquad_filter.h"
#include "modules/audio_processing/aec3/frame_blocker.h"
#include BOSS_WEBRTC_U_modules__audio_processing__audio_buffer_h //original-code:"modules/audio_processing/audio_buffer.h"
#include BOSS_WEBRTC_U_modules__audio_processing__logging__apm_data_dumper_h //original-code:"modules/audio_processing/logging/apm_data_dumper.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"
#include BOSS_WEBRTC_U_rtc_base__race_checker_h //original-code:"rtc_base/race_checker.h"
#include BOSS_WEBRTC_U_rtc_base__swap_queue_h //original-code:"rtc_base/swap_queue.h"

namespace webrtc {

// Functor for verifying the invariance of the frames being put into the render
// queue.
class Aec3RenderQueueItemVerifier {
 public:
  explicit Aec3RenderQueueItemVerifier(size_t num_bands, size_t frame_length)
      : num_bands_(num_bands), frame_length_(frame_length) {}

  bool operator()(const std::vector<std::vector<float>>& v) const {
    if (v.size() != num_bands_) {
      return false;
    }
    for (const auto& v_k : v) {
      if (v_k.size() != frame_length_) {
        return false;
      }
    }
    return true;
  }

 private:
  const size_t num_bands_;
  const size_t frame_length_;
};

// Main class for the echo canceller3.
// It does 4 things:
// -Receives 10 ms frames of band-split audio.
// -Optionally applies an anti-hum (high-pass) filter on the
// received signals.
// -Provides the lower level echo canceller functionality with
// blocks of 64 samples of audio data.
// -Partially handles the jitter in the render and capture API
// call sequence.
//
// The class is supposed to be used in a non-concurrent manner apart from the
// AnalyzeRender call which can be called concurrently with the other methods.
class EchoCanceller3 : public EchoControl {
 public:
  // Normal c-tor to use.
  EchoCanceller3(const EchoCanceller3Config& config,
                 int sample_rate_hz,
                 bool use_highpass_filter);
  // Testing c-tor that is used only for testing purposes.
  EchoCanceller3(const EchoCanceller3Config& config,
                 int sample_rate_hz,
                 bool use_highpass_filter,
                 std::unique_ptr<BlockProcessor> block_processor);
  ~EchoCanceller3() override;
  // Analyzes and stores an internal copy of the split-band domain render
  // signal.
  void AnalyzeRender(AudioBuffer* farend) override;
  // Analyzes the full-band domain capture signal to detect signal saturation.
  void AnalyzeCapture(AudioBuffer* capture) override;
  // Processes the split-band domain capture signal in order to remove any echo
  // present in the signal.
  void ProcessCapture(AudioBuffer* capture, bool level_change) override;
  // Collect current metrics from the echo canceller.
  Metrics GetMetrics() const override;
  // Provides an optional external estimate of the audio buffer delay.
  void SetAudioBufferDelay(size_t delay_ms) override;

  // Signals whether an external detector has detected echo leakage from the
  // echo canceller.
  // Note that in the case echo leakage has been flagged, it should be unflagged
  // once it is no longer occurring.
  void UpdateEchoLeakageStatus(bool leakage_detected) {
    RTC_DCHECK_RUNS_SERIALIZED(&capture_race_checker_);
    block_processor_->UpdateEchoLeakageStatus(leakage_detected);
  }

 private:
  class RenderWriter;

  // Empties the render SwapQueue.
  void EmptyRenderQueue();

  rtc::RaceChecker capture_race_checker_;
  rtc::RaceChecker render_race_checker_;

  // State that is accessed by the AnalyzeRender call.
  std::unique_ptr<RenderWriter> render_writer_
      RTC_GUARDED_BY(render_race_checker_);

  // State that may be accessed by the capture thread.
  static int instance_count_;
  std::unique_ptr<ApmDataDumper> data_dumper_;
  const int sample_rate_hz_;
  const int num_bands_;
  const size_t frame_length_;
  BlockFramer output_framer_ RTC_GUARDED_BY(capture_race_checker_);
  FrameBlocker capture_blocker_ RTC_GUARDED_BY(capture_race_checker_);
  FrameBlocker render_blocker_ RTC_GUARDED_BY(capture_race_checker_);
  SwapQueue<std::vector<std::vector<float>>, Aec3RenderQueueItemVerifier>
      render_transfer_queue_;
  std::unique_ptr<BlockProcessor> block_processor_
      RTC_GUARDED_BY(capture_race_checker_);
  std::vector<std::vector<float>> render_queue_output_frame_
      RTC_GUARDED_BY(capture_race_checker_);
  std::unique_ptr<CascadedBiQuadFilter> capture_highpass_filter_
      RTC_GUARDED_BY(capture_race_checker_);
  bool saturated_microphone_signal_ RTC_GUARDED_BY(capture_race_checker_) =
      false;
  std::vector<std::vector<float>> block_ RTC_GUARDED_BY(capture_race_checker_);
  std::vector<rtc::ArrayView<float>> sub_frame_view_
      RTC_GUARDED_BY(capture_race_checker_);

  RTC_DISALLOW_IMPLICIT_CONSTRUCTORS(EchoCanceller3);
};
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AEC3_ECHO_CANCELLER3_H_
