/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__desktop_capture__win__screen_capturer_win_directx_h //original-code:"modules/desktop_capture/win/screen_capturer_win_directx.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capture_metrics_helper_h //original-code:"modules/desktop_capture/desktop_capture_metrics_helper.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_capture_types_h //original-code:"modules/desktop_capture/desktop_capture_types.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_frame_h //original-code:"modules/desktop_capture/desktop_frame.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__win__screen_capture_utils_h //original-code:"modules/desktop_capture/win/screen_capture_utils.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__time_utils_h //original-code:"rtc_base/time_utils.h"
#include BOSS_WEBRTC_U_rtc_base__trace_event_h //original-code:"rtc_base/trace_event.h"
#include BOSS_WEBRTC_U_system_wrappers__include__metrics_h //original-code:"system_wrappers/include/metrics.h"

namespace webrtc {

using Microsoft::WRL::ComPtr;

// static
bool ScreenCapturerWinDirectx::IsSupported() {
  // Forwards IsSupported() function call to DxgiDuplicatorController.
  return DxgiDuplicatorController::Instance()->IsSupported();
}

// static
bool ScreenCapturerWinDirectx::RetrieveD3dInfo(D3dInfo* info) {
  // Forwards SupportedFeatureLevels() function call to
  // DxgiDuplicatorController.
  return DxgiDuplicatorController::Instance()->RetrieveD3dInfo(info);
}

// static
bool ScreenCapturerWinDirectx::IsCurrentSessionSupported() {
  return DxgiDuplicatorController::IsCurrentSessionSupported();
}

// static
bool ScreenCapturerWinDirectx::GetScreenListFromDeviceNames(
    const std::vector<std::string>& device_names,
    DesktopCapturer::SourceList* screens) {
  RTC_DCHECK(screens->empty());

  DesktopCapturer::SourceList gdi_screens;
  std::vector<std::string> gdi_names;
  if (!GetScreenList(&gdi_screens, &gdi_names)) {
    return false;
  }

  RTC_DCHECK_EQ(gdi_screens.size(), gdi_names.size());

  ScreenId max_screen_id = -1;
  for (const DesktopCapturer::Source& screen : gdi_screens) {
    max_screen_id = std::max(max_screen_id, screen.id);
  }

  for (const auto& device_name : device_names) {
    const auto it = std::find(gdi_names.begin(), gdi_names.end(), device_name);
    if (it == gdi_names.end()) {
      // devices_names[i] has not been found in gdi_names, so use max_screen_id.
      max_screen_id++;
      screens->push_back({max_screen_id});
    } else {
      screens->push_back({gdi_screens[it - gdi_names.begin()]});
    }
  }

  return true;
}

// static
int ScreenCapturerWinDirectx::GetIndexFromScreenId(
    ScreenId id,
    const std::vector<std::string>& device_names) {
  DesktopCapturer::SourceList screens;
  if (!GetScreenListFromDeviceNames(device_names, &screens)) {
    return -1;
  }

  RTC_DCHECK_EQ(device_names.size(), screens.size());

  for (size_t i = 0; i < screens.size(); i++) {
    if (screens[i].id == id) {
      return static_cast<int>(i);
    }
  }

  return -1;
}

ScreenCapturerWinDirectx::ScreenCapturerWinDirectx()
    : controller_(DxgiDuplicatorController::Instance()) {}

ScreenCapturerWinDirectx::~ScreenCapturerWinDirectx() = default;

void ScreenCapturerWinDirectx::Start(Callback* callback) {
  RTC_DCHECK(!callback_);
  RTC_DCHECK(callback);
  RecordCapturerImpl(DesktopCapturerId::kScreenCapturerWinDirectx);

  callback_ = callback;
}

void ScreenCapturerWinDirectx::SetSharedMemoryFactory(
    std::unique_ptr<SharedMemoryFactory> shared_memory_factory) {
  shared_memory_factory_ = std::move(shared_memory_factory);
}

void ScreenCapturerWinDirectx::CaptureFrame() {
  RTC_DCHECK(callback_);
  TRACE_EVENT0("webrtc", "ScreenCapturerWinDirectx::CaptureFrame");

  int64_t capture_start_time_nanos = rtc::TimeNanos();

  frames_.MoveToNextFrame();
  if (!frames_.current_frame()) {
    frames_.ReplaceCurrentFrame(
        std::make_unique<DxgiFrame>(shared_memory_factory_.get()));
  }

  DxgiDuplicatorController::Result result;
  if (current_screen_id_ == kFullDesktopScreenId) {
    result = controller_->Duplicate(frames_.current_frame());
  } else {
    result = controller_->DuplicateMonitor(frames_.current_frame(),
                                           current_screen_id_);
  }

  using DuplicateResult = DxgiDuplicatorController::Result;
  if (result != DuplicateResult::SUCCEEDED) {
    RTC_LOG(LS_ERROR) << "DxgiDuplicatorController failed to capture desktop, "
                         "error code "
                      << DxgiDuplicatorController::ResultName(result);
  }
  switch (result) {
    case DuplicateResult::UNSUPPORTED_SESSION: {
      RTC_LOG(LS_ERROR)
          << "Current binary is running on a session not supported "
             "by DirectX screen capturer.";
      callback_->OnCaptureResult(Result::ERROR_PERMANENT, nullptr);
      break;
    }
    case DuplicateResult::FRAME_PREPARE_FAILED: {
      RTC_LOG(LS_ERROR) << "Failed to allocate a new DesktopFrame.";
      // This usually means we do not have enough memory or SharedMemoryFactory
      // cannot work correctly.
      callback_->OnCaptureResult(Result::ERROR_PERMANENT, nullptr);
      break;
    }
    case DuplicateResult::INVALID_MONITOR_ID: {
      RTC_LOG(LS_ERROR) << "Invalid monitor id " << current_screen_id_;
      callback_->OnCaptureResult(Result::ERROR_PERMANENT, nullptr);
      break;
    }
    case DuplicateResult::INITIALIZATION_FAILED:
    case DuplicateResult::DUPLICATION_FAILED: {
      callback_->OnCaptureResult(Result::ERROR_TEMPORARY, nullptr);
      break;
    }
    case DuplicateResult::SUCCEEDED: {
      std::unique_ptr<DesktopFrame> frame =
          frames_.current_frame()->frame()->Share();

      int capture_time_ms = (rtc::TimeNanos() - capture_start_time_nanos) /
                            rtc::kNumNanosecsPerMillisec;
      RTC_HISTOGRAM_COUNTS_1000(
          "WebRTC.DesktopCapture.Win.DirectXCapturerFrameTime",
          capture_time_ms);
      frame->set_capture_time_ms(capture_time_ms);
      frame->set_capturer_id(DesktopCapturerId::kScreenCapturerWinDirectx);

      // TODO(julien.isorce): http://crbug.com/945468. Set the icc profile on
      // the frame, see WindowCapturerMac::CaptureFrame.

      callback_->OnCaptureResult(Result::SUCCESS, std::move(frame));
      break;
    }
  }
}

bool ScreenCapturerWinDirectx::GetSourceList(SourceList* sources) {
  std::vector<std::string> device_names;
  if (!controller_->GetDeviceNames(&device_names)) {
    return false;
  }

  return GetScreenListFromDeviceNames(device_names, sources);
}

bool ScreenCapturerWinDirectx::SelectSource(SourceId id) {
  if (id == kFullDesktopScreenId) {
    current_screen_id_ = id;
    return true;
  }

  std::vector<std::string> device_names;
  if (!controller_->GetDeviceNames(&device_names)) {
    return false;
  }

  int index;
  index = GetIndexFromScreenId(id, device_names);
  if (index == -1) {
    return false;
  }

  current_screen_id_ = index;
  return true;
}

}  // namespace webrtc
