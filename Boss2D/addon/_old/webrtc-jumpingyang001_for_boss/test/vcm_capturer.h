/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef TEST_VCM_CAPTURER_H_
#define TEST_VCM_CAPTURER_H_

#include <memory>

#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_common_video__libyuv__include__webrtc_libyuv_h //original-code:"common_video/libyuv/include/webrtc_libyuv.h"
#include BOSS_WEBRTC_U_modules__video_capture__video_capture_h //original-code:"modules/video_capture/video_capture.h"
#include BOSS_WEBRTC_U_rtc_base__criticalsection_h //original-code:"rtc_base/criticalsection.h"
#include BOSS_WEBRTC_U_rtc_base__scoped_ref_ptr_h //original-code:"rtc_base/scoped_ref_ptr.h"
#include "test/video_capturer.h"

namespace webrtc {
namespace test {

class VcmCapturer : public VideoCapturer,
                    public rtc::VideoSinkInterface<VideoFrame> {
 public:
  static VcmCapturer* Create(size_t width,
                             size_t height,
                             size_t target_fps,
                             size_t capture_device_index);
  virtual ~VcmCapturer();

  void Start() override;
  void Stop() override;
  void AddOrUpdateSink(rtc::VideoSinkInterface<VideoFrame>* sink,
                       const rtc::VideoSinkWants& wants) override;
  void RemoveSink(rtc::VideoSinkInterface<VideoFrame>* sink) override;

  void OnFrame(const VideoFrame& frame) override;

 private:
  VcmCapturer();
  bool Init(size_t width,
            size_t height,
            size_t target_fps,
            size_t capture_device_index);
  void Destroy();

  rtc::CriticalSection crit_;
  bool started_ RTC_GUARDED_BY(crit_);
  rtc::VideoSinkInterface<VideoFrame>* sink_ RTC_GUARDED_BY(crit_);
  rtc::scoped_refptr<VideoCaptureModule> vcm_;
  VideoCaptureCapability capability_;
};

}  // namespace test
}  // namespace webrtc

#endif  // TEST_VCM_CAPTURER_H_
