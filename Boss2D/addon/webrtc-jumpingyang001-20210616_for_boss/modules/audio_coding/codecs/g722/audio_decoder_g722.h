/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_CODECS_G722_AUDIO_DECODER_G722_H_
#define MODULES_AUDIO_CODING_CODECS_G722_AUDIO_DECODER_G722_H_

#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_h //original-code:"api/audio_codecs/audio_decoder.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

typedef struct WebRtcG722DecInst G722DecInst;

namespace webrtc {

class AudioDecoderG722Impl final : public AudioDecoder {
 public:
  AudioDecoderG722Impl();
  ~AudioDecoderG722Impl() override;
  bool HasDecodePlc() const override;
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
  G722DecInst* dec_state_;
  RTC_DISALLOW_COPY_AND_ASSIGN(AudioDecoderG722Impl);
};

class AudioDecoderG722StereoImpl final : public AudioDecoder {
 public:
  AudioDecoderG722StereoImpl();
  ~AudioDecoderG722StereoImpl() override;
  void Reset() override;
  std::vector<ParseResult> ParsePayload(rtc::Buffer&& payload,
                                        uint32_t timestamp) override;
  int SampleRateHz() const override;
  size_t Channels() const override;

 protected:
  int DecodeInternal(const uint8_t* encoded,
                     size_t encoded_len,
                     int sample_rate_hz,
                     int16_t* decoded,
                     SpeechType* speech_type) override;

 private:
  // Splits the stereo-interleaved payload in |encoded| into separate payloads
  // for left and right channels. The separated payloads are written to
  // |encoded_deinterleaved|, which must hold at least |encoded_len| samples.
  // The left channel starts at offset 0, while the right channel starts at
  // offset encoded_len / 2 into |encoded_deinterleaved|.
  void SplitStereoPacket(const uint8_t* encoded,
                         size_t encoded_len,
                         uint8_t* encoded_deinterleaved);

  G722DecInst* dec_state_left_;
  G722DecInst* dec_state_right_;
  RTC_DISALLOW_COPY_AND_ASSIGN(AudioDecoderG722StereoImpl);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_CODECS_G722_AUDIO_DECODER_G722_H_
