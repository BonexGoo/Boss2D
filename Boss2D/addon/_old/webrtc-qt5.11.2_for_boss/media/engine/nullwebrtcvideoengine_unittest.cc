/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_media__engine__nullwebrtcvideoengine_h //original-code:"media/engine/nullwebrtcvideoengine.h"
#include BOSS_WEBRTC_U_media__engine__webrtcvoiceengine_h //original-code:"media/engine/webrtcvoiceengine.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__mock_audio_device_h //original-code:"modules/audio_device/include/mock_audio_device.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include "test/mock_audio_decoder_factory.h"
#include "test/mock_audio_encoder_factory.h"

namespace cricket {

class WebRtcMediaEngineNullVideo
    : public CompositeMediaEngine<WebRtcVoiceEngine, NullWebRtcVideoEngine> {
 public:
  WebRtcMediaEngineNullVideo(
      webrtc::AudioDeviceModule* adm,
      const rtc::scoped_refptr<webrtc::AudioEncoderFactory>&
          audio_encoder_factory,
      const rtc::scoped_refptr<webrtc::AudioDecoderFactory>&
          audio_decoder_factory)
      : CompositeMediaEngine<WebRtcVoiceEngine, NullWebRtcVideoEngine>(
            std::forward_as_tuple(adm,
                                  audio_encoder_factory,
                                  audio_decoder_factory,
                                  nullptr,
                                  webrtc::AudioProcessingBuilder().Create()),
            std::forward_as_tuple()) {}
};

// Simple test to check if NullWebRtcVideoEngine implements the methods
// required by CompositeMediaEngine.
TEST(NullWebRtcVideoEngineTest, CheckInterface) {
  testing::NiceMock<webrtc::test::MockAudioDeviceModule> adm;
  WebRtcMediaEngineNullVideo engine(
      &adm, webrtc::MockAudioEncoderFactory::CreateUnusedFactory(),
      webrtc::MockAudioDecoderFactory::CreateUnusedFactory());
  EXPECT_TRUE(engine.Init());
}

}  // namespace cricket
