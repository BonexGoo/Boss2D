﻿/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_DEVICE_WIN_CORE_AUDIO_UTILITY_WIN_H_
#define MODULES_AUDIO_DEVICE_WIN_CORE_AUDIO_UTILITY_WIN_H_

#include <Audioclient.h>
#include <Audiopolicy.h>
#include <Mmdeviceapi.h>
#include <avrt.h>
#include <comdef.h>
#include <objbase.h>
#include <propidl.h>
#include <wrl/client.h>

#include <string>

#include BOSS_WEBRTC_U_api__units__time_delta_h //original-code:"api/units/time_delta.h"
#include BOSS_WEBRTC_U_modules__audio_device__audio_device_name_h //original-code:"modules/audio_device/audio_device_name.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__audio_device_defines_h //original-code:"modules/audio_device/include/audio_device_defines.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

#pragma comment(lib, "Avrt.lib")

namespace webrtc {
namespace webrtc_win {

static const int64_t kNumMicrosecsPerSec = webrtc::TimeDelta::seconds(1).us();

// Utility class which registers a thread with MMCSS in the constructor and
// deregisters MMCSS in the destructor. The task name is given by |task_name|.
// The Multimedia Class Scheduler service (MMCSS) enables multimedia
// applications to ensure that their time-sensitive processing receives
// prioritized access to CPU resources without denying CPU resources to
// lower-priority applications.
class ScopedMMCSSRegistration {
 public:
  explicit ScopedMMCSSRegistration(const TCHAR* task_name) {
    RTC_DLOG(INFO) << "ScopedMMCSSRegistration: " << rtc::ToUtf8(task_name);
    // Register the calling thread with MMCSS for the supplied |task_name|.
    DWORD mmcss_task_index = 0;
    mmcss_handle_ = AvSetMmThreadCharacteristics(task_name, &mmcss_task_index);
    if (mmcss_handle_ == nullptr) {
      RTC_LOG(LS_ERROR) << "Failed to enable MMCSS on this thread: "
                        << GetLastError();
    }
  }

  ~ScopedMMCSSRegistration() {
    if (Succeeded()) {
      // Deregister with MMCSS.
      RTC_DLOG(INFO) << "~ScopedMMCSSRegistration";
      AvRevertMmThreadCharacteristics(mmcss_handle_);
    }
  }

  ScopedMMCSSRegistration(const ScopedMMCSSRegistration&) = delete;
  ScopedMMCSSRegistration& operator=(const ScopedMMCSSRegistration&) = delete;

  bool Succeeded() const { return mmcss_handle_ != nullptr; }

 private:
  HANDLE mmcss_handle_ = nullptr;
};

// Initializes COM in the constructor (STA or MTA), and uninitializes COM in the
// destructor. Taken from base::win::ScopedCOMInitializer.
//
// WARNING: This should only be used once per thread, ideally scoped to a
// similar lifetime as the thread itself.  You should not be using this in
// random utility functions that make COM calls; instead ensure that these
// functions are running on a COM-supporting thread!
// See https://msdn.microsoft.com/en-us/library/ms809971.aspx for details.
class ScopedCOMInitializer {
 public:
  // Enum value provided to initialize the thread as an MTA instead of STA.
  // There are two types of apartments, Single Threaded Apartments (STAs)
  // and Multi Threaded Apartments (MTAs). Within a given process there can
  // be multiple STA’s but there is only one MTA. STA is typically used by
  // "GUI applications" and MTA by "worker threads" with no UI message loop.
  enum SelectMTA { kMTA };

  // Constructor for STA initialization.
  ScopedCOMInitializer() {
    RTC_DLOG(INFO) << "Single-Threaded Apartment (STA) COM thread";
    Initialize(COINIT_APARTMENTTHREADED);
  }

  // Constructor for MTA initialization.
  explicit ScopedCOMInitializer(SelectMTA mta) {
    RTC_DLOG(INFO) << "Multi-Threaded Apartment (MTA) COM thread";
    Initialize(COINIT_MULTITHREADED);
  }

