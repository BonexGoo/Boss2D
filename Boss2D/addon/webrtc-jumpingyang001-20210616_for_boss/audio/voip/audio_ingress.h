/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef AUDIO_VOIP_AUDIO_INGRESS_H_
#define AUDIO_VOIP_AUDIO_INGRESS_H_

#include <algorithm>
#include <atomic>
#include <map>
#include <memory>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__audio__audio_mixer_h //original-code:"api/audio/audio_mixer.h"
#include BOSS_WEBRTC_U_api__rtp_headers_h //original-code:"api/rtp_headers.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__voip__voip_statistics_h //original-code:"api/voip/voip_statistics.h"
#include BOSS_WEBRTC_U_audio__audio_level_h //original-code:"audio/audio_level.h"
#include BOSS_WEBRTC_U_modules__audio_coding__acm2__acm_receiver_h //original-code:"modules/audio_coding/acm2/acm_receiver.h"
#include BOSS_WEBRTC_U_modules__audio_coding__include__audio_coding_module_h //original-code:"modules/audio_coding/include/audio_coding_module.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__receive_statistics_h //original-code:"modules/rtp_rtcp/include/receive_statistics.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__remote_ntp_time_estimator_h //original-code:"modules/rtp_rtcp/include/remote_ntp_time_estimator.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_received_h //original-code:"modules/rtp_rtcp/source/rtp_packet_received.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_rtcp_interface_h //original-code:"modules/rtp_rtcp/source/rtp_rtcp_interface.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__time_utils_h //original-code:"rtc_base/time_utils.h"

namespace webrtc {

// AudioIngress handles incoming RTP/RTCP packets from the remote
// media endpoint. Received RTP packets are injected into AcmReceiver and
// when audio output thread requests for audio samples to play through system
// output such as speaker device, AudioIngress provides the samples via its
// implementation on AudioMixer::Source interface.
//
// Note that this class is originally based on ChannelReceive in
// audio/channel_receive.cc with non-audio related logic trimmed as aimed for
// smaller footprint.
class AudioIngress : public AudioMixer::Source {
 public:
  AudioIngress(RtpRtcpInterface* rtp_rtcp,
               Clock* clock,
               ReceiveStatistics* receive_statistics,
               rtc::scoped_refptr<AudioDecoderFactory> decoder_factory);
  ~AudioIngress() override;

  // Start or stop receiving operation of AudioIngress.
  bool StartPlay();
  void StopPlay() {
    playing_ = false;
    output_audio_level_.ResetLevelFullRange();
  }

  // Query the state of the AudioIngress.
  bool IsPlaying() const { return playing_; }

  // Set the decoder formats and payload type for AcmReceiver where the
  // key type (int) of the map is the payload type of SdpAudioFormat.
  void SetReceiveCodecs(const std::map<int, SdpAudioFormat>& codecs);

  // APIs to handle received RTP/RTCP packets from caller.
  void ReceivedRTPPacket(rtc::ArrayView<const uint8_t> rtp_packet);
  void ReceivedRTCPPacket(rtc::ArrayView<const uint8_t> rtcp_packet);

  // See comments on LevelFullRange, TotalEnergy, TotalDuration from
  // audio/audio_level.h.
  int GetOutputAudioLevel() const {
    return output_audio_level_.LevelFullRange();
  }
  double GetOutputTotalEnergy() { return output_audio_level_.TotalEnergy(); }
  double GetOutputTotalDuration() {
    return output_audio_level_.TotalDuration();
  }

  NetworkStatistics GetNetworkStatistics() const {
    NetworkStatistics stats;
    acm_receiver_.GetNetworkStatistics(&stats,
                                       /*get_and_clear_legacy_stats=*/false);
    return stats;
  }

  ChannelStatistics GetChannelStatistics();

  // Implementation of AudioMixer::Source interface.
  AudioMixer::Source::AudioFrameInfo GetAudioFrameWithInfo(
      int sampling_rate,
      AudioFrame* audio_frame) override;
  int Ssrc() const override {
    return rtc::dchecked_cast<int>(remote_ssrc_.load());
  }
  int PreferredSampleRate() const override {
    // If we haven't received any RTP packet from remote and thus
    // last_packet_sampling_rate is not available then use NetEq's sampling
    // rate as that would be what would be used for audio output sample.
    return std::max(acm_receiver_.last_packet_sample_rate_hz().value_or(0),
                    acm_receiver_.last_output_sample_rate_hz());
  }

 private:
  // Indicates AudioIngress status as caller invokes Start/StopPlaying.
  // If not playing, incoming RTP data processing is skipped, thus
  // producing no data to output device.
  std::atomic<bool> playing_;

  // Currently active remote ssrc from remote media endpoint.
  std::atomic<uint32_t> remote_ssrc_;

  // The first rtp timestamp of the output audio frame that is used to
  // calculate elasped time for subsequent audio frames.
  std::atomic<int64_t> first_rtp_timestamp_;

  // Synchronizaton is handled internally by ReceiveStatistics.
  ReceiveStatistics* const rtp_receive_statistics_;

  // Synchronizaton is handled internally by RtpRtcpInterface.
  RtpRtcpInterface* const rtp_rtcp_;

  // Synchronizaton is handled internally by acm2::AcmReceiver.
  acm2::AcmReceiver acm_receiver_;

  // Synchronizaton is handled internally by voe::AudioLevel.
  voe::AudioLevel output_audio_level_;

  Mutex lock_;

  RemoteNtpTimeEstimator ntp_estimator_ RTC_GUARDED_BY(lock_);

  // For receiving RTP statistics, this tracks the sampling rate value
  // per payload type set when caller set via SetReceiveCodecs.
  std::map<int, int> receive_codec_info_ RTC_GUARDED_BY(lock_);

  rtc::TimestampWrapAroundHandler timestamp_wrap_handler_ RTC_GUARDED_BY(lock_);
};

}  // namespace webrtc

#endif  // AUDIO_VOIP_AUDIO_INGRESS_H_
