/*  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stdint.h>

#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__video_coding__jitter_estimator_h //original-code:"modules/video_coding/jitter_estimator.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__jitter_upper_bound_experiment_h //original-code:"rtc_base/experiments/jitter_upper_bound_experiment.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__histogram_percentile_counter_h //original-code:"rtc_base/numerics/histogram_percentile_counter.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"
#include BOSS_WEBRTC_U_rtc_base__time_utils_h //original-code:"rtc_base/time_utils.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"
#include BOSS_WEBRTC_U_test__field_trial_h //original-code:"test/field_trial.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

class TestVCMJitterEstimator : public ::testing::Test {
 protected:
  TestVCMJitterEstimator() : fake_clock_(0) {}

  virtual void SetUp() {
    estimator_ = std::make_unique<VCMJitterEstimator>(&fake_clock_);
  }

  void AdvanceClock(int64_t microseconds) {
    fake_clock_.AdvanceTimeMicroseconds(microseconds);
  }

  SimulatedClock fake_clock_;
  std::unique_ptr<VCMJitterEstimator> estimator_;
};

// Generates some simple test data in the form of a sawtooth wave.
class ValueGenerator {
 public:
  explicit ValueGenerator(int32_t amplitude)
      : amplitude_(amplitude), counter_(0) {}
  virtual ~ValueGenerator() {}

  int64_t Delay() const { return ((counter_ % 11) - 5) * amplitude_; }

  uint32_t FrameSize() const { return 1000 + Delay(); }

  void Advance() { ++counter_; }

 private:
  const int32_t amplitude_;
  int64_t counter_;
};

// 5 fps, disable jitter delay altogether.
TEST_F(TestVCMJitterEstimator, TestLowRate) {
  ValueGenerator gen(10);
  uint64_t time_delta_us = rtc::kNumMicrosecsPerSec / 5;
  for (int i = 0; i < 60; ++i) {
    estimator_->UpdateEstimate(gen.Delay(), gen.FrameSize());
    AdvanceClock(time_delta_us);
    if (i > 2)
      EXPECT_EQ(estimator_->GetJitterEstimate(0, absl::nullopt), 0);
    gen.Advance();
  }
}

TEST_F(TestVCMJitterEstimator, TestLowRateDisabled) {
  test::ScopedFieldTrials field_trials(
      "WebRTC-ReducedJitterDelayKillSwitch/Enabled/");
  SetUp();

  ValueGenerator gen(10);
  uint64_t time_delta_us = rtc::kNumMicrosecsPerSec / 5;
  for (int i = 0; i < 60; ++i) {
    estimator_->UpdateEstimate(gen.Delay(), gen.FrameSize());
    AdvanceClock(time_delta_us);
    if (i > 2)
      EXPECT_GT(estimator_->GetJitterEstimate(0, absl::nullopt), 0);
    gen.Advance();
  }
}

TEST_F(TestVCMJitterEstimator, TestUpperBound) {
  struct TestContext {
    TestContext()
        : upper_bound(0.0),
          rtt_mult(0),
          rtt_mult_add_cap_ms(absl::nullopt),
          percentiles(1000) {}
    double upper_bound;
    double rtt_mult;
    absl::optional<double> rtt_mult_add_cap_ms;
    rtc::HistogramPercentileCounter percentiles;
  };
  std::vector<TestContext> test_cases(4);

  // Large upper bound, rtt_mult = 0, and nullopt for rtt_mult addition cap.
  test_cases[0].upper_bound = 100.0;
  test_cases[0].rtt_mult = 0;
  test_cases[0].rtt_mult_add_cap_ms = absl::nullopt;
  // Small upper bound, rtt_mult = 0, and nullopt for rtt_mult addition cap.
  test_cases[1].upper_bound = 3.5;
  test_cases[1].rtt_mult = 0;
  test_cases[1].rtt_mult_add_cap_ms = absl::nullopt;
  // Large upper bound, rtt_mult = 1, and large rtt_mult addition cap value.
  test_cases[2].upper_bound = 1000.0;
  test_cases[2].rtt_mult = 1.0;
  test_cases[2].rtt_mult_add_cap_ms = 200.0;
  // Large upper bound, rtt_mult = 1, and small rtt_mult addition cap value.
  test_cases[3].upper_bound = 1000.0;
  test_cases[3].rtt_mult = 1.0;
  test_cases[3].rtt_mult_add_cap_ms = 10.0;

  // Test jitter buffer upper_bound and rtt_mult addition cap sizes.
  for (TestContext& context : test_cases) {
    // Set up field trial and reset jitter estimator.
    char string_buf[64];
    rtc::SimpleStringBuilder ssb(string_buf);
    ssb << JitterUpperBoundExperiment::kJitterUpperBoundExperimentName
        << "/Enabled-" << context.upper_bound << "/";
    test::ScopedFieldTrials field_trials(ssb.str());
    SetUp();

    ValueGenerator gen(50);
    uint64_t time_delta_us = rtc::kNumMicrosecsPerSec / 30;
    constexpr int64_t kRttMs = 250;
    for (int i = 0; i < 100; ++i) {
      estimator_->UpdateEstimate(gen.Delay(), gen.FrameSize());
      AdvanceClock(time_delta_us);
      estimator_->FrameNacked();      // To test rtt_mult.
      estimator_->UpdateRtt(kRttMs);  // To test rtt_mult.
      context.percentiles.Add(
          static_cast<uint32_t>(estimator_->GetJitterEstimate(
              context.rtt_mult, context.rtt_mult_add_cap_ms)));
      gen.Advance();
    }
  }

  // Median should be similar after three seconds. Allow 5% error margin.
  uint32_t median_unbound = *test_cases[0].percentiles.GetPercentile(0.5);
  uint32_t median_bounded = *test_cases[1].percentiles.GetPercentile(0.5);
  EXPECT_NEAR(median_unbound, median_bounded, (median_unbound * 5) / 100);

  // Max should be lower for the bounded case.
  uint32_t max_unbound = *test_cases[0].percentiles.GetPercentile(1.0);
  uint32_t max_bounded = *test_cases[1].percentiles.GetPercentile(1.0);
  EXPECT_GT(max_unbound, static_cast<uint32_t>(max_bounded * 1.25));

  // With rtt_mult = 1, max should be lower with small rtt_mult add cap value.
  max_unbound = *test_cases[2].percentiles.GetPercentile(1.0);
  max_bounded = *test_cases[3].percentiles.GetPercentile(1.0);
  EXPECT_GT(max_unbound, static_cast<uint32_t>(max_bounded * 1.25));
}

}  // namespace webrtc
