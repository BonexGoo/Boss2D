/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef AUDIO_AUDIO_SEND_STREAM_H_
#define AUDIO_AUDIO_SEND_STREAM_H_

#include <memory>
#include <utility>
#include <vector>

#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_audio__audio_level_h //original-code:"audio/audio_level.h"
#include BOSS_WEBRTC_U_audio__channel_send_h //original-code:"audio/channel_send.h"
#include BOSS_WEBRTC_U_call__audio_send_stream_h //original-code:"call/audio_send_stream.h"
#include BOSS_WEBRTC_U_call__audio_state_h //original-code:"call/audio_state.h"
#include BOSS_WEBRTC_U_call__bitrate_allocator_h //original-code:"call/bitrate_allocator.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_rtcp_interface_h //original-code:"modules/rtp_rtcp/source/rtp_rtcp_interface.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__struct_parameters_parser_h //original-code:"rtc_base/experiments/struct_parameters_parser.h"
#include BOSS_WEBRTC_U_rtc_base__race_checker_h //original-code:"rtc_base/race_checker.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"

namespace webrtc {
class RtcEventLog;
class RtcpBandwidthObserver;
class RtcpRttStats;
class RtpTransportControllerSendInterface;

struct AudioAllocationConfig {
  static constexpr char kKey[] = "WebRTC-Audio-Allocation";
  // Field Trial configured bitrates to use as overrides over default/user
  // configured bitrate range when audio bitrate allocation is enabled.
  absl::optional<DataRate> min_bitrate;
  absl::optional<DataRate> max_bitrate;
  DataRate priority_bitrate = DataRate::Zero();
  // By default the priority_bitrate is compensated for packet overhead.
  // Use this flag to configure a raw value instead.
  absl::optional<DataRate> priority_bitrate_raw;
  absl::optional<double> bitrate_priority;

