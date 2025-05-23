/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__isac__audio_encoder_isac_fix_h //original-code:"api/audio_codecs/isac/audio_encoder_isac_fix.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include "test/fuzzers/audio_encoder_fuzzer.h"

namespace webrtc {

void FuzzOneInput(const uint8_t* data, size_t size) {
  AudioEncoderIsacFix::Config config;
  RTC_CHECK(config.IsOk());
  constexpr int kPayloadType = 100;
  FuzzAudioEncoder(
      /*data_view=*/{data, size},
      /*encoder=*/AudioEncoderIsacFix::MakeAudioEncoder(config, kPayloadType));
}

}  // namespace webrtc
