/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_DESKTOP_CAPTURE_WIN_DXGI_FRAME_H_
#define MODULES_DESKTOP_CAPTURE_WIN_DXGI_FRAME_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capture_types_h //original-code:"modules/desktop_capture/desktop_capture_types.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capturer_h //original-code:"modules/desktop_capture/desktop_capturer.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_geometry_h //original-code:"modules/desktop_capture/desktop_geometry.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__resolution_tracker_h //original-code:"modules/desktop_capture/resolution_tracker.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__shared_desktop_frame_h //original-code:"modules/desktop_capture/shared_desktop_frame.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__shared_memory_h //original-code:"modules/desktop_capture/shared_memory.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__win__dxgi_context_h //original-code:"modules/desktop_capture/win/dxgi_context.h"

namespace webrtc {

class DxgiDuplicatorController;

// A pair of a SharedDesktopFrame and a DxgiDuplicatorController::Context for
// the client of DxgiDuplicatorController.
class DxgiFrame final {
 public:
  using Context = DxgiFrameContext;

  // DxgiFrame does not take ownership of |factory|, consumers should ensure it
  // outlives this instance. nullptr is acceptable.
  explicit DxgiFrame(SharedMemoryFactory* factory);
  ~DxgiFrame();

  // Should not be called if Prepare() is not executed or returns false.
  SharedDesktopFrame* frame() const;

 private:
  // Allows DxgiDuplicatorController to access Prepare() and context() function
  // as well as Context class.
  friend class DxgiDuplicatorController;

  // Prepares current instance with desktop size and source id.
  bool Prepare(DesktopSize size, DesktopCapturer::SourceId source_id);

  // Should not be called if Prepare() is not executed or returns false.
  Context* context();

  SharedMemoryFactory* const factory_;
  ResolutionTracker resolution_tracker_;
  DesktopCapturer::SourceId source_id_ = kFullDesktopScreenId;
  std::unique_ptr<SharedDesktopFrame> frame_;
  Context context_;
};

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_WIN_DXGI_FRAME_H_
