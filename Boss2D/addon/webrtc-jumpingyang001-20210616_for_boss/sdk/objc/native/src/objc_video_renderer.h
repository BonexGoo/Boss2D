/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef SDK_OBJC_NATIVE_SRC_OBJC_VIDEO_RENDERER_H_
#define SDK_OBJC_NATIVE_SRC_OBJC_VIDEO_RENDERER_H_

#import <CoreGraphics/CoreGraphics.h>
#import <Foundation/Foundation.h>

#import "base/RTCMacros.h"

#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video__video_sink_interface_h //original-code:"api/video/video_sink_interface.h"

@protocol RTC_OBJC_TYPE
(RTCVideoRenderer);

namespace webrtc {

class ObjCVideoRenderer : public rtc::VideoSinkInterface<VideoFrame> {
 public:
  ObjCVideoRenderer(id<RTC_OBJC_TYPE(RTCVideoRenderer)> renderer);
  void OnFrame(const VideoFrame& nativeVideoFrame) override;

 private:
  id<RTC_OBJC_TYPE(RTCVideoRenderer)> renderer_;
  CGSize size_;
};

}  // namespace webrtc

#endif  // SDK_OBJC_NATIVE_SRC_OBJC_VIDEO_RENDERER_H_
