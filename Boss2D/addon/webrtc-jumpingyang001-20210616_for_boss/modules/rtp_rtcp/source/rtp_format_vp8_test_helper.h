/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file contains the class RtpFormatVp8TestHelper. The class is
// responsible for setting up a fake VP8 bitstream according to the
// RTPVideoHeaderVP8 header. The packetizer can then be provided to this helper
// class, which will then extract all packets and compare to the expected
// outcome.

#ifndef MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_VP8_TEST_HELPER_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_VP8_TEST_HELPER_H_

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_format_vp8_h //original-code:"modules/rtp_rtcp/source/rtp_format_vp8.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp8__include__vp8_globals_h //original-code:"modules/video_coding/codecs/vp8/include/vp8_globals.h"
#include BOSS_WEBRTC_U_rtc_base__buffer_h //original-code:"rtc_base/buffer.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

class RtpFormatVp8TestHelper {
 public:
  RtpFormatVp8TestHelper(const RTPVideoHeaderVP8* hdr, size_t payload_len);
  ~RtpFormatVp8TestHelper();
  void GetAllPacketsAndCheck(RtpPacketizerVp8* packetizer,
                             rtc::ArrayView<const size_t> expected_sizes);

  rtc::ArrayView<const uint8_t> payload() const { return payload_; }
  size_t payload_size() const { return payload_.size(); }

 private:
  // Returns header size, i.e. payload offset.
  int CheckHeader(rtc::ArrayView<const uint8_t> rtp_payload, bool first);
  void CheckPictureID(rtc::ArrayView<const uint8_t> rtp_payload, int* offset);
  void CheckTl0PicIdx(rtc::ArrayView<const uint8_t> rtp_payload, int* offset);
  void CheckTIDAndKeyIdx(rtc::ArrayView<const uint8_t> rtp_payload,
                         int* offset);
  void CheckPayload(const uint8_t* data_ptr);

  const RTPVideoHeaderVP8* const hdr_info_;
  rtc::Buffer payload_;

  RTC_DISALLOW_COPY_AND_ASSIGN(RtpFormatVp8TestHelper);
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_RTP_FORMAT_VP8_TEST_HELPER_H_
