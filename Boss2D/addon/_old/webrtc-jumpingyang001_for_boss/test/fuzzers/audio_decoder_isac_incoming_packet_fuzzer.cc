/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_coding__codecs__isac__main__include__audio_decoder_isac_h //original-code:"modules/audio_coding/codecs/isac/main/include/audio_decoder_isac.h"
#include "test/fuzzers/audio_decoder_fuzzer.h"

namespace webrtc {
void FuzzOneInput(const uint8_t* data, size_t size) {
  AudioDecoderIsacFloatImpl dec(16000);
  FuzzAudioDecoderIncomingPacket(data, size, &dec);
}
}  // namespace webrtc
