/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_DESKTOP_CAPTURE_MOUSE_CURSOR_H_
#define MODULES_DESKTOP_CAPTURE_MOUSE_CURSOR_H_

#include <memory>

#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_geometry_h //original-code:"modules/desktop_capture/desktop_geometry.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

class DesktopFrame;

class MouseCursor {
 public:
  MouseCursor();

  // Takes ownership of |image|. |hotspot| must be within |image| boundaries.
  MouseCursor(DesktopFrame* image, const DesktopVector& hotspot);

  ~MouseCursor();

  static MouseCursor* CopyOf(const MouseCursor& cursor);

  void set_image(DesktopFrame* image) { image_.reset(image); }
  const DesktopFrame* image() const { return image_.get(); }

  void set_hotspot(const DesktopVector& hotspot ) { hotspot_ = hotspot; }
  const DesktopVector& hotspot() const { return hotspot_; }

 private:
  std::unique_ptr<DesktopFrame> image_;
  DesktopVector hotspot_;

  RTC_DISALLOW_COPY_AND_ASSIGN(MouseCursor);
};

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_MOUSE_CURSOR_H_
