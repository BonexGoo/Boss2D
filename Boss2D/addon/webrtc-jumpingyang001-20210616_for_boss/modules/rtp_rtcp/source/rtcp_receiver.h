/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_RTCP_RECEIVER_H_
#define MODULES_RTP_RTCP_SOURCE_RTCP_RECEIVER_H_

#include <list>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__report_block_data_h //original-code:"modules/rtp_rtcp/include/report_block_data.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtcp_statistics_h //original-code:"modules/rtp_rtcp/include/rtcp_statistics.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_nack_stats_h //original-code:"modules/rtp_rtcp/source/rtcp_nack_stats.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__dlrr_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/dlrr.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__tmmb_item_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/tmmb_item.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_rtcp_interface_h //original-code:"modules/rtp_rtcp/source/rtp_rtcp_interface.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"
#include BOSS_WEBRTC_U_system_wrappers__include__ntp_time_h //original-code:"system_wrappers/include/ntp_time.h"

namespace webrtc {
class VideoBitrateAllocationObserver;
namespace rtcp {
class CommonHeader;
class ReportBlock;
class Rrtr;
class TargetBitrate;
class TmmbItem;
}  // namespace rtcp

class RTCPReceiver final {
 public:
  class ModuleRtpRtcp {
   public:
    virtual void SetTmmbn(std::vector<rtcp::TmmbItem> bounding_set) = 0;
    virtual void OnRequestSendReport() = 0;
    virtual void OnReceivedNack(
        const std::vector<uint16_t>& nack_sequence_numbers) = 0;
    virtual void OnReceivedRtcpReportBlocks(
        const ReportBlockList& report_blocks) = 0;

   protected:
    virtual ~ModuleRtpRtcp() = default;
  };

  RTCPReceiver(const RtpRtcpInterface::Configuration& config,
               ModuleRtpRtcp* owner);
  ~RTCPReceiver();

  void IncomingPacket(const uint8_t* packet, size_t packet_size) {
    IncomingPacket(rtc::MakeArrayView(packet, packet_size));
  }
  void IncomingPacket(rtc::ArrayView<const uint8_t> packet);

  int64_t LastReceivedReportBlockMs() const;

  void SetRemoteSSRC(uint32_t ssrc);
  uint32_t RemoteSSRC() const;

  // Get received NTP.
  // The types for the arguments below derive from the specification:
  // - `remote_sender_packet_count`: `RTCSentRtpStreamStats.packetsSent` [1]
  // - `remote_sender_octet_count`: `RTCSentRtpStreamStats.bytesSent` [1]
  // - `remote_sender_reports_count`:
  //   `RTCRemoteOutboundRtpStreamStats.reportsSent` [2]
  // [1] https://www.w3.org/TR/webrtc-stats/#remoteoutboundrtpstats-dict*
  // [2] https://www.w3.org/TR/webrtc-stats/#dom-rtcsentrtpstreamstats
  bool NTP(uint32_t* received_ntp_secs,
           uint32_t* received_ntp_frac,
           uint32_t* rtcp_arrival_time_secs,
           uint32_t* rtcp_arrival_time_frac,
           uint32_t* rtcp_timestamp,
           uint32_t* remote_sender_packet_count,
           uint64_t* remote_sender_octet_count,
           uint64_t* remote_sender_reports_count) const;

  std::vector<rtcp::ReceiveTimeInfo> ConsumeReceivedXrReferenceTimeInfo();

  // Get rtt.
  int32_t RTT(uint32_t remote_ssrc,
              int64_t* last_rtt_ms,
              int64_t* avg_rtt_ms,
              int64_t* min_rtt_ms,
              int64_t* max_rtt_ms) const;

  bool GetAndResetXrRrRtt(int64_t* rtt_ms);

  // Called once per second on the worker thread to do rtt calculations.
  // Returns an optional rtt value if one is available.
  absl::optional<TimeDelta> OnPeriodicRttUpdate(Timestamp newer_than,
                                                bool sending);

  // A snapshot of Report Blocks with additional data of interest to statistics.
  // Within this list, the sender-source SSRC pair is unique and per-pair the
  // ReportBlockData represents the latest Report Block that was received for
  // that pair.
  std::vector<ReportBlockData> GetLatestReportBlockData() const;

  // Returns true if we haven't received an RTCP RR for several RTCP
  // intervals, but only triggers true once.
  bool RtcpRrTimeout();

