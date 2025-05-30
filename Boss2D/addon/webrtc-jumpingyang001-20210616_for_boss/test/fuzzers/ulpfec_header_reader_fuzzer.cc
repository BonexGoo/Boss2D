/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <algorithm>

#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__forward_error_correction_h //original-code:"modules/rtp_rtcp/source/forward_error_correction.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__ulpfec_header_reader_writer_h //original-code:"modules/rtp_rtcp/source/ulpfec_header_reader_writer.h"

namespace webrtc {

using Packet = ForwardErrorCorrection::Packet;
using ReceivedFecPacket = ForwardErrorCorrection::ReceivedFecPacket;

void FuzzOneInput(const uint8_t* data, size_t size) {
  ReceivedFecPacket packet;
  packet.pkt = rtc::scoped_refptr<Packet>(new Packet());
  const size_t packet_size =
      std::min(size, static_cast<size_t>(IP_PACKET_SIZE));
  packet.pkt->data.SetSize(packet_size);
  packet.pkt->data.EnsureCapacity(IP_PACKET_SIZE);
  memcpy(packet.pkt->data.MutableData(), data, packet_size);

  UlpfecHeaderReader ulpfec_reader;
  ulpfec_reader.ReadFecHeader(&packet);
}

}  // namespace webrtc
