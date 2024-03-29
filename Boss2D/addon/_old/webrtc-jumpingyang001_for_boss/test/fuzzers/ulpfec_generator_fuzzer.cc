/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>

#include BOSS_WEBRTC_U_modules__include__module_common_types_public_h //original-code:"modules/include/module_common_types_public.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__byte_io_h //original-code:"modules/rtp_rtcp/source/byte_io.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__fec_test_helper_h //original-code:"modules/rtp_rtcp/source/fec_test_helper.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__ulpfec_generator_h //original-code:"modules/rtp_rtcp/source/ulpfec_generator.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {

namespace {
constexpr uint8_t kFecPayloadType = 96;
constexpr uint8_t kRedPayloadType = 97;
}  // namespace

void FuzzOneInput(const uint8_t* data, size_t size) {
  UlpfecGenerator generator;
  size_t i = 0;
  if (size < 4)
    return;
  FecProtectionParams params = {
      data[i++] % 128, static_cast<int>(data[i++] % 10), kFecMaskBursty};
  generator.SetFecParameters(params);
  uint16_t seq_num = data[i++];
  uint16_t prev_seq_num = 0;
  while (i + 3 < size) {
    size_t rtp_header_length = data[i++] % 10 + 12;
    size_t payload_size = data[i++] % 10;
    if (i + payload_size + rtp_header_length + 2 > size)
      break;
    std::unique_ptr<uint8_t[]> packet(
        new uint8_t[payload_size + rtp_header_length]);
    memcpy(packet.get(), &data[i], payload_size + rtp_header_length);

    // Make sure sequence numbers are increasing.
    ByteWriter<uint16_t>::WriteBigEndian(&packet[2], seq_num++);
    i += payload_size + rtp_header_length;
    const bool protect = data[i++] % 2 == 1;

    // Check the sequence numbers are monotonic. In rare case the packets number
    // may loop around and in the same FEC-protected group the packet sequence
    // number became out of order.
    if (protect && IsNewerSequenceNumber(seq_num, prev_seq_num) &&
        seq_num < prev_seq_num + kUlpfecMaxMediaPackets) {
      generator.AddRtpPacketAndGenerateFec(packet.get(), payload_size,
                                           rtp_header_length);
      prev_seq_num = seq_num;
    }
    const size_t num_fec_packets = generator.NumAvailableFecPackets();
    if (num_fec_packets > 0) {
      std::vector<std::unique_ptr<RedPacket>> fec_packets =
          generator.GetUlpfecPacketsAsRed(kRedPayloadType, kFecPayloadType,
                                          100);
      RTC_CHECK_EQ(num_fec_packets, fec_packets.size());
    }
  }
}
}  // namespace webrtc
