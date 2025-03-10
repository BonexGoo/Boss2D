/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_MOCK_AUDIO_ENCODER_FACTORY_H_
#define TEST_MOCK_AUDIO_ENCODER_FACTORY_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__audio_codecs__audio_encoder_factory_h //original-code:"api/audio_codecs/audio_encoder_factory.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

class MockAudioEncoderFactory
    : public ::testing::NiceMock<AudioEncoderFactory> {
 public:
  MOCK_METHOD(std::vector<AudioCodecSpec>,
              GetSupportedEncoders,
              (),
              (override));
  MOCK_METHOD(absl::optional<AudioCodecInfo>,
              QueryAudioEncoder,
              (const SdpAudioFormat& format),
              (override));

  std::unique_ptr<AudioEncoder> MakeAudioEncoder(
      int payload_type,
      const SdpAudioFormat& format,
      absl::optional<AudioCodecPairId> codec_pair_id) override {
    std::unique_ptr<AudioEncoder> return_value;
    MakeAudioEncoderMock(payload_type, format, codec_pair_id, &return_value);
    return return_value;
  }
  MOCK_METHOD(void,
              MakeAudioEncoderMock,
              (int payload_type,
               const SdpAudioFormat& format,
               absl::optional<AudioCodecPairId> codec_pair_id,
               std::unique_ptr<AudioEncoder>*));

  // Creates a MockAudioEncoderFactory with no formats and that may not be
  // invoked to create a codec - useful for initializing a voice engine, for
  // example.
  static rtc::scoped_refptr<webrtc::MockAudioEncoderFactory>
  CreateUnusedFactory() {
    using ::testing::_;
    using ::testing::AnyNumber;
    using ::testing::Return;

    rtc::scoped_refptr<webrtc::MockAudioEncoderFactory> factory =
        new rtc::RefCountedObject<webrtc::MockAudioEncoderFactory>;
    ON_CALL(*factory.get(), GetSupportedEncoders())
        .WillByDefault(Return(std::vector<webrtc::AudioCodecSpec>()));
    ON_CALL(*factory.get(), QueryAudioEncoder(_))
        .WillByDefault(Return(absl::nullopt));

    EXPECT_CALL(*factory.get(), GetSupportedEncoders()).Times(AnyNumber());
    EXPECT_CALL(*factory.get(), QueryAudioEncoder(_)).Times(AnyNumber());
    EXPECT_CALL(*factory.get(), MakeAudioEncoderMock(_, _, _, _)).Times(0);
    return factory;
  }

  // Creates a MockAudioEncoderFactory with no formats that may be invoked to
  // create a codec any number of times. It will, though, return nullptr on each
  // call, since it supports no codecs.
  static rtc::scoped_refptr<webrtc::MockAudioEncoderFactory>
  CreateEmptyFactory() {
    using ::testing::_;
    using ::testing::AnyNumber;
    using ::testing::Return;
    using ::testing::SetArgPointee;

    rtc::scoped_refptr<webrtc::MockAudioEncoderFactory> factory =
        new rtc::RefCountedObject<webrtc::MockAudioEncoderFactory>;
    ON_CALL(*factory.get(), GetSupportedEncoders())
        .WillByDefault(Return(std::vector<webrtc::AudioCodecSpec>()));
    ON_CALL(*factory.get(), QueryAudioEncoder(_))
        .WillByDefault(Return(absl::nullopt));
    ON_CALL(*factory.get(), MakeAudioEncoderMock(_, _, _, _))
        .WillByDefault(SetArgPointee<3>(nullptr));

    EXPECT_CALL(*factory.get(), GetSupportedEncoders()).Times(AnyNumber());
    EXPECT_CALL(*factory.get(), QueryAudioEncoder(_)).Times(AnyNumber());
    EXPECT_CALL(*factory.get(), MakeAudioEncoderMock(_, _, _, _))
        .Times(AnyNumber());
    return factory;
  }
};

}  // namespace webrtc

#endif  // TEST_MOCK_AUDIO_ENCODER_FACTORY_H_
