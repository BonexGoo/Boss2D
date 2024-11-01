// author BOSS

/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_RTCP_SENDER_H_
#define MODULES_RTP_RTCP_SOURCE_RTCP_SENDER_H_

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__call__transport_h //original-code:"api/call/transport.h"
#include BOSS_WEBRTC_U_api__video__video_bitrate_allocation_h //original-code:"api/video/video_bitrate_allocation.h"
#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__include__bwe_defines_h //original-code:"modules/remote_bitrate_estimator/include/bwe_defines.h"
#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__include__remote_bitrate_estimator_h //original-code:"modules/remote_bitrate_estimator/include/remote_bitrate_estimator.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__receive_statistics_h //original-code:"modules/rtp_rtcp/include/receive_statistics.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_nack_stats_h //original-code:"modules/rtp_rtcp/source/rtcp_nack_stats.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet_h //original-code:"modules/rtp_rtcp/source/rtcp_packet.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__dlrr_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/dlrr.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__report_block_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/report_block.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__tmmb_item_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/tmmb_item.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"
#include BOSS_WEBRTC_U_rtc_base__criticalsection_h //original-code:"rtc_base/criticalsection.h"
#include BOSS_WEBRTC_U_rtc_base__random_h //original-code:"rtc_base/random.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

class ModuleRtpRtcpImpl;
class RtcEventLog;

class RTCPSender {
 public:
  struct FeedbackState {
    FeedbackState();
    FeedbackState(const FeedbackState&);
    FeedbackState(FeedbackState&&);

    ~FeedbackState();

    uint32_t packets_sent;
    size_t media_bytes_sent;
    uint32_t send_bitrate;

    uint32_t last_rr_ntp_secs;
    uint32_t last_rr_ntp_frac;
    uint32_t remote_sr;

    std::vector<rtcp::ReceiveTimeInfo> last_xr_rtis;

    // Used when generating TMMBR.
    ModuleRtpRtcpImpl* module;
  };

  RTCPSender(bool audio,
             Clock* clock,
             ReceiveStatisticsProvider* receive_statistics,
             RtcpPacketTypeCounterObserver* packet_type_counter_observer,
             RtcEventLog* event_log,
             Transport* outgoing_transport,
             RtcpIntervalConfig interval_config);
  virtual ~RTCPSender();

  RtcpMode Status() const;
  void SetRTCPStatus(RtcpMode method);

  bool Sending() const;
  int32_t SetSendingStatus(const FeedbackState& feedback_state,
                           bool enabled);  // combine the functions

  int32_t SetNackStatus(bool enable);

  void SetTimestampOffset(uint32_t timestamp_offset);

  void SetLastRtpTime(uint32_t rtp_timestamp, int64_t capture_time_ms);

  uint32_t SSRC() const;

  void SetSSRC(uint32_t ssrc);

  void SetRemoteSSRC(uint32_t ssrc);

  int32_t SetCNAME(const char* cName);

  int32_t AddMixedCNAME(uint32_t SSRC, const char* c_name);

  int32_t RemoveMixedCNAME(uint32_t SSRC);

  bool TimeToSendRTCPReport(bool sendKeyframeBeforeRTP = false) const;

  int32_t SendRTCP(const FeedbackState& feedback_state,
                   RTCPPacketType packetType,
                   int32_t nackSize = 0,
                   const uint16_t* nackList = 0);

  int32_t SendCompoundRTCP(const FeedbackState& feedback_state,
                           const std::set<RTCPPacketType>& packetTypes,
                           int32_t nackSize = 0,
                           const uint16_t* nackList = 0);

  void SetRemb(int64_t bitrate_bps, std::vector<uint32_t> ssrcs);

  void UnsetRemb();

  bool TMMBR() const;

  void SetTMMBRStatus(bool enable);

  void SetMaxRtpPacketSize(size_t max_packet_size);

  void SetTmmbn(std::vector<rtcp::TmmbItem> bounding_set);

  int32_t SetApplicationSpecificData(uint8_t subType,
                                     uint32_t name,
                                     const uint8_t* data,
                                     uint16_t length);
  int32_t SetRTCPVoIPMetrics(const RTCPVoIPMetric* VoIPMetric);

  void SendRtcpXrReceiverReferenceTime(bool enable);

  bool RtcpXrReceiverReferenceTime() const;

  void SetCsrcs(const std::vector<uint32_t>& csrcs);

  void SetTargetBitrate(unsigned int target_bitrate);
  void SetVideoBitrateAllocation(const VideoBitrateAllocation& bitrate);
  bool SendFeedbackPacket(const rtcp::TransportFeedback& packet);

  int64_t RtcpAudioReportInverval() const;
  int64_t RtcpVideoReportInverval() const;

 private:
  class RtcpContext;

