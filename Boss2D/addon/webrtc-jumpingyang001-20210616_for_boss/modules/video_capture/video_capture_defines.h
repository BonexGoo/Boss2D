/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CAPTURE_VIDEO_CAPTURE_DEFINES_H_
#define MODULES_VIDEO_CAPTURE_VIDEO_CAPTURE_DEFINES_H_

#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_common_video__libyuv__include__webrtc_libyuv_h //original-code:"common_video/libyuv/include/webrtc_libyuv.h"

namespace webrtc {

enum {
  kVideoCaptureUniqueNameLength = 1024
};  // Max unique capture device name lenght
enum { kVideoCaptureDeviceNameLength = 256 };  // Max capture device name lenght
enum { kVideoCaptureProductIdLength = 128 };   // Max product id length

struct VideoCaptureCapability {
  int32_t width;
  int32_t height;
  int32_t maxFPS;
  VideoType videoType;
  bool interlaced;

  VideoCaptureCapability() {
    width = 0;
    height = 0;
    maxFPS = 0;
    videoType = VideoType::kUnknown;
    interlaced = false;
  }
  bool operator!=(const VideoCaptureCapability& other) const {
    if (width != other.width)
      return true;
    if (height != other.height)
      return true;
    if (maxFPS != other.maxFPS)
      return true;
    if (videoType != other.videoType)
      return true;
    if (interlaced != other.interlaced)
      return true;
    return false;
  }
  bool operator==(const VideoCaptureCapability& other) const {
    return !operator!=(other);
  }
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CAPTURE_VIDEO_CAPTURE_DEFINES_H_
