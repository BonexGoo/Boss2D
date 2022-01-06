/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CAPTURE_MAIN_SOURCE_DEVICE_INFO_IMPL_H_
#define MODULES_VIDEO_CAPTURE_MAIN_SOURCE_DEVICE_INFO_IMPL_H_

#include <stdint.h>

#include <vector>

#include BOSS_WEBRTC_U_api__video__video_rotation_h //original-code:"api/video/video_rotation.h"
#include BOSS_WEBRTC_U_modules__video_capture__video_capture_h //original-code:"modules/video_capture/video_capture.h"
#include BOSS_WEBRTC_U_modules__video_capture__video_capture_defines_h //original-code:"modules/video_capture/video_capture_defines.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {
namespace videocapturemodule {
class DeviceInfoImpl : public VideoCaptureModule::DeviceInfo {
 public:
  DeviceInfoImpl();
  ~DeviceInfoImpl(void) override;
  int32_t NumberOfCapabilities(const char* deviceUniqueIdUTF8) override;
  int32_t GetCapability(const char* deviceUniqueIdUTF8,
                        const uint32_t deviceCapabilityNumber,
                        VideoCaptureCapability& capability) override;

  int32_t GetBestMatchedCapability(const char* deviceUniqueIdUTF8,
                                   const VideoCaptureCapability& requested,
                                   VideoCaptureCapability& resulting) override;
  int32_t GetOrientation(const char* deviceUniqueIdUTF8,
                         VideoRotation& orientation) override;

 protected:
  /* Initialize this object*/

  virtual int32_t Init() = 0;
  /*
   * Fills the member variable _captureCapabilities with capabilities for the
   * given device name.
   */
  virtual int32_t CreateCapabilityMap(const char* deviceUniqueIdUTF8)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(_apiLock) = 0;

 protected:
  // Data members
  typedef std::vector<VideoCaptureCapability> VideoCaptureCapabilities;
  VideoCaptureCapabilities _captureCapabilities RTC_GUARDED_BY(_apiLock);
  Mutex _apiLock;
  char* _lastUsedDeviceName RTC_GUARDED_BY(_apiLock);
  uint32_t _lastUsedDeviceNameLength RTC_GUARDED_BY(_apiLock);
};
}  // namespace videocapturemodule
}  // namespace webrtc
#endif  // MODULES_VIDEO_CAPTURE_MAIN_SOURCE_DEVICE_INFO_IMPL_H_
