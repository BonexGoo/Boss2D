/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__video__video_frame_metadata_h //original-code:"api/video/video_frame_metadata.h"

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_video_header_h //original-code:"modules/rtp_rtcp/source/rtp_video_header.h"

namespace webrtc {

VideoFrameMetadata::VideoFrameMetadata(const RTPVideoHeader& header)
    : width_(header.width), height_(header.height) {
  if (header.generic) {
    frame_id_ = header.generic->frame_id;
    spatial_index_ = header.generic->spatial_index;
    temporal_index_ = header.generic->temporal_index;
    frame_dependencies_ = header.generic->dependencies;
    decode_target_indications_ = header.generic->decode_target_indications;
  }
}

}  // namespace webrtc