  // Returns true if we haven't received an RTCP RR telling the receive side
  // has not received RTP packets for too long, i.e. extended highest sequence
  // number hasn't increased for several RTCP intervals. The function only
  // returns true once until a new RR is received.
  bool RtcpRrSequenceNumberTimeout();

  std::vector<rtcp::TmmbItem> TmmbrReceived();
  // Return true if new bandwidth should be set.
  bool UpdateTmmbrTimers();
  std::vector<rtcp::TmmbItem> BoundingSet(bool* tmmbr_owner);
  // Set new bandwidth and notify remote clients about it.
  void NotifyTmmbrUpdated();

 private:
  // A lightweight inlined set of local SSRCs.
  class RegisteredSsrcs {
   public:
    static constexpr size_t kMaxSsrcs = 3;
    // Initializes the set of registered local SSRCS by extracting them from the
    // provided `config`.
    explicit RegisteredSsrcs(const RtpRtcpInterface::Configuration& config);

    // Indicates if `ssrc` is in the set of registered local SSRCs.
    bool contains(uint32_t ssrc) const {
      return absl::c_linear_search(ssrcs_, ssrc);
    }

   private:
    absl::InlinedVector<uint32_t, kMaxSsrcs> ssrcs_;
  };

  struct PacketInformation;

  // Structure for handing TMMBR and TMMBN rtcp messages (RFC5104,
  // section 3.5.4).
  struct TmmbrInformation {
    struct TimedTmmbrItem {
      rtcp::TmmbItem tmmbr_item;
      int64_t last_updated_ms;
    };

    int64_t last_time_received_ms = 0;

    bool ready_for_delete = false;

    std::vector<rtcp::TmmbItem> tmmbn;
    std::map<uint32_t, TimedTmmbrItem> tmmbr;
  };

  // Structure for storing received RRTR RTCP messages (RFC3611, section 4.4).
  struct RrtrInformation {
    RrtrInformation(uint32_t ssrc,
                    uint32_t received_remote_mid_ntp_time,
                    uint32_t local_receive_mid_ntp_time)
        : ssrc(ssrc),
          received_remote_mid_ntp_time(received_remote_mid_ntp_time),
          local_receive_mid_ntp_time(local_receive_mid_ntp_time) {}

    uint32_t ssrc;
    // Received NTP timestamp in compact representation.
    uint32_t received_remote_mid_ntp_time;
    // NTP time when the report was received in compact representation.
    uint32_t local_receive_mid_ntp_time;
  };

  struct LastFirStatus {
    LastFirStatus(int64_t now_ms, uint8_t sequence_number)
        : request_ms(now_ms), sequence_number(sequence_number) {}
    int64_t request_ms;
    uint8_t sequence_number;
  };

  // TODO(boivie): `ReportBlockDataMap` and `ReportBlockMap` should be converted
  // to std::unordered_map, but as there are too many tests that assume a
  // specific order, it's not easily done.

  // RTCP report blocks mapped by remote SSRC.
  using ReportBlockDataMap = std::map<uint32_t, ReportBlockData>;
  // RTCP report blocks map mapped by source SSRC.
  using ReportBlockMap = std::map<uint32_t, ReportBlockDataMap>;

  bool ParseCompoundPacket(rtc::ArrayView<const uint8_t> packet,
                           PacketInformation* packet_information);

  void TriggerCallbacksFromRtcpPacket(
      const PacketInformation& packet_information);

