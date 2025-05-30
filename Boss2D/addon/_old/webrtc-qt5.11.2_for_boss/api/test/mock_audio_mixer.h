/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_TEST_MOCK_AUDIO_MIXER_H_
#define API_TEST_MOCK_AUDIO_MIXER_H_

#include BOSS_WEBRTC_U_api__audio__audio_mixer_h //original-code:"api/audio/audio_mixer.h"

#include "test/gmock.h"

namespace webrtc {
namespace test {

class MockAudioMixer : public AudioMixer {
 public:
  MOCK_METHOD1(AddSource, bool(Source* audio_source));
  MOCK_METHOD1(RemoveSource, void(Source* audio_source));
  MOCK_METHOD2(Mix,
               void(size_t number_of_channels,
                    AudioFrame* audio_frame_for_mixing));
};
}  // namespace test
}  // namespace webrtc

#endif  // API_TEST_MOCK_AUDIO_MIXER_H_