  ~ScopedCOMInitializer() {
    if (Succeeded()) {
      CoUninitialize();
    }
  }

  ScopedCOMInitializer(const ScopedCOMInitializer&) = delete;
  ScopedCOMInitializer& operator=(const ScopedCOMInitializer&) = delete;

  bool Succeeded() { return SUCCEEDED(hr_); }

 private:
  void Initialize(COINIT init) {
    // Initializes the COM library for use by the calling thread, sets the
    // thread's concurrency model, and creates a new apartment for the thread
    // if one is required. CoInitializeEx must be called at least once, and is
    // usually called only once, for each thread that uses the COM library.
    hr_ = CoInitializeEx(NULL, init);
    RTC_CHECK_NE(RPC_E_CHANGED_MODE, hr_)
        << "Invalid COM thread model change (MTA->STA)";
    // Multiple calls to CoInitializeEx by the same thread are allowed as long
    // as they pass the same concurrency flag, but subsequent valid calls
    // return S_FALSE. To close the COM library gracefully on a thread, each
    // successful call to CoInitializeEx, including any call that returns
    // S_FALSE, must be balanced by a corresponding call to CoUninitialize.
    if (hr_ == S_OK) {
      RTC_DLOG(INFO)
          << "The COM library was initialized successfully on this thread";
    } else if (hr_ == S_FALSE) {
      RTC_DLOG(WARNING)
          << "The COM library is already initialized on this thread";
    }
  }
  HRESULT hr_;
};

// A PROPVARIANT that is automatically initialized and cleared upon respective
// construction and destruction of this class.
class ScopedPropVariant {
 public:
  ScopedPropVariant() { PropVariantInit(&pv_); }

  ~ScopedPropVariant() { Reset(); }

  ScopedPropVariant(const ScopedPropVariant&) = delete;
  ScopedPropVariant& operator=(const ScopedPropVariant&) = delete;
  bool operator==(const ScopedPropVariant&) const = delete;
  bool operator!=(const ScopedPropVariant&) const = delete;

  // Returns a pointer to the underlying PROPVARIANT for use as an out param in
  // a function call.
  PROPVARIANT* Receive() {
    RTC_DCHECK_EQ(pv_.vt, VT_EMPTY);
    return &pv_;
  }

  // Clears the instance to prepare it for re-use (e.g., via Receive).
  void Reset() {
    if (pv_.vt != VT_EMPTY) {
      HRESULT result = PropVariantClear(&pv_);
      RTC_DCHECK_EQ(result, S_OK);
    }
  }

  const PROPVARIANT& get() const { return pv_; }
  const PROPVARIANT* ptr() const { return &pv_; }

 private:
  PROPVARIANT pv_;
};

// Simple scoped memory releaser class for COM allocated memory.
template <typename T>
class ScopedCoMem {
 public:
  ScopedCoMem() : mem_ptr_(nullptr) {}

  ~ScopedCoMem() { Reset(nullptr); }

  ScopedCoMem(const ScopedCoMem&) = delete;
  ScopedCoMem& operator=(const ScopedCoMem&) = delete;

  T** operator&() {                   // NOLINT
    RTC_DCHECK(mem_ptr_ == nullptr);  // To catch memory leaks.
    return &mem_ptr_;
  }

  operator T*() { return mem_ptr_; }

  T* operator->() {
    RTC_DCHECK(mem_ptr_ != nullptr);
    return mem_ptr_;
  }

  const T* operator->() const {
    RTC_DCHECK(mem_ptr_ != nullptr);
    return mem_ptr_;
  }

  explicit operator bool() const { return mem_ptr_; }

  friend bool operator==(const ScopedCoMem& lhs, std::nullptr_t) {
    return lhs.Get() == nullptr;
  }

  friend bool operator==(std::nullptr_t, const ScopedCoMem& rhs) {
    return rhs.Get() == nullptr;
  }

