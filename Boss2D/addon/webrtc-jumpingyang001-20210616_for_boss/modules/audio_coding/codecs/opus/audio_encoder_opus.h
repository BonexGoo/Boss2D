/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_CODECS_OPUS_AUDIO_ENCODER_OPUS_H_
#define MODULES_AUDIO_CODING_CODECS_OPUS_AUDIO_ENCODER_OPUS_H_

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_encoder_h //original-code:"api/audio_codecs/audio_encoder.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_format_h //original-code:"api/audio_codecs/audio_format.h"
#include BOSS_WEBRTC_U_api__audio_codecs__opus__audio_encoder_opus_config_h //original-code:"api/audio_codecs/opus/audio_encoder_opus_config.h"
#include BOSS_WEBRTC_U_common_audio__smoothing_filter_h //original-code:"common_audio/smoothing_filter.h"
#include BOSS_WEBRTC_U_modules__audio_coding__audio_network_adaptor__include__audio_network_adaptor_h //original-code:"modules/audio_coding/audio_network_adaptor/include/audio_network_adaptor.h"
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__opus__opus_interface_h //original-code:"modules/audio_coding/codecs/opus/opus_interface.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

class RtcEventLog;

class AudioEncoderOpusImpl final : public AudioEncoder {
 public:
  // Returns empty if the current bitrate falls within the hysteresis window,
  // defined by complexity_threshold_bps +/- complexity_threshold_window_bps.
  // Otherwise, returns the current complexity depending on whether the
  // current bitrate is above or below complexity_threshold_bps.
  static absl::optional<int> GetNewComplexity(
      const AudioEncoderOpusConfig& config);

  // Returns OPUS_AUTO if the the current bitrate is above wideband threshold.
  // Returns empty if it is below, but bandwidth coincides with the desired one.
  // Otherwise returns the desired bandwidth.
  static absl::optional<int> GetNewBandwidth(
      const AudioEncoderOpusConfig& config,
      OpusEncInst* inst);

  using AudioNetworkAdaptorCreator =
      std::function<std::unique_ptr<AudioNetworkAdaptor>(const std::string&,
                                                         RtcEventLog*)>;

  AudioEncoderOpusImpl(const AudioEncoderOpusConfig& config, int payload_type);

  // Dependency injection for testing.
  AudioEncoderOpusImpl(
      const AudioEncoderOpusConfig& config,
      int payload_type,
      const AudioNetworkAdaptorCreator& audio_network_adaptor_creator,
      std::unique_ptr<SmoothingFilter> bitrate_smoother);

  AudioEncoderOpusImpl(int payload_type, const SdpAudioFormat& format);
  ~AudioEncoderOpusImpl() override;

  int SampleRateHz() const override;
  size_t NumChannels() const override;
  int RtpTimestampRateHz() const override;
  size_t Num10MsFramesInNextPacket() const override;
  size_t Max10MsFramesInAPacket() const override;
  int GetTargetBitrate() const override;

  void Reset() override;
  bool SetFec(bool enable) override;

  // Set Opus DTX. Once enabled, Opus stops transmission, when it detects
  // voice being inactive. During that, it still sends 2 packets (one for
  // content, one for signaling) about every 400 ms.
  bool SetDtx(bool enable) override;
  bool GetDtx() const override;

  bool SetApplication(Application application) override;
  void SetMaxPlaybackRate(int frequency_hz) override;
  bool EnableAudioNetworkAdaptor(const std::string& config_string,
                                 RtcEventLog* event_log) override;
  void DisableAudioNetworkAdaptor() override;
  void OnReceivedUplinkPacketLossFraction(
      float uplink_packet_loss_fraction) override;
  void OnReceivedTargetAudioBitrate(int target_audio_bitrate_bps) override;
  void OnReceivedUplinkBandwidth(
      int target_audio_bitrate_bps,
      absl::optional<int64_t> bwe_period_ms) override;
  void OnReceivedUplinkAllocation(BitrateAllocationUpdate update) override;
  void OnReceivedRtt(int rtt_ms) override;
  void OnReceivedOverhead(size_t overhead_bytes_per_packet) override;
  void SetReceiverFrameLengthRange(int min_frame_length_ms,
                                   int max_frame_length_ms) override;
  ANAStats GetANAStats() const override;
  absl::optional<std::pair<TimeDelta, TimeDelta> > GetFrameLengthRange()
      const override;
  rtc::ArrayView<const int> supported_frame_lengths_ms() const {
    return config_.supported_frame_lengths_ms;
  }

