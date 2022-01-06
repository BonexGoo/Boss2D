/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef TEST_FAKE_TEXTURE_FRAME_H_
#define TEST_FAKE_TEXTURE_FRAME_H_

#include BOSS_WEBRTC_U_api__video__i420_buffer_h //original-code:"api/video/i420_buffer.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_common_video__include__video_frame_buffer_h //original-code:"common_video/include/video_frame_buffer.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {
namespace test {

class FakeNativeBuffer : public VideoFrameBuffer {
 public:
  static VideoFrame CreateFrame(int width,
                                int height,
                                uint32_t timestamp,
                                int64_t render_time_ms,
                                VideoRotation rotation);

  FakeNativeBuffer(int width, int height) : width_(width), height_(height) {}

  Type type() const override;
  int width() const override;
  int height() const override;

 private:
  rtc::scoped_refptr<I420BufferInterface> ToI420() override;

  const int width_;
  const int height_;
};

}  // namespace test
}  // namespace webrtc
#endif  //  TEST_FAKE_TEXTURE_FRAME_H_