  friend bool operator!=(const ScopedCoMem& lhs, std::nullptr_t) {
    return lhs.Get() != nullptr;
  }

  friend bool operator!=(std::nullptr_t, const ScopedCoMem& rhs) {
    return rhs.Get() != nullptr;
  }

  void Reset(T* ptr) {
    if (mem_ptr_)
      CoTaskMemFree(mem_ptr_);
    mem_ptr_ = ptr;
  }

  T* Get() const { return mem_ptr_; }

 private:
  T* mem_ptr_;
};

// A HANDLE that is automatically initialized and closed upon respective
// construction and destruction of this class.
class ScopedHandle {
 public:
  ScopedHandle() : handle_(nullptr) {}
  explicit ScopedHandle(HANDLE h) : handle_(nullptr) { Set(h); }

  ~ScopedHandle() { Close(); }

  ScopedHandle& operator=(const ScopedHandle&) = delete;
  bool operator==(const ScopedHandle&) const = delete;
  bool operator!=(const ScopedHandle&) const = delete;

  // Use this instead of comparing to INVALID_HANDLE_VALUE.
  bool IsValid() const { return handle_ != nullptr; }

  void Set(HANDLE new_handle) {
    Close();
    // Windows is inconsistent about invalid handles.
    // See https://blogs.msdn.microsoft.com/oldnewthing/20040302-00/?p=40443
    // for details.
    if (new_handle != INVALID_HANDLE_VALUE) {
      handle_ = new_handle;
    }
  }

  HANDLE Get() const { return handle_; }

  operator HANDLE() const { return handle_; }

  void Close() {
    if (handle_) {
      if (!::CloseHandle(handle_)) {
        RTC_NOTREACHED();
      }
      handle_ = nullptr;
    }
  }

