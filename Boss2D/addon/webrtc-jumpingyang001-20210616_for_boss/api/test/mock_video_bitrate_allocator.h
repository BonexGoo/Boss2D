/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_TEST_MOCK_VIDEO_BITRATE_ALLOCATOR_H_
#define API_TEST_MOCK_VIDEO_BITRATE_ALLOCATOR_H_

#include BOSS_WEBRTC_U_api__video__video_bitrate_allocator_h //original-code:"api/video/video_bitrate_allocator.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

class MockVideoBitrateAllocator : public webrtc::VideoBitrateAllocator {
  MOCK_METHOD(VideoBitrateAllocation,
              Allocate,
              (VideoBitrateAllocationParameters parameters),
              (override));
};

}  // namespace webrtc

#endif  // API_TEST_MOCK_VIDEO_BITRATE_ALLOCATOR_H_
