/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_SDES_H_
#define MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_SDES_H_

#include <string>
#include <vector>

#include "modules/rtp_rtcp/source/rtcp_packet.h"
#include BOSS_WEBRTC_U_rtc_base__basictypes_h //original-code:"rtc_base/basictypes.h"

namespace webrtc {
namespace rtcp {
class CommonHeader;
// Source Description (SDES) (RFC 3550).
class Sdes : public RtcpPacket {
 public:
  struct Chunk {
    uint32_t ssrc;
    std::string cname;
  };
  static constexpr uint8_t kPacketType = 202;
  static constexpr size_t kMaxNumberOfChunks = 0x1f;

  Sdes();
  ~Sdes() override;

  // Parse assumes header is already parsed and validated.
  bool Parse(const CommonHeader& packet);

  bool AddCName(uint32_t ssrc, std::string cname);

  const std::vector<Chunk>& chunks() const { return chunks_; }

  size_t BlockLength() const override;

  bool Create(uint8_t* packet,
              size_t* index,
              size_t max_length,
              PacketReadyCallback callback) const override;

 private:
  std::vector<Chunk> chunks_;
  size_t block_length_;
};
}  // namespace rtcp
}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_RTCP_PACKET_SDES_H_
