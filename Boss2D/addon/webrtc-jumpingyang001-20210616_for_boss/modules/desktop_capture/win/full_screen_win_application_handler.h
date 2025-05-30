/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_DESKTOP_CAPTURE_WIN_FULL_SCREEN_WIN_APPLICATION_HANDLER_H_
#define MODULES_DESKTOP_CAPTURE_WIN_FULL_SCREEN_WIN_APPLICATION_HANDLER_H_

#include <memory>
#include BOSS_WEBRTC_U_modules__desktop_capture__full_screen_application_handler_h //original-code:"modules/desktop_capture/full_screen_application_handler.h"

namespace webrtc {

std::unique_ptr<FullScreenApplicationHandler>
CreateFullScreenWinApplicationHandler(DesktopCapturer::SourceId sourceId);

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_WIN_FULL_SCREEN_WIN_APPLICATION_HANDLER_H_
