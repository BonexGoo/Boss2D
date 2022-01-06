/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_video_header_h //original-code:"modules/rtp_rtcp/source/rtp_video_header.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_vp8_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer_vp8.h"

namespace webrtc {
void FuzzOneInput(const uint8_t* data, size_t size) {
  RTPVideoHeader video_header;
  VideoRtpDepacketizerVp8::ParseRtpPayload(rtc::MakeArrayView(data, size),
                                           &video_header);
}
}  // namespace webrtc
