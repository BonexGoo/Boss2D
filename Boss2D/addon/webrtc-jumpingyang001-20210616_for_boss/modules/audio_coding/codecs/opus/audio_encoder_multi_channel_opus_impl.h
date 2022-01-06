/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_CODECS_OPUS_AUDIO_ENCODER_MULTI_CHANNEL_OPUS_IMPL_H_
#define MODULES_AUDIO_CODING_CODECS_OPUS_AUDIO_ENCODER_MULTI_CHANNEL_OPUS_IMPL_H_

#include <memory>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_encoder_h //original-code:"api/audio_codecs/audio_encoder.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_format_h //original-code:"api/audio_codecs/audio_format.h"
#include BOSS_WEBRTC_U_api__audio_codecs__opus__audio_encoder_multi_channel_opus_config_h //original-code:"api/audio_codecs/opus/audio_encoder_multi_channel_opus_config.h"
#include BOSS_WEBRTC_U_api__units__time_delta_h //original-code:"api/units/time_delta.h"
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__opus__opus_interface_h //original-code:"modules/audio_coding/codecs/opus/opus_interface.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

class RtcEventLog;

class AudioEncoderMultiChannelOpusImpl final : public AudioEncoder {
 public:
  AudioEncoderMultiChannelOpusImpl(
      const AudioEncoderMultiChannelOpusConfig& config,
      int payload_type);
  ~AudioEncoderMultiChannelOpusImpl() override;

  // Static interface for use by BuiltinAudioEncoderFactory.
  static constexpr const char* GetPayloadName() { return "multiopus"; }
  static absl::optional<AudioCodecInfo> QueryAudioEncoder(
      const SdpAudioFormat& format);

  int SampleRateHz() const override;
  size_t NumChannels() const override;
  size_t Num10MsFramesInNextPacket() const override;
  size_t Max10MsFramesInAPacket() const override;
  int GetTargetBitrate() const override;

  void Reset() override;
  absl::optional<std::pair<TimeDelta, TimeDelta>> GetFrameLengthRange()
      const override;

 protected:
  EncodedInfo EncodeImpl(uint32_t rtp_timestamp,
                         rtc::ArrayView<const int16_t> audio,
                         rtc::Buffer* encoded) override;

 private:
  static absl::optional<AudioEncoderMultiChannelOpusConfig> SdpToConfig(
      const SdpAudioFormat& format);
  static AudioCodecInfo QueryAudioEncoder(
      const AudioEncoderMultiChannelOpusConfig& config);
  static std::unique_ptr<AudioEncoder> MakeAudioEncoder(
      const AudioEncoderMultiChannelOpusConfig&,
      int payload_type);

  size_t Num10msFramesPerPacket() const;
  size_t SamplesPer10msFrame() const;
  size_t SufficientOutputBufferSize() const;
  bool RecreateEncoderInstance(
      const AudioEncoderMultiChannelOpusConfig& config);
  void SetFrameLength(int frame_length_ms);
  void SetNumChannelsToEncode(size_t num_channels_to_encode);
  void SetProjectedPacketLossRate(float fraction);

  AudioEncoderMultiChannelOpusConfig config_;
  const int payload_type_;
  std::vector<int16_t> input_buffer_;
  OpusEncInst* inst_;
  uint32_t first_timestamp_in_buffer_;
  size_t num_channels_to_encode_;
  int next_frame_length_ms_;

  friend struct AudioEncoderMultiChannelOpus;
  RTC_DISALLOW_COPY_AND_ASSIGN(AudioEncoderMultiChannelOpusImpl);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_CODECS_OPUS_AUDIO_ENCODER_MULTI_CHANNEL_OPUS_IMPL_H_
