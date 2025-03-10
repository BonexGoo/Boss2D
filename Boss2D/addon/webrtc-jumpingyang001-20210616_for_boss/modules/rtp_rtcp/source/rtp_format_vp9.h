/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

//
// This file contains the declaration of the VP9 packetizer class.
// A packetizer object is created for each encoded video frame. The
// constructor is called with the payload data and size.
//
// After creating the packetizer, the method NextPacket is called
// repeatedly to get all packets for the frame. The method returns
// false as long as there are more packets left to fetch.
//

#ifndef MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_VP9_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_VP9_H_

#include <stddef.h>
#include <stdint.h>

#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_format_h //original-code:"modules/rtp_rtcp/source/rtp_format.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_to_send_h //original-code:"modules/rtp_rtcp/source/rtp_packet_to_send.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp9__include__vp9_globals_h //original-code:"modules/video_coding/codecs/vp9/include/vp9_globals.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

class RtpPacketizerVp9 : public RtpPacketizer {
 public:
  // The |payload| must be one encoded VP9 layer frame.
  RtpPacketizerVp9(rtc::ArrayView<const uint8_t> payload,
                   PayloadSizeLimits limits,
                   const RTPVideoHeaderVP9& hdr);

  ~RtpPacketizerVp9() override;

  size_t NumPackets() const override;

  // Gets the next payload with VP9 payload header.
  // Write payload and set marker bit of the |packet|.
  // Returns true on success, false otherwise.
  bool NextPacket(RtpPacketToSend* packet) override;

 private:
  // Writes the payload descriptor header.
  // |layer_begin| and |layer_end| indicates the postision of the packet in
  // the layer frame. Returns false on failure.
  bool WriteHeader(bool layer_begin,
                   bool layer_end,
                   rtc::ArrayView<uint8_t> rtp_payload) const;

  const RTPVideoHeaderVP9 hdr_;
  const int header_size_;
  const int first_packet_extra_header_size_;
  rtc::ArrayView<const uint8_t> remaining_payload_;
  std::vector<int> payload_sizes_;
  std::vector<int>::const_iterator current_packet_;

  RTC_DISALLOW_COPY_AND_ASSIGN(RtpPacketizerVp9);
};

}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_VP9_H_
