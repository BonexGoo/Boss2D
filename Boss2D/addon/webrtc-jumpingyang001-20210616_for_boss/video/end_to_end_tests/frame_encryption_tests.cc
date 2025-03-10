/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__test__fake_frame_decryptor_h //original-code:"api/test/fake_frame_decryptor.h"
#include BOSS_WEBRTC_U_api__test__fake_frame_encryptor_h //original-code:"api/test/fake_frame_encryptor.h"
#include BOSS_WEBRTC_U_media__engine__internal_decoder_factory_h //original-code:"media/engine/internal_decoder_factory.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_dependency_descriptor_extension_h //original-code:"modules/rtp_rtcp/source/rtp_dependency_descriptor_extension.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp8__include__vp8_h //original-code:"modules/video_coding/codecs/vp8/include/vp8.h"
#include BOSS_WEBRTC_U_test__call_test_h //original-code:"test/call_test.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {
namespace {

using FrameEncryptionEndToEndTest = test::CallTest;

enum : int {  // The first valid value is 1.
  kGenericDescriptorExtensionId = 1,
};

class DecryptedFrameObserver : public test::EndToEndTest,
                               public rtc::VideoSinkInterface<VideoFrame> {
 public:
  DecryptedFrameObserver()
      : EndToEndTest(test::CallTest::kDefaultTimeoutMs),
        encoder_factory_([] { return VP8Encoder::Create(); }) {}

 private:
  void ModifyVideoConfigs(
      VideoSendStream::Config* send_config,
      std::vector<VideoReceiveStream::Config>* receive_configs,
      VideoEncoderConfig* encoder_config) override {
    // Use VP8 instead of FAKE.
    send_config->encoder_settings.encoder_factory = &encoder_factory_;
    send_config->rtp.payload_name = "VP8";
    send_config->rtp.payload_type = test::CallTest::kVideoSendPayloadType;
    send_config->frame_encryptor = new FakeFrameEncryptor();
    send_config->crypto_options.sframe.require_frame_encryption = true;
    encoder_config->codec_type = kVideoCodecVP8;
    VideoReceiveStream::Decoder decoder =
        test::CreateMatchingDecoder(*send_config);
    for (auto& recv_config : *receive_configs) {
      recv_config.decoder_factory = &decoder_factory_;
      recv_config.decoders.clear();
      recv_config.decoders.push_back(decoder);
      recv_config.renderer = this;
      recv_config.frame_decryptor = new FakeFrameDecryptor();
      recv_config.crypto_options.sframe.require_frame_encryption = true;
    }
  }

  void OnFrame(const VideoFrame& video_frame) override {
    observation_complete_.Set();
  }

  void PerformTest() override {
    EXPECT_TRUE(Wait())
        << "Timed out waiting for decrypted frames to be rendered.";
  }

  std::unique_ptr<VideoEncoder> encoder_;
  test::FunctionVideoEncoderFactory encoder_factory_;
  InternalDecoderFactory decoder_factory_;
};

// Validates that payloads cannot be sent without a frame encryptor and frame
// decryptor attached.
TEST_F(FrameEncryptionEndToEndTest,
       WithGenericFrameDescriptorRequireFrameEncryptionEnforced) {
  RegisterRtpExtension(RtpExtension(RtpExtension::kGenericFrameDescriptorUri00,
                                    kGenericDescriptorExtensionId));
  DecryptedFrameObserver test;
  RunBaseTest(&test);
}

TEST_F(FrameEncryptionEndToEndTest,
       WithDependencyDescriptorRequireFrameEncryptionEnforced) {
  RegisterRtpExtension(RtpExtension(RtpExtension::kDependencyDescriptorUri,
                                    kGenericDescriptorExtensionId));
  DecryptedFrameObserver test;
  RunBaseTest(&test);
}
}  // namespace
}  // namespace webrtc
