/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__desktop_capture__win__screen_capture_utils_h //original-code:"modules/desktop_capture/win/screen_capture_utils.h"

#include <string>
#include <vector>

#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capture_types_h //original-code:"modules/desktop_capture/desktop_capture_types.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capturer_h //original-code:"modules/desktop_capture/desktop_capturer.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
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

TEST(ScreenCaptureUtilsTest, DeviceIndexToHmonitor) {
  DesktopCapturer::SourceList screens;
  ASSERT_TRUE(GetScreenList(&screens));
  if (screens.size() == 0) {
    RTC_LOG(LS_INFO) << "Skip screen capture test on systems with no monitors.";
    GTEST_SKIP();
  }

  HMONITOR hmonitor;
  ASSERT_TRUE(GetHmonitorFromDeviceIndex(screens[0].id, &hmonitor));
  ASSERT_TRUE(IsMonitorValid(hmonitor));
}

TEST(ScreenCaptureUtilsTest, FullScreenDeviceIndexToHmonitor) {
  HMONITOR hmonitor;
  ASSERT_TRUE(GetHmonitorFromDeviceIndex(kFullDesktopScreenId, &hmonitor));
  ASSERT_EQ(hmonitor, static_cast<HMONITOR>(0));
  ASSERT_TRUE(IsMonitorValid(hmonitor));
}

TEST(ScreenCaptureUtilsTest, InvalidDeviceIndexToHmonitor) {
  HMONITOR hmonitor;
  ASSERT_FALSE(GetHmonitorFromDeviceIndex(kInvalidScreenId, &hmonitor));
}

}  // namespace webrtc
