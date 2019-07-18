// author BOSS

/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_RECEIVER_REPORT_H_
#define MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_RECEIVER_REPORT_H_

#include <vector>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet_h //original-code:"modules/rtp_rtcp/source/rtcp_packet.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__report_block_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/report_block.h"

namespace webrtc {
namespace rtcp {
class CommonHeader;

class ReceiverReport : public RtcpPacket {
 public:
  static constexpr uint8_t kPacketType = 201;
  static constexpr size_t kMaxNumberOfReportBlocks = 0x1f;

  ReceiverReport();
  ~ReceiverReport() override;

  // Parse assumes header is already parsed and validated.
  bool Parse(const CommonHeader& packet);

  void SetSenderSsrc(uint32_t ssrc) { sender_ssrc_ = ssrc; }
  bool AddReportBlock(const ReportBlock_rtcp_BOSS& block); // modified by BOSS, original-code: ReportBlock, replace-code: ReportBlock_rtcp_BOSS
  bool SetReportBlocks(std::vector<ReportBlock_rtcp_BOSS> blocks); // modified by BOSS, original-code: ReportBlock, replace-code: ReportBlock_rtcp_BOSS

  uint32_t sender_ssrc() const { return sender_ssrc_; }
  const std::vector<ReportBlock_rtcp_BOSS>& report_blocks() const { // modified by BOSS, original-code: ReportBlock, replace-code: ReportBlock_rtcp_BOSS
    return report_blocks_;
  }

  size_t BlockLength() const override;

  bool Create(uint8_t* packet,
              size_t* index,
              size_t max_length,
              PacketReadyCallback callback) const override;

 private:
  static const size_t kRrBaseLength = 4;

  uint32_t sender_ssrc_;
  std::vector<ReportBlock_rtcp_BOSS> report_blocks_; // modified by BOSS, original-code: ReportBlock, replace-code: ReportBlock_rtcp_BOSS
};

}  // namespace rtcp
}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_RECEIVER_REPORT_H_
