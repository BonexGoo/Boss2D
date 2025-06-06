/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"

#include <ctype.h>
#include <string.h>

#include <type_traits>

#include BOSS_ABSEILCPP_U_absl__algorithm__container_h //original-code:"absl/algorithm/container.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_h //original-code:"modules/rtp_rtcp/source/rtp_packet.h"

namespace webrtc {

namespace {
constexpr size_t kMidRsidMaxSize = 16;

// Check if passed character is a "token-char" from RFC 4566.
bool IsTokenChar(char ch) {
  return ch == 0x21 || (ch >= 0x23 && ch <= 0x27) || ch == 0x2a || ch == 0x2b ||
         ch == 0x2d || ch == 0x2e || (ch >= 0x30 && ch <= 0x39) ||
         (ch >= 0x41 && ch <= 0x5a) || (ch >= 0x5e && ch <= 0x7e);
}
}  // namespace

bool IsLegalMidName(absl::string_view name) {
  return (name.size() <= kMidRsidMaxSize && !name.empty() &&
          absl::c_all_of(name, IsTokenChar));
}

bool IsLegalRsidName(absl::string_view name) {
  return (name.size() <= kMidRsidMaxSize && !name.empty() &&
          absl::c_all_of(name, isalnum));
}

StreamDataCounters::StreamDataCounters() : first_packet_time_ms(-1) {}

RtpPacketCounter::RtpPacketCounter(const RtpPacket& packet)
    : header_bytes(packet.headers_size()),
      payload_bytes(packet.payload_size()),
      padding_bytes(packet.padding_size()),
      packets(1) {}

void RtpPacketCounter::AddPacket(const RtpPacket& packet) {
  ++packets;
  header_bytes += packet.headers_size();
  padding_bytes += packet.padding_size();
  payload_bytes += packet.payload_size();
}

}  // namespace webrtc
