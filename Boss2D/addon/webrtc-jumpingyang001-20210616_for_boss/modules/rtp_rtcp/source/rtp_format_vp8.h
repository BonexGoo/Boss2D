/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

/*
 * This file contains the declaration of the VP8 packetizer class.
 * A packetizer object is created for each encoded video frame. The
 * constructor is called with the payload data and size,
 * together with the fragmentation information and a packetizer mode
 * of choice. Alternatively, if no fragmentation info is available, the
 * second constructor can be used with only payload data and size; in that
 * case the mode kEqualSize is used.
 *
 * After creating the packetizer, the method NextPacket is called
 * repeatedly to get all packets for the frame. The method returns
 * false as long as there are more packets left to fetch.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_VP8_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_VP8_H_

#include <stddef.h>

#include <cstdint>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__container__inlined_vector_h //original-code:"absl/container/inlined_vector.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_format_h //original-code:"modules/rtp_rtcp/source/rtp_format.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_to_send_h //original-code:"modules/rtp_rtcp/source/rtp_packet_to_send.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp8__include__vp8_globals_h //original-code:"modules/video_coding/codecs/vp8/include/vp8_globals.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

// Packetizer for VP8.
class RtpPacketizerVp8 : public RtpPacketizer {
 public:
  // Initialize with payload from encoder.
  // The payload_data must be exactly one encoded VP8 frame.
  RtpPacketizerVp8(rtc::ArrayView<const uint8_t> payload,
                   PayloadSizeLimits limits,
                   const RTPVideoHeaderVP8& hdr_info);

  ~RtpPacketizerVp8() override;

  size_t NumPackets() const override;

  // Get the next payload with VP8 payload header.
  // Write payload and set marker bit of the |packet|.
  // Returns true on success, false otherwise.
  bool NextPacket(RtpPacketToSend* packet) override;

 private:
  // VP8 header can use up to 6 bytes.
  using RawHeader = absl::InlinedVector<uint8_t, 6>;
  static RawHeader BuildHeader(const RTPVideoHeaderVP8& header);

  RawHeader hdr_;
  rtc::ArrayView<const uint8_t> remaining_payload_;
  std::vector<int> payload_sizes_;
  std::vector<int>::const_iterator current_packet_;

  RTC_DISALLOW_COPY_AND_ASSIGN(RtpPacketizerVp8);
};

}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_VP8_H_
