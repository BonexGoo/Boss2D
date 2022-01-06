/*
 *  Copyright 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__local_audio_source_h //original-code:"pc/local_audio_source.h"

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

using webrtc::LocalAudioSource;

TEST(LocalAudioSourceTest, InitWithAudioOptions) {
  cricket::AudioOptions audio_options;
  audio_options.highpass_filter = true;
  rtc::scoped_refptr<LocalAudioSource> source =
      LocalAudioSource::Create(&audio_options);
  EXPECT_EQ(true, source->options().highpass_filter);
}

TEST(LocalAudioSourceTest, InitWithNoOptions) {
  rtc::scoped_refptr<LocalAudioSource> source =
      LocalAudioSource::Create(nullptr);
  EXPECT_EQ(absl::nullopt, source->options().highpass_filter);
}
