/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__aec3__reverb_model_estimator_h //original-code:"modules/audio_processing/aec3/reverb_model_estimator.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <numeric>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__aec3_common_h //original-code:"modules/audio_processing/aec3/aec3_common.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__aec3_fft_h //original-code:"modules/audio_processing/aec3/aec3_fft.h"
#include BOSS_WEBRTC_U_modules__audio_processing__aec3__fft_data_h //original-code:"modules/audio_processing/aec3/fft_data.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

namespace {

EchoCanceller3Config CreateConfigForTest(float default_decay) {
  EchoCanceller3Config cfg;
  cfg.ep_strength.default_len = default_decay;
  cfg.filter.refined.length_blocks = 40;
  return cfg;
}

constexpr int kFilterDelayBlocks = 2;

}  // namespace

class ReverbModelEstimatorTest {
 public:
  ReverbModelEstimatorTest(float default_decay, size_t num_capture_channels)
      : aec3_config_(CreateConfigForTest(default_decay)),
        estimated_decay_(default_decay),
        h_(num_capture_channels,
           std::vector<float>(
               aec3_config_.filter.refined.length_blocks * kBlockSize,
               0.f)),
        H2_(num_capture_channels,
            std::vector<std::array<float, kFftLengthBy2Plus1>>(
                aec3_config_.filter.refined.length_blocks)),
        quality_linear_(num_capture_channels, 1.0f) {
    CreateImpulseResponseWithDecay();
  }
  void RunEstimator();
  float GetDecay() { return estimated_decay_; }
  float GetTrueDecay() { return kTruePowerDecay; }
  float GetPowerTailDb() { return 10.f * std::log10(estimated_power_tail_); }
  float GetTruePowerTailDb() { return 10.f * std::log10(true_power_tail_); }

 private:
  void CreateImpulseResponseWithDecay();
  static constexpr bool kStationaryBlock = false;
  static constexpr float kTruePowerDecay = 0.5f;
  const EchoCanceller3Config aec3_config_;
  float estimated_decay_;
  float estimated_power_tail_ = 0.f;
  float true_power_tail_ = 0.f;
  std::vector<std::vector<float>> h_;
  std::vector<std::vector<std::array<float, kFftLengthBy2Plus1>>> H2_;
  std::vector<absl::optional<float>> quality_linear_;
};

void ReverbModelEstimatorTest::CreateImpulseResponseWithDecay() {
  const Aec3Fft fft;
  for (const auto& h_k : h_) {
    RTC_DCHECK_EQ(h_k.size(),
                  aec3_config_.filter.refined.length_blocks * kBlockSize);
  }
  for (const auto& H2_k : H2_) {
    RTC_DCHECK_EQ(H2_k.size(), aec3_config_.filter.refined.length_blocks);
  }
  RTC_DCHECK_EQ(kFilterDelayBlocks, 2);

  float decay_sample = std::sqrt(powf(kTruePowerDecay, 1.f / kBlockSize));
  const size_t filter_delay_coefficients = kFilterDelayBlocks * kBlockSize;
  for (auto& h_i : h_) {
    std::fill(h_i.begin(), h_i.end(), 0.f);
    h_i[filter_delay_coefficients] = 1.f;
    for (size_t k = filter_delay_coefficients + 1; k < h_i.size(); ++k) {
      h_i[k] = h_i[k - 1] * decay_sample;
    }
  }

  for (size_t ch = 0; ch < H2_.size(); ++ch) {
    for (size_t j = 0, k = 0; j < H2_[ch].size(); ++j, k += kBlockSize) {
      std::array<float, kFftLength> fft_data;
      fft_data.fill(0.f);
      std::copy(h_[ch].begin() + k, h_[ch].begin() + k + kBlockSize,
                fft_data.begin());
      FftData H_j;
      fft.Fft(&fft_data, &H_j);
      H_j.Spectrum(Aec3Optimization::kNone, H2_[ch][j]);
    }
  }
  rtc::ArrayView<float> H2_tail(H2_[0][H2_[0].size() - 1]);
  true_power_tail_ = std::accumulate(H2_tail.begin(), H2_tail.end(), 0.f);
}
void ReverbModelEstimatorTest::RunEstimator() {
  const size_t num_capture_channels = H2_.size();
  constexpr bool kUsableLinearEstimate = true;
  ReverbModelEstimator estimator(aec3_config_, num_capture_channels);
  std::vector<bool> usable_linear_estimates(num_capture_channels,
                                            kUsableLinearEstimate);
  std::vector<int> filter_delay_blocks(num_capture_channels,
                                       kFilterDelayBlocks);
  for (size_t k = 0; k < 3000; ++k) {
    estimator.Update(h_, H2_, quality_linear_, filter_delay_blocks,
                     usable_linear_estimates, kStationaryBlock);
  }
  estimated_decay_ = estimator.ReverbDecay();
  auto freq_resp_tail = estimator.GetReverbFrequencyResponse();
  estimated_power_tail_ =
      std::accumulate(freq_resp_tail.begin(), freq_resp_tail.end(), 0.f);
}

TEST(ReverbModelEstimatorTests, NotChangingDecay) {
  constexpr float kDefaultDecay = 0.9f;
  for (size_t num_capture_channels : {1, 2, 4, 8}) {
    ReverbModelEstimatorTest test(kDefaultDecay, num_capture_channels);
    test.RunEstimator();
    EXPECT_EQ(test.GetDecay(), kDefaultDecay);
    EXPECT_NEAR(test.GetPowerTailDb(), test.GetTruePowerTailDb(), 5.f);
  }
}

TEST(ReverbModelEstimatorTests, ChangingDecay) {
  constexpr float kDefaultDecay = -0.9f;
  for (size_t num_capture_channels : {1, 2, 4, 8}) {
    ReverbModelEstimatorTest test(kDefaultDecay, num_capture_channels);
    test.RunEstimator();
    EXPECT_NEAR(test.GetDecay(), test.GetTrueDecay(), 0.1);
    EXPECT_NEAR(test.GetPowerTailDb(), test.GetTruePowerTailDb(), 5.f);
  }
}

}  // namespace webrtc
