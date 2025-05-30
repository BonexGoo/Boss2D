/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>

#include BOSS_WEBRTC_U_modules__audio_coding__codecs__g711__audio_encoder_pcm_h //original-code:"modules/audio_coding/codecs/g711/audio_encoder_pcm.h"
#include "modules/audio_coding/neteq/tools/neteq_quality_test.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__flags_h //original-code:"rtc_base/flags.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"
#include "test/testsupport/fileutils.h"

using testing::InitGoogleTest;

namespace webrtc {
namespace test {
namespace {
static const int kInputSampleRateKhz = 8;
static const int kOutputSampleRateKhz = 8;

DEFINE_int(frame_size_ms, 20, "Codec frame size (milliseconds).");

}  // namespace

class NetEqPcmuQualityTest : public NetEqQualityTest {
 protected:
  NetEqPcmuQualityTest()
      : NetEqQualityTest(FLAG_frame_size_ms,
                         kInputSampleRateKhz,
                         kOutputSampleRateKhz,
                         NetEqDecoder::kDecoderPCMu) {
    // Flag validation
    RTC_CHECK(FLAG_frame_size_ms >= 10 && FLAG_frame_size_ms <= 60 &&
              (FLAG_frame_size_ms % 10) == 0)
        << "Invalid frame size, should be 10, 20, ..., 60 ms.";
  }

  void SetUp() override {
    ASSERT_EQ(1u, channels_) << "PCMu supports only mono audio.";
    AudioEncoderPcmU::Config config;
    config.frame_size_ms = FLAG_frame_size_ms;
    encoder_.reset(new AudioEncoderPcmU(config));
    NetEqQualityTest::SetUp();
  }

  int EncodeBlock(int16_t* in_data,
                  size_t block_size_samples,
                  rtc::Buffer* payload,
                  size_t max_bytes) override {
    const size_t kFrameSizeSamples = 80;  // Samples per 10 ms.
    size_t encoded_samples = 0;
    uint32_t dummy_timestamp = 0;
    AudioEncoder::EncodedInfo info;
    do {
      info = encoder_->Encode(dummy_timestamp,
                              rtc::ArrayView<const int16_t>(
                                  in_data + encoded_samples, kFrameSizeSamples),
                              payload);
      encoded_samples += kFrameSizeSamples;
    } while (info.encoded_bytes == 0);
    return rtc::checked_cast<int>(info.encoded_bytes);
  }

 private:
  std::unique_ptr<AudioEncoderPcmU> encoder_;
};

TEST_F(NetEqPcmuQualityTest, Test) {
  Simulate();
}

}  // namespace test
}  // namespace webrtc
