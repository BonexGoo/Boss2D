/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef COMMON_AUDIO_VAD_MOCK_MOCK_VAD_H_
#define COMMON_AUDIO_VAD_MOCK_MOCK_VAD_H_

#include BOSS_WEBRTC_U_common_audio__vad__include__vad_h //original-code:"common_audio/vad/include/vad.h"
#include "test/gmock.h"

namespace webrtc {

class MockVad : public Vad {
 public:
  virtual ~MockVad() { Die(); }
  MOCK_METHOD0(Die, void());

  MOCK_METHOD3(VoiceActivity,
               enum Activity(const int16_t* audio,
                             size_t num_samples,
                             int sample_rate_hz));
  MOCK_METHOD0(Reset, void());
};

}  // namespace webrtc

#endif  // COMMON_AUDIO_VAD_MOCK_MOCK_VAD_H_
