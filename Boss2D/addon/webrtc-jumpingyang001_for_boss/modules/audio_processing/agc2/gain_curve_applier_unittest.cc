/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__gain_curve_applier_h //original-code:"modules/audio_processing/agc2/gain_curve_applier.h"

#include BOSS_WEBRTC_U_common_audio__include__audio_util_h //original-code:"common_audio/include/audio_util.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__agc2_common_h //original-code:"modules/audio_processing/agc2/agc2_common.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__agc2_testing_common_h //original-code:"modules/audio_processing/agc2/agc2_testing_common.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__vector_float_frame_h //original-code:"modules/audio_processing/agc2/vector_float_frame.h"
#include BOSS_WEBRTC_U_modules__audio_processing__logging__apm_data_dumper_h //original-code:"modules/audio_processing/logging/apm_data_dumper.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"

namespace webrtc {

TEST(GainCurveApplier, GainCurveApplierShouldConstructAndRun) {
  const int sample_rate_hz = 48000;
  ApmDataDumper apm_data_dumper(0);

  GainCurveApplier gain_curve_applier(sample_rate_hz, &apm_data_dumper);

  VectorFloatFrame vectors_with_float_frame(1, sample_rate_hz / 100,
                                            kMaxAbsFloatS16Value);
  gain_curve_applier.Process(vectors_with_float_frame.float_frame_view());
}

TEST(GainCurveApplier, OutputVolumeAboveThreshold) {
  const int sample_rate_hz = 48000;
  const float input_level =
      (kMaxAbsFloatS16Value + DbfsToFloatS16(test::kLimiterMaxInputLevelDbFs)) /
      2.f;
  ApmDataDumper apm_data_dumper(0);

  GainCurveApplier gain_curve_applier(sample_rate_hz, &apm_data_dumper);

  // Give the level estimator time to adapt.
  for (int i = 0; i < 5; ++i) {
    VectorFloatFrame vectors_with_float_frame(1, sample_rate_hz / 100,
                                              input_level);
    gain_curve_applier.Process(vectors_with_float_frame.float_frame_view());
  }

  VectorFloatFrame vectors_with_float_frame(1, sample_rate_hz / 100,
                                            input_level);
  gain_curve_applier.Process(vectors_with_float_frame.float_frame_view());
  rtc::ArrayView<const float> channel =
      vectors_with_float_frame.float_frame_view().channel(0);

  for (const auto& sample : channel) {
    EXPECT_LT(0.9f * kMaxAbsFloatS16Value, sample);
  }
}

}  // namespace webrtc