  TmmbrInformation* FindOrCreateTmmbrInfo(uint32_t remote_ssrc)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);
  // Update TmmbrInformation (if present) is alive.
  void UpdateTmmbrRemoteIsAlive(uint32_t remote_ssrc)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);
  TmmbrInformation* GetTmmbrInformation(uint32_t remote_ssrc)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleSenderReport(const rtcp::CommonHeader& rtcp_block,
                          PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleReceiverReport(const rtcp::CommonHeader& rtcp_block,
                            PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleReportBlock(const rtcp::ReportBlock& report_block,
                         PacketInformation* packet_information,
                         uint32_t remote_ssrc)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleSdes(const rtcp::CommonHeader& rtcp_block,
                  PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleXr(const rtcp::CommonHeader& rtcp_block,
                PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleXrReceiveReferenceTime(uint32_t sender_ssrc,
                                    const rtcp::Rrtr& rrtr)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleXrDlrrReportBlock(const rtcp::ReceiveTimeInfo& rti)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleXrTargetBitrate(uint32_t ssrc,
                             const rtcp::TargetBitrate& target_bitrate,
                             PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleNack(const rtcp::CommonHeader& rtcp_block,
                  PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleApp(const rtcp::CommonHeader& rtcp_block,
                 PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleBye(const rtcp::CommonHeader& rtcp_block)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandlePli(const rtcp::CommonHeader& rtcp_block,
                 PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandlePsfbApp(const rtcp::CommonHeader& rtcp_block,
                     PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleTmmbr(const rtcp::CommonHeader& rtcp_block,
                   PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleTmmbn(const rtcp::CommonHeader& rtcp_block,
                   PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleSrReq(const rtcp::CommonHeader& rtcp_block,
                   PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleFir(const rtcp::CommonHeader& rtcp_block,
                 PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  void HandleTransportFeedback(const rtcp::CommonHeader& rtcp_block,
                               PacketInformation* packet_information)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  bool RtcpRrTimeoutLocked(Timestamp now)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  bool RtcpRrSequenceNumberTimeoutLocked(Timestamp now)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(rtcp_receiver_lock_);

  Clock* const clock_;
  const bool receiver_only_;
  ModuleRtpRtcp* const rtp_rtcp_;
  const uint32_t main_ssrc_;
  // The set of registered local SSRCs.
  const RegisteredSsrcs registered_ssrcs_;

  RtcpBandwidthObserver* const rtcp_bandwidth_observer_;
  RtcpIntraFrameObserver* const rtcp_intra_frame_observer_;
  RtcpLossNotificationObserver* const rtcp_loss_notification_observer_;
  NetworkStateEstimateObserver* const network_state_estimate_observer_;
  TransportFeedbackObserver* const transport_feedback_observer_;
  VideoBitrateAllocationObserver* const bitrate_allocation_observer_;
  const TimeDelta report_interval_;

  mutable Mutex rtcp_receiver_lock_;
  uint32_t remote_ssrc_ RTC_GUARDED_BY(rtcp_receiver_lock_);

  // Received sender report.
  NtpTime remote_sender_ntp_time_ RTC_GUARDED_BY(rtcp_receiver_lock_);
  uint32_t remote_sender_rtp_time_ RTC_GUARDED_BY(rtcp_receiver_lock_);
  // When did we receive the last send report.
  NtpTime last_received_sr_ntp_ RTC_GUARDED_BY(rtcp_receiver_lock_);
  uint32_t remote_sender_packet_count_ RTC_GUARDED_BY(rtcp_receiver_lock_);
  uint64_t remote_sender_octet_count_ RTC_GUARDED_BY(rtcp_receiver_lock_);
  uint64_t remote_sender_reports_count_ RTC_GUARDED_BY(rtcp_receiver_lock_);

  // Received RRTR information in ascending receive time order.
  std::list<RrtrInformation> received_rrtrs_
      RTC_GUARDED_BY(rtcp_receiver_lock_);
  // Received RRTR information mapped by remote ssrc.
  std::unordered_map<uint32_t, std::list<RrtrInformation>::iterator>
      received_rrtrs_ssrc_it_ RTC_GUARDED_BY(rtcp_receiver_lock_);

  // Estimated rtt, zero when there is no valid estimate.
  const bool xr_rrtr_status_;
  int64_t xr_rr_rtt_ms_;

  int64_t oldest_tmmbr_info_ms_ RTC_GUARDED_BY(rtcp_receiver_lock_);
  // Mapped by remote ssrc.
  std::unordered_map<uint32_t, TmmbrInformation> tmmbr_infos_
      RTC_GUARDED_BY(rtcp_receiver_lock_);

  ReportBlockMap received_report_blocks_ RTC_GUARDED_BY(rtcp_receiver_lock_);
  std::unordered_map<uint32_t, LastFirStatus> last_fir_
      RTC_GUARDED_BY(rtcp_receiver_lock_);

  // The last time we received an RTCP Report block for this module.
  Timestamp last_received_rb_ RTC_GUARDED_BY(rtcp_receiver_lock_) =
      Timestamp::PlusInfinity();

  // The time we last received an RTCP RR telling we have successfully
  // delivered RTP packet to the remote side.
  Timestamp last_increased_sequence_number_ = Timestamp::PlusInfinity();

  RtcpCnameCallback* const cname_callback_;
  ReportBlockDataObserver* const report_block_data_observer_;

  RtcpPacketTypeCounterObserver* const packet_type_counter_observer_;
  RtcpPacketTypeCounter packet_type_counter_;

  RtcpNackStats nack_stats_;

  size_t num_skipped_packets_;
  int64_t last_skipped_packets_warning_ms_;
};
}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_RTCP_RECEIVER_H_
