/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_test__platform_video_capturer_h //original-code:"test/platform_video_capturer.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#if defined(WEBRTC_MAC)
#include BOSS_WEBRTC_U_test__mac_capturer_h //original-code:"test/mac_capturer.h"
#else
#include BOSS_WEBRTC_U_test__vcm_capturer_h //original-code:"test/vcm_capturer.h"
#endif

namespace webrtc {
namespace test {

std::unique_ptr<TestVideoCapturer> CreateVideoCapturer(
    size_t width,
    size_t height,
    size_t target_fps,
    size_t capture_device_index) {
#if defined(WEBRTC_MAC)
  return absl::WrapUnique<TestVideoCapturer>(test::MacCapturer::Create(
      width, height, target_fps, capture_device_index));
#else
  return absl::WrapUnique<TestVideoCapturer>(test::VcmCapturer::Create(
      width, height, target_fps, capture_device_index));
#endif
}

}  // namespace test
}  // namespace webrtc
