/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_DESKTOP_CAPTURE_LINUX_X_WINDOW_PROPERTY_H_
#define MODULES_DESKTOP_CAPTURE_LINUX_X_WINDOW_PROPERTY_H_

#include <X11/X.h>
#include <X11/Xlib.h>

#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

class XWindowPropertyBase {
 public:
  XWindowPropertyBase(Display* display,
                      Window window,
                      Atom property,
                      int expected_size);
  virtual ~XWindowPropertyBase();

  // True if we got properly value successfully.
  bool is_valid() const { return is_valid_; }

  // Size and value of the property.
  size_t size() const { return size_; }

 protected:
  unsigned char* data_ = nullptr;

 private:
  bool is_valid_ = false;
  unsigned long size_ = 0;  // NOLINT: type required by XGetWindowProperty

  RTC_DISALLOW_COPY_AND_ASSIGN(XWindowPropertyBase);
};

// Convenience wrapper for XGetWindowProperty() results.
template <class PropertyType>
class XWindowProperty : public XWindowPropertyBase {
 public:
  XWindowProperty(Display* display, const Window window, const Atom property)
      : XWindowPropertyBase(display, window, property, sizeof(PropertyType)) {}
  ~XWindowProperty() override = default;

  const PropertyType* data() const {
    return reinterpret_cast<PropertyType*>(data_);
  }
  PropertyType* data() { return reinterpret_cast<PropertyType*>(data_); }

  RTC_DISALLOW_COPY_AND_ASSIGN(XWindowProperty);
};

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_LINUX_X_WINDOW_PROPERTY_H_
