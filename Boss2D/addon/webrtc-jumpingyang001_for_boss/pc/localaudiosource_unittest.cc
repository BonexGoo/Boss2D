/*
 *  Copyright 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__localaudiosource_h //original-code:"pc/localaudiosource.h"

#include <string>
#include <vector>

#include BOSS_WEBRTC_U_api__test__fakeconstraints_h //original-code:"api/test/fakeconstraints.h"
#include BOSS_WEBRTC_U_media__base__fakemediaengine_h //original-code:"media/base/fakemediaengine.h"
#include BOSS_WEBRTC_U_media__base__fakevideorenderer_h //original-code:"media/base/fakevideorenderer.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"

using webrtc::LocalAudioSource;
using webrtc::MediaConstraintsInterface;
using webrtc::MediaSourceInterface;

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
