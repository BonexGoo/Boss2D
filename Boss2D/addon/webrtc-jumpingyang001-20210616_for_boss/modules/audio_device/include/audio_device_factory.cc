/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_device__include__audio_device_factory_h //original-code:"modules/audio_device/include/audio_device_factory.h"

#include <memory>

#if defined(WEBRTC_WIN)
#include BOSS_WEBRTC_U_modules__audio_device__win__audio_device_module_win_h //original-code:"modules/audio_device/win/audio_device_module_win.h"
#include BOSS_WEBRTC_U_modules__audio_device__win__core_audio_input_win_h //original-code:"modules/audio_device/win/core_audio_input_win.h"
#include BOSS_WEBRTC_U_modules__audio_device__win__core_audio_output_win_h //original-code:"modules/audio_device/win/core_audio_output_win.h"
#include BOSS_WEBRTC_U_modules__audio_device__win__core_audio_utility_win_h //original-code:"modules/audio_device/win/core_audio_utility_win.h"
#endif

#include BOSS_WEBRTC_U_api__task_queue__task_queue_factory_h //original-code:"api/task_queue/task_queue_factory.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace webrtc {

rtc::scoped_refptr<AudioDeviceModule> CreateWindowsCoreAudioAudioDeviceModule(
    TaskQueueFactory* task_queue_factory,
    bool automatic_restart) {
  RTC_DLOG(INFO) << __FUNCTION__;
  return CreateWindowsCoreAudioAudioDeviceModuleForTest(task_queue_factory,
                                                        automatic_restart);
}

rtc::scoped_refptr<AudioDeviceModuleForTest>
CreateWindowsCoreAudioAudioDeviceModuleForTest(
    TaskQueueFactory* task_queue_factory,
    bool automatic_restart) {
  RTC_DLOG(INFO) << __FUNCTION__;
  // Returns NULL if Core Audio is not supported or if COM has not been
  // initialized correctly using ScopedCOMInitializer.
  if (!webrtc_win::core_audio_utility::IsSupported()) {
    RTC_LOG(LS_ERROR)
        << "Unable to create ADM since Core Audio is not supported";
    return nullptr;
  }
  return CreateWindowsCoreAudioAudioDeviceModuleFromInputAndOutput(
      std::make_unique<webrtc_win::CoreAudioInput>(automatic_restart),
      std::make_unique<webrtc_win::CoreAudioOutput>(automatic_restart),
      task_queue_factory);
}

}  // namespace webrtc
