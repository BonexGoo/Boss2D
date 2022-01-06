/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_H264_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_H264_H_

#include <stddef.h>
#include <stdint.h>

#include <deque>
#include <memory>
#include <queue>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_format_h //original-code:"modules/rtp_rtcp/source/rtp_format.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_to_send_h //original-code:"modules/rtp_rtcp/source/rtp_packet_to_send.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__h264__include__h264_globals_h //original-code:"modules/video_coding/codecs/h264/include/h264_globals.h"
#include BOSS_WEBRTC_U_rtc_base__buffer_h //original-code:"rtc_base/buffer.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

class RtpPacketizerH264 : public RtpPacketizer {
 public:
  // Initialize with payload from encoder.
  // The payload_data must be exactly one encoded H264 frame.
  RtpPacketizerH264(rtc::ArrayView<const uint8_t> payload,
                    PayloadSizeLimits limits,
                    H264PacketizationMode packetization_mode);

  ~RtpPacketizerH264() override;

  size_t NumPackets() const override;

  // Get the next payload with H264 payload header.
  // Write payload and set marker bit of the |packet|.
  // Returns true on success, false otherwise.
  bool NextPacket(RtpPacketToSend* rtp_packet) override;

 private:
  // A packet unit (H264 packet), to be put into an RTP packet:
  // If a NAL unit is too large for an RTP packet, this packet unit will
  // represent a FU-A packet of a single fragment of the NAL unit.
  // If a NAL unit is small enough to fit within a single RTP packet, this
  // packet unit may represent a single NAL unit or a STAP-A packet, of which
  // there may be multiple in a single RTP packet (if so, aggregated = true).
  struct PacketUnit {
    PacketUnit(rtc::ArrayView<const uint8_t> source_fragment,
               bool first_fragment,
               bool last_fragment,
               bool aggregated,
               uint8_t header)
        : source_fragment(source_fragment),
          first_fragment(first_fragment),
          last_fragment(last_fragment),
          aggregated(aggregated),
          header(header) {}

    rtc::ArrayView<const uint8_t> source_fragment;
    bool first_fragment;
    bool last_fragment;
    bool aggregated;
    uint8_t header;
  };

  bool GeneratePackets(H264PacketizationMode packetization_mode);
  bool PacketizeFuA(size_t fragment_index);
  size_t PacketizeStapA(size_t fragment_index);
  bool PacketizeSingleNalu(size_t fragment_index);

  void NextAggregatePacket(RtpPacketToSend* rtp_packet);
  void NextFragmentPacket(RtpPacketToSend* rtp_packet);

  const PayloadSizeLimits limits_;
  size_t num_packets_left_;
  std::deque<rtc::ArrayView<const uint8_t>> input_fragments_;
  std::queue<PacketUnit> packets_;

  RTC_DISALLOW_COPY_AND_ASSIGN(RtpPacketizerH264);
};
}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_H264_H_
