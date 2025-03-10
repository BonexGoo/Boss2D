/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_VIDEO_VIDEO_SOURCE_INTERFACE_H_
#define API_VIDEO_VIDEO_SOURCE_INTERFACE_H_

#include <limits>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__video__video_sink_interface_h //original-code:"api/video/video_sink_interface.h"

namespace rtc {

// VideoSinkWants is used for notifying the source of properties a video frame
// should have when it is delivered to a certain sink.
struct VideoSinkWants {
  VideoSinkWants();
  VideoSinkWants(const VideoSinkWants&);
  ~VideoSinkWants();
  // Tells the source whether the sink wants frames with rotation applied.
  // By default, any rotation must be applied by the sink.
  bool rotation_applied = false;

  // Tells the source that the sink only wants black frames.
  bool black_frames = false;

  // Tells the source the maximum number of pixels the sink wants.
  int max_pixel_count = std::numeric_limits<int>::max();
  // Tells the source the desired number of pixels the sinks wants. This will
  // typically be used when stepping the resolution up again when conditions
  // have improved after an earlier downgrade. The source should select the
  // closest resolution to this pixel count, but if max_pixel_count is set, it
  // still sets the absolute upper bound.
  absl::optional<int> target_pixel_count;
  // Tells the source the maximum framerate the sink wants.
  int max_framerate_fps = std::numeric_limits<int>::max();
};

template <typename VideoFrameT>
class VideoSourceInterface {
 public:
  virtual void AddOrUpdateSink(VideoSinkInterface<VideoFrameT>* sink,
                               const VideoSinkWants& wants) = 0;
  // RemoveSink must guarantee that at the time the method returns,
  // there is no current and no future calls to VideoSinkInterface::OnFrame.
  virtual void RemoveSink(VideoSinkInterface<VideoFrameT>* sink) = 0;

 protected:
  // Non-public, since one shouldn't own sources via this interface.
  virtual ~VideoSourceInterface() {}
};

}  // namespace rtc
#endif  // API_VIDEO_VIDEO_SOURCE_INTERFACE_H_
