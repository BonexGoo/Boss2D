/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_media__engine__null_webrtc_video_engine_h //original-code:"media/engine/null_webrtc_video_engine.h"

#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_api__task_queue__default_task_queue_factory_h //original-code:"api/task_queue/default_task_queue_factory.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_factory_h //original-code:"api/task_queue/task_queue_factory.h"
#include BOSS_WEBRTC_U_api__transport__field_trial_based_config_h //original-code:"api/transport/field_trial_based_config.h"
#include BOSS_WEBRTC_U_media__engine__webrtc_voice_engine_h //original-code:"media/engine/webrtc_voice_engine.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__mock_audio_device_h //original-code:"modules/audio_device/include/mock_audio_device.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include BOSS_WEBRTC_U_test__mock_audio_decoder_factory_h //original-code:"test/mock_audio_decoder_factory.h"
#include BOSS_WEBRTC_U_test__mock_audio_encoder_factory_h //original-code:"test/mock_audio_encoder_factory.h"

namespace cricket {

// Simple test to check if NullWebRtcVideoEngine implements the methods
// required by CompositeMediaEngine.
TEST(NullWebRtcVideoEngineTest, CheckInterface) {
  std::unique_ptr<webrtc::TaskQueueFactory> task_queue_factory =
      webrtc::CreateDefaultTaskQueueFactory();
  rtc::scoped_refptr<webrtc::test::MockAudioDeviceModule> adm =
      webrtc::test::MockAudioDeviceModule::CreateNice();
  webrtc::FieldTrialBasedConfig trials;
  auto audio_engine = std::make_unique<WebRtcVoiceEngine>(
      task_queue_factory.get(), adm,
      webrtc::MockAudioEncoderFactory::CreateUnusedFactory(),
      webrtc::MockAudioDecoderFactory::CreateUnusedFactory(), nullptr,
      webrtc::AudioProcessingBuilder().Create(), nullptr, trials);

  CompositeMediaEngine engine(std::move(audio_engine),
                              std::make_unique<NullWebRtcVideoEngine>());
  engine.Init();
}

}  // namespace cricket
