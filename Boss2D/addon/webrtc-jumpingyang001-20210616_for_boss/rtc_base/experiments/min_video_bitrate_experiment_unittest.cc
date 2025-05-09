/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__experiments__min_video_bitrate_experiment_h //original-code:"rtc_base/experiments/min_video_bitrate_experiment.h"

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__units__data_rate_h //original-code:"api/units/data_rate.h"
#include BOSS_WEBRTC_U_api__video__video_codec_type_h //original-code:"api/video/video_codec_type.h"
#include BOSS_WEBRTC_U_test__field_trial_h //original-code:"test/field_trial.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {
namespace {

TEST(GetExperimentalMinVideoBitrateTest,
     NulloptForAllCodecsIfFieldTrialUndefined) {
  test::ScopedFieldTrials field_trials("");

  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecGeneric),
            absl::nullopt);
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP8),
            absl::nullopt);
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP9),
            absl::nullopt);
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecH264),
            absl::nullopt);
  EXPECT_EQ(
      GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecMultiplex),
      absl::nullopt);
}

TEST(GetExperimentalMinVideoBitrateTest,
     NulloptForAllCodecsIfFieldTrialDisabled) {
  test::ScopedFieldTrials field_trials(
      "WebRTC-Video-MinVideoBitrate/Disabled,br:123kbps/");

  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecGeneric),
            absl::nullopt);
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP8),
            absl::nullopt);
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP9),
            absl::nullopt);
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecH264),
            absl::nullopt);
  EXPECT_EQ(
      GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecMultiplex),
      absl::nullopt);
}

TEST(GetExperimentalMinVideoBitrateTest, BrForAllCodecsIfDefined) {
  test::ScopedFieldTrials field_trials(
      "WebRTC-Video-MinVideoBitrate/Enabled,br:123kbps/");

  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecGeneric),
            absl::make_optional(DataRate::KilobitsPerSec(123)));
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP8),
            absl::make_optional(DataRate::KilobitsPerSec(123)));
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP9),
            absl::make_optional(DataRate::KilobitsPerSec(123)));
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecH264),
            absl::make_optional(DataRate::KilobitsPerSec(123)));
  EXPECT_EQ(
      GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecMultiplex),
      absl::make_optional(DataRate::KilobitsPerSec(123)));
}

TEST(GetExperimentalMinVideoBitrateTest, BrTrumpsSpecificCodecConfigs) {
  test::ScopedFieldTrials field_trials(
      "WebRTC-Video-MinVideoBitrate/"
      "Enabled,br:123kbps,vp8_br:100kbps,vp9_br:200kbps,h264_br:300kbps/");

  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecGeneric),
            absl::make_optional(DataRate::KilobitsPerSec(123)));
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP8),
            absl::make_optional(DataRate::KilobitsPerSec(123)));
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP9),
            absl::make_optional(DataRate::KilobitsPerSec(123)));
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecH264),
            absl::make_optional(DataRate::KilobitsPerSec(123)));
  EXPECT_EQ(
      GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecMultiplex),
      absl::make_optional(DataRate::KilobitsPerSec(123)));
}

TEST(GetExperimentalMinVideoBitrateTest,
     SpecificCodecConfigsIgnoredIfExpDisabled) {
  test::ScopedFieldTrials field_trials(
      "WebRTC-Video-MinVideoBitrate/"
      "Disabled,vp8_br:100kbps,vp9_br:200kbps,h264_br:300kbps/");

  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecGeneric),
            absl::nullopt);
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP8),
            absl::nullopt);
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP9),
            absl::nullopt);
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecH264),
            absl::nullopt);
  EXPECT_EQ(
      GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecMultiplex),
      absl::nullopt);
}

TEST(GetExperimentalMinVideoBitrateTest, SpecificCodecConfigsUsedIfExpEnabled) {
  test::ScopedFieldTrials field_trials(
      "WebRTC-Video-MinVideoBitrate/"
      "Enabled,vp8_br:100kbps,vp9_br:200kbps,h264_br:300kbps/");

  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecGeneric),
            absl::nullopt);
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP8),
            absl::make_optional(DataRate::KilobitsPerSec(100)));
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP9),
            absl::make_optional(DataRate::KilobitsPerSec(200)));
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecH264),
            absl::make_optional(DataRate::KilobitsPerSec(300)));
  EXPECT_EQ(
      GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecMultiplex),
      absl::nullopt);
}

TEST(GetExperimentalMinVideoBitrateTest,
     Vp8BitrateValueTakenFromFallbackIfAvailable) {
  test::ScopedFieldTrials field_trials(
      "WebRTC-Video-MinVideoBitrate/"
      "Enabled,vp8_br:100kbps,vp9_br:200kbps,h264_br:300kbps/"
      "WebRTC-VP8-Forced-Fallback-Encoder-v2/"
      "Enabled-444444,555555,666666/");

  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP8),
            absl::make_optional(DataRate::BitsPerSec(666666)));
}

TEST(GetExperimentalMinVideoBitrateTest,
     NonVp8BitrateValuesTakenFromMinVideoBitrate) {
  test::ScopedFieldTrials field_trials(
      "WebRTC-Video-MinVideoBitrate/"
      "Enabled,vp8_br:100kbps,vp9_br:200kbps,h264_br:300kbps/"
      "WebRTC-VP8-Forced-Fallback-Encoder-v2/"
      "Enabled-444444,555555,666666/");

  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecGeneric),
            absl::nullopt);
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecVP9),
            absl::make_optional(DataRate::KilobitsPerSec(200)));
  EXPECT_EQ(GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecH264),
            absl::make_optional(DataRate::KilobitsPerSec(300)));
  EXPECT_EQ(
      GetExperimentalMinVideoBitrate(VideoCodecType::kVideoCodecMultiplex),
      absl::nullopt);
}

}  // namespace
}  // namespace webrtc
