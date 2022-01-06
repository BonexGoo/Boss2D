/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CAPTURE_MAIN_SOURCE_VIDEO_CAPTURE_IMPL_H_
#define MODULES_VIDEO_CAPTURE_MAIN_SOURCE_VIDEO_CAPTURE_IMPL_H_

/*
 * video_capture_impl.h
 */

#include <stddef.h>
#include <stdint.h>

#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video__video_rotation_h //original-code:"api/video/video_rotation.h"
#include BOSS_WEBRTC_U_api__video__video_sink_interface_h //original-code:"api/video/video_sink_interface.h"
#include BOSS_WEBRTC_U_modules__video_capture__video_capture_h //original-code:"modules/video_capture/video_capture.h"
#include BOSS_WEBRTC_U_modules__video_capture__video_capture_config_h //original-code:"modules/video_capture/video_capture_config.h"
#include BOSS_WEBRTC_U_modules__video_capture__video_capture_defines_h //original-code:"modules/video_capture/video_capture_defines.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"

namespace webrtc {

namespace videocapturemodule {
// Class definitions
class VideoCaptureImpl : public VideoCaptureModule {
 public:
  /*
   *   Create a video capture module object
   *
   *   id              - unique identifier of this video capture module object
   *   deviceUniqueIdUTF8 -  name of the device. Available names can be found by
   * using GetDeviceName
   */
  static rtc::scoped_refptr<VideoCaptureModule> Create(
      const char* deviceUniqueIdUTF8);

  static DeviceInfo* CreateDeviceInfo();

  // Helpers for converting between (integral) degrees and
  // VideoRotation values.  Return 0 on success.
  static int32_t RotationFromDegrees(int degrees, VideoRotation* rotation);
  static int32_t RotationInDegrees(VideoRotation rotation, int* degrees);

  // Call backs
  void RegisterCaptureDataCallback(
      rtc::VideoSinkInterface<VideoFrame>* dataCallback) override;
  void DeRegisterCaptureDataCallback() override;

  int32_t SetCaptureRotation(VideoRotation rotation) override;
  bool SetApplyRotation(bool enable) override;
  bool GetApplyRotation() override;

  const char* CurrentDeviceName() const override;

  // |capture_time| must be specified in NTP time format in milliseconds.
  int32_t IncomingFrame(uint8_t* videoFrame,
                        size_t videoFrameLength,
                        const VideoCaptureCapability& frameInfo,
                        int64_t captureTime = 0);

  // Platform dependent
  int32_t StartCapture(const VideoCaptureCapability& capability) override;
  int32_t StopCapture() override;
  bool CaptureStarted() override;
  int32_t CaptureSettings(VideoCaptureCapability& /*settings*/) override;

 protected:
  VideoCaptureImpl();
  ~VideoCaptureImpl() override;

  char* _deviceUniqueId;  // current Device unique name;
  Mutex api_lock_;
  VideoCaptureCapability _requestedCapability;  // Should be set by platform
                                                // dependent code in
                                                // StartCapture.
 private:
  void UpdateFrameCount();
  uint32_t CalculateFrameRate(int64_t now_ns);
  int32_t DeliverCapturedFrame(VideoFrame& captureFrame);

  // last time the module process function was called.
  int64_t _lastProcessTimeNanos;
  // last time the frame rate callback function was called.
  int64_t _lastFrameRateCallbackTimeNanos;

  rtc::VideoSinkInterface<VideoFrame>* _dataCallBack;

  int64_t _lastProcessFrameTimeNanos;
  // timestamp for local captured frames
  int64_t _incomingFrameTimesNanos[kFrameRateCountHistorySize];
  VideoRotation _rotateFrame;  // Set if the frame should be rotated by the
                               // capture module.

  // Indicate whether rotation should be applied before delivered externally.
  bool apply_rotation_;
};
}  // namespace videocapturemodule
}  // namespace webrtc
#endif  // MODULES_VIDEO_CAPTURE_MAIN_SOURCE_VIDEO_CAPTURE_IMPL_H_
