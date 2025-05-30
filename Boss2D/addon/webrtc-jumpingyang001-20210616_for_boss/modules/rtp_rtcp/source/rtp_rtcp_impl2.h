/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_RTP_RTCP_IMPL2_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_RTCP_IMPL2_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <set>
#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__rtp_headers_h //original-code:"api/rtp_headers.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_base_h //original-code:"api/task_queue/task_queue_base.h"
#include BOSS_WEBRTC_U_api__video__video_bitrate_allocation_h //original-code:"api/video/video_bitrate_allocation.h"
#include BOSS_WEBRTC_U_modules__include__module_fec_types_h //original-code:"modules/include/module_fec_types.h"
#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__include__remote_bitrate_estimator_h //original-code:"modules/remote_bitrate_estimator/include/remote_bitrate_estimator.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"  // RTCPPacketType
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__tmmb_item_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/tmmb_item.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_receiver_h //original-code:"modules/rtp_rtcp/source/rtcp_receiver.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_sender_h //original-code:"modules/rtp_rtcp/source/rtcp_sender.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_history_h //original-code:"modules/rtp_rtcp/source/rtp_packet_history.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_to_send_h //original-code:"modules/rtp_rtcp/source/rtp_packet_to_send.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_rtcp_impl2_h //original-code:"modules/rtp_rtcp/source/rtp_rtcp_impl2.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_sender_h //original-code:"modules/rtp_rtcp/source/rtp_sender.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_sender_egress_h //original-code:"modules/rtp_rtcp/source/rtp_sender_egress.h"
#include BOSS_WEBRTC_U_rtc_base__gtest_prod_util_h //original-code:"rtc_base/gtest_prod_util.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__system__no_unique_address_h //original-code:"rtc_base/system/no_unique_address.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__pending_task_safety_flag_h //original-code:"rtc_base/task_utils/pending_task_safety_flag.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__repeating_task_h //original-code:"rtc_base/task_utils/repeating_task.h"

