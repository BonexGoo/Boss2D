/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/desktop_capture/win/screen_capture_utils.h"

#include <vector>
#include <string>

#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capturer_h //original-code:"modules/desktop_capture/desktop_capturer.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

TEST(ScreenCaptureUtilsTest, GetScreenList) {
  DesktopCapturer::SourceList screens;
  std::vector<std::string> device_names;

  ASSERT_TRUE(GetScreenList(&screens));
  screens.clear();
  ASSERT_TRUE(GetScreenList(&screens, &device_names));

  ASSERT_EQ(screens.size(), device_names.size());
}

}  // namespace webrtc
