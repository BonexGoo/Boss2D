/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_DESKTOP_CAPTURE_MAC_SCREEN_CAPTURER_MAC_H_
#define MODULES_DESKTOP_CAPTURE_MAC_SCREEN_CAPTURER_MAC_H_

#include <CoreGraphics/CoreGraphics.h>

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capture_options_h //original-code:"modules/desktop_capture/desktop_capture_options.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capturer_h //original-code:"modules/desktop_capture/desktop_capturer.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_frame_h //original-code:"modules/desktop_capture/desktop_frame.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_geometry_h //original-code:"modules/desktop_capture/desktop_geometry.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_region_h //original-code:"modules/desktop_capture/desktop_region.h"
#include "modules/desktop_capture/mac/desktop_configuration.h"
#include "modules/desktop_capture/mac/desktop_configuration_monitor.h"
#include "modules/desktop_capture/mac/desktop_frame_provider.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__screen_capture_frame_queue_h //original-code:"modules/desktop_capture/screen_capture_frame_queue.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__screen_capturer_helper_h //original-code:"modules/desktop_capture/screen_capturer_helper.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__shared_desktop_frame_h //original-code:"modules/desktop_capture/shared_desktop_frame.h"

namespace webrtc {

class DisplayStreamManager;

// A class to perform video frame capturing for mac.
class ScreenCapturerMac final : public DesktopCapturer {
 public:
  ScreenCapturerMac(
      rtc::scoped_refptr<DesktopConfigurationMonitor> desktop_config_monitor,
      bool detect_updated_region,
      bool allow_iosurface);
  ~ScreenCapturerMac() override;

  // TODO(julien.isorce): Remove Init() or make it private.
  bool Init();

  // DesktopCapturer interface.
  void Start(Callback* callback) override;
  void CaptureFrame() override;
  void SetExcludedWindow(WindowId window) override;
  bool GetSourceList(SourceList* screens) override;
  bool SelectSource(SourceId id) override;

 private:
  // Returns false if the selected screen is no longer valid.
  bool CgBlit(const DesktopFrame& frame, const DesktopRegion& region);

  // Called when the screen configuration is changed.
  void ScreenConfigurationChanged();

  bool RegisterRefreshAndMoveHandlers();
  void UnregisterRefreshAndMoveHandlers();

  void ScreenRefresh(CGDirectDisplayID display_id,
                     CGRectCount count,
                     const CGRect* rect_array,
                     DesktopVector display_origin,
                     IOSurfaceRef io_surface);
  void ReleaseBuffers();

  std::unique_ptr<DesktopFrame> CreateFrame();

  const bool detect_updated_region_;

  Callback* callback_ = nullptr;

  // Queue of the frames buffers.
  ScreenCaptureFrameQueue<SharedDesktopFrame> queue_;

  // Current display configuration.
  MacDesktopConfiguration desktop_config_;

  // Currently selected display, or 0 if the full desktop is selected. On OS X
  // 10.6 and before, this is always 0.
  CGDirectDisplayID current_display_ = 0;

  // The physical pixel bounds of the current screen.
  DesktopRect screen_pixel_bounds_;

  // The dip to physical pixel scale of the current screen.
  float dip_to_pixel_scale_ = 1.0f;

  // A thread-safe list of invalid rectangles, and the size of the most
  // recently captured screen.
  ScreenCapturerHelper helper_;

  // Contains an invalid region from the previous capture.
  DesktopRegion last_invalid_region_;

  // Monitoring display reconfiguration.
  rtc::scoped_refptr<DesktopConfigurationMonitor> desktop_config_monitor_;

  CGWindowID excluded_window_ = 0;

  // List of streams, one per screen.
  std::vector<CGDisplayStreamRef> display_streams_;

  // Container holding latest state of the snapshot per displays.
  DesktopFrameProvider desktop_frame_provider_;

  // Start, CaptureFrame and destructor have to called in the same thread.
  SequenceChecker thread_checker_;

  RTC_DISALLOW_COPY_AND_ASSIGN(ScreenCapturerMac);
};

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_MAC_SCREEN_CAPTURER_MAC_H_
