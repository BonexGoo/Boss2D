/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_VIDEO_VIDEO_STREAM_DECODER_H_
#define API_VIDEO_VIDEO_STREAM_DECODER_H_

#include <map>
#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_api__units__time_delta_h //original-code:"api/units/time_delta.h"
#include BOSS_WEBRTC_U_api__video__encoded_frame_h //original-code:"api/video/encoded_frame.h"
#include BOSS_WEBRTC_U_api__video__video_content_type_h //original-code:"api/video/video_content_type.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_factory_h //original-code:"api/video_codecs/video_decoder_factory.h"

namespace webrtc {
// NOTE: This class is still under development and may change without notice.
class VideoStreamDecoderInterface {
 public:
  class Callbacks {
   public:
    virtual ~Callbacks() = default;

    struct FrameInfo {
      absl::optional<int> qp;
      VideoContentType content_type;
    };

    // Called when the VideoStreamDecoder enters a non-decodable state.
    virtual void OnNonDecodableState() = 0;

    virtual void OnContinuousUntil(int64_t frame_id) {}

    virtual void OnDecodedFrame(VideoFrame frame,
                                const FrameInfo& frame_info) = 0;
  };

  virtual ~VideoStreamDecoderInterface() = default;

  virtual void OnFrame(std::unique_ptr<EncodedFrame> frame) = 0;

  virtual void SetMinPlayoutDelay(TimeDelta min_delay) = 0;
  virtual void SetMaxPlayoutDelay(TimeDelta max_delay) = 0;
};

}  // namespace webrtc

#endif  // API_VIDEO_VIDEO_STREAM_DECODER_H_
