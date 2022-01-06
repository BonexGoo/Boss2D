/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_TEST_AUDIO_BUFFER_TOOLS_H_
#define MODULES_AUDIO_PROCESSING_TEST_AUDIO_BUFFER_TOOLS_H_

#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__audio_processing__audio_buffer_h //original-code:"modules/audio_processing/audio_buffer.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"

namespace webrtc {
namespace test {

// Copies a vector into an audiobuffer.
void CopyVectorToAudioBuffer(const StreamConfig& stream_config,
                             rtc::ArrayView<const float> source,
                             AudioBuffer* destination);

// Extracts a vector from an audiobuffer.
void ExtractVectorFromAudioBuffer(const StreamConfig& stream_config,
                                  AudioBuffer* source,
                                  std::vector<float>* destination);

// Sets all values in `audio_buffer` to `value`.
void FillBuffer(float value, AudioBuffer& audio_buffer);

// Sets all values channel `channel` for `audio_buffer` to `value`.
void FillBufferChannel(float value, int channel, AudioBuffer& audio_buffer);

}  // namespace test
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_TEST_AUDIO_BUFFER_TOOLS_H_
