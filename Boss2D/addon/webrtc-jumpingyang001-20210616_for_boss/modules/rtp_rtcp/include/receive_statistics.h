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
#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_call__rtp_packet_sink_interface_h //original-code:"call/rtp_packet_sink_interface.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtcp_statistics_h //original-code:"modules/rtp_rtcp/include/rtcp_statistics.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__report_block_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/report_block.h"

namespace webrtc {

class Clock;

class ReceiveStatisticsProvider {
 public:
  virtual ~ReceiveStatisticsProvider() = default;
  // Collects receive statistic in a form of rtcp report blocks.
  // Returns at most |max_blocks| report blocks.
  virtual std::vector<rtcp::ReportBlock> RtcpReportBlocks(
      size_t max_blocks) = 0;
};

class StreamStatistician {
 public:
  virtual ~StreamStatistician();

  virtual RtpReceiveStats GetStats() const = 0;

  // Returns average over the stream life time.
  virtual absl::optional<int> GetFractionLostInPercent() const = 0;

  // TODO(nisse): Delete, migrate users to the above the GetStats method.
  // Gets received stream data counters (includes reset counter values).
  virtual StreamDataCounters GetReceiveStreamDataCounters() const = 0;

  virtual uint32_t BitrateReceived() const = 0;
};

class ReceiveStatistics : public ReceiveStatisticsProvider,
                          public RtpPacketSinkInterface {
 public:
  ~ReceiveStatistics() override = default;

  // Returns a thread-safe instance of ReceiveStatistics.
  // https://chromium.googlesource.com/chromium/src/+/lkgr/docs/threading_and_tasks.md#threading-lexicon
  static std::unique_ptr<ReceiveStatistics> Create(Clock* clock);
  // Returns a thread-compatible instance of ReceiveStatistics.
  static std::unique_ptr<ReceiveStatistics> CreateThreadCompatible(
      Clock* clock);

  // Returns a pointer to the statistician of an ssrc.
  virtual StreamStatistician* GetStatistician(uint32_t ssrc) const = 0;

  // TODO(bugs.webrtc.org/10669): Deprecated, delete as soon as downstream
  // projects are updated. This method sets the max reordering threshold of all
  // current and future streams.
  virtual void SetMaxReorderingThreshold(int max_reordering_threshold) = 0;

  // Sets the max reordering threshold in number of packets.
  virtual void SetMaxReorderingThreshold(uint32_t ssrc,
                                         int max_reordering_threshold) = 0;
  // Detect retransmissions, enabling updates of the retransmitted counters. The
  // default is false.
  virtual void EnableRetransmitDetection(uint32_t ssrc, bool enable) = 0;
};

}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_INCLUDE_RECEIVE_STATISTICS_H_
