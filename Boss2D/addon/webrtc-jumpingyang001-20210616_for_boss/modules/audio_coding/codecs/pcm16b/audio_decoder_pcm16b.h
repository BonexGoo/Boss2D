/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_CODECS_PCM16B_AUDIO_DECODER_PCM16B_H_
#define MODULES_AUDIO_CODING_CODECS_PCM16B_AUDIO_DECODER_PCM16B_H_

#include <stddef.h>
#include <stdint.h>

#include <vector>

#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_h //original-code:"api/audio_codecs/audio_decoder.h"
#include BOSS_WEBRTC_U_rtc_base__buffer_h //original-code:"rtc_base/buffer.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

class AudioDecoderPcm16B final : public AudioDecoder {
 public:
  AudioDecoderPcm16B(int sample_rate_hz, size_t num_channels);
  void Reset() override;
  std::vector<ParseResult> ParsePayload(rtc::Buffer&& payload,
                                        uint32_t timestamp) override;
  int PacketDuration(const uint8_t* encoded, size_t encoded_len) const override;
  int SampleRateHz() const override;
  size_t Channels() const override;

 protected:
  int DecodeInternal(const uint8_t* encoded,
                     size_t encoded_len,
                     int sample_rate_hz,
                     int16_t* decoded,
                     SpeechType* speech_type) override;

 private:
  const int sample_rate_hz_;
  const size_t num_channels_;
  RTC_DISALLOW_COPY_AND_ASSIGN(AudioDecoderPcm16B);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_CODECS_PCM16B_AUDIO_DECODER_PCM16B_H_
