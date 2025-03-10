/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file contains interfaces used for creating the VideoCaptureModule
// and DeviceInfo.

#ifndef MODULES_VIDEO_CAPTURE_VIDEO_CAPTURE_FACTORY_H_
#define MODULES_VIDEO_CAPTURE_VIDEO_CAPTURE_FACTORY_H_

#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_modules__video_capture__video_capture_h //original-code:"modules/video_capture/video_capture.h"
#include BOSS_WEBRTC_U_modules__video_capture__video_capture_defines_h //original-code:"modules/video_capture/video_capture_defines.h"

namespace webrtc {

class VideoCaptureFactory {
 public:
  // Create a video capture module object
  // id - unique identifier of this video capture module object.
  // deviceUniqueIdUTF8 - name of the device.
  //                      Available names can be found by using GetDeviceName
  static rtc::scoped_refptr<VideoCaptureModule> Create(
      const char* deviceUniqueIdUTF8);

  static VideoCaptureModule::DeviceInfo* CreateDeviceInfo();

 private:
  ~VideoCaptureFactory();
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CAPTURE_VIDEO_CAPTURE_FACTORY_H_
