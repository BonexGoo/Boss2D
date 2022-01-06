/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__vad_with_level_h //original-code:"modules/audio_processing/agc2/vad_with_level.h"

#include <algorithm>
#include <array>
#include <cmath>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_common_audio__include__audio_util_h //original-code:"common_audio/include/audio_util.h"
#include BOSS_WEBRTC_U_common_audio__resampler__include__push_resampler_h //original-code:"common_audio/resampler/include/push_resampler.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__agc2_common_h //original-code:"modules/audio_processing/agc2/agc2_common.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__common_h //original-code:"modules/audio_processing/agc2/rnn_vad/common.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__features_extraction_h //original-code:"modules/audio_processing/agc2/rnn_vad/features_extraction.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__rnn_h //original-code:"modules/audio_processing/agc2/rnn_vad/rnn.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {
namespace {

//removed by BOSS:using VoiceActivityDetector = VadLevelAnalyzer::VoiceActivityDetector;

// Default VAD that combines a resampler and the RNN VAD.
// Computes the speech probability on the first channel.
class Vad : public VadLevelAnalyzer::VoiceActivityDetector { //added by BOSS:VadLevelAnalyzer::
 public:
  explicit Vad(const AvailableCpuFeatures& cpu_features)
      : features_extractor_(cpu_features), rnn_vad_(cpu_features) {}
  Vad(const Vad&) = delete;
  Vad& operator=(const Vad&) = delete;
  ~Vad() = default;

  void Reset() override { rnn_vad_.Reset(); }

  float ComputeProbability(AudioFrameView<const float> frame) override {
    // The source number of channels is 1, because we always use the 1st
    // channel.
    resampler_.InitializeIfNeeded(
        /*sample_rate_hz=*/static_cast<int>(frame.samples_per_channel() * 100),
        rnn_vad::kSampleRate24kHz,
        /*num_channels=*/1);

    std::array<float, rnn_vad::kFrameSize10ms24kHz> work_frame;
    // Feed the 1st channel to the resampler.
    resampler_.Resample(frame.channel(0).data(), frame.samples_per_channel(),
                        work_frame.data(), rnn_vad::kFrameSize10ms24kHz);

    std::array<float, rnn_vad::kFeatureVectorSize> feature_vector;
    const bool is_silence = features_extractor_.CheckSilenceComputeFeatures(
        work_frame, feature_vector);
    return rnn_vad_.ComputeVadProbability(feature_vector, is_silence);
  }

 private:
  PushResampler<float> resampler_;
  rnn_vad::FeaturesExtractor features_extractor_;
  rnn_vad::RnnVad rnn_vad_;
};

}  // namespace

VadLevelAnalyzer::VadLevelAnalyzer(int vad_reset_period_ms,
                                   const AvailableCpuFeatures& cpu_features)
    : VadLevelAnalyzer(vad_reset_period_ms,
                       std::make_unique<Vad>(cpu_features)) {}

VadLevelAnalyzer::VadLevelAnalyzer(int vad_reset_period_ms,
                                   std::unique_ptr<VadLevelAnalyzer::VoiceActivityDetector> vad) //added by BOSS:VadLevelAnalyzer::
    : vad_(std::move(vad)),
      vad_reset_period_frames_(
          rtc::CheckedDivExact(vad_reset_period_ms, kFrameDurationMs)),
      time_to_vad_reset_(vad_reset_period_frames_) {
  RTC_DCHECK(vad_);
  RTC_DCHECK_GT(vad_reset_period_frames_, 1);
}

VadLevelAnalyzer::~VadLevelAnalyzer() = default;

VadLevelAnalyzer::Result VadLevelAnalyzer::AnalyzeFrame(
    AudioFrameView<const float> frame) {
  // Periodically reset the VAD.
  time_to_vad_reset_--;
  if (time_to_vad_reset_ <= 0) {
    vad_->Reset();
    time_to_vad_reset_ = vad_reset_period_frames_;
  }
  // Compute levels.
  float peak = 0.0f;
  float rms = 0.0f;
  for (const auto& x : frame.channel(0)) {
    peak = std::max(std::fabs(x), peak);
    rms += x * x;
  }
  return {vad_->ComputeProbability(frame),
          FloatS16ToDbfs(std::sqrt(rms / frame.samples_per_channel())),
          FloatS16ToDbfs(peak)};
}

}  // namespace webrtc