  // Determine which RTCP messages should be sent and setup flags.
  void PrepareReport(const FeedbackState& feedback_state)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);

  std::vector<rtcp::ReportBlock_rtcp_BOSS> CreateReportBlocks( // modified by BOSS, original-code: ReportBlock, replace-code: ReportBlock_rtcp_BOSS
      const FeedbackState& feedback_state)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);

  std::unique_ptr<rtcp::RtcpPacket> BuildSR(const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  std::unique_ptr<rtcp::RtcpPacket> BuildRR(const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  std::unique_ptr<rtcp::RtcpPacket> BuildSDES(const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  std::unique_ptr<rtcp::RtcpPacket> BuildPLI(const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  std::unique_ptr<rtcp::RtcpPacket> BuildREMB(const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  std::unique_ptr<rtcp::RtcpPacket> BuildTMMBR(const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  std::unique_ptr<rtcp::RtcpPacket> BuildTMMBN(const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  std::unique_ptr<rtcp::RtcpPacket> BuildAPP(const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  std::unique_ptr<rtcp::RtcpPacket> BuildExtendedReports(
      const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  std::unique_ptr<rtcp::RtcpPacket> BuildBYE(const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  std::unique_ptr<rtcp::RtcpPacket> BuildFIR(const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  std::unique_ptr<rtcp::RtcpPacket> BuildNACK(const RtcpContext& context)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);

 private:
  const bool audio_;
  Clock* const clock_;
  Random random_ RTC_GUARDED_BY(critical_section_rtcp_sender_);
  RtcpMode method_ RTC_GUARDED_BY(critical_section_rtcp_sender_);

  RtcEventLog* const event_log_;
  Transport* const transport_;

  const RtcpIntervalConfig interval_config_;

  rtc::CriticalSection critical_section_rtcp_sender_;
  bool using_nack_ RTC_GUARDED_BY(critical_section_rtcp_sender_);
  bool sending_ RTC_GUARDED_BY(critical_section_rtcp_sender_);

  int64_t next_time_to_send_rtcp_ RTC_GUARDED_BY(critical_section_rtcp_sender_);

  uint32_t timestamp_offset_ RTC_GUARDED_BY(critical_section_rtcp_sender_);
  uint32_t last_rtp_timestamp_ RTC_GUARDED_BY(critical_section_rtcp_sender_);
  int64_t last_frame_capture_time_ms_
      RTC_GUARDED_BY(critical_section_rtcp_sender_);
  uint32_t ssrc_ RTC_GUARDED_BY(critical_section_rtcp_sender_);
  // SSRC that we receive on our RTP channel
  uint32_t remote_ssrc_ RTC_GUARDED_BY(critical_section_rtcp_sender_);
  std::string cname_ RTC_GUARDED_BY(critical_section_rtcp_sender_);

  ReceiveStatisticsProvider* receive_statistics_
      RTC_GUARDED_BY(critical_section_rtcp_sender_);
  std::map<uint32_t, std::string> csrc_cnames_
      RTC_GUARDED_BY(critical_section_rtcp_sender_);

  // send CSRCs
  std::vector<uint32_t> csrcs_ RTC_GUARDED_BY(critical_section_rtcp_sender_);

  // Full intra request
  uint8_t sequence_number_fir_ RTC_GUARDED_BY(critical_section_rtcp_sender_);

  // REMB
  int64_t remb_bitrate_ RTC_GUARDED_BY(critical_section_rtcp_sender_);
  std::vector<uint32_t> remb_ssrcs_
      RTC_GUARDED_BY(critical_section_rtcp_sender_);

  std::vector<rtcp::TmmbItem> tmmbn_to_send_
      RTC_GUARDED_BY(critical_section_rtcp_sender_);
  uint32_t tmmbr_send_bps_ RTC_GUARDED_BY(critical_section_rtcp_sender_);
  uint32_t packet_oh_send_ RTC_GUARDED_BY(critical_section_rtcp_sender_);
  size_t max_packet_size_ RTC_GUARDED_BY(critical_section_rtcp_sender_);

  // APP
  uint8_t app_sub_type_ RTC_GUARDED_BY(critical_section_rtcp_sender_);
  uint32_t app_name_ RTC_GUARDED_BY(critical_section_rtcp_sender_);
  std::unique_ptr<uint8_t[]> app_data_
      RTC_GUARDED_BY(critical_section_rtcp_sender_);
  uint16_t app_length_ RTC_GUARDED_BY(critical_section_rtcp_sender_);

  // True if sending of XR Receiver reference time report is enabled.
  bool xr_send_receiver_reference_time_enabled_
      RTC_GUARDED_BY(critical_section_rtcp_sender_);

  // XR VoIP metric
  absl::optional<RTCPVoIPMetric> xr_voip_metric_
      RTC_GUARDED_BY(critical_section_rtcp_sender_);

  RtcpPacketTypeCounterObserver* const packet_type_counter_observer_;
  RtcpPacketTypeCounter packet_type_counter_
      RTC_GUARDED_BY(critical_section_rtcp_sender_);

  RtcpNackStats nack_stats_ RTC_GUARDED_BY(critical_section_rtcp_sender_);

  absl::optional<VideoBitrateAllocation> video_bitrate_allocation_
      RTC_GUARDED_BY(critical_section_rtcp_sender_);

  void SetFlag(uint32_t type, bool is_volatile)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  void SetFlags(const std::set<RTCPPacketType>& types, bool is_volatile)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  bool IsFlagPresent(uint32_t type) const
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  bool ConsumeFlag(uint32_t type, bool forced = false)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  bool AllVolatileFlagsConsumed() const
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critical_section_rtcp_sender_);
  struct ReportFlag {
    ReportFlag(uint32_t type, bool is_volatile)
        : type(type), is_volatile(is_volatile) {}
    bool operator<(const ReportFlag& flag) const { return type < flag.type; }
    bool operator==(const ReportFlag& flag) const { return type == flag.type; }
    const uint32_t type;
    const bool is_volatile;
  };

  std::set<ReportFlag> report_flags_
      RTC_GUARDED_BY(critical_section_rtcp_sender_);

  typedef std::unique_ptr<rtcp::RtcpPacket> (RTCPSender::*BuilderFunc)(
      const RtcpContext&);
  // Map from RTCPPacketType to builder.
  std::map<uint32_t, BuilderFunc> builders_;

  RTC_DISALLOW_IMPLICIT_CONSTRUCTORS(RTCPSender);
};
}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_RTCP_SENDER_H_
