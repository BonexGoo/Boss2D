/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_CODECS_ILBC_AUDIO_ENCODER_ILBC_H_
#define MODULES_AUDIO_CODING_CODECS_ILBC_AUDIO_ENCODER_ILBC_H_

#include BOSS_WEBRTC_U_api__audio_codecs__audio_encoder_h //original-code:"api/audio_codecs/audio_encoder.h"
#include BOSS_WEBRTC_U_api__audio_codecs__ilbc__audio_encoder_ilbc_config_h //original-code:"api/audio_codecs/ilbc/audio_encoder_ilbc_config.h"
#include "modules/audio_coding/codecs/ilbc/ilbc.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

struct CodecInst;

class AudioEncoderIlbcImpl final : public AudioEncoder {
 public:
  AudioEncoderIlbcImpl(const AudioEncoderIlbcConfig& config, int payload_type);
  explicit AudioEncoderIlbcImpl(const CodecInst& codec_inst);
  ~AudioEncoderIlbcImpl() override;

  int SampleRateHz() const override;
  size_t NumChannels() const override;
  size_t Num10MsFramesInNextPacket() const override;
  size_t Max10MsFramesInAPacket() const override;
  int GetTargetBitrate() const override;
  EncodedInfo EncodeImpl(uint32_t rtp_timestamp,
                         rtc::ArrayView<const int16_t> audio,
                         rtc::Buffer* encoded) override;
  void Reset() override;

 private:
  size_t RequiredOutputSizeBytes() const;

  static constexpr size_t kMaxSamplesPerPacket = 480;
  const int frame_size_ms_;
  const int payload_type_;
  const size_t num_10ms_frames_per_packet_;
  size_t num_10ms_frames_buffered_;
  uint32_t first_timestamp_in_buffer_;
  int16_t input_buffer_[kMaxSamplesPerPacket];
  IlbcEncoderInstance* encoder_;
  RTC_DISALLOW_COPY_AND_ASSIGN(AudioEncoderIlbcImpl);
};

}  // namespace webrtc
#endif  // MODULES_AUDIO_CODING_CODECS_ILBC_AUDIO_ENCODER_ILBC_H_
