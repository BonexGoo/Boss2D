/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_api__test__create_video_quality_test_fixture_h //original-code:"api/test/create_video_quality_test_fixture.h"
#include BOSS_WEBRTC_U_video__video_quality_test_h //original-code:"video/video_quality_test.h"

namespace webrtc {

std::unique_ptr<VideoQualityTestFixtureInterface>
CreateVideoQualityTestFixture() {
  // By default, we don't override the FEC module, so pass an empty factory.
  return absl::make_unique<VideoQualityTest>(nullptr);
}

std::unique_ptr<VideoQualityTestFixtureInterface>
CreateVideoQualityTestFixture(
    std::unique_ptr<FecControllerFactoryInterface> fec_controller_factory) {
  return absl::make_unique<VideoQualityTest>(std::move(fec_controller_factory));
}

}  // namespace webrtc


