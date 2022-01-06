/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_CODECS_OPUS_AUDIO_DECODER_MULTI_CHANNEL_OPUS_IMPL_H_
#define MODULES_AUDIO_CODING_CODECS_OPUS_AUDIO_DECODER_MULTI_CHANNEL_OPUS_IMPL_H_

#include <stddef.h>

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_h //original-code:"api/audio_codecs/audio_decoder.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_format_h //original-code:"api/audio_codecs/audio_format.h"
#include BOSS_WEBRTC_U_api__audio_codecs__opus__audio_decoder_multi_channel_opus_config_h //original-code:"api/audio_codecs/opus/audio_decoder_multi_channel_opus_config.h"
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__opus__opus_interface_h //original-code:"modules/audio_coding/codecs/opus/opus_interface.h"
#include BOSS_WEBRTC_U_rtc_base__buffer_h //original-code:"rtc_base/buffer.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

class AudioDecoderMultiChannelOpusImpl final : public AudioDecoder {
 public:
  static std::unique_ptr<AudioDecoderMultiChannelOpusImpl> MakeAudioDecoder(
      AudioDecoderMultiChannelOpusConfig config);

  ~AudioDecoderMultiChannelOpusImpl() override;

  std::vector<ParseResult> ParsePayload(rtc::Buffer&& payload,
                                        uint32_t timestamp) override;
  void Reset() override;
  int PacketDuration(const uint8_t* encoded, size_t encoded_len) const override;
  int PacketDurationRedundant(const uint8_t* encoded,
                              size_t encoded_len) const override;
  bool PacketHasFec(const uint8_t* encoded, size_t encoded_len) const override;
  int SampleRateHz() const override;
  size_t Channels() const override;

  static absl::optional<AudioDecoderMultiChannelOpusConfig> SdpToConfig(
      const SdpAudioFormat& format);

 protected:
  int DecodeInternal(const uint8_t* encoded,
                     size_t encoded_len,
                     int sample_rate_hz,
                     int16_t* decoded,
                     SpeechType* speech_type) override;
  int DecodeRedundantInternal(const uint8_t* encoded,
                              size_t encoded_len,
                              int sample_rate_hz,
                              int16_t* decoded,
                              SpeechType* speech_type) override;

 private:
  AudioDecoderMultiChannelOpusImpl(OpusDecInst* dec_state,
                                   AudioDecoderMultiChannelOpusConfig config);

  OpusDecInst* dec_state_;
  const AudioDecoderMultiChannelOpusConfig config_;
  RTC_DISALLOW_COPY_AND_ASSIGN(AudioDecoderMultiChannelOpusImpl);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_CODECS_OPUS_AUDIO_DECODER_MULTI_CHANNEL_OPUS_IMPL_H_
