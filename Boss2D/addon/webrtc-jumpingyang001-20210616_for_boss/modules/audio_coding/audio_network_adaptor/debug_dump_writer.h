/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_DEBUG_DUMP_WRITER_H_
#define MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_DEBUG_DUMP_WRITER_H_

#include <memory>

#include BOSS_WEBRTC_U_modules__audio_coding__audio_network_adaptor__controller_h //original-code:"modules/audio_coding/audio_network_adaptor/controller.h"
#include BOSS_WEBRTC_U_modules__audio_coding__audio_network_adaptor__include__audio_network_adaptor_h //original-code:"modules/audio_coding/audio_network_adaptor/include/audio_network_adaptor.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__ignore_wundef_h //original-code:"rtc_base/ignore_wundef.h"
#include BOSS_WEBRTC_U_rtc_base__system__file_wrapper_h //original-code:"rtc_base/system/file_wrapper.h"
#if WEBRTC_ENABLE_PROTOBUF
RTC_PUSH_IGNORING_WUNDEF()
#ifdef WEBRTC_ANDROID_PLATFORM_BUILD
#include "external/webrtc/webrtc/modules/audio_coding/audio_network_adaptor/config.pb.h"
#else
#include "modules/audio_coding/audio_network_adaptor/config.pb.h"
#endif
RTC_POP_IGNORING_WUNDEF()
#endif

namespace webrtc {

class DebugDumpWriter {
 public:
  static std::unique_ptr<DebugDumpWriter> Create(FILE* file_handle);

  virtual ~DebugDumpWriter() = default;

  virtual void DumpEncoderRuntimeConfig(const AudioEncoderRuntimeConfig& config,
                                        int64_t timestamp) = 0;

  virtual void DumpNetworkMetrics(const Controller::NetworkMetrics& metrics,
                                  int64_t timestamp) = 0;

#if WEBRTC_ENABLE_PROTOBUF
  virtual void DumpControllerManagerConfig(
      const audio_network_adaptor::config::ControllerManager&
          controller_manager_config,
      int64_t timestamp) = 0;
#endif
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_DEBUG_DUMP_WRITER_H_
