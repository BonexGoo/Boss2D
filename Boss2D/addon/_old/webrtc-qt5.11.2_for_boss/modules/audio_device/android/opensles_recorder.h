/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_DEVICE_ANDROID_OPENSLES_RECORDER_H_
#define MODULES_AUDIO_DEVICE_ANDROID_OPENSLES_RECORDER_H_

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <SLES/OpenSLES_AndroidConfiguration.h>

#include <memory>

#include "modules/audio_device/android/audio_common.h"
#include "modules/audio_device/android/audio_manager.h"
#include "modules/audio_device/android/opensles_common.h"
#include "modules/audio_device/audio_device_generic.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__audio_device_defines_h //original-code:"modules/audio_device/include/audio_device_defines.h"
#include "modules/utility/include/helpers_android.h"
#include BOSS_WEBRTC_U_rtc_base__thread_checker_h //original-code:"rtc_base/thread_checker.h"

namespace webrtc {

class FineAudioBuffer;

// Implements 16-bit mono PCM audio input support for Android using the
// C based OpenSL ES API. No calls from C/C++ to Java using JNI is done.
//
// An instance must be created and destroyed on one and the same thread.
// All public methods must also be called on the same thread. A thread checker
// will RTC_DCHECK if any method is called on an invalid thread. Recorded audio
// buffers are provided on a dedicated internal thread managed by the OpenSL
// ES layer.
//
// The existing design forces the user to call InitRecording() after
// StopRecording() to be able to call StartRecording() again. This is inline
// with how the Java-based implementation works.
//
// As of API level 21, lower latency audio input is supported on select devices.
// To take advantage of this feature, first confirm that lower latency output is
// available. The capability for lower latency output is a prerequisite for the
// lower latency input feature. Then, create an AudioRecorder with the same
// sample rate and buffer size as would be used for output. OpenSL ES interfaces
// for input effects preclude the lower latency path.
// See https://developer.android.com/ndk/guides/audio/opensl-prog-notes.html
// for more details.
class OpenSLESRecorder {
 public:
  // Beginning with API level 17 (Android 4.2), a buffer count of 2 or more is
  // required for lower latency. Beginning with API level 18 (Android 4.3), a
  // buffer count of 1 is sufficient for lower latency. In addition, the buffer
  // size and sample rate must be compatible with the device's native input
  // configuration provided via the audio manager at construction.
  // TODO(henrika): perhaps set this value dynamically based on OS version.
  static const int kNumOfOpenSLESBuffers = 2;

  explicit OpenSLESRecorder(AudioManager* audio_manager);
  ~OpenSLESRecorder();

  int Init();
  int Terminate();

  int InitRecording();
  bool RecordingIsInitialized() const { return initialized_; }

  int StartRecording();
  int StopRecording();
  bool Recording() const { return recording_; }

  void AttachAudioBuffer(AudioDeviceBuffer* audio_buffer);

  // TODO(henrika): add support using OpenSL ES APIs when available.
  int EnableBuiltInAEC(bool enable);
  int EnableBuiltInAGC(bool enable);
  int EnableBuiltInNS(bool enable);

 private:
  // Obtaines the SL Engine Interface from the existing global Engine object.
  // The interface exposes creation methods of all the OpenSL ES object types.
  // This method defines the |engine_| member variable.
  bool ObtainEngineInterface();

  // Creates/destroys the audio recorder and the simple-buffer queue object.
  bool CreateAudioRecorder();
  void DestroyAudioRecorder();

  // Allocate memory for audio buffers which will be used to capture audio
  // via the SLAndroidSimpleBufferQueueItf interface.
  void AllocateDataBuffers();

  // These callback methods are called when data has been written to the input
  // buffer queue. They are both called from an internal "OpenSL ES thread"
  // which is not attached to the Dalvik VM.
  static void SimpleBufferQueueCallback(SLAndroidSimpleBufferQueueItf caller,
                                        void* context);
  void ReadBufferQueue();

  // Wraps calls to SLAndroidSimpleBufferQueueState::Enqueue() and it can be
  // called both on the main thread (but before recording has started) and from
  // the internal audio thread while input streaming is active. It uses
  // |simple_buffer_queue_| but no lock is needed since the initial calls from
  // the main thread and the native callback thread are mutually exclusive.
  bool EnqueueAudioBuffer();

  // Returns the current recorder state.
  SLuint32 GetRecordState() const;

  // Returns the current buffer queue state.
  SLAndroidSimpleBufferQueueState GetBufferQueueState() const;

  // Number of buffers currently in the queue.
  SLuint32 GetBufferCount();

  // Prints a log message of the current queue state. Can be used for debugging
  // purposes.
  void LogBufferState() const;

  // Ensures that methods are called from the same thread as this object is
  // created on.
  rtc::ThreadChecker thread_checker_;

  // Stores thread ID in first call to SimpleBufferQueueCallback() from internal
  // non-application thread which is not attached to the Dalvik JVM.
  // Detached during construction of this object.
  rtc::ThreadChecker thread_checker_opensles_;

  // Raw pointer to the audio manager injected at construction. Used to cache
  // audio parameters and to access the global SL engine object needed by the
  // ObtainEngineInterface() method. The audio manager outlives any instance of
  // this class.
  AudioManager* const audio_manager_;

  // Contains audio parameters provided to this class at construction by the
  // AudioManager.
  const AudioParameters audio_parameters_;

  // Raw pointer handle provided to us in AttachAudioBuffer(). Owned by the
  // AudioDeviceModuleImpl class and called by AudioDeviceModule::Create().
  AudioDeviceBuffer* audio_device_buffer_;

  // PCM-type format definition.
  // TODO(henrika): add support for SLAndroidDataFormat_PCM_EX (android-21) if
  // 32-bit float representation is needed.
  SLDataFormat_PCM pcm_format_;

  bool initialized_;
  bool recording_;

  // This interface exposes creation methods for all the OpenSL ES object types.
  // It is the OpenSL ES API entry point.
  SLEngineItf engine_;

  // The audio recorder media object records audio to the destination specified
  // by the data sink capturing it from the input specified by the data source.
  webrtc::ScopedSLObjectItf recorder_object_;

  // This interface is supported on the audio recorder object and it controls
  // the state of the audio recorder.
  SLRecordItf recorder_;

  // The Android Simple Buffer Queue interface is supported on the audio
  // recorder. For recording, an app should enqueue empty buffers. When a
  // registered callback sends notification that the system has finished writing
  // data to the buffer, the app can read the buffer.
  SLAndroidSimpleBufferQueueItf simple_buffer_queue_;

  // Consumes audio of native buffer size and feeds the WebRTC layer with 10ms
  // chunks of audio.
  std::unique_ptr<FineAudioBuffer> fine_audio_buffer_;

  // Queue of audio buffers to be used by the recorder object for capturing
  // audio. They will be used in a Round-robin way and the size of each buffer
  // is given by AudioParameters::GetBytesPerBuffer(), i.e., it corresponds to
  // the native OpenSL ES buffer size.
  std::unique_ptr<std::unique_ptr<SLint8[]>[]> audio_buffers_;

  // Keeps track of active audio buffer 'n' in the audio_buffers_[n] queue.
  // Example (kNumOfOpenSLESBuffers = 2): counts 0, 1, 0, 1, ...
  int buffer_index_;

  // Last time the OpenSL ES layer delivered recorded audio data.
  uint32_t last_rec_time_;
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_DEVICE_ANDROID_OPENSLES_RECORDER_H_