 private:
  HANDLE handle_;
};

// Utility methods for the Core Audio API on Windows.
// Always ensure that Core Audio is supported before using these methods.
// Use webrtc_win::core_audio_utility::IsSupported() for this purpose.
// Also, all methods must be called on a valid COM thread. This can be done
// by using the webrtc_win::ScopedCOMInitializer helper class.
// These methods are based on media::CoreAudioUtil in Chrome.
namespace core_audio_utility {

// Returns true if Windows Core Audio is supported.
// Always verify that this method returns true before using any of the
// other methods in this class.
bool IsSupported();

// Returns true if Multimedia Class Scheduler service (MMCSS) is supported.
// The MMCSS enables multimedia applications to ensure that their time-sensitive
// processing receives prioritized access to CPU resources without denying CPU
// resources to lower-priority applications.
bool IsMMCSSSupported();

// The MMDevice API lets clients discover the audio endpoint devices in the
// system and determine which devices are suitable for the application to use.
// Header file Mmdeviceapi.h defines the interfaces in the MMDevice API.

// Number of active audio devices in the specified data flow direction.
// Set |data_flow| to eAll to retrieve the total number of active audio
// devices.
int NumberOfActiveDevices(EDataFlow data_flow);

// Creates an IMMDeviceEnumerator interface which provides methods for
// enumerating audio endpoint devices.
// TODO(henrika): IMMDeviceEnumerator::RegisterEndpointNotificationCallback.
Microsoft::WRL::ComPtr<IMMDeviceEnumerator> CreateDeviceEnumerator();

// These functions return the unique device id of the default or
// communications input/output device, or an empty string if no such device
// exists or if the device has been disabled.
std::string GetDefaultInputDeviceID();
std::string GetDefaultOutputDeviceID();
std::string GetCommunicationsInputDeviceID();
std::string GetCommunicationsOutputDeviceID();

// Creates an IMMDevice interface corresponding to the unique device id in
// |device_id|, or by data-flow direction and role if |device_id| is set to
// AudioDeviceName::kDefaultDeviceId.
Microsoft::WRL::ComPtr<IMMDevice> CreateDevice(const std::string& device_id,
                                               EDataFlow data_flow,
                                               ERole role);

// Returns the unique ID and user-friendly name of a given endpoint device.
// Example: "{0.0.1.00000000}.{8db6020f-18e3-4f25-b6f5-7726c9122574}", and
//          "Microphone (Realtek High Definition Audio)".
webrtc::AudioDeviceName GetDeviceName(IMMDevice* device);

// Gets the user-friendly name of the endpoint device which is represented
// by a unique id in |device_id|, or by data-flow direction and role if
// |device_id| is set to AudioDeviceName::kDefaultDeviceId.
std::string GetFriendlyName(const std::string& device_id,
                            EDataFlow data_flow,
                            ERole role);

// Query if the audio device is a rendering device or a capture device.
EDataFlow GetDataFlow(IMMDevice* device);

// Enumerates all input devices and adds the names (friendly name and unique
// device id) to the list in |device_names|.
bool GetInputDeviceNames(webrtc::AudioDeviceNames* device_names);

// Enumerates all output devices and adds the names (friendly name and unique
// device id) to the list in |device_names|.
bool GetOutputDeviceNames(webrtc::AudioDeviceNames* device_names);

// The Windows Audio Session API (WASAPI) enables client applications to
// manage the flow of audio data between the application and an audio endpoint
// device. Header files Audioclient.h and Audiopolicy.h define the WASAPI
// interfaces.

// Creates an IAudioSessionManager2 interface for the specified |device|.
// This interface provides access to e.g. the IAudioSessionEnumerator
Microsoft::WRL::ComPtr<IAudioSessionManager2> CreateSessionManager2(
    IMMDevice* device);

// Creates an IAudioSessionEnumerator interface for the specified |device|.
// The client can use the interface to enumerate audio sessions on the audio
// device
Microsoft::WRL::ComPtr<IAudioSessionEnumerator> CreateSessionEnumerator(
    IMMDevice* device);

// Number of active audio sessions for the given |device|. Expired or inactive
// sessions are not included.
int NumberOfActiveSessions(IMMDevice* device);

// Creates an IAudioClient instance for a specific device or the default
// device specified by data-flow direction and role.
Microsoft::WRL::ComPtr<IAudioClient> CreateClient(const std::string& device_id,
                                                  EDataFlow data_flow,
                                                  ERole role);

Microsoft::WRL::ComPtr<IAudioClient2>
CreateClient2(const std::string& device_id, EDataFlow data_flow, ERole role);

// Sets the AudioCategory_Communications category. Should be called before
// GetSharedModeMixFormat() and IsFormatSupported().
// Minimum supported client: Windows 8.
// TODO(henrika): evaluate effect (if any).
HRESULT SetClientProperties(IAudioClient2* client);

// Get the mix format that the audio engine uses internally for processing
// of shared-mode streams. The client can call this method before calling
// IAudioClient::Initialize. When creating a shared-mode stream for an audio
// endpoint device, the Initialize method always accepts the stream format
// obtained by this method.
HRESULT GetSharedModeMixFormat(IAudioClient* client,
                               WAVEFORMATEXTENSIBLE* format);

// Returns true if the specified |client| supports the format in |format|
// for the given |share_mode| (shared or exclusive). The client can call this
// method before calling IAudioClient::Initialize.
bool IsFormatSupported(IAudioClient* client,
                       AUDCLNT_SHAREMODE share_mode,
                       const WAVEFORMATEXTENSIBLE* format);

// For a shared-mode stream, the audio engine periodically processes the
// data in the endpoint buffer at the period obtained in |device_period|.
// For an exclusive mode stream, |device_period| corresponds to the minimum
// time interval between successive processing by the endpoint device.
// This period plus the stream latency between the buffer and endpoint device
// represents the minimum possible latency that an audio application can
// achieve. The time in |device_period| is expressed in 100-nanosecond units.
HRESULT GetDevicePeriod(IAudioClient* client,
                        AUDCLNT_SHAREMODE share_mode,
                        REFERENCE_TIME* device_period);

// Get the preferred audio parameters for the given |device_id|. The acquired
// values should only be utilized for shared mode streamed since there are no
// preferred settings for an exclusive mode stream.
HRESULT GetPreferredAudioParameters(const std::string& device_id,
                                    bool is_output_device,
                                    webrtc::AudioParameters* params);

HRESULT GetPreferredAudioParameters(IAudioClient* client,
                                    webrtc::AudioParameters* params);

// After activating an IAudioClient interface on an audio endpoint device,
// the client must initialize it once, and only once, to initialize the audio
// stream between the client and the device. In shared mode, the client
// connects indirectly through the audio engine which does the mixing.
// If a valid event is provided in |event_handle|, the client will be
// initialized for event-driven buffer handling. If |event_handle| is set to
// nullptr, event-driven buffer handling is not utilized.
// The output parameter |endpoint_buffer_size| contains the size of the
// endpoint buffer and it is expressed as the number of audio frames the
// buffer can hold.
// TODO(henrika):
// - use IAudioClient2::SetClientProperties before calling this method
// - IAudioClient::Initialize(your_format, AUDCLNT_STREAMFLAGS_AUTOCONVERTPCM
//                            AUDCLNT_STREAMFLAGS_SRC_DEFAULT_QUALITY)
HRESULT SharedModeInitialize(IAudioClient* client,
                             const WAVEFORMATEXTENSIBLE* format,
                             HANDLE event_handle,
                             uint32_t* endpoint_buffer_size);

// Creates an IAudioRenderClient client for an existing IAudioClient given by
// |client|. The IAudioRenderClient interface enables a client to write
// output data to a rendering endpoint buffer. The methods in this interface
// manage the movement of data packets that contain audio-rendering data.
Microsoft::WRL::ComPtr<IAudioRenderClient> CreateRenderClient(
    IAudioClient* client);

// Creates an IAudioCaptureClient client for an existing IAudioClient given by
// |client|. The IAudioCaptureClient interface enables a client to read
// input data from a capture endpoint buffer. The methods in this interface
// manage the movement of data packets that contain capture data.
Microsoft::WRL::ComPtr<IAudioCaptureClient> CreateCaptureClient(
    IAudioClient* client);

// Creates an IAudioClock interface for an existing IAudioClient given by
// |client|. The IAudioClock interface enables a client to monitor a stream's
// data rate and the current position in the stream.
Microsoft::WRL::ComPtr<IAudioClock> CreateAudioClock(IAudioClient* client);

// Creates an ISimpleAudioVolume interface for an existing IAudioClient given by
// |client|. This interface enables a client to control the master volume level
// of an active audio session.
Microsoft::WRL::ComPtr<ISimpleAudioVolume> CreateSimpleAudioVolume(
    IAudioClient* client);

// Fills up the endpoint rendering buffer with silence for an existing
// IAudioClient given by |client| and a corresponding IAudioRenderClient
// given by |render_client|.
bool FillRenderEndpointBufferWithSilence(IAudioClient* client,
                                         IAudioRenderClient* render_client);
// Transforms a WAVEFORMATEXTENSIBLE struct to a human-readable string.
std::string WaveFormatExToString(const WAVEFORMATEXTENSIBLE* format);

// Converts Windows internal REFERENCE_TIME (100 nanosecond units) into
// generic webrtc::TimeDelta which then can be converted to any time unit.
webrtc::TimeDelta ReferenceTimeToTimeDelta(REFERENCE_TIME time);

// Converts a COM error into a human-readable string.
std::string ErrorToString(const _com_error& error);

}  // namespace core_audio_utility
}  // namespace webrtc_win
}  // namespace webrtc

#endif  //  MODULES_AUDIO_DEVICE_WIN_CORE_AUDIO_UTILITY_WIN_H_
