// author BOSS

/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_INCLUDE_RECEIVE_STATISTICS_H_
#define MODULES_RTP_RTCP_INCLUDE_RECEIVE_STATISTICS_H_

#include <map>
#include <vector>

#include BOSS_WEBRTC_U_modules__include__module_h //original-code:"modules/include/module.h"
#include BOSS_WEBRTC_U_modules__include__module_common_types_h //original-code:"modules/include/module_common_types.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__report_block_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/report_block.h"

namespace webrtc {

class Clock;

class ReceiveStatisticsProvider {
 public:
  virtual ~ReceiveStatisticsProvider() = default;
  // Collects receive statistic in a form of rtcp report blocks.
  // Returns at most |max_blocks| report blocks.
  virtual std::vector<rtcp::ReportBlock_rtcp_BOSS> RtcpReportBlocks( // modified by BOSS, original-code: ReportBlock, replace-code: ReportBlock_rtcp_BOSS
      size_t max_blocks) = 0;
};

class StreamStatistician {
 public:
  virtual ~StreamStatistician();

  // If |update_fraction_lost| is true, |fraction_lost| will be recomputed
  // between now and the last time |update_fraction_lost| was true. Otherwise
  // the last-computed value of |fraction_lost| will be returned.
  //
  // |update_fraction_lost| should be true any time an RTCP SR or RR is being
  // generated, since RFC3550 defines it as the fraction of packets lost since
  // the previous SR or RR packet was sent.
  //
  // Aside from |fraction_lost|, every other value will be freshly computed.
  virtual bool GetStatistics(RtcpStatistics* statistics,
                             bool update_fraction_lost) = 0;
  virtual void GetDataCounters(size_t* bytes_received,
                               uint32_t* packets_received) const = 0;

  // Gets received stream data counters (includes reset counter values).
  virtual void GetReceiveStreamDataCounters(
      StreamDataCounters* data_counters) const = 0;

  virtual uint32_t BitrateReceived() const = 0;

  // Returns true if the packet with RTP header |header| is likely to be a
  // retransmitted packet, false otherwise.
  virtual bool IsRetransmitOfOldPacket(const RTPHeader& header) const = 0;
};

class ReceiveStatistics : public ReceiveStatisticsProvider {
 public:
  ~ReceiveStatistics() override = default;

  static ReceiveStatistics* Create(Clock* clock);

  // Updates the receive statistics with this packet.
  virtual void IncomingPacket(const RTPHeader& rtp_header,
                              size_t packet_length,
                              bool retransmitted) = 0;

  // Increment counter for number of FEC packets received.
  virtual void FecPacketReceived(const RTPHeader& header,
                                 size_t packet_length) = 0;

  // Returns a pointer to the statistician of an ssrc.
  virtual StreamStatistician* GetStatistician(uint32_t ssrc) const = 0;

  // Sets the max reordering threshold in number of packets.
  virtual void SetMaxReorderingThreshold(int max_reordering_threshold) = 0;

  // Called on new RTCP stats creation.
  virtual void RegisterRtcpStatisticsCallback(
      RtcpStatisticsCallback* callback) = 0;

  // Called on new RTP stats creation.
  virtual void RegisterRtpStatisticsCallback(
      StreamDataCountersCallback* callback) = 0;
};

}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_INCLUDE_RECEIVE_STATISTICS_H_
