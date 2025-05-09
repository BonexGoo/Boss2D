/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_INCLUDE_ULPFEC_RECEIVER_H_
#define MODULES_RTP_RTCP_INCLUDE_ULPFEC_RECEIVER_H_

#include <memory>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__rtp_parameters_h //original-code:"api/rtp_parameters.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_received_h //original-code:"modules/rtp_rtcp/source/rtp_packet_received.h"

namespace webrtc {

struct FecPacketCounter {
  FecPacketCounter() = default;
  size_t num_packets = 0;  // Number of received packets.
  size_t num_bytes = 0;
  size_t num_fec_packets = 0;  // Number of received FEC packets.
  size_t num_recovered_packets =
      0;  // Number of recovered media packets using FEC.
  int64_t first_packet_time_ms = -1;  // Time when first packet is received.
};

class UlpfecReceiver {
 public:
  static std::unique_ptr<UlpfecReceiver> Create(
      uint32_t ssrc,
      RecoveredPacketReceiver* callback,
      rtc::ArrayView<const RtpExtension> extensions);

  virtual ~UlpfecReceiver() {}

  // Takes a RED packet, strips the RED header, and adds the resulting
  // "virtual" RTP packet(s) into the internal buffer.
  //
  // TODO(brandtr): Set |ulpfec_payload_type| during constructor call,
  // rather than as a parameter here.
  virtual bool AddReceivedRedPacket(const RtpPacketReceived& rtp_packet,
                                    uint8_t ulpfec_payload_type) = 0;

  // Sends the received packets to the FEC and returns all packets
  // (both original media and recovered) through the callback.
  virtual int32_t ProcessReceivedFec() = 0;

  // Returns a counter describing the added and recovered packets.
  virtual FecPacketCounter GetPacketCounter() const = 0;
};
}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_INCLUDE_ULPFEC_RECEIVER_H_
