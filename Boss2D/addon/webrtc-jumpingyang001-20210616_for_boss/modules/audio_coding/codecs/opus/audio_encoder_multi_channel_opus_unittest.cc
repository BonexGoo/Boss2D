/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio_codecs__opus__audio_encoder_multi_channel_opus_h //original-code:"api/audio_codecs/opus/audio_encoder_multi_channel_opus.h"

#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {
using ::testing::NiceMock;
using ::testing::Return;

namespace {
constexpr int kOpusPayloadType = 120;
}  // namespace

TEST(AudioEncoderMultiOpusTest, CheckConfigValidity) {
  {
    const SdpAudioFormat sdp_format("multiopus", 48000, 2,
                                    {{"channel_mapping", "3,0"},
                                     {"coupled_streams", "1"},
                                     {"num_streams", "2"}});
    const absl::optional<AudioEncoderMultiChannelOpus::Config> encoder_config =
        AudioEncoderMultiChannelOpus::SdpToConfig(sdp_format);
    ASSERT_TRUE(encoder_config.has_value());

    // Maps input channel 0 to coded channel 3, which doesn't exist.
    EXPECT_FALSE(encoder_config->IsOk());
  }

  {
    const SdpAudioFormat sdp_format("multiopus", 48000, 2,
                                    {{"channel_mapping", "0"},
                                     {"coupled_streams", "1"},
                                     {"num_streams", "2"}});
    const absl::optional<AudioEncoderMultiChannelOpus::Config> encoder_config =
        AudioEncoderMultiChannelOpus::SdpToConfig(sdp_format);
    ASSERT_TRUE(encoder_config.has_value());

    // The mapping is too short.
    EXPECT_FALSE(encoder_config->IsOk());
  }
  {
    const SdpAudioFormat sdp_format("multiopus", 48000, 3,
                                    {{"channel_mapping", "0,0,0"},
                                     {"coupled_streams", "0"},
                                     {"num_streams", "1"}});
    const absl::optional<AudioEncoderMultiChannelOpus::Config> encoder_config =
        AudioEncoderMultiChannelOpus::SdpToConfig(sdp_format);
    ASSERT_TRUE(encoder_config.has_value());

    // Coded channel 0 comes from both input channels 0, 1 and 2.
    EXPECT_FALSE(encoder_config->IsOk());
  }
  {
    const SdpAudioFormat sdp_format("multiopus", 48000, 3,
                                    {{"channel_mapping", "0,255,255"},
                                     {"coupled_streams", "0"},
                                     {"num_streams", "1"}});
    const absl::optional<AudioEncoderMultiChannelOpus::Config> encoder_config =
        AudioEncoderMultiChannelOpus::SdpToConfig(sdp_format);
    ASSERT_TRUE(encoder_config.has_value());

    // This is fine, because channels 1, 2 are set to be ignored.
    EXPECT_TRUE(encoder_config->IsOk());
  }
  {
    const SdpAudioFormat sdp_format("multiopus", 48000, 3,
                                    {{"channel_mapping", "0,255,255"},
                                     {"coupled_streams", "0"},
                                     {"num_streams", "2"}});
    const absl::optional<AudioEncoderMultiChannelOpus::Config> encoder_config =
        AudioEncoderMultiChannelOpus::SdpToConfig(sdp_format);
    ASSERT_TRUE(encoder_config.has_value());

    // This is NOT fine, because channels nothing says how coded channel 1
    // should be coded.
    EXPECT_FALSE(encoder_config->IsOk());
  }
}

TEST(AudioEncoderMultiOpusTest, ConfigValuesAreParsedCorrectly) {
  SdpAudioFormat sdp_format({"multiopus",
                             48000,
                             6,
                             {{"minptime", "10"},
                              {"useinbandfec", "1"},
                              {"channel_mapping", "0,4,1,2,3,5"},
                              {"num_streams", "4"},
                              {"coupled_streams", "2"}}});
  const absl::optional<AudioEncoderMultiChannelOpus::Config> encoder_config =
      AudioEncoderMultiChannelOpus::SdpToConfig(sdp_format);
  ASSERT_TRUE(encoder_config.has_value());

  EXPECT_EQ(encoder_config->coupled_streams, 2);
  EXPECT_EQ(encoder_config->num_streams, 4);
  EXPECT_THAT(
      encoder_config->channel_mapping,
      testing::ContainerEq(std::vector<unsigned char>({0, 4, 1, 2, 3, 5})));
}

TEST(AudioEncoderMultiOpusTest, CreateFromValidOrInvalidConfig) {
  {
    const SdpAudioFormat sdp_format("multiopus", 48000, 3,
                                    {{"channel_mapping", "0,255,255"},
                                     {"coupled_streams", "0"},
                                     {"num_streams", "2"}});
    const absl::optional<AudioEncoderMultiChannelOpus::Config> encoder_config =
        AudioEncoderMultiChannelOpus::SdpToConfig(sdp_format);
    ASSERT_TRUE(encoder_config.has_value());

    // Invalid config from the ConfigValidity test. It's not allowed by our
    // checks, but Opus is more forgiving.
    EXPECT_FALSE(encoder_config->IsOk());

    const std::unique_ptr<AudioEncoder> opus_encoder =
        AudioEncoderMultiChannelOpus::MakeAudioEncoder(*encoder_config,
                                                       kOpusPayloadType);

    // Shouldn't be possible (but shouldn't result in a crash) to create an
    // Encoder from an invalid config.
    EXPECT_FALSE(opus_encoder);
  }
  {
    const SdpAudioFormat sdp_format("multiopus", 48000, 3,
                                    {{"channel_mapping", "1,255,0"},
                                     {"coupled_streams", "1"},
                                     {"num_streams", "1"}});
    const absl::optional<AudioEncoderMultiChannelOpus::Config> encoder_config =
        AudioEncoderMultiChannelOpus::SdpToConfig(sdp_format);
    ASSERT_TRUE(encoder_config.has_value());

    EXPECT_THAT(encoder_config->channel_mapping,
                testing::ContainerEq(std::vector<unsigned char>({1, 255, 0})));

    EXPECT_TRUE(encoder_config->IsOk());

    const std::unique_ptr<AudioEncoder> opus_encoder =
        AudioEncoderMultiChannelOpus::MakeAudioEncoder(*encoder_config,
                                                       kOpusPayloadType);

    // Creating an encoder from a valid config should work.
    EXPECT_TRUE(opus_encoder);
  }
}

TEST(AudioEncoderMultiOpusTest, AdvertisedCodecsCanBeCreated) {
  std::vector<AudioCodecSpec> specs;
  AudioEncoderMultiChannelOpus::AppendSupportedEncoders(&specs);

  EXPECT_FALSE(specs.empty());

  for (const AudioCodecSpec& spec : specs) {
    const absl::optional<AudioEncoderMultiChannelOpus::Config> encoder_config =
        AudioEncoderMultiChannelOpus::SdpToConfig(spec.format);
    ASSERT_TRUE(encoder_config.has_value());

    const std::unique_ptr<AudioEncoder> opus_encoder =
        AudioEncoderMultiChannelOpus::MakeAudioEncoder(*encoder_config,
                                                       kOpusPayloadType);

    EXPECT_TRUE(opus_encoder);
  }
}

}  // namespace webrtc
