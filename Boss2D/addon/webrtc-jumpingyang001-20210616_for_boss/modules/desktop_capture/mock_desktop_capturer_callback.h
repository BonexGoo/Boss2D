/* Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_DESKTOP_CAPTURE_MOCK_DESKTOP_CAPTURER_CALLBACK_H_
#define MODULES_DESKTOP_CAPTURE_MOCK_DESKTOP_CAPTURER_CALLBACK_H_

#include <memory>

#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capturer_h //original-code:"modules/desktop_capture/desktop_capturer.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

class MockDesktopCapturerCallback : public DesktopCapturer::Callback {
 public:
  MockDesktopCapturerCallback();
  ~MockDesktopCapturerCallback() override;

  MOCK_METHOD(void,
              OnCaptureResultPtr,
              (DesktopCapturer::Result result,
               std::unique_ptr<DesktopFrame>* frame));
  void OnCaptureResult(DesktopCapturer::Result result,
                       std::unique_ptr<DesktopFrame> frame) final;

 private:
  RTC_DISALLOW_COPY_AND_ASSIGN(MockDesktopCapturerCallback);
};

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_MOCK_DESKTOP_CAPTURER_CALLBACK_H_
