/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef MODULES_RTP_RTCP_SOURCE_RTP_VIDEO_HEADER_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_VIDEO_HEADER_H_

#include BOSS_ABSEILCPP_U_absl__container__inlined_vector_h //original-code:"absl/container/inlined_vector.h"
#include BOSS_ABSEILCPP_U_absl__types__variant_h //original-code:"absl/types/variant.h"
#include BOSS_WEBRTC_U_api__video__video_content_type_h //original-code:"api/video/video_content_type.h"
#include BOSS_WEBRTC_U_api__video__video_rotation_h //original-code:"api/video/video_rotation.h"
#include BOSS_WEBRTC_U_api__video__video_timing_h //original-code:"api/video/video_timing.h"
#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_modules__video_coding__codecs__h264__include__h264_globals_h //original-code:"modules/video_coding/codecs/h264/include/h264_globals.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp8__include__vp8_globals_h //original-code:"modules/video_coding/codecs/vp8/include/vp8_globals.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp9__include__vp9_globals_h //original-code:"modules/video_coding/codecs/vp9/include/vp9_globals.h"

namespace webrtc {
using RTPVideoTypeHeader =
    absl::variant<RTPVideoHeaderVP8, RTPVideoHeaderVP9, RTPVideoHeaderH264>;

struct RTPVideoHeader {
  RTPVideoHeader();
  RTPVideoHeader(const RTPVideoHeader& other);

  ~RTPVideoHeader();

  // TODO(philipel): Remove when downstream projects have been updated.
  RTPVideoHeaderVP8& vp8() {
    if (!absl::holds_alternative<RTPVideoHeaderVP8>(video_type_header))
      video_type_header.emplace<RTPVideoHeaderVP8>();

    return absl::get<RTPVideoHeaderVP8>(video_type_header);
  }
  // TODO(philipel): Remove when downstream projects have been updated.
  const RTPVideoHeaderVP8& vp8() const {
    if (!absl::holds_alternative<RTPVideoHeaderVP8>(video_type_header))
      video_type_header.emplace<RTPVideoHeaderVP8>();

    return absl::get<RTPVideoHeaderVP8>(video_type_header);
  }

  // Information for generic codec descriptor.
  int64_t frame_id = kNoPictureId;
  int spatial_index = 0;
  int temporal_index = 0;
  absl::InlinedVector<int64_t, 5> dependencies;
  absl::InlinedVector<int, 5> higher_spatial_layers;

  uint16_t width = 0;
  uint16_t height = 0;
  VideoRotation rotation = VideoRotation::kVideoRotation_0;
  VideoContentType content_type = VideoContentType::UNSPECIFIED;
  bool is_first_packet_in_frame = false;
  uint8_t simulcastIdx = 0;
  VideoCodecType codec = VideoCodecType::kVideoCodecUnknown;

  PlayoutDelay playout_delay;
  VideoSendTiming video_timing;
  // TODO(philipel): remove mutable when downstream projects have been updated.
  mutable RTPVideoTypeHeader video_type_header;
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_SOURCE_RTP_VIDEO_HEADER_H_
