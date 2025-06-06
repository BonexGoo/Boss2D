/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_DESKTOP_CAPTURE_SHARED_DESKTOP_FRAME_H_
#define MODULES_DESKTOP_CAPTURE_SHARED_DESKTOP_FRAME_H_

#include <memory>

#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_frame_h //original-code:"modules/desktop_capture/desktop_frame.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__system__rtc_export_h //original-code:"rtc_base/system/rtc_export.h"

namespace webrtc {

// SharedDesktopFrame is a DesktopFrame that may have multiple instances all
// sharing the same buffer.
class RTC_EXPORT SharedDesktopFrame final : public DesktopFrame {
 public:
  ~SharedDesktopFrame() override;

  static std::unique_ptr<SharedDesktopFrame> Wrap(
      std::unique_ptr<DesktopFrame> desktop_frame);

  // Deprecated.
  // TODO(sergeyu): remove this method.
  static SharedDesktopFrame* Wrap(DesktopFrame* desktop_frame);

  // Deprecated. Clients do not need to know the underlying DesktopFrame
  // instance.
  // TODO(zijiehe): Remove this method.
  // Returns the underlying instance of DesktopFrame.
  DesktopFrame* GetUnderlyingFrame();

  // Returns whether |this| and |other| share the underlying DesktopFrame.
  bool ShareFrameWith(const SharedDesktopFrame& other) const;

  // Creates a clone of this object.
  std::unique_ptr<SharedDesktopFrame> Share();

  // Checks if the frame is currently shared. If it returns false it's
  // guaranteed that there are no clones of the object.
  bool IsShared();

 private:
  typedef rtc::FinalRefCountedObject<std::unique_ptr<DesktopFrame>> Core;

  SharedDesktopFrame(rtc::scoped_refptr<Core> core);

  const rtc::scoped_refptr<Core> core_;

  RTC_DISALLOW_COPY_AND_ASSIGN(SharedDesktopFrame);
};

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_SHARED_DESKTOP_FRAME_H_
