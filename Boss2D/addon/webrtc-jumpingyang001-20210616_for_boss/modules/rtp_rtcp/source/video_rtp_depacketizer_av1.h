/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_VIDEO_RTP_DEPACKETIZER_AV1_H_
#define MODULES_RTP_RTCP_SOURCE_VIDEO_RTP_DEPACKETIZER_AV1_H_

#include <stddef.h>
#include <stdint.h>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__video__encoded_image_h //original-code:"api/video/encoded_image.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer.h"
#include BOSS_WEBRTC_U_rtc_base__copy_on_write_buffer_h //original-code:"rtc_base/copy_on_write_buffer.h"

namespace webrtc {

class VideoRtpDepacketizerAv1 : public VideoRtpDepacketizer {
 public:
  VideoRtpDepacketizerAv1() = default;
  VideoRtpDepacketizerAv1(const VideoRtpDepacketizerAv1&) = delete;
  VideoRtpDepacketizerAv1& operator=(const VideoRtpDepacketizerAv1&) = delete;
  ~VideoRtpDepacketizerAv1() override = default;

  rtc::scoped_refptr<EncodedImageBuffer> AssembleFrame(
      rtc::ArrayView<const rtc::ArrayView<const uint8_t>> rtp_payloads)
      override;

  absl::optional<ParsedRtpPayload> Parse(
      rtc::CopyOnWriteBuffer rtp_payload) override;
};

}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_VIDEO_RTP_DEPACKETIZER_AV1_H_
