/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_common_audio__resampler__include__push_resampler_h //original-code:"common_audio/resampler/include/push_resampler.h"

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"  // RTC_DCHECK_IS_ON
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include "test/testsupport/rtc_expect_death.h"

// Quality testing of PushResampler is handled through output_mixer_unittest.cc.

namespace webrtc {

// The below tests are temporarily disabled on WEBRTC_WIN due to problems
// with clang debug builds.
// TODO(tommi): Re-enable when we've figured out what the problem is.
// http://crbug.com/615050
#if !defined(WEBRTC_WIN) && defined(__clang__) && !defined(NDEBUG)
TEST(PushResamplerTest, VerifiesInputParameters) {
  PushResampler<int16_t> resampler;
  EXPECT_EQ(0, resampler.InitializeIfNeeded(16000, 16000, 1));
  EXPECT_EQ(0, resampler.InitializeIfNeeded(16000, 16000, 2));
  EXPECT_EQ(0, resampler.InitializeIfNeeded(16000, 16000, 8));
}

#if RTC_DCHECK_IS_ON && GTEST_HAS_DEATH_TEST && !defined(WEBRTC_ANDROID)
TEST(PushResamplerDeathTest, VerifiesBadInputParameters1) {
  PushResampler<int16_t> resampler;
  RTC_EXPECT_DEATH(resampler.InitializeIfNeeded(-1, 16000, 1),
                   "src_sample_rate_hz");
}

TEST(PushResamplerDeathTest, VerifiesBadInputParameters2) {
  PushResampler<int16_t> resampler;
  RTC_EXPECT_DEATH(resampler.InitializeIfNeeded(16000, -1, 1),
                   "dst_sample_rate_hz");
}

TEST(PushResamplerDeathTest, VerifiesBadInputParameters3) {
  PushResampler<int16_t> resampler;
  RTC_EXPECT_DEATH(resampler.InitializeIfNeeded(16000, 16000, 0),
                   "num_channels");
}

#endif
#endif

}  // namespace webrtc
