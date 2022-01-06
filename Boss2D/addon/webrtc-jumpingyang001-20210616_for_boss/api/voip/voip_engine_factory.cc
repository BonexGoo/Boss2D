/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__voip__voip_engine_factory_h //original-code:"api/voip/voip_engine_factory.h"

#include <utility>

#include BOSS_WEBRTC_U_audio__voip__voip_core_h //original-code:"audio/voip/voip_core.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace webrtc {

std::unique_ptr<VoipEngine> CreateVoipEngine(VoipEngineConfig config) {
  RTC_CHECK(config.encoder_factory);
  RTC_CHECK(config.decoder_factory);
  RTC_CHECK(config.task_queue_factory);
  RTC_CHECK(config.audio_device_module);

  if (!config.audio_processing) {
    RTC_DLOG(INFO) << "No audio processing functionality provided.";
  }

  return std::make_unique<VoipCore>(std::move(config.encoder_factory),
                                    std::move(config.decoder_factory),
                                    std::move(config.task_queue_factory),
                                    std::move(config.audio_device_module),
                                    std::move(config.audio_processing));
}

}  // namespace webrtc
