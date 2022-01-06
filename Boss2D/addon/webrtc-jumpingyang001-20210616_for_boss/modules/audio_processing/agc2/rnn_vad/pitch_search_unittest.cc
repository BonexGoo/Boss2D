/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__pitch_search_h //original-code:"modules/audio_processing/agc2/rnn_vad/pitch_search.h"

#include <algorithm>
#include <vector>

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__cpu_features_h //original-code:"modules/audio_processing/agc2/cpu_features.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__pitch_search_internal_h //original-code:"modules/audio_processing/agc2/rnn_vad/pitch_search_internal.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__test_utils_h //original-code:"modules/audio_processing/agc2/rnn_vad/test_utils.h"
// TODO(bugs.webrtc.org/8948): Add when the issue is fixed.
// #include "test/fpe_observer.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {
namespace rnn_vad {

// Checks that the computed pitch period is bit-exact and that the computed
// pitch gain is within tolerance given test input data.
TEST(RnnVadTest, PitchSearchWithinTolerance) {
  ChunksFileReader reader = CreateLpResidualAndPitchInfoReader();
  const int num_frames = std::min(reader.num_chunks, 300);  // Max 3 s.
  std::vector<float> lp_residual(kBufSize24kHz);
  float expected_pitch_period, expected_pitch_strength;
  const AvailableCpuFeatures cpu_features = GetAvailableCpuFeatures();
  PitchEstimator pitch_estimator(cpu_features);
  {
    // TODO(bugs.webrtc.org/8948): Add when the issue is fixed.
    // FloatingPointExceptionObserver fpe_observer;
    for (int i = 0; i < num_frames; ++i) {
      SCOPED_TRACE(i);
      ASSERT_TRUE(reader.reader->ReadChunk(lp_residual));
      ASSERT_TRUE(reader.reader->ReadValue(expected_pitch_period));
      ASSERT_TRUE(reader.reader->ReadValue(expected_pitch_strength));
      int pitch_period =
          pitch_estimator.Estimate({lp_residual.data(), kBufSize24kHz});
      EXPECT_EQ(expected_pitch_period, pitch_period);
      EXPECT_NEAR(expected_pitch_strength,
                  pitch_estimator.GetLastPitchStrengthForTesting(), 15e-6f);
    }
  }
}

}  // namespace rnn_vad
}  // namespace webrtc