namespace webrtc {

class Clock;
struct PacedPacketInfo;
struct RTPVideoHeader;

class ModuleRtpRtcpImpl2 final : public RtpRtcpInterface,
                                 public Module,
                                 public RTCPReceiver::ModuleRtpRtcp {
 public:
  explicit ModuleRtpRtcpImpl2(
      const RtpRtcpInterface::Configuration& configuration);
  ~ModuleRtpRtcpImpl2() override;

  // This method is provided to easy with migrating away from the
  // RtpRtcp::Create factory method. Since this is an internal implementation
  // detail though, creating an instance of ModuleRtpRtcpImpl2 directly should
  // be fine.
  static std::unique_ptr<ModuleRtpRtcpImpl2> Create(
      const Configuration& configuration);

  // Returns the number of milliseconds until the module want a worker thread to
  // call Process.
  int64_t TimeUntilNextProcess() override;

  // Process any pending tasks such as timeouts.
  void Process() override;

  // Receiver part.

  // Called when we receive an RTCP packet.
  void IncomingRtcpPacket(const uint8_t* incoming_packet,
                          size_t incoming_packet_length) override;

  void SetRemoteSSRC(uint32_t ssrc) override;

  // Sender part.
  void RegisterSendPayloadFrequency(int payload_type,
                                    int payload_frequency) override;

  int32_t DeRegisterSendPayload(int8_t payload_type) override;

  void SetExtmapAllowMixed(bool extmap_allow_mixed) override;

  void RegisterRtpHeaderExtension(absl::string_view uri, int id) override;
  int32_t DeregisterSendRtpHeaderExtension(RTPExtensionType type) override;
  void DeregisterSendRtpHeaderExtension(absl::string_view uri) override;

  bool SupportsPadding() const override;
  bool SupportsRtxPayloadPadding() const override;

  // Get start timestamp.
  uint32_t StartTimestamp() const override;

  // Configure start timestamp, default is a random number.
  void SetStartTimestamp(uint32_t timestamp) override;

  uint16_t SequenceNumber() const override;

  // Set SequenceNumber, default is a random number.
  void SetSequenceNumber(uint16_t seq) override;

  void SetRtpState(const RtpState& rtp_state) override;
  void SetRtxState(const RtpState& rtp_state) override;
  RtpState GetRtpState() const override;
  RtpState GetRtxState() const override;

  uint32_t SSRC() const override { return rtcp_sender_.SSRC(); }

  void SetRid(const std::string& rid) override;

  void SetMid(const std::string& mid) override;

  void SetCsrcs(const std::vector<uint32_t>& csrcs) override;

  RTCPSender::FeedbackState GetFeedbackState();

  void SetRtxSendStatus(int mode) override;
  int RtxSendStatus() const override;
  absl::optional<uint32_t> RtxSsrc() const override;

  void SetRtxSendPayloadType(int payload_type,
                             int associated_payload_type) override;

  absl::optional<uint32_t> FlexfecSsrc() const override;

  // Sends kRtcpByeCode when going from true to false.
  int32_t SetSendingStatus(bool sending) override;

  bool Sending() const override;

  // Drops or relays media packets.
  void SetSendingMediaStatus(bool sending) override;

  bool SendingMedia() const override;

  bool IsAudioConfigured() const override;

  void SetAsPartOfAllocation(bool part_of_allocation) override;

  bool OnSendingRtpFrame(uint32_t timestamp,
                         int64_t capture_time_ms,
                         int payload_type,
                         bool force_sender_report) override;

  bool TrySendPacket(RtpPacketToSend* packet,
                     const PacedPacketInfo& pacing_info) override;

  void SetFecProtectionParams(const FecProtectionParams& delta_params,
                              const FecProtectionParams& key_params) override;

  std::vector<std::unique_ptr<RtpPacketToSend>> FetchFecPackets() override;

  void OnPacketsAcknowledged(
      rtc::ArrayView<const uint16_t> sequence_numbers) override;

  std::vector<std::unique_ptr<RtpPacketToSend>> GeneratePadding(
      size_t target_size_bytes) override;

  std::vector<RtpSequenceNumberMap::Info> GetSentRtpPacketInfos(
      rtc::ArrayView<const uint16_t> sequence_numbers) const override;

  size_t ExpectedPerPacketOverhead() const override;

  // RTCP part.

  // Get RTCP status.
  RtcpMode RTCP() const override;

  // Configure RTCP status i.e on/off.
  void SetRTCPStatus(RtcpMode method) override;

  // Set RTCP CName.
  int32_t SetCNAME(const char* c_name) override;

  // Get remote NTP.
  int32_t RemoteNTP(uint32_t* received_ntp_secs,
                    uint32_t* received_ntp_frac,
                    uint32_t* rtcp_arrival_time_secs,
                    uint32_t* rtcp_arrival_time_frac,
                    uint32_t* rtcp_timestamp) const override;

  // Get RoundTripTime.
  int32_t RTT(uint32_t remote_ssrc,
              int64_t* rtt,
              int64_t* avg_rtt,
              int64_t* min_rtt,
              int64_t* max_rtt) const override;

  int64_t ExpectedRetransmissionTimeMs() const override;

  // Force a send of an RTCP packet.
  // Normal SR and RR are triggered via the process function.
  int32_t SendRTCP(RTCPPacketType rtcpPacketType) override;

  void GetSendStreamDataCounters(
      StreamDataCounters* rtp_counters,
      StreamDataCounters* rtx_counters) const override;

  // A snapshot of the most recent Report Block with additional data of
  // interest to statistics. Used to implement RTCRemoteInboundRtpStreamStats.
  // Within this list, the ReportBlockData::RTCPReportBlock::source_ssrc(),
  // which is the SSRC of the corresponding outbound RTP stream, is unique.
  std::vector<ReportBlockData> GetLatestReportBlockData() const override;
  absl::optional<SenderReportStats> GetSenderReportStats() const override;

  // (REMB) Receiver Estimated Max Bitrate.
  void SetRemb(int64_t bitrate_bps, std::vector<uint32_t> ssrcs) override;
  void UnsetRemb() override;

  void SetTmmbn(std::vector<rtcp::TmmbItem> bounding_set) override;

  size_t MaxRtpPacketSize() const override;

  void SetMaxRtpPacketSize(size_t max_packet_size) override;

  // (NACK) Negative acknowledgment part.

  // Send a Negative acknowledgment packet.
  // TODO(philipel): Deprecate SendNACK and use SendNack instead.
  int32_t SendNACK(const uint16_t* nack_list, uint16_t size) override;

  void SendNack(const std::vector<uint16_t>& sequence_numbers) override;

  // Store the sent packets, needed to answer to a negative acknowledgment
  // requests.
  void SetStorePacketsStatus(bool enable, uint16_t number_to_store) override;

  void SendCombinedRtcpPacket(
      std::vector<std::unique_ptr<rtcp::RtcpPacket>> rtcp_packets) override;

  // Video part.
  int32_t SendLossNotification(uint16_t last_decoded_seq_num,
                               uint16_t last_received_seq_num,
                               bool decodability_flag,
                               bool buffering_allowed) override;

  RtpSendRates GetSendRates() const override;

  void OnReceivedNack(
      const std::vector<uint16_t>& nack_sequence_numbers) override;
  void OnReceivedRtcpReportBlocks(
      const ReportBlockList& report_blocks) override;
  void OnRequestSendReport() override;

  void SetVideoBitrateAllocation(
      const VideoBitrateAllocation& bitrate) override;

  RTPSender* RtpSender() override;
  const RTPSender* RtpSender() const override;

 private:
  FRIEND_TEST_ALL_PREFIXES(RtpRtcpImpl2Test, Rtt);
  FRIEND_TEST_ALL_PREFIXES(RtpRtcpImpl2Test, RttForReceiverOnly);

  struct RtpSenderContext : public SequenceNumberAssigner {
    explicit RtpSenderContext(const RtpRtcpInterface::Configuration& config);
    void AssignSequenceNumber(RtpPacketToSend* packet) override;
    // Storage of packets, for retransmissions and padding, if applicable.
    RtpPacketHistory packet_history;
    // Handles final time timestamping/stats/etc and handover to Transport.
    RtpSenderEgress packet_sender;
    // If no paced sender configured, this class will be used to pass packets
    // from |packet_generator_| to |packet_sender_|.
    RtpSenderEgress::NonPacedPacketSender non_paced_sender;
    // Handles creation of RTP packets to be sent.
    RTPSender packet_generator;
  };

  void set_rtt_ms(int64_t rtt_ms);
  int64_t rtt_ms() const;

  bool TimeToSendFullNackList(int64_t now) const;

  // Called on a timer, once a second, on the worker_queue_, to update the RTT,
  // check if we need to send RTCP report, send TMMBR updates and fire events.
  void PeriodicUpdate();

  // Returns true if the module is configured to store packets.
  bool StorePackets() const;

  TaskQueueBase* const worker_queue_;
  RTC_NO_UNIQUE_ADDRESS SequenceChecker process_thread_checker_;

  std::unique_ptr<RtpSenderContext> rtp_sender_;

  RTCPSender rtcp_sender_;
  RTCPReceiver rtcp_receiver_;

  Clock* const clock_;

  int64_t last_rtt_process_time_;
  int64_t next_process_time_;
  uint16_t packet_overhead_;

  // Send side
  int64_t nack_last_time_sent_full_ms_;
  uint16_t nack_last_seq_number_sent_;

  RemoteBitrateEstimator* const remote_bitrate_;

  RtcpRttStats* const rtt_stats_;
  RepeatingTaskHandle rtt_update_task_ RTC_GUARDED_BY(worker_queue_);

  // The processed RTT from RtcpRttStats.
  mutable Mutex mutex_rtt_;
  int64_t rtt_ms_ RTC_GUARDED_BY(mutex_rtt_);
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_RTP_RTCP_IMPL2_H_
