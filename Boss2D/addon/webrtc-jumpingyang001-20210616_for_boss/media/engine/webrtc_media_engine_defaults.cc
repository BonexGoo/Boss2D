/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_media__engine__webrtc_media_engine_defaults_h //original-code:"media/engine/webrtc_media_engine_defaults.h"

#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_decoder_factory_h //original-code:"api/audio_codecs/builtin_audio_decoder_factory.h"
#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_encoder_factory_h //original-code:"api/audio_codecs/builtin_audio_encoder_factory.h"
#include BOSS_WEBRTC_U_api__task_queue__default_task_queue_factory_h //original-code:"api/task_queue/default_task_queue_factory.h"
#include BOSS_WEBRTC_U_api__video__builtin_video_bitrate_allocator_factory_h //original-code:"api/video/builtin_video_bitrate_allocator_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__builtin_video_decoder_factory_h //original-code:"api/video_codecs/builtin_video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__builtin_video_encoder_factory_h //original-code:"api/video_codecs/builtin_video_encoder_factory.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {

void SetMediaEngineDefaults(cricket::MediaEngineDependencies* deps) {
  RTC_DCHECK(deps);
  if (deps->task_queue_factory == nullptr) {
    static TaskQueueFactory* const task_queue_factory =
        CreateDefaultTaskQueueFactory().release();
    deps->task_queue_factory = task_queue_factory;
  }
  if (deps->audio_encoder_factory == nullptr)
    deps->audio_encoder_factory = CreateBuiltinAudioEncoderFactory();
  if (deps->audio_decoder_factory == nullptr)
    deps->audio_decoder_factory = CreateBuiltinAudioDecoderFactory();
  if (deps->audio_processing == nullptr)
    deps->audio_processing = AudioProcessingBuilder().Create();

  if (deps->video_encoder_factory == nullptr)
    deps->video_encoder_factory = CreateBuiltinVideoEncoderFactory();
  if (deps->video_decoder_factory == nullptr)
    deps->video_decoder_factory = CreateBuiltinVideoDecoderFactory();
}

}  // namespace webrtc
