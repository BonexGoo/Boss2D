/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AGC2_ADAPTIVE_DIGITAL_GAIN_APPLIER_H_
#define MODULES_AUDIO_PROCESSING_AGC2_ADAPTIVE_DIGITAL_GAIN_APPLIER_H_

#include <vector>

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__gain_applier_h //original-code:"modules/audio_processing/agc2/gain_applier.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_frame_view_h //original-code:"modules/audio_processing/include/audio_frame_view.h"

namespace webrtc {

class ApmDataDumper;

// TODO(bugs.webrtc.org): Split into `GainAdaptor` and `GainApplier`.
// Selects the target digital gain, decides when and how quickly to adapt to the
// target and applies the current gain to 10 ms frames.
class AdaptiveDigitalGainApplier {
 public:
  // Information about a frame to process.
  struct FrameInfo {
    float speech_probability;     // Probability of speech in the [0, 1] range.
    float speech_level_dbfs;      // Estimated speech level (dBFS).
    bool speech_level_reliable;   // True with reliable speech level estimation.
    float noise_rms_dbfs;         // Estimated noise RMS level (dBFS).
    float headroom_db;            // Headroom (dB).
    float limiter_envelope_dbfs;  // Envelope level from the limiter (dBFS).
  };

  // Ctor. `adjacent_speech_frames_threshold` indicates how many adjacent speech
  // frames must be observed in order to consider the sequence as speech.
  // `max_gain_change_db_per_second` limits the adaptation speed (uniformly
  // operated across frames). `max_output_noise_level_dbfs` limits the output
  // noise level. If `dry_run` is true, `Process()` will not modify the audio.
  AdaptiveDigitalGainApplier(ApmDataDumper* apm_data_dumper,
                             int adjacent_speech_frames_threshold,
                             float max_gain_change_db_per_second,
                             float max_output_noise_level_dbfs,
                             bool dry_run);
  AdaptiveDigitalGainApplier(const AdaptiveDigitalGainApplier&) = delete;
  AdaptiveDigitalGainApplier& operator=(const AdaptiveDigitalGainApplier&) =
      delete;

  void Initialize(int sample_rate_hz, int num_channels);

  // Analyzes `info`, updates the digital gain and applies it to a 10 ms
  // `frame`. Supports any sample rate supported by APM.
  void Process(const FrameInfo& info, AudioFrameView<float> frame);

 private:
  ApmDataDumper* const apm_data_dumper_;
  GainApplier gain_applier_;

  const int adjacent_speech_frames_threshold_;
  const float max_gain_change_db_per_10ms_;
  const float max_output_noise_level_dbfs_;
  const bool dry_run_;

  int calls_since_last_gain_log_;
  int frames_to_gain_increase_allowed_;
  float last_gain_db_;

  std::vector<std::vector<float>> dry_run_frame_;
  std::vector<float*> dry_run_channels_;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AGC2_ADAPTIVE_DIGITAL_GAIN_APPLIER_H_
