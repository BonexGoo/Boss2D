/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_DESKTOP_CAPTURE_WIN_DXGI_DUPLICATOR_CONTROLLER_H_
#define MODULES_DESKTOP_CAPTURE_WIN_DXGI_DUPLICATOR_CONTROLLER_H_

#include <d3dcommon.h>

#include <atomic>
#include <string>
#include <vector>

#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_geometry_h //original-code:"modules/desktop_capture/desktop_geometry.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__shared_desktop_frame_h //original-code:"modules/desktop_capture/shared_desktop_frame.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__win__d3d_device_h //original-code:"modules/desktop_capture/win/d3d_device.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__win__display_configuration_monitor_h //original-code:"modules/desktop_capture/win/display_configuration_monitor.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__win__dxgi_adapter_duplicator_h //original-code:"modules/desktop_capture/win/dxgi_adapter_duplicator.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__win__dxgi_context_h //original-code:"modules/desktop_capture/win/dxgi_context.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__win__dxgi_frame_h //original-code:"modules/desktop_capture/win/dxgi_frame.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"

namespace webrtc {

// A controller for all the objects we need to call Windows DirectX capture APIs
// It's a singleton because only one IDXGIOutputDuplication instance per monitor
// is allowed per application.
//
// Consumers should create a DxgiDuplicatorController::Context and keep it
// throughout their lifetime, and pass it when calling Duplicate(). Consumers
// can also call IsSupported() to determine whether the system supports DXGI
// duplicator or not. If a previous IsSupported() function call returns true,
// but a later Duplicate() returns false, this usually means the display mode is
// changing. Consumers should retry after a while. (Typically 50 milliseconds,
// but according to hardware performance, this time may vary.)
class DxgiDuplicatorController {
 public:
  using Context = DxgiFrameContext;

  // A collection of D3d information we are interested on, which may impact
  // capturer performance or reliability.
  struct D3dInfo {
    // Each video adapter has its own D3D_FEATURE_LEVEL, so this structure
    // contains the minimum and maximium D3D_FEATURE_LEVELs current system
    // supports.
    // Both fields can be 0, which is the default value to indicate no valid
    // D3D_FEATURE_LEVEL has been retrieved from underlying OS APIs.
    D3D_FEATURE_LEVEL min_feature_level;
    D3D_FEATURE_LEVEL max_feature_level;

    // TODO(zijiehe): Add more fields, such as manufacturer name, mode, driver
    // version.
  };

  enum class Result {
    SUCCEEDED,
    UNSUPPORTED_SESSION,
    FRAME_PREPARE_FAILED,
    INITIALIZATION_FAILED,
    DUPLICATION_FAILED,
    INVALID_MONITOR_ID,
  };

  // Converts |result| into user-friendly string representation. The return
  // value should not be used to identify error types.
  static std::string ResultName(Result result);

  // Returns the singleton instance of DxgiDuplicatorController.
  static rtc::scoped_refptr<DxgiDuplicatorController> Instance();

  // See ScreenCapturerWinDirectx::IsCurrentSessionSupported().
  static bool IsCurrentSessionSupported();

  // All the following public functions implicitly call Initialize() function.

  // Detects whether the system supports DXGI based capturer.
  bool IsSupported();

  // Returns a copy of D3dInfo composed by last Initialize() function call. This
  // function always copies the latest information into |info|. But once the
  // function returns false, the information in |info| may not accurate.
  bool RetrieveD3dInfo(D3dInfo* info);

  // Captures current screen and writes into |frame|.
  // TODO(zijiehe): Windows cannot guarantee the frames returned by each
  // IDXGIOutputDuplication are synchronized. But we are using a totally
  // different threading model than the way Windows suggested, it's hard to
  // synchronize them manually. We should find a way to do it.
  Result Duplicate(DxgiFrame* frame);

  // Captures one monitor and writes into target. |monitor_id| should >= 0. If
  // |monitor_id| is greater than the total screen count of all the Duplicators,
  // this function returns false.
  Result DuplicateMonitor(DxgiFrame* frame, int monitor_id);

  // Returns dpi of current system. Returns an empty DesktopVector if system
  // does not support DXGI based capturer.
  DesktopVector dpi();

  // Returns the count of screens on the system. These screens can be retrieved
  // by an integer in the range of [0, ScreenCount()). If system does not
  // support DXGI based capturer, this function returns 0.
  int ScreenCount();

  // Returns the device names of all screens on the system in utf8 encoding.
  // These screens can be retrieved by an integer in the range of
  // [0, output->size()). If system does not support DXGI based capturer, this
  // function returns false.
  bool GetDeviceNames(std::vector<std::string>* output);

 private:
  // DxgiFrameContext calls private Unregister(Context*) function in Reset().
  friend void DxgiFrameContext::Reset();

