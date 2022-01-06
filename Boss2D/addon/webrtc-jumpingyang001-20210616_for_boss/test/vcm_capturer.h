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
#include <vector>

#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_modules__video_capture__video_capture_h //original-code:"modules/video_capture/video_capture.h"
#include BOSS_WEBRTC_U_test__test_video_capturer_h //original-code:"test/test_video_capturer.h"

namespace webrtc {
namespace test {

class VcmCapturer : public TestVideoCapturer,
                    public rtc::VideoSinkInterface<VideoFrame> {
 public:
  static VcmCapturer* Create(size_t width,
                             size_t height,
                             size_t target_fps,
                             size_t capture_device_index);
  virtual ~VcmCapturer();

  void OnFrame(const VideoFrame& frame) override;

 private:
  VcmCapturer();
  bool Init(size_t width,
            size_t height,
            size_t target_fps,
            size_t capture_device_index);
  void Destroy();

  rtc::scoped_refptr<VideoCaptureModule> vcm_;
  VideoCaptureCapability capability_;
};

}  // namespace test
}  // namespace webrtc

#endif  // TEST_VCM_CAPTURER_H_
