/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>

#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capture_types_h //original-code:"modules/desktop_capture/desktop_capture_types.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__mouse_cursor_monitor_h //original-code:"modules/desktop_capture/mouse_cursor_monitor.h"

#if defined(WEBRTC_USE_X11)
#include "modules/desktop_capture/linux/mouse_cursor_monitor_x11.h"
#endif  // defined(WEBRTC_USE_X11)

namespace webrtc {

// static
MouseCursorMonitor* MouseCursorMonitor::CreateForWindow(
    const DesktopCaptureOptions& options,
    WindowId window) {
#if defined(WEBRTC_USE_X11)
  return MouseCursorMonitorX11::CreateForWindow(options, window);
#else
  return nullptr;
#endif  // defined(WEBRTC_USE_X11)
}

// static
MouseCursorMonitor* MouseCursorMonitor::CreateForScreen(
    const DesktopCaptureOptions& options,
    ScreenId screen) {
#if defined(WEBRTC_USE_X11)
  return MouseCursorMonitorX11::CreateForScreen(options, screen);
#else
  return nullptr;
#endif  // defined(WEBRTC_USE_X11)
}

// static
std::unique_ptr<MouseCursorMonitor> MouseCursorMonitor::Create(
    const DesktopCaptureOptions& options) {
#if defined(WEBRTC_USE_X11)
  return MouseCursorMonitorX11::Create(options);
#else
  return nullptr;
#endif  // defined(WEBRTC_USE_X11)
}

}  // namespace webrtc