  std::unique_ptr<StructParametersParser> Parser();
  AudioAllocationConfig();
};
namespace internal {
class AudioState;

class AudioSendStream final : public webrtc::AudioSendStream,
                              public webrtc::BitrateAllocatorObserver {
 public:
  AudioSendStream(Clock* clock,
                  const webrtc::AudioSendStream::Config& config,
                  const rtc::scoped_refptr<webrtc::AudioState>& audio_state,
                  TaskQueueFactory* task_queue_factory,
                  ProcessThread* module_process_thread,
                  RtpTransportControllerSendInterface* rtp_transport,
                  BitrateAllocatorInterface* bitrate_allocator,
                  RtcEventLog* event_log,
                  RtcpRttStats* rtcp_rtt_stats,
                  const absl::optional<RtpState>& suspended_rtp_state);
  // For unit tests, which need to supply a mock ChannelSend.
  AudioSendStream(Clock* clock,
                  const webrtc::AudioSendStream::Config& config,
                  const rtc::scoped_refptr<webrtc::AudioState>& audio_state,
                  TaskQueueFactory* task_queue_factory,
                  RtpTransportControllerSendInterface* rtp_transport,
                  BitrateAllocatorInterface* bitrate_allocator,
                  RtcEventLog* event_log,
                  const absl::optional<RtpState>& suspended_rtp_state,
                  std::unique_ptr<voe::ChannelSendInterface> channel_send);

  AudioSendStream() = delete;
  AudioSendStream(const AudioSendStream&) = delete;
  AudioSendStream& operator=(const AudioSendStream&) = delete;

  ~AudioSendStream() override;

  // webrtc::AudioSendStream implementation.
  const webrtc::AudioSendStream::Config& GetConfig() const override;
  void Reconfigure(const webrtc::AudioSendStream::Config& config) override;
  void Start() override;
  void Stop() override;
  void SendAudioData(std::unique_ptr<AudioFrame> audio_frame) override;
  bool SendTelephoneEvent(int payload_type,
                          int payload_frequency,
                          int event,
                          int duration_ms) override;
  void SetMuted(bool muted) override;
  webrtc::AudioSendStream::Stats GetStats() const override;
  webrtc::AudioSendStream::Stats GetStats(
      bool has_remote_tracks) const override;

  void DeliverRtcp(const uint8_t* packet, size_t length);

  // Implements BitrateAllocatorObserver.
  uint32_t OnBitrateUpdated(BitrateAllocationUpdate update) override;

  void SetTransportOverhead(int transport_overhead_per_packet_bytes);

  RtpState GetRtpState() const;
  const voe::ChannelSendInterface* GetChannel() const;

  // Returns combined per-packet overhead.
  size_t TestOnlyGetPerPacketOverheadBytes() const
      RTC_LOCKS_EXCLUDED(overhead_per_packet_lock_);

 private:
  class TimedTransport;
  // Constraints including overhead.
  struct TargetAudioBitrateConstraints {
    DataRate min;
    DataRate max;
  };

  internal::AudioState* audio_state();
  const internal::AudioState* audio_state() const;

  void StoreEncoderProperties(int sample_rate_hz, size_t num_channels)
      RTC_RUN_ON(worker_thread_checker_);

  void ConfigureStream(const Config& new_config, bool first_time)
      RTC_RUN_ON(worker_thread_checker_);
  bool SetupSendCodec(const Config& new_config)
      RTC_RUN_ON(worker_thread_checker_);
  bool ReconfigureSendCodec(const Config& new_config)
      RTC_RUN_ON(worker_thread_checker_);
  void ReconfigureANA(const Config& new_config)
      RTC_RUN_ON(worker_thread_checker_);
  void ReconfigureCNG(const Config& new_config)
      RTC_RUN_ON(worker_thread_checker_);
  void ReconfigureBitrateObserver(const Config& new_config)
      RTC_RUN_ON(worker_thread_checker_);

  void ConfigureBitrateObserver() RTC_RUN_ON(worker_thread_checker_);
  void RemoveBitrateObserver() RTC_RUN_ON(worker_thread_checker_);

  // Returns bitrate constraints, maybe including overhead when enabled by
  // field trial.
  absl::optional<TargetAudioBitrateConstraints> GetMinMaxBitrateConstraints()
      const RTC_RUN_ON(worker_thread_checker_);

  // Sets per-packet overhead on encoded (for ANA) based on current known values
  // of transport and packetization overheads.
  void UpdateOverheadForEncoder()
      RTC_EXCLUSIVE_LOCKS_REQUIRED(overhead_per_packet_lock_);

  // Returns combined per-packet overhead.
  size_t GetPerPacketOverheadBytes() const
      RTC_EXCLUSIVE_LOCKS_REQUIRED(overhead_per_packet_lock_);

  void RegisterCngPayloadType(int payload_type, int clockrate_hz)
      RTC_RUN_ON(worker_thread_checker_);

  void UpdateCachedTargetAudioBitrateConstraints()
      RTC_RUN_ON(worker_thread_checker_);

  Clock* clock_;

  SequenceChecker worker_thread_checker_;
  SequenceChecker pacer_thread_checker_;
  rtc::RaceChecker audio_capture_race_checker_;
  rtc::TaskQueue* rtp_transport_queue_;

  const bool allocate_audio_without_feedback_;
  const bool force_no_audio_feedback_ = allocate_audio_without_feedback_;
  const bool enable_audio_alr_probing_;
  const bool send_side_bwe_with_overhead_;
  const AudioAllocationConfig allocation_settings_;

  webrtc::AudioSendStream::Config config_
      RTC_GUARDED_BY(worker_thread_checker_);
  rtc::scoped_refptr<webrtc::AudioState> audio_state_;
  const std::unique_ptr<voe::ChannelSendInterface> channel_send_;
  RtcEventLog* const event_log_;
  const bool use_legacy_overhead_calculation_;

  int encoder_sample_rate_hz_ RTC_GUARDED_BY(worker_thread_checker_) = 0;
  size_t encoder_num_channels_ RTC_GUARDED_BY(worker_thread_checker_) = 0;
  bool sending_ RTC_GUARDED_BY(worker_thread_checker_) = false;
  mutable Mutex audio_level_lock_;
  // Keeps track of audio level, total audio energy and total samples duration.
  // https://w3c.github.io/webrtc-stats/#dom-rtcaudiohandlerstats-totalaudioenergy
  webrtc::voe::AudioLevel audio_level_ RTC_GUARDED_BY(audio_level_lock_);

  BitrateAllocatorInterface* const bitrate_allocator_
      RTC_GUARDED_BY(rtp_transport_queue_);
  // Constrains cached to be accessed from |rtp_transport_queue_|.
  absl::optional<AudioSendStream::TargetAudioBitrateConstraints>
      cached_constraints_ RTC_GUARDED_BY(rtp_transport_queue_) = absl::nullopt;
  RtpTransportControllerSendInterface* const rtp_transport_;

  RtpRtcpInterface* const rtp_rtcp_module_;
  absl::optional<RtpState> const suspended_rtp_state_;

  // RFC 5285: Each distinct extension MUST have a unique ID. The value 0 is
  // reserved for padding and MUST NOT be used as a local identifier.
  // So it should be safe to use 0 here to indicate "not configured".
  struct ExtensionIds {
    int audio_level = 0;
    int abs_send_time = 0;
    int abs_capture_time = 0;
    int transport_sequence_number = 0;
    int mid = 0;
    int rid = 0;
    int repaired_rid = 0;
  };
  static ExtensionIds FindExtensionIds(
      const std::vector<RtpExtension>& extensions);
  static int TransportSeqNumId(const Config& config);

  mutable Mutex overhead_per_packet_lock_;
  size_t overhead_per_packet_ RTC_GUARDED_BY(overhead_per_packet_lock_) = 0;

  // Current transport overhead (ICE, TURN, etc.)
  size_t transport_overhead_per_packet_bytes_
      RTC_GUARDED_BY(overhead_per_packet_lock_) = 0;

  bool registered_with_allocator_ RTC_GUARDED_BY(worker_thread_checker_) =
      false;
  size_t total_packet_overhead_bytes_ RTC_GUARDED_BY(worker_thread_checker_) =
      0;
  absl::optional<std::pair<TimeDelta, TimeDelta>> frame_length_range_
      RTC_GUARDED_BY(worker_thread_checker_);
};
}  // namespace internal
}  // namespace webrtc

#endif  // AUDIO_AUDIO_SEND_STREAM_H_
