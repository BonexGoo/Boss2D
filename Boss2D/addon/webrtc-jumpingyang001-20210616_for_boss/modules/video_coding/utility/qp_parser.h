/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_UTILITY_QP_PARSER_H_
#define MODULES_VIDEO_CODING_UTILITY_QP_PARSER_H_

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__video__video_codec_constants_h //original-code:"api/video/video_codec_constants.h"
#include BOSS_WEBRTC_U_api__video__video_codec_type_h //original-code:"api/video/video_codec_type.h"
#include BOSS_WEBRTC_U_common_video__h264__h264_bitstream_parser_h //original-code:"common_video/h264/h264_bitstream_parser.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"

namespace webrtc {
class QpParser {
 public:
  absl::optional<uint32_t> Parse(VideoCodecType codec_type,
                                 size_t spatial_idx,
                                 const uint8_t* frame_data,
                                 size_t frame_size);

 private:
  // A thread safe wrapper for H264 bitstream parser.
  class H264QpParser {
   public:
    absl::optional<uint32_t> Parse(const uint8_t* frame_data,
                                   size_t frame_size);

   private:
    Mutex mutex_;
    H264BitstreamParser bitstream_parser_ RTC_GUARDED_BY(mutex_);
  };

  H264QpParser h264_parsers_[kMaxSimulcastStreams];
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_UTILITY_QP_PARSER_H_
