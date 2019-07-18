/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_call__rtp_rtcp_demuxer_helper_h //original-code:"call/rtp_rtcp_demuxer_helper.h"

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__byte_io_h //original-code:"modules/rtp_rtcp/source/byte_io.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__bye_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/bye.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__common_header_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/common_header.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__extended_reports_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/extended_reports.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__psfb_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/psfb.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__receiver_report_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/receiver_report.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__rtpfb_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/rtpfb.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtcp_packet__sender_report_h //original-code:"modules/rtp_rtcp/source/rtcp_packet/sender_report.h"

namespace webrtc {

absl::optional<uint32_t> ParseRtcpPacketSenderSsrc(
    rtc::ArrayView<const uint8_t> packet) {
  rtcp::CommonHeader header;
  for (const uint8_t* next_packet = packet.begin(); next_packet < packet.end();
       next_packet = header.NextPacket()) {
    if (!header.Parse(next_packet, packet.end() - next_packet)) {
      return absl::nullopt;
    }

    switch (header.type()) {
      case rtcp::Bye::kPacketType:
      case rtcp::ExtendedReports::kPacketType:
      case rtcp::Psfb::kPacketType:
      case rtcp::ReceiverReport::kPacketType:
      case rtcp::Rtpfb::kPacketType:
      case rtcp::SenderReport::kPacketType: {
        // Sender SSRC at the beginning of the RTCP payload.
        if (header.payload_size_bytes() >= sizeof(uint32_t)) {
          const uint32_t ssrc_sender =
              ByteReader<uint32_t>::ReadBigEndian(header.payload());
          return ssrc_sender;
        } else {
          return absl::nullopt;
        }
      }
    }
  }

  return absl::nullopt;
}

}  // namespace webrtc
