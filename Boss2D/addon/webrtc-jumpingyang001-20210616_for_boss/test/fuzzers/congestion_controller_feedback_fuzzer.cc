/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_ABSEILCPP_U_absl__functional__bind_front_h //original-code:"absl/functional/bind_front.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__include__receive_side_congestion_controller_h //original-code:"modules/congestion_controller/include/receive_side_congestion_controller.h"
#include BOSS_WEBRTC_U_modules__pacing__packet_router_h //original-code:"modules/pacing/packet_router.h"
#include BOSS_WEBRTC_U_modules__remote_bitrate_estimator__include__remote_bitrate_estimator_h //original-code:"modules/remote_bitrate_estimator/include/remote_bitrate_estimator.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__byte_io_h //original-code:"modules/rtp_rtcp/source/byte_io.h"

namespace webrtc {

void FuzzOneInput(const uint8_t* data, size_t size) {
  size_t i = 0;
  if (size < sizeof(int64_t) + sizeof(uint8_t) + sizeof(uint32_t))
    return;
  SimulatedClock clock(data[i++]);
  PacketRouter packet_router;
  ReceiveSideCongestionController cc(
      &clock,
      absl::bind_front(&PacketRouter::SendCombinedRtcpPacket, &packet_router),
      absl::bind_front(&PacketRouter::SendRemb, &packet_router), nullptr);
  RemoteBitrateEstimator* rbe = cc.GetRemoteBitrateEstimator(true);
  RTPHeader header;
  header.ssrc = ByteReader<uint32_t>::ReadBigEndian(&data[i]);
  i += sizeof(uint32_t);
  header.extension.hasTransportSequenceNumber = true;
  int64_t arrival_time_ms = std::min<int64_t>(
      std::max<int64_t>(ByteReader<int64_t>::ReadBigEndian(&data[i]), 0),
      std::numeric_limits<int64_t>::max() / 2);
  i += sizeof(int64_t);
  const size_t kMinPacketSize =
      sizeof(size_t) + sizeof(uint16_t) + sizeof(uint8_t);
  while (i + kMinPacketSize < size) {
    size_t payload_size = ByteReader<size_t>::ReadBigEndian(&data[i]) % 1500;
    i += sizeof(size_t);
    header.extension.transportSequenceNumber =
        ByteReader<uint16_t>::ReadBigEndian(&data[i]);
    i += sizeof(uint16_t);
    rbe->IncomingPacket(arrival_time_ms, payload_size, header);
    clock.AdvanceTimeMilliseconds(5);
    arrival_time_ms += ByteReader<uint8_t>::ReadBigEndian(&data[i]);
    i += sizeof(uint8_t);
  }
  rbe->Process();
}
}  // namespace webrtc
