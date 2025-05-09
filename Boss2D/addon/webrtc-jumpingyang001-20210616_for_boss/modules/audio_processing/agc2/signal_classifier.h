/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AGC2_SIGNAL_CLASSIFIER_H_
#define MODULES_AUDIO_PROCESSING_AGC2_SIGNAL_CLASSIFIER_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_common_audio__third_party__ooura__fft_size_128__ooura_fft_h //original-code:"common_audio/third_party/ooura/fft_size_128/ooura_fft.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__down_sampler_h //original-code:"modules/audio_processing/agc2/down_sampler.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__noise_spectrum_estimator_h //original-code:"modules/audio_processing/agc2/noise_spectrum_estimator.h"

namespace webrtc {

class ApmDataDumper;
class AudioBuffer;

class SignalClassifier {
 public:
  enum class SignalType { kNonStationary, kStationary };

  explicit SignalClassifier(ApmDataDumper* data_dumper);

  SignalClassifier() = delete;
  SignalClassifier(const SignalClassifier&) = delete;
  SignalClassifier& operator=(const SignalClassifier&) = delete;

  ~SignalClassifier();

  void Initialize(int sample_rate_hz);
  SignalType Analyze(rtc::ArrayView<const float> signal);

 private:
  class FrameExtender {
   public:
    FrameExtender(size_t frame_size, size_t extended_frame_size);

    FrameExtender() = delete;
    FrameExtender(const FrameExtender&) = delete;
    FrameExtender& operator=(const FrameExtender&) = delete;

    ~FrameExtender();

    void ExtendFrame(rtc::ArrayView<const float> x,
                     rtc::ArrayView<float> x_extended);

   private:
    std::vector<float> x_old_;
  };

  ApmDataDumper* const data_dumper_;
  DownSampler down_sampler_;
  std::unique_ptr<FrameExtender> frame_extender_;
  NoiseSpectrumEstimator noise_spectrum_estimator_;
  int sample_rate_hz_;
  int initialization_frames_left_;
  int consistent_classification_counter_;
  SignalType last_signal_type_;
  const OouraFft ooura_fft_;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AGC2_SIGNAL_CLASSIFIER_H_