  // Getters for testing.
  float packet_loss_rate() const { return packet_loss_rate_; }
  AudioEncoderOpusConfig::ApplicationMode application() const {
    return config_.application;
  }
  bool fec_enabled() const { return config_.fec_enabled; }
  size_t num_channels_to_encode() const { return num_channels_to_encode_; }
  int next_frame_length_ms() const { return next_frame_length_ms_; }

 protected:
  EncodedInfo EncodeImpl(uint32_t rtp_timestamp,
                         rtc::ArrayView<const int16_t> audio,
                         rtc::Buffer* encoded) override;

 private:
  class PacketLossFractionSmoother;

  static absl::optional<AudioEncoderOpusConfig> SdpToConfig(
      const SdpAudioFormat& format);
  static void AppendSupportedEncoders(std::vector<AudioCodecSpec>* specs);
  static AudioCodecInfo QueryAudioEncoder(const AudioEncoderOpusConfig& config);
  static std::unique_ptr<AudioEncoder> MakeAudioEncoder(
      const AudioEncoderOpusConfig&,
      int payload_type);

  size_t Num10msFramesPerPacket() const;
  size_t SamplesPer10msFrame() const;
  size_t SufficientOutputBufferSize() const;
  bool RecreateEncoderInstance(const AudioEncoderOpusConfig& config);
  void SetFrameLength(int frame_length_ms);
  void SetNumChannelsToEncode(size_t num_channels_to_encode);
  void SetProjectedPacketLossRate(float fraction);

  void OnReceivedUplinkBandwidth(
      int target_audio_bitrate_bps,
      absl::optional<int64_t> bwe_period_ms,
      absl::optional<int64_t> link_capacity_allocation);

  // TODO(minyue): remove "override" when we can deprecate
  // |AudioEncoder::SetTargetBitrate|.
  void SetTargetBitrate(int target_bps) override;

  void ApplyAudioNetworkAdaptor();
  std::unique_ptr<AudioNetworkAdaptor> DefaultAudioNetworkAdaptorCreator(
      const std::string& config_string,
      RtcEventLog* event_log) const;

  void MaybeUpdateUplinkBandwidth();

  AudioEncoderOpusConfig config_;
  const int payload_type_;
  const bool send_side_bwe_with_overhead_;
  const bool use_stable_target_for_adaptation_;
  const bool adjust_bandwidth_;
  bool bitrate_changed_;
  // A multiplier for bitrates at 5 kbps and higher. The target bitrate
  // will be multiplied by these multipliers, each multiplier is applied to a
  // 1 kbps range.
  std::vector<float> bitrate_multipliers_;
  float packet_loss_rate_;
  std::vector<int16_t> input_buffer_;
  OpusEncInst* inst_;
  uint32_t first_timestamp_in_buffer_;
  size_t num_channels_to_encode_;
  int next_frame_length_ms_;
  int complexity_;
  std::unique_ptr<PacketLossFractionSmoother> packet_loss_fraction_smoother_;
  const AudioNetworkAdaptorCreator audio_network_adaptor_creator_;
  std::unique_ptr<AudioNetworkAdaptor> audio_network_adaptor_;
  absl::optional<size_t> overhead_bytes_per_packet_;
  const std::unique_ptr<SmoothingFilter> bitrate_smoother_;
  absl::optional<int64_t> bitrate_smoother_last_update_time_;
  int consecutive_dtx_frames_;

  friend struct AudioEncoderOpus;
  RTC_DISALLOW_COPY_AND_ASSIGN(AudioEncoderOpusImpl);
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_CODECS_OPUS_AUDIO_ENCODER_OPUS_H_