  // scoped_refptr<DxgiDuplicatorController> accesses private AddRef() and
  // Release() functions.
  friend class rtc::scoped_refptr<DxgiDuplicatorController>;

  // A private constructor to ensure consumers to use
  // DxgiDuplicatorController::Instance().
  DxgiDuplicatorController();

  // Not implemented: The singleton DxgiDuplicatorController instance should not
  // be deleted.
  ~DxgiDuplicatorController();

  // RefCountedInterface implementations.
  void AddRef();
  void Release();

  // Does the real duplication work. Setting |monitor_id| < 0 to capture entire
  // screen. This function calls Initialize(). And if the duplication failed,
  // this function calls Deinitialize() to ensure the Dxgi components can be
  // reinitialized next time.
  Result DoDuplicate(DxgiFrame* frame, int monitor_id);

  // Unload all the DXGI components and releases the resources. This function
  // wraps Deinitialize() with |mutex_|.
  void Unload();

  // Unregisters Context from this instance and all DxgiAdapterDuplicator(s)
  // it owns.
  void Unregister(const Context* const context);

  // All functions below should be called in |mutex_| locked scope and should be
  // after a successful Initialize().

  // If current instance has not been initialized, executes DoInitialize()
  // function, and returns initialize result. Otherwise directly returns true.
  // This function may calls Deinitialize() if initialization failed.
  bool Initialize() RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Does the real initialization work, this function should only be called in
  // Initialize().
  bool DoInitialize() RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Clears all COM components referred by this instance. So next Duplicate()
  // call will eventually initialize this instance again.
  void Deinitialize() RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // A helper function to check whether a Context has been expired.
  bool ContextExpired(const Context* const context) const
      RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Updates Context if needed.
  void Setup(Context* context) RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  bool DoDuplicateUnlocked(Context* context,
                           int monitor_id,
                           SharedDesktopFrame* target)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Captures all monitors.
  bool DoDuplicateAll(Context* context, SharedDesktopFrame* target)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Captures one monitor.
  bool DoDuplicateOne(Context* context,
                      int monitor_id,
                      SharedDesktopFrame* target)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // The minimum GetNumFramesCaptured() returned by |duplicators_|.
  int64_t GetNumFramesCaptured() const RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Returns a DesktopSize to cover entire |desktop_rect_|.
  DesktopSize desktop_size() const RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Returns the size of one screen. |id| should be >= 0. If system does not
  // support DXGI based capturer, or |id| is greater than the total screen count
  // of all the Duplicators, this function returns an empty DesktopRect.
  DesktopRect ScreenRect(int id) const RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  int ScreenCountUnlocked() const RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  void GetDeviceNamesUnlocked(std::vector<std::string>* output) const
      RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Returns the desktop size of the selected screen |monitor_id|. Setting
  // |monitor_id| < 0 to return the entire screen size.
  DesktopSize SelectedDesktopSize(int monitor_id) const
      RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Retries DoDuplicateAll() for several times until GetNumFramesCaptured() is
  // large enough. Returns false if DoDuplicateAll() returns false, or
  // GetNumFramesCaptured() has never reached the requirement.
  // According to http://crbug.com/682112, dxgi capturer returns a black frame
  // during first several capture attempts.
  bool EnsureFrameCaptured(Context* context, SharedDesktopFrame* target)
      RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // Moves |desktop_rect_| and all underlying |duplicators_|, putting top left
  // corner of the desktop at (0, 0). This is necessary because DXGI_OUTPUT_DESC
  // may return negative coordinates. Called from DoInitialize() after all
  // DxgiAdapterDuplicator and DxgiOutputDuplicator instances are initialized.
  void TranslateRect() RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);

  // The count of references which are now "living".
  std::atomic_int refcount_;

  // This lock must be locked whenever accessing any of the following objects.
  Mutex mutex_;

  // A self-incremented integer to compare with the one in Context. It ensures
  // a Context instance is always initialized after DxgiDuplicatorController.
  int identity_ RTC_GUARDED_BY(mutex_) = 0;
  DesktopRect desktop_rect_ RTC_GUARDED_BY(mutex_);
  DesktopVector dpi_ RTC_GUARDED_BY(mutex_);
  std::vector<DxgiAdapterDuplicator> duplicators_ RTC_GUARDED_BY(mutex_);
  D3dInfo d3d_info_ RTC_GUARDED_BY(mutex_);
  DisplayConfigurationMonitor display_configuration_monitor_
      RTC_GUARDED_BY(mutex_);
  // A number to indicate how many succeeded duplications have been performed.
  uint32_t succeeded_duplications_ RTC_GUARDED_BY(mutex_) = 0;
};

}  // namespace webrtc

#endif  // MODULES_DESKTOP_CAPTURE_WIN_DXGI_DUPLICATOR_CONTROLLER_H_
