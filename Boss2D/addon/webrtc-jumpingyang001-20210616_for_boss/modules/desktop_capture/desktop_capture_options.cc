/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capture_options_h //original-code:"modules/desktop_capture/desktop_capture_options.h"
#if defined(WEBRTC_MAC) && !defined(WEBRTC_IOS)
#include "modules/desktop_capture/mac/full_screen_mac_application_handler.h"
#elif defined(WEBRTC_WIN)
#include BOSS_WEBRTC_U_modules__desktop_capture__win__full_screen_win_application_handler_h //original-code:"modules/desktop_capture/win/full_screen_win_application_handler.h"
#endif

namespace webrtc {

DesktopCaptureOptions::DesktopCaptureOptions() {}
DesktopCaptureOptions::DesktopCaptureOptions(
    const DesktopCaptureOptions& options) = default;
DesktopCaptureOptions::DesktopCaptureOptions(DesktopCaptureOptions&& options) =
    default;
DesktopCaptureOptions::~DesktopCaptureOptions() {}

DesktopCaptureOptions& DesktopCaptureOptions::operator=(
    const DesktopCaptureOptions& options) = default;
DesktopCaptureOptions& DesktopCaptureOptions::operator=(
    DesktopCaptureOptions&& options) = default;

// static
DesktopCaptureOptions DesktopCaptureOptions::CreateDefault() {
  DesktopCaptureOptions result;
#if defined(WEBRTC_USE_X11)
  result.set_x_display(SharedXDisplay::CreateDefault());
#endif
#if defined(WEBRTC_MAC) && !defined(WEBRTC_IOS)
  result.set_configuration_monitor(new DesktopConfigurationMonitor());
  result.set_full_screen_window_detector(
      new FullScreenWindowDetector(CreateFullScreenMacApplicationHandler));
#elif defined(WEBRTC_WIN)
  result.set_full_screen_window_detector(
      new FullScreenWindowDetector(CreateFullScreenWinApplicationHandler));
#endif
  return result;
}

}  // namespace webrtc
