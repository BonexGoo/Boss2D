/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__create_video_rtp_depacketizer_h //original-code:"modules/rtp_rtcp/source/create_video_rtp_depacketizer.h"

#include <memory>

#include BOSS_WEBRTC_U_api__video__video_codec_type_h //original-code:"api/video/video_codec_type.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_av1_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer_av1.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_generic_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer_generic.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_h264_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer_h264.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_vp8_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer_vp8.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_vp9_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer_vp9.h"

namespace webrtc {

std::unique_ptr<VideoRtpDepacketizer> CreateVideoRtpDepacketizer(
    VideoCodecType codec) {
  switch (codec) {
    case kVideoCodecH264:
      return std::make_unique<VideoRtpDepacketizerH264>();
    case kVideoCodecVP8:
      return std::make_unique<VideoRtpDepacketizerVp8>();
    case kVideoCodecVP9:
      return std::make_unique<VideoRtpDepacketizerVp9>();
    case kVideoCodecAV1:
      return std::make_unique<VideoRtpDepacketizerAv1>();
    case kVideoCodecGeneric:
    case kVideoCodecMultiplex:
      return std::make_unique<VideoRtpDepacketizerGeneric>();
  }
  RTC_CHECK_NOTREACHED();
}

}  // namespace webrtc
