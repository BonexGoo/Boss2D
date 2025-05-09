/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_DESKTOP_CAPTURE_FULL_SCREEN_APPLICATION_HANDLER_H_
#define MODULES_DESKTOP_CAPTURE_FULL_SCREEN_APPLICATION_HANDLER_H_

#include <memory>
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capturer_h //original-code:"modules/desktop_capture/desktop_capturer.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

// Base class for application specific handler to check criteria for switch to
// full-screen mode and find if possible the full-screen window to share.
// Supposed to be created and owned by platform specific
// FullScreenWindowDetector.
class FullScreenApplicationHandler {
 public:
  virtual ~FullScreenApplicationHandler() {}

  explicit FullScreenApplicationHandler(DesktopCapturer::SourceId sourceId);

  // Returns the full-screen window in place of the original window if all the
  // criteria are met, or 0 if no such window found.
  virtual DesktopCapturer::SourceId FindFullScreenWindow(
      const DesktopCapturer::SourceList& window_list,
      int64_t timestamp) const;

  // Returns source id of original window associated with
  // FullScreenApplicationHandler
  DesktopCapturer::SourceId GetSourceId() const;

 private:
  const DesktopCapturer::SourceId source_id_;

  RTC_DISALLOW_COPY_AND_ASSIGN(FullScreenApplicationHandler);
};

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_FULL_SCREEN_APPLICATION_HANDLER_H_
