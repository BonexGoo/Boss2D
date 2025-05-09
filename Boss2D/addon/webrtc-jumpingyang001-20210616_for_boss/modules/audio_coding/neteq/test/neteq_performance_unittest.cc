/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_coding__neteq__tools__neteq_performance_test_h //original-code:"modules/audio_coding/neteq/tools/neteq_performance_test.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include "test/testsupport/perf_test.h"

// Runs a test with 10% packet losses and 10% clock drift, to exercise
// both loss concealment and time-stretching code.
TEST(NetEqPerformanceTest, 10_Pl_10_Drift) {
  const int kSimulationTimeMs = 10000000;
  const int kQuickSimulationTimeMs = 100000;
  const int kLossPeriod = 10;  // Drop every 10th packet.
  const double kDriftFactor = 0.1;
  int64_t runtime = webrtc::test::NetEqPerformanceTest::Run(
      webrtc::field_trial::IsEnabled("WebRTC-QuickPerfTest")
          ? kQuickSimulationTimeMs
          : kSimulationTimeMs,
      kLossPeriod, kDriftFactor);
  ASSERT_GT(runtime, 0);
  webrtc::test::PrintResult("neteq_performance", "", "10_pl_10_drift", runtime,
                            "ms", true);
}

// Runs a test with neither packet losses nor clock drift, to put
// emphasis on the "good-weather" code path, which is presumably much
// more lightweight.
TEST(NetEqPerformanceTest, 0_Pl_0_Drift) {
  const int kSimulationTimeMs = 10000000;
  const int kQuickSimulationTimeMs = 100000;
  const int kLossPeriod = 0;        // No losses.
  const double kDriftFactor = 0.0;  // No clock drift.
  int64_t runtime = webrtc::test::NetEqPerformanceTest::Run(
      webrtc::field_trial::IsEnabled("WebRTC-QuickPerfTest")
          ? kQuickSimulationTimeMs
          : kSimulationTimeMs,
      kLossPeriod, kDriftFactor);
  ASSERT_GT(runtime, 0);
  webrtc::test::PrintResult("neteq_performance", "", "0_pl_0_drift", runtime,
                            "ms", true);
}
