/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__noise_level_estimator_h //original-code:"modules/audio_processing/agc2/noise_level_estimator.h"

#include <array>
#include <cmath>
#include <functional>
#include <limits>

#include BOSS_WEBRTC_U_api__function_view_h //original-code:"api/function_view.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__agc2_testing_common_h //original-code:"modules/audio_processing/agc2/agc2_testing_common.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__vector_float_frame_h //original-code:"modules/audio_processing/agc2/vector_float_frame.h"
#include BOSS_WEBRTC_U_modules__audio_processing__logging__apm_data_dumper_h //original-code:"modules/audio_processing/logging/apm_data_dumper.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"

namespace webrtc {
namespace {

constexpr int kNumIterations = 200;
constexpr int kFramesPerSecond = 100;

// Runs the noise estimator on audio generated by 'sample_generator'
// for kNumIterations. Returns the last noise level estimate.
float RunEstimator(rtc::FunctionView<float()> sample_generator,
                   NoiseLevelEstimator& estimator,
                   int sample_rate_hz) {
  const int samples_per_channel =
      rtc::CheckedDivExact(sample_rate_hz, kFramesPerSecond);
  VectorFloatFrame signal(1, samples_per_channel, 0.0f);
  for (int i = 0; i < kNumIterations; ++i) {
    AudioFrameView<float> frame_view = signal.float_frame_view();
    for (int j = 0; j < samples_per_channel; ++j) {
      frame_view.channel(0)[j] = sample_generator();
    }
    estimator.Analyze(frame_view);
  }
  return estimator.Analyze(signal.float_frame_view());
}

class NoiseEstimatorParametrization : public ::testing::TestWithParam<int> {
 protected:
  int sample_rate_hz() const { return GetParam(); }
};

// White random noise is stationary, but does not trigger the detector
// every frame due to the randomness.
TEST_P(NoiseEstimatorParametrization, StationaryNoiseEstimatorWithRandomNoise) {
  ApmDataDumper data_dumper(0);
  auto estimator = CreateStationaryNoiseEstimator(&data_dumper);

  test::WhiteNoiseGenerator gen(/*min_amplitude=*/test::kMinS16,
                                /*max_amplitude=*/test::kMaxS16);
  const float noise_level_dbfs =
      RunEstimator(gen, *estimator, sample_rate_hz());
  EXPECT_NEAR(noise_level_dbfs, -5.5f, 1.0f);
}

// Sine curves are (very) stationary. They trigger the detector all
// the time. Except for a few initial frames.
TEST_P(NoiseEstimatorParametrization, StationaryNoiseEstimatorWithSineTone) {
  ApmDataDumper data_dumper(0);
  auto estimator = CreateStationaryNoiseEstimator(&data_dumper);

  test::SineGenerator gen(/*amplitude=*/test::kMaxS16, /*frequency_hz=*/600.0f,
                          sample_rate_hz());
  const float noise_level_dbfs =
      RunEstimator(gen, *estimator, sample_rate_hz());
  EXPECT_NEAR(noise_level_dbfs, -3.0f, 1.0f);
}

// Pulses are transient if they are far enough apart. They shouldn't
// trigger the noise detector.
TEST_P(NoiseEstimatorParametrization, StationaryNoiseEstimatorWithPulseTone) {
  ApmDataDumper data_dumper(0);
  auto estimator = CreateStationaryNoiseEstimator(&data_dumper);

  test::PulseGenerator gen(/*pulse_amplitude=*/test::kMaxS16,
                           /*no_pulse_amplitude=*/10.0f, /*frequency_hz=*/20.0f,
                           sample_rate_hz());
  const int noise_level_dbfs = RunEstimator(gen, *estimator, sample_rate_hz());
  EXPECT_NEAR(noise_level_dbfs, -79.0f, 1.0f);
}

// Checks that full scale white noise maps to about -5.5 dBFS.
TEST_P(NoiseEstimatorParametrization, NoiseFloorEstimatorWithRandomNoise) {
  ApmDataDumper data_dumper(0);
  auto estimator = CreateNoiseFloorEstimator(&data_dumper);

  test::WhiteNoiseGenerator gen(/*min_amplitude=*/test::kMinS16,
                                /*max_amplitude=*/test::kMaxS16);
  const float noise_level_dbfs =
      RunEstimator(gen, *estimator, sample_rate_hz());
  EXPECT_NEAR(noise_level_dbfs, -5.5f, 0.5f);
}

// Checks that a full scale sine wave maps to about -3 dBFS.
TEST_P(NoiseEstimatorParametrization, NoiseFloorEstimatorWithSineTone) {
  ApmDataDumper data_dumper(0);
  auto estimator = CreateNoiseFloorEstimator(&data_dumper);

  test::SineGenerator gen(/*amplitude=*/test::kMaxS16, /*frequency_hz=*/600.0f,
                          sample_rate_hz());
  const float noise_level_dbfs =
      RunEstimator(gen, *estimator, sample_rate_hz());
  EXPECT_NEAR(noise_level_dbfs, -3.0f, 0.1f);
}

// Check that sufficiently spaced periodic pulses do not raise the estimated
// noise floor, which is determined by the amplitude of the non-pulse samples.
TEST_P(NoiseEstimatorParametrization, NoiseFloorEstimatorWithPulseTone) {
  ApmDataDumper data_dumper(0);
  auto estimator = CreateNoiseFloorEstimator(&data_dumper);

  constexpr float kNoPulseAmplitude = 10.0f;
  test::PulseGenerator gen(/*pulse_amplitude=*/test::kMaxS16, kNoPulseAmplitude,
                           /*frequency_hz=*/20.0f, sample_rate_hz());
  const int noise_level_dbfs = RunEstimator(gen, *estimator, sample_rate_hz());
  const float expected_noise_floor_dbfs =
      20.0f * std::log10f(kNoPulseAmplitude / test::kMaxS16);
  EXPECT_NEAR(noise_level_dbfs, expected_noise_floor_dbfs, 0.5f);
}

INSTANTIATE_TEST_SUITE_P(GainController2NoiseEstimator,
                         NoiseEstimatorParametrization,
                         ::testing::Values(8000, 16000, 32000, 48000));

}  // namespace
}  // namespace webrtc