/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef TEST_VIDEO_CAPTURER_H_
#define TEST_VIDEO_CAPTURER_H_

#include <stddef.h>

#include <memory>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__video__i420_buffer_h //original-code:"api/video/i420_buffer.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video__video_source_interface_h //original-code:"api/video/video_source_interface.h"
#include BOSS_WEBRTC_U_media__base__videoadapter_h //original-code:"media/base/videoadapter.h"
#include BOSS_WEBRTC_U_rtc_base__criticalsection_h //original-code:"rtc_base/criticalsection.h"

namespace cricket {
class VideoAdapter;
}  // namespace cricket

namespace webrtc {
class Clock;
namespace test {

class VideoCapturer : public rtc::VideoSourceInterface<VideoFrame> {
 public:
  VideoCapturer();
  virtual ~VideoCapturer();

  virtual void Start() = 0;
  virtual void Stop() = 0;

  void AddOrUpdateSink(rtc::VideoSinkInterface<VideoFrame>* sink,
                       const rtc::VideoSinkWants& wants) override;

 protected:
  absl::optional<VideoFrame> AdaptFrame(const VideoFrame& frame);
  rtc::VideoSinkWants GetSinkWants();

 private:
  const std::unique_ptr<cricket::VideoAdapter> video_adapter_;
};
}  // namespace test
}  // namespace webrtc

#endif  // TEST_VIDEO_CAPTURER_H_
