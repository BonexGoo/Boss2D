/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__desktop_capture__mouse_cursor_h //original-code:"modules/desktop_capture/mouse_cursor.h"

#include <assert.h>

#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_frame_h //original-code:"modules/desktop_capture/desktop_frame.h"

namespace webrtc {

MouseCursor::MouseCursor() {}

MouseCursor::MouseCursor(DesktopFrame* image, const DesktopVector& hotspot)
    : image_(image), hotspot_(hotspot) {
  assert(0 <= hotspot_.x() && hotspot_.x() <= image_->size().width());
  assert(0 <= hotspot_.y() && hotspot_.y() <= image_->size().height());
}

MouseCursor::~MouseCursor() {}

// static
MouseCursor* MouseCursor::CopyOf(const MouseCursor& cursor) {
  return cursor.image()
             ? new MouseCursor(BasicDesktopFrame::CopyOf(*cursor.image()),
                               cursor.hotspot())
             : new MouseCursor();
}

}  // namespace webrtc
