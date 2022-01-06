/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_FUZZERS_AUDIO_ENCODER_FUZZER_H_
#define TEST_FUZZERS_AUDIO_ENCODER_FUZZER_H_

#include <memory>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_encoder_h //original-code:"api/audio_codecs/audio_encoder.h"

namespace webrtc {

void FuzzAudioEncoder(rtc::ArrayView<const uint8_t> data_view,
                      std::unique_ptr<AudioEncoder> encoder);

}  // namespace webrtc

#endif  // TEST_FUZZERS_AUDIO_ENCODER_FUZZER_H_
