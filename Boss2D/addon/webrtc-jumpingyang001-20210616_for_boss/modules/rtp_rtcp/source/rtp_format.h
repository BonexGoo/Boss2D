/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_H_

#include <stdint.h>

#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_video_header_h //original-code:"modules/rtp_rtcp/source/rtp_video_header.h"

namespace webrtc {

class RtpPacketToSend;

class RtpPacketizer {
 public:
  struct PayloadSizeLimits {
    int max_payload_len = 1200;
    int first_packet_reduction_len = 0;
    int last_packet_reduction_len = 0;
    // Reduction len for packet that is first & last at the same time.
    int single_packet_reduction_len = 0;
  };

  // If type is not set, returns a raw packetizer.
  static std::unique_ptr<RtpPacketizer> Create(
      absl::optional<VideoCodecType> type,
      rtc::ArrayView<const uint8_t> payload,
      PayloadSizeLimits limits,
      // Codec-specific details.
      const RTPVideoHeader& rtp_video_header);

  virtual ~RtpPacketizer() = default;

  // Returns number of remaining packets to produce by the packetizer.
  virtual size_t NumPackets() const = 0;

  // Get the next payload with payload header.
  // Write payload and set marker bit of the |packet|.
  // Returns true on success, false otherwise.
  virtual bool NextPacket(RtpPacketToSend* packet) = 0;

  // Split payload_len into sum of integers with respect to |limits|.
  // Returns empty vector on failure.
  static std::vector<int> SplitAboutEqually(int payload_len,
                                            const PayloadSizeLimits& limits);
};
}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_H_
