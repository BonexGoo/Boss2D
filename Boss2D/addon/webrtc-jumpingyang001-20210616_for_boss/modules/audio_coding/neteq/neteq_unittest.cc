/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__neteq__neteq_h //original-code:"api/neteq/neteq.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>  // memset

#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__flags__flag_h //original-code:"absl/flags/flag.h"
#include BOSS_WEBRTC_U_api__audio__audio_frame_h //original-code:"api/audio/audio_frame.h"
#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_decoder_factory_h //original-code:"api/audio_codecs/builtin_audio_decoder_factory.h"
#include BOSS_WEBRTC_U_modules__audio_coding__codecs__pcm16b__pcm16b_h //original-code:"modules/audio_coding/codecs/pcm16b/pcm16b.h"
#include "modules/audio_coding/neteq/test/neteq_decoding_test.h"
#include BOSS_WEBRTC_U_modules__audio_coding__neteq__tools__audio_loop_h //original-code:"modules/audio_coding/neteq/tools/audio_loop.h"
#include BOSS_WEBRTC_U_modules__audio_coding__neteq__tools__neteq_packet_source_input_h //original-code:"modules/audio_coding/neteq/tools/neteq_packet_source_input.h"
#include BOSS_WEBRTC_U_modules__audio_coding__neteq__tools__neteq_test_h //original-code:"modules/audio_coding/neteq/tools/neteq_test.h"
#include BOSS_WEBRTC_U_modules__include__module_common_types_public_h //original-code:"modules/include/module_common_types_public.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtcp_statistics_h //original-code:"modules/rtp_rtcp/include/rtcp_statistics.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_rtc_base__ignore_wundef_h //original-code:"rtc_base/ignore_wundef.h"
#include BOSS_WEBRTC_U_rtc_base__message_digest_h //original-code:"rtc_base/message_digest.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"
#include BOSS_WEBRTC_U_rtc_base__string_encode_h //original-code:"rtc_base/string_encode.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"
#include BOSS_WEBRTC_U_rtc_base__system__arch_h //original-code:"rtc_base/system/arch.h"
#include BOSS_WEBRTC_U_test__field_trial_h //original-code:"test/field_trial.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include "test/testsupport/file_utils.h"

ABSL_FLAG(bool, gen_ref, false, "Generate reference files.");

namespace webrtc {

namespace {

const std::string& PlatformChecksum(const std::string& checksum_general,
                                    const std::string& checksum_android_32,
                                    const std::string& checksum_android_64,
                                    const std::string& checksum_win_32,
                                    const std::string& checksum_win_64) {
#if defined(WEBRTC_ANDROID)
#ifdef WEBRTC_ARCH_64_BITS
  return checksum_android_64;
#else
  return checksum_android_32;
#endif  // WEBRTC_ARCH_64_BITS
#elif defined(WEBRTC_WIN)
#ifdef WEBRTC_ARCH_64_BITS
  return checksum_win_64;
#else
  return checksum_win_32;
#endif  // WEBRTC_ARCH_64_BITS
#else
  return checksum_general;
#endif  // WEBRTC_WIN
}

}  // namespace


#if !defined(WEBRTC_IOS) && defined(WEBRTC_NETEQ_UNITTEST_BITEXACT) && \
    (defined(WEBRTC_CODEC_ISAC) || defined(WEBRTC_CODEC_ISACFX)) &&    \
    defined(WEBRTC_CODEC_ILBC) && !defined(WEBRTC_ARCH_ARM64)
#define MAYBE_TestBitExactness TestBitExactness
#else
#define MAYBE_TestBitExactness DISABLED_TestBitExactness
#endif
TEST_F(NetEqDecodingTest, MAYBE_TestBitExactness) {
  const std::string input_rtp_file =
      webrtc::test::ResourcePath("audio_coding/neteq_universal_new", "rtp");

  const std::string output_checksum =
      PlatformChecksum("6c35140ce4d75874bdd60aa1872400b05fd05ca2",
                       "ab451bb8301d9a92fbf4de91556b56f1ea38b4ce", "not used",
                       "6c35140ce4d75874bdd60aa1872400b05fd05ca2",
                       "64b46bb3c1165537a880ae8404afce2efba456c0");

  const std::string network_stats_checksum =
      PlatformChecksum("90594d85fa31d3d9584d79293bf7aa4ee55ed751",
                       "77b9c3640b81aff6a38d69d07dd782d39c15321d", "not used",
                       "90594d85fa31d3d9584d79293bf7aa4ee55ed751",
                       "90594d85fa31d3d9584d79293bf7aa4ee55ed751");

  DecodeAndCompare(input_rtp_file, output_checksum, network_stats_checksum,
                   absl::GetFlag(FLAGS_gen_ref));
}

#if !defined(WEBRTC_IOS) && defined(WEBRTC_NETEQ_UNITTEST_BITEXACT) && \
    defined(WEBRTC_CODEC_OPUS)
#define MAYBE_TestOpusBitExactness TestOpusBitExactness
#else
#define MAYBE_TestOpusBitExactness DISABLED_TestOpusBitExactness
#endif
// TODO(http://bugs.webrtc.org/12518): Enable the test after Opus has been
// updated.
TEST_F(NetEqDecodingTest, DISABLED_TestOpusBitExactness) {
  const std::string input_rtp_file =
      webrtc::test::ResourcePath("audio_coding/neteq_opus", "rtp");

  const std::string maybe_sse =
      "c7887ff60eecf460332c6c7a28c81561f9e8a40f"
      "|673dd422cfc174152536d3b13af64f9722520ab5";
  const std::string output_checksum = PlatformChecksum(
      maybe_sse, "e39283dd61a89cead3786ef8642d2637cc447296",
      "53d8073eb848b70974cba9e26424f4946508fd19", maybe_sse, maybe_sse);

  const std::string network_stats_checksum =
      PlatformChecksum("c438bfa3b018f77691279eb9c63730569f54585c",
                       "8a474ed0992591e0c84f593824bb05979c3de157",
                       "9a05378dbf7e6edd56cdeb8ec45bcd6d8589623c",
                       "c438bfa3b018f77691279eb9c63730569f54585c",
                       "c438bfa3b018f77691279eb9c63730569f54585c");

  DecodeAndCompare(input_rtp_file, output_checksum, network_stats_checksum,
                   absl::GetFlag(FLAGS_gen_ref));
}

// TODO(http://bugs.webrtc.org/12518): Enable the test after Opus has been
// updated.
TEST_F(NetEqDecodingTest, DISABLED_TestOpusDtxBitExactness) {
  const std::string input_rtp_file =
      webrtc::test::ResourcePath("audio_coding/neteq_opus_dtx", "rtp");

  const std::string maybe_sse =
      "0fb0a3d6b3758ca6e108368bb777cd38d0a865af"
      "|79cfb99a21338ba977eb0e15eb8464e2db9436f8";
  const std::string output_checksum = PlatformChecksum(
      maybe_sse, "b6632690f8d7c2340c838df2821fc014f1cc8360",
      "f890b9eb9bc5ab8313489230726b297f6a0825af", maybe_sse, maybe_sse);

  const std::string network_stats_checksum =
      "18983bb67a57628c604dbdefa99574c6e0c5bb48";

  DecodeAndCompare(input_rtp_file, output_checksum, network_stats_checksum,
                   absl::GetFlag(FLAGS_gen_ref));
}

// Use fax mode to avoid time-scaling. This is to simplify the testing of
// packet waiting times in the packet buffer.
class NetEqDecodingTestFaxMode : public NetEqDecodingTest {
 protected:
  NetEqDecodingTestFaxMode() : NetEqDecodingTest() {
    config_.for_test_no_time_stretching = true;
  }
  void TestJitterBufferDelay(bool apply_packet_loss);
};

TEST_F(NetEqDecodingTestFaxMode, TestFrameWaitingTimeStatistics) {
  // Insert 30 dummy packets at once. Each packet contains 10 ms 16 kHz audio.
  size_t num_frames = 30;
  const size_t kSamples = 10 * 16;
  const size_t kPayloadBytes = kSamples * 2;
  for (size_t i = 0; i < num_frames; ++i) {
    const uint8_t payload[kPayloadBytes] = {0};
    RTPHeader rtp_info;
    rtp_info.sequenceNumber = rtc::checked_cast<uint16_t>(i);
    rtp_info.timestamp = rtc::checked_cast<uint32_t>(i * kSamples);
    rtp_info.ssrc = 0x1234;     // Just an arbitrary SSRC.
    rtp_info.payloadType = 94;  // PCM16b WB codec.
    rtp_info.markerBit = 0;
    ASSERT_EQ(0, neteq_->InsertPacket(rtp_info, payload));
  }
  // Pull out all data.
  for (size_t i = 0; i < num_frames; ++i) {
    bool muted;
    ASSERT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
    ASSERT_EQ(kBlockSize16kHz, out_frame_.samples_per_channel_);
  }

  NetEqNetworkStatistics stats;
  EXPECT_EQ(0, neteq_->NetworkStatistics(&stats));
  // Since all frames are dumped into NetEQ at once, but pulled out with 10 ms
  // spacing (per definition), we expect the delay to increase with 10 ms for
  // each packet. Thus, we are calculating the statistics for a series from 10
  // to 300, in steps of 10 ms.
  EXPECT_EQ(155, stats.mean_waiting_time_ms);
  EXPECT_EQ(155, stats.median_waiting_time_ms);
  EXPECT_EQ(10, stats.min_waiting_time_ms);
  EXPECT_EQ(300, stats.max_waiting_time_ms);

  // Check statistics again and make sure it's been reset.
  EXPECT_EQ(0, neteq_->NetworkStatistics(&stats));
  EXPECT_EQ(-1, stats.mean_waiting_time_ms);
  EXPECT_EQ(-1, stats.median_waiting_time_ms);
  EXPECT_EQ(-1, stats.min_waiting_time_ms);
  EXPECT_EQ(-1, stats.max_waiting_time_ms);
}


TEST_F(NetEqDecodingTest, LongCngWithNegativeClockDrift) {
  // Apply a clock drift of -25 ms / s (sender faster than receiver).
  const double kDriftFactor = 1000.0 / (1000.0 + 25.0);
  const double kNetworkFreezeTimeMs = 0.0;
  const bool kGetAudioDuringFreezeRecovery = false;
  const int kDelayToleranceMs = 20;
  const int kMaxTimeToSpeechMs = 100;
  LongCngWithClockDrift(kDriftFactor, kNetworkFreezeTimeMs,
                        kGetAudioDuringFreezeRecovery, kDelayToleranceMs,
                        kMaxTimeToSpeechMs);
}

TEST_F(NetEqDecodingTest, LongCngWithPositiveClockDrift) {
  // Apply a clock drift of +25 ms / s (sender slower than receiver).
  const double kDriftFactor = 1000.0 / (1000.0 - 25.0);
  const double kNetworkFreezeTimeMs = 0.0;
  const bool kGetAudioDuringFreezeRecovery = false;
  const int kDelayToleranceMs = 40;
  const int kMaxTimeToSpeechMs = 100;
  LongCngWithClockDrift(kDriftFactor, kNetworkFreezeTimeMs,
                        kGetAudioDuringFreezeRecovery, kDelayToleranceMs,
                        kMaxTimeToSpeechMs);
}

TEST_F(NetEqDecodingTest, LongCngWithNegativeClockDriftNetworkFreeze) {
  // Apply a clock drift of -25 ms / s (sender faster than receiver).
  const double kDriftFactor = 1000.0 / (1000.0 + 25.0);
  const double kNetworkFreezeTimeMs = 5000.0;
  const bool kGetAudioDuringFreezeRecovery = false;
  const int kDelayToleranceMs = 60;
  const int kMaxTimeToSpeechMs = 200;
  LongCngWithClockDrift(kDriftFactor, kNetworkFreezeTimeMs,
                        kGetAudioDuringFreezeRecovery, kDelayToleranceMs,
                        kMaxTimeToSpeechMs);
}

TEST_F(NetEqDecodingTest, LongCngWithPositiveClockDriftNetworkFreeze) {
  // Apply a clock drift of +25 ms / s (sender slower than receiver).
  const double kDriftFactor = 1000.0 / (1000.0 - 25.0);
  const double kNetworkFreezeTimeMs = 5000.0;
  const bool kGetAudioDuringFreezeRecovery = false;
  const int kDelayToleranceMs = 40;
  const int kMaxTimeToSpeechMs = 100;
  LongCngWithClockDrift(kDriftFactor, kNetworkFreezeTimeMs,
                        kGetAudioDuringFreezeRecovery, kDelayToleranceMs,
                        kMaxTimeToSpeechMs);
}

TEST_F(NetEqDecodingTest, LongCngWithPositiveClockDriftNetworkFreezeExtraPull) {
  // Apply a clock drift of +25 ms / s (sender slower than receiver).
  const double kDriftFactor = 1000.0 / (1000.0 - 25.0);
  const double kNetworkFreezeTimeMs = 5000.0;
  const bool kGetAudioDuringFreezeRecovery = true;
  const int kDelayToleranceMs = 40;
  const int kMaxTimeToSpeechMs = 100;
  LongCngWithClockDrift(kDriftFactor, kNetworkFreezeTimeMs,
                        kGetAudioDuringFreezeRecovery, kDelayToleranceMs,
                        kMaxTimeToSpeechMs);
}

TEST_F(NetEqDecodingTest, LongCngWithoutClockDrift) {
  const double kDriftFactor = 1.0;  // No drift.
  const double kNetworkFreezeTimeMs = 0.0;
  const bool kGetAudioDuringFreezeRecovery = false;
  const int kDelayToleranceMs = 10;
  const int kMaxTimeToSpeechMs = 50;
  LongCngWithClockDrift(kDriftFactor, kNetworkFreezeTimeMs,
                        kGetAudioDuringFreezeRecovery, kDelayToleranceMs,
                        kMaxTimeToSpeechMs);
}

TEST_F(NetEqDecodingTest, UnknownPayloadType) {
  const size_t kPayloadBytes = 100;
  uint8_t payload[kPayloadBytes] = {0};
  RTPHeader rtp_info;
  PopulateRtpInfo(0, 0, &rtp_info);
  rtp_info.payloadType = 1;  // Not registered as a decoder.
  EXPECT_EQ(NetEq::kFail, neteq_->InsertPacket(rtp_info, payload));
}

#if defined(WEBRTC_CODEC_ISAC) || defined(WEBRTC_CODEC_ISACFX)
#define MAYBE_DecoderError DecoderError
#else
#define MAYBE_DecoderError DISABLED_DecoderError
#endif

TEST_F(NetEqDecodingTest, MAYBE_DecoderError) {
  const size_t kPayloadBytes = 100;
  uint8_t payload[kPayloadBytes] = {0};
  RTPHeader rtp_info;
  PopulateRtpInfo(0, 0, &rtp_info);
  rtp_info.payloadType = 103;  // iSAC, but the payload is invalid.
  EXPECT_EQ(0, neteq_->InsertPacket(rtp_info, payload));
  // Set all of |out_data_| to 1, and verify that it was set to 0 by the call
  // to GetAudio.
  int16_t* out_frame_data = out_frame_.mutable_data();
  for (size_t i = 0; i < AudioFrame::kMaxDataSizeSamples; ++i) {
    out_frame_data[i] = 1;
  }
  bool muted;
  EXPECT_EQ(NetEq::kFail, neteq_->GetAudio(&out_frame_, &muted));
  ASSERT_FALSE(muted);

  // Verify that the first 160 samples are set to 0.
  static const int kExpectedOutputLength = 160;  // 10 ms at 16 kHz sample rate.
  const int16_t* const_out_frame_data = out_frame_.data();
  for (int i = 0; i < kExpectedOutputLength; ++i) {
    rtc::StringBuilder ss;
    ss << "i = " << i;
    SCOPED_TRACE(ss.str());  // Print out the parameter values on failure.
    EXPECT_EQ(0, const_out_frame_data[i]);
  }
}

TEST_F(NetEqDecodingTest, GetAudioBeforeInsertPacket) {
  // Set all of |out_data_| to 1, and verify that it was set to 0 by the call
  // to GetAudio.
  int16_t* out_frame_data = out_frame_.mutable_data();
  for (size_t i = 0; i < AudioFrame::kMaxDataSizeSamples; ++i) {
    out_frame_data[i] = 1;
  }
  bool muted;
  EXPECT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
  ASSERT_FALSE(muted);
  // Verify that the first block of samples is set to 0.
  static const int kExpectedOutputLength =
      kInitSampleRateHz / 100;  // 10 ms at initial sample rate.
  const int16_t* const_out_frame_data = out_frame_.data();
  for (int i = 0; i < kExpectedOutputLength; ++i) {
    rtc::StringBuilder ss;
    ss << "i = " << i;
    SCOPED_TRACE(ss.str());  // Print out the parameter values on failure.
    EXPECT_EQ(0, const_out_frame_data[i]);
  }
  // Verify that the sample rate did not change from the initial configuration.
  EXPECT_EQ(config_.sample_rate_hz, neteq_->last_output_sample_rate_hz());
}

class NetEqBgnTest : public NetEqDecodingTest {
 protected:
  void CheckBgn(int sampling_rate_hz) {
    size_t expected_samples_per_channel = 0;
    uint8_t payload_type = 0xFF;  // Invalid.
    if (sampling_rate_hz == 8000) {
      expected_samples_per_channel = kBlockSize8kHz;
      payload_type = 93;  // PCM 16, 8 kHz.
    } else if (sampling_rate_hz == 16000) {
      expected_samples_per_channel = kBlockSize16kHz;
      payload_type = 94;  // PCM 16, 16 kHZ.
    } else if (sampling_rate_hz == 32000) {
      expected_samples_per_channel = kBlockSize32kHz;
      payload_type = 95;  // PCM 16, 32 kHz.
    } else {
      ASSERT_TRUE(false);  // Unsupported test case.
    }

    AudioFrame output;
    test::AudioLoop input;
    // We are using the same 32 kHz input file for all tests, regardless of
    // |sampling_rate_hz|. The output may sound weird, but the test is still
    // valid.
    ASSERT_TRUE(input.Init(
        webrtc::test::ResourcePath("audio_coding/testfile32kHz", "pcm"),
        10 * sampling_rate_hz,  // Max 10 seconds loop length.
        expected_samples_per_channel));

    // Payload of 10 ms of PCM16 32 kHz.
    uint8_t payload[kBlockSize32kHz * sizeof(int16_t)];
    RTPHeader rtp_info;
    PopulateRtpInfo(0, 0, &rtp_info);
    rtp_info.payloadType = payload_type;

    uint32_t receive_timestamp = 0;
    bool muted;
    for (int n = 0; n < 10; ++n) {  // Insert few packets and get audio.
      auto block = input.GetNextBlock();
      ASSERT_EQ(expected_samples_per_channel, block.size());
      size_t enc_len_bytes =
          WebRtcPcm16b_Encode(block.data(), block.size(), payload);
      ASSERT_EQ(enc_len_bytes, expected_samples_per_channel * 2);

      ASSERT_EQ(0, neteq_->InsertPacket(rtp_info, rtc::ArrayView<const uint8_t>(
                                                      payload, enc_len_bytes)));
      output.Reset();
      ASSERT_EQ(0, neteq_->GetAudio(&output, &muted));
      ASSERT_EQ(1u, output.num_channels_);
      ASSERT_EQ(expected_samples_per_channel, output.samples_per_channel_);
      ASSERT_EQ(AudioFrame::kNormalSpeech, output.speech_type_);

      // Next packet.
      rtp_info.timestamp +=
          rtc::checked_cast<uint32_t>(expected_samples_per_channel);
      rtp_info.sequenceNumber++;
      receive_timestamp +=
          rtc::checked_cast<uint32_t>(expected_samples_per_channel);
    }

    output.Reset();

    // Get audio without inserting packets, expecting PLC and PLC-to-CNG. Pull
    // one frame without checking speech-type. This is the first frame pulled
    // without inserting any packet, and might not be labeled as PLC.
    ASSERT_EQ(0, neteq_->GetAudio(&output, &muted));
    ASSERT_EQ(1u, output.num_channels_);
    ASSERT_EQ(expected_samples_per_channel, output.samples_per_channel_);

    // To be able to test the fading of background noise we need at lease to
    // pull 611 frames.
    const int kFadingThreshold = 611;

    // Test several CNG-to-PLC packet for the expected behavior. The number 20
    // is arbitrary, but sufficiently large to test enough number of frames.
    const int kNumPlcToCngTestFrames = 20;
    bool plc_to_cng = false;
    for (int n = 0; n < kFadingThreshold + kNumPlcToCngTestFrames; ++n) {
      output.Reset();
      // Set to non-zero.
      memset(output.mutable_data(), 1, AudioFrame::kMaxDataSizeBytes);
      ASSERT_EQ(0, neteq_->GetAudio(&output, &muted));
      ASSERT_FALSE(muted);
      ASSERT_EQ(1u, output.num_channels_);
      ASSERT_EQ(expected_samples_per_channel, output.samples_per_channel_);
      if (output.speech_type_ == AudioFrame::kPLCCNG) {
        plc_to_cng = true;
        double sum_squared = 0;
        const int16_t* output_data = output.data();
        for (size_t k = 0;
             k < output.num_channels_ * output.samples_per_channel_; ++k)
          sum_squared += output_data[k] * output_data[k];
        EXPECT_EQ(0, sum_squared);
      } else {
        EXPECT_EQ(AudioFrame::kPLC, output.speech_type_);
      }
    }
    EXPECT_TRUE(plc_to_cng);  // Just to be sure that PLC-to-CNG has occurred.
  }
};

TEST_F(NetEqBgnTest, RunTest) {
  CheckBgn(8000);
  CheckBgn(16000);
  CheckBgn(32000);
}

TEST_F(NetEqDecodingTest, SequenceNumberWrap) {
  // Start with a sequence number that will soon wrap.
  std::set<uint16_t> drop_seq_numbers;  // Don't drop any packets.
  WrapTest(0xFFFF - 10, 0, drop_seq_numbers, true, false);
}

TEST_F(NetEqDecodingTest, SequenceNumberWrapAndDrop) {
  // Start with a sequence number that will soon wrap.
  std::set<uint16_t> drop_seq_numbers;
  drop_seq_numbers.insert(0xFFFF);
  drop_seq_numbers.insert(0x0);
  WrapTest(0xFFFF - 10, 0, drop_seq_numbers, true, false);
}

TEST_F(NetEqDecodingTest, TimestampWrap) {
  // Start with a timestamp that will soon wrap.
  std::set<uint16_t> drop_seq_numbers;
  WrapTest(0, 0xFFFFFFFF - 3000, drop_seq_numbers, false, true);
}

TEST_F(NetEqDecodingTest, TimestampAndSequenceNumberWrap) {
  // Start with a timestamp and a sequence number that will wrap at the same
  // time.
  std::set<uint16_t> drop_seq_numbers;
  WrapTest(0xFFFF - 10, 0xFFFFFFFF - 5000, drop_seq_numbers, true, true);
}

TEST_F(NetEqDecodingTest, DiscardDuplicateCng) {
  uint16_t seq_no = 0;
  uint32_t timestamp = 0;
  const int kFrameSizeMs = 10;
  const int kSampleRateKhz = 16;
  const int kSamples = kFrameSizeMs * kSampleRateKhz;
  const size_t kPayloadBytes = kSamples * 2;

  const int algorithmic_delay_samples =
      std::max(algorithmic_delay_ms_ * kSampleRateKhz, 5 * kSampleRateKhz / 8);
  // Insert three speech packets. Three are needed to get the frame length
  // correct.
  uint8_t payload[kPayloadBytes] = {0};
  RTPHeader rtp_info;
  bool muted;
  for (int i = 0; i < 3; ++i) {
    PopulateRtpInfo(seq_no, timestamp, &rtp_info);
    ASSERT_EQ(0, neteq_->InsertPacket(rtp_info, payload));
    ++seq_no;
    timestamp += kSamples;

    // Pull audio once.
    ASSERT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
    ASSERT_EQ(kBlockSize16kHz, out_frame_.samples_per_channel_);
  }
  // Verify speech output.
  EXPECT_EQ(AudioFrame::kNormalSpeech, out_frame_.speech_type_);

  // Insert same CNG packet twice.
  const int kCngPeriodMs = 100;
  const int kCngPeriodSamples = kCngPeriodMs * kSampleRateKhz;
  size_t payload_len;
  PopulateCng(seq_no, timestamp, &rtp_info, payload, &payload_len);
  // This is the first time this CNG packet is inserted.
  ASSERT_EQ(0, neteq_->InsertPacket(rtp_info, rtc::ArrayView<const uint8_t>(
                                                  payload, payload_len)));

  // Pull audio once and make sure CNG is played.
  ASSERT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
  ASSERT_EQ(kBlockSize16kHz, out_frame_.samples_per_channel_);
  EXPECT_EQ(AudioFrame::kCNG, out_frame_.speech_type_);
  EXPECT_FALSE(
      neteq_->GetPlayoutTimestamp());  // Returns empty value during CNG.
  EXPECT_EQ(timestamp - algorithmic_delay_samples,
            out_frame_.timestamp_ + out_frame_.samples_per_channel_);

  // Insert the same CNG packet again. Note that at this point it is old, since
  // we have already decoded the first copy of it.
  ASSERT_EQ(0, neteq_->InsertPacket(rtp_info, rtc::ArrayView<const uint8_t>(
                                                  payload, payload_len)));

  // Pull audio until we have played |kCngPeriodMs| of CNG. Start at 10 ms since
  // we have already pulled out CNG once.
  for (int cng_time_ms = 10; cng_time_ms < kCngPeriodMs; cng_time_ms += 10) {
    ASSERT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
    ASSERT_EQ(kBlockSize16kHz, out_frame_.samples_per_channel_);
    EXPECT_EQ(AudioFrame::kCNG, out_frame_.speech_type_);
    EXPECT_FALSE(
        neteq_->GetPlayoutTimestamp());  // Returns empty value during CNG.
    EXPECT_EQ(timestamp - algorithmic_delay_samples,
              out_frame_.timestamp_ + out_frame_.samples_per_channel_);
  }

  // Insert speech again.
  ++seq_no;
  timestamp += kCngPeriodSamples;
  PopulateRtpInfo(seq_no, timestamp, &rtp_info);
  ASSERT_EQ(0, neteq_->InsertPacket(rtp_info, payload));

  // Pull audio once and verify that the output is speech again.
  ASSERT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
  ASSERT_EQ(kBlockSize16kHz, out_frame_.samples_per_channel_);
  EXPECT_EQ(AudioFrame::kNormalSpeech, out_frame_.speech_type_);
  absl::optional<uint32_t> playout_timestamp = neteq_->GetPlayoutTimestamp();
  ASSERT_TRUE(playout_timestamp);
  EXPECT_EQ(timestamp + kSamples - algorithmic_delay_samples,
            *playout_timestamp);
}

TEST_F(NetEqDecodingTest, CngFirst) {
  uint16_t seq_no = 0;
  uint32_t timestamp = 0;
  const int kFrameSizeMs = 10;
  const int kSampleRateKhz = 16;
  const int kSamples = kFrameSizeMs * kSampleRateKhz;
  const int kPayloadBytes = kSamples * 2;
  const int kCngPeriodMs = 100;
  const int kCngPeriodSamples = kCngPeriodMs * kSampleRateKhz;
  size_t payload_len;

  uint8_t payload[kPayloadBytes] = {0};
  RTPHeader rtp_info;

  PopulateCng(seq_no, timestamp, &rtp_info, payload, &payload_len);
  ASSERT_EQ(NetEq::kOK,
            neteq_->InsertPacket(
                rtp_info, rtc::ArrayView<const uint8_t>(payload, payload_len)));
  ++seq_no;
  timestamp += kCngPeriodSamples;

  // Pull audio once and make sure CNG is played.
  bool muted;
  ASSERT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
  ASSERT_EQ(kBlockSize16kHz, out_frame_.samples_per_channel_);
  EXPECT_EQ(AudioFrame::kCNG, out_frame_.speech_type_);

  // Insert some speech packets.
  const uint32_t first_speech_timestamp = timestamp;
  int timeout_counter = 0;
  do {
    ASSERT_LT(timeout_counter++, 20) << "Test timed out";
    PopulateRtpInfo(seq_no, timestamp, &rtp_info);
    ASSERT_EQ(0, neteq_->InsertPacket(rtp_info, payload));
    ++seq_no;
    timestamp += kSamples;

    // Pull audio once.
    ASSERT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
    ASSERT_EQ(kBlockSize16kHz, out_frame_.samples_per_channel_);
  } while (!IsNewerTimestamp(out_frame_.timestamp_, first_speech_timestamp));
  // Verify speech output.
  EXPECT_EQ(AudioFrame::kNormalSpeech, out_frame_.speech_type_);
}

class NetEqDecodingTestWithMutedState : public NetEqDecodingTest {
 public:
  NetEqDecodingTestWithMutedState() : NetEqDecodingTest() {
    config_.enable_muted_state = true;
  }

 protected:
  static constexpr size_t kSamples = 10 * 16;
  static constexpr size_t kPayloadBytes = kSamples * 2;

  void InsertPacket(uint32_t rtp_timestamp) {
    uint8_t payload[kPayloadBytes] = {0};
    RTPHeader rtp_info;
    PopulateRtpInfo(0, rtp_timestamp, &rtp_info);
    EXPECT_EQ(0, neteq_->InsertPacket(rtp_info, payload));
  }

  void InsertCngPacket(uint32_t rtp_timestamp) {
    uint8_t payload[kPayloadBytes] = {0};
    RTPHeader rtp_info;
    size_t payload_len;
    PopulateCng(0, rtp_timestamp, &rtp_info, payload, &payload_len);
    EXPECT_EQ(NetEq::kOK,
              neteq_->InsertPacket(rtp_info, rtc::ArrayView<const uint8_t>(
                                                 payload, payload_len)));
  }

  bool GetAudioReturnMuted() {
    bool muted;
    EXPECT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
    return muted;
  }

  void GetAudioUntilMuted() {
    while (!GetAudioReturnMuted()) {
      ASSERT_LT(counter_++, 1000) << "Test timed out";
    }
  }

  void GetAudioUntilNormal() {
    bool muted = false;
    while (out_frame_.speech_type_ != AudioFrame::kNormalSpeech) {
      EXPECT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
      ASSERT_LT(counter_++, 1000) << "Test timed out";
    }
    EXPECT_FALSE(muted);
  }

  int counter_ = 0;
};

// Verifies that NetEq goes in and out of muted state as expected.
TEST_F(NetEqDecodingTestWithMutedState, MutedState) {
  // Insert one speech packet.
  InsertPacket(0);
  // Pull out audio once and expect it not to be muted.
  EXPECT_FALSE(GetAudioReturnMuted());
  // Pull data until faded out.
  GetAudioUntilMuted();
  EXPECT_TRUE(out_frame_.muted());

  // Verify that output audio is not written during muted mode. Other parameters
  // should be correct, though.
  AudioFrame new_frame;
  int16_t* frame_data = new_frame.mutable_data();
  for (size_t i = 0; i < AudioFrame::kMaxDataSizeSamples; i++) {
    frame_data[i] = 17;
  }
  bool muted;
  EXPECT_EQ(0, neteq_->GetAudio(&new_frame, &muted));
  EXPECT_TRUE(muted);
  EXPECT_TRUE(out_frame_.muted());
  for (size_t i = 0; i < AudioFrame::kMaxDataSizeSamples; i++) {
    EXPECT_EQ(17, frame_data[i]);
  }
  EXPECT_EQ(out_frame_.timestamp_ + out_frame_.samples_per_channel_,
            new_frame.timestamp_);
  EXPECT_EQ(out_frame_.samples_per_channel_, new_frame.samples_per_channel_);
  EXPECT_EQ(out_frame_.sample_rate_hz_, new_frame.sample_rate_hz_);
  EXPECT_EQ(out_frame_.num_channels_, new_frame.num_channels_);
  EXPECT_EQ(out_frame_.speech_type_, new_frame.speech_type_);
  EXPECT_EQ(out_frame_.vad_activity_, new_frame.vad_activity_);

  // Insert new data. Timestamp is corrected for the time elapsed since the last
  // packet. Verify that normal operation resumes.
  InsertPacket(kSamples * counter_);
  GetAudioUntilNormal();
  EXPECT_FALSE(out_frame_.muted());

  NetEqNetworkStatistics stats;
  EXPECT_EQ(0, neteq_->NetworkStatistics(&stats));
  // NetEqNetworkStatistics::expand_rate tells the fraction of samples that were
  // concealment samples, in Q14 (16384 = 100%) .The vast majority should be
  // concealment samples in this test.
  EXPECT_GT(stats.expand_rate, 14000);
  // And, it should be greater than the speech_expand_rate.
  EXPECT_GT(stats.expand_rate, stats.speech_expand_rate);
}

// Verifies that NetEq goes out of muted state when given a delayed packet.
TEST_F(NetEqDecodingTestWithMutedState, MutedStateDelayedPacket) {
  // Insert one speech packet.
  InsertPacket(0);
  // Pull out audio once and expect it not to be muted.
  EXPECT_FALSE(GetAudioReturnMuted());
  // Pull data until faded out.
  GetAudioUntilMuted();
  // Insert new data. Timestamp is only corrected for the half of the time
  // elapsed since the last packet. That is, the new packet is delayed. Verify
  // that normal operation resumes.
  InsertPacket(kSamples * counter_ / 2);
  GetAudioUntilNormal();
}

// Verifies that NetEq goes out of muted state when given a future packet.
TEST_F(NetEqDecodingTestWithMutedState, MutedStateFuturePacket) {
  // Insert one speech packet.
  InsertPacket(0);
  // Pull out audio once and expect it not to be muted.
  EXPECT_FALSE(GetAudioReturnMuted());
  // Pull data until faded out.
  GetAudioUntilMuted();
  // Insert new data. Timestamp is over-corrected for the time elapsed since the
  // last packet. That is, the new packet is too early. Verify that normal
  // operation resumes.
  InsertPacket(kSamples * counter_ * 2);
  GetAudioUntilNormal();
}

// Verifies that NetEq goes out of muted state when given an old packet.
TEST_F(NetEqDecodingTestWithMutedState, MutedStateOldPacket) {
  // Insert one speech packet.
  InsertPacket(0);
  // Pull out audio once and expect it not to be muted.
  EXPECT_FALSE(GetAudioReturnMuted());
  // Pull data until faded out.
  GetAudioUntilMuted();

  EXPECT_NE(AudioFrame::kNormalSpeech, out_frame_.speech_type_);
  // Insert a few packets which are older than the first packet.
  for (int i = 0; i < 5; ++i) {
    InsertPacket(kSamples * (i - 1000));
  }
  EXPECT_FALSE(GetAudioReturnMuted());
  EXPECT_EQ(AudioFrame::kNormalSpeech, out_frame_.speech_type_);
}

// Verifies that NetEq doesn't enter muted state when CNG mode is active and the
// packet stream is suspended for a long time.
TEST_F(NetEqDecodingTestWithMutedState, DoNotMuteExtendedCngWithoutPackets) {
  // Insert one CNG packet.
  InsertCngPacket(0);

  // Pull 10 seconds of audio (10 ms audio generated per lap).
  for (int i = 0; i < 1000; ++i) {
    bool muted;
    EXPECT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
    ASSERT_FALSE(muted);
  }
  EXPECT_EQ(AudioFrame::kCNG, out_frame_.speech_type_);
}

// Verifies that NetEq goes back to normal after a long CNG period with the
// packet stream suspended.
TEST_F(NetEqDecodingTestWithMutedState, RecoverAfterExtendedCngWithoutPackets) {
  // Insert one CNG packet.
  InsertCngPacket(0);

  // Pull 10 seconds of audio (10 ms audio generated per lap).
  for (int i = 0; i < 1000; ++i) {
    bool muted;
    EXPECT_EQ(0, neteq_->GetAudio(&out_frame_, &muted));
  }

  // Insert new data. Timestamp is corrected for the time elapsed since the last
  // packet. Verify that normal operation resumes.
  InsertPacket(kSamples * counter_);
  GetAudioUntilNormal();
}

namespace {
::testing::AssertionResult AudioFramesEqualExceptData(const AudioFrame& a,
                                                      const AudioFrame& b) {
  if (a.timestamp_ != b.timestamp_)
    return ::testing::AssertionFailure() << "timestamp_ diff (" << a.timestamp_
                                         << " != " << b.timestamp_ << ")";
  if (a.sample_rate_hz_ != b.sample_rate_hz_)
    return ::testing::AssertionFailure()
           << "sample_rate_hz_ diff (" << a.sample_rate_hz_
           << " != " << b.sample_rate_hz_ << ")";
  if (a.samples_per_channel_ != b.samples_per_channel_)
    return ::testing::AssertionFailure()
           << "samples_per_channel_ diff (" << a.samples_per_channel_
           << " != " << b.samples_per_channel_ << ")";
  if (a.num_channels_ != b.num_channels_)
    return ::testing::AssertionFailure()
           << "num_channels_ diff (" << a.num_channels_
           << " != " << b.num_channels_ << ")";
  if (a.speech_type_ != b.speech_type_)
    return ::testing::AssertionFailure()
           << "speech_type_ diff (" << a.speech_type_
           << " != " << b.speech_type_ << ")";
  if (a.vad_activity_ != b.vad_activity_)
    return ::testing::AssertionFailure()
           << "vad_activity_ diff (" << a.vad_activity_
           << " != " << b.vad_activity_ << ")";
  return ::testing::AssertionSuccess();
}

::testing::AssertionResult AudioFramesEqual(const AudioFrame& a,
                                            const AudioFrame& b) {
  ::testing::AssertionResult res = AudioFramesEqualExceptData(a, b);
  if (!res)
    return res;
  if (memcmp(a.data(), b.data(),
             a.samples_per_channel_ * a.num_channels_ * sizeof(*a.data())) !=
      0) {
    return ::testing::AssertionFailure() << "data_ diff";
  }
  return ::testing::AssertionSuccess();
}

}  // namespace

TEST_F(NetEqDecodingTestTwoInstances, CompareMutedStateOnOff) {
  ASSERT_FALSE(config_.enable_muted_state);
  config2_.enable_muted_state = true;
  CreateSecondInstance();

  // Insert one speech packet into both NetEqs.
  const size_t kSamples = 10 * 16;
  const size_t kPayloadBytes = kSamples * 2;
  uint8_t payload[kPayloadBytes] = {0};
  RTPHeader rtp_info;
  PopulateRtpInfo(0, 0, &rtp_info);
  EXPECT_EQ(0, neteq_->InsertPacket(rtp_info, payload));
  EXPECT_EQ(0, neteq2_->InsertPacket(rtp_info, payload));

  AudioFrame out_frame1, out_frame2;
  bool muted;
  for (int i = 0; i < 1000; ++i) {
    rtc::StringBuilder ss;
    ss << "i = " << i;
    SCOPED_TRACE(ss.str());  // Print out the loop iterator on failure.
    EXPECT_EQ(0, neteq_->GetAudio(&out_frame1, &muted));
    EXPECT_FALSE(muted);
    EXPECT_EQ(0, neteq2_->GetAudio(&out_frame2, &muted));
    if (muted) {
      EXPECT_TRUE(AudioFramesEqualExceptData(out_frame1, out_frame2));
    } else {
      EXPECT_TRUE(AudioFramesEqual(out_frame1, out_frame2));
    }
  }
  EXPECT_TRUE(muted);

  // Insert new data. Timestamp is corrected for the time elapsed since the last
  // packet.
  for (int i = 0; i < 5; ++i) {
    PopulateRtpInfo(0, kSamples * 1000 + kSamples * i, &rtp_info);
    EXPECT_EQ(0, neteq_->InsertPacket(rtp_info, payload));
    EXPECT_EQ(0, neteq2_->InsertPacket(rtp_info, payload));
  }

  int counter = 0;
  while (out_frame1.speech_type_ != AudioFrame::kNormalSpeech) {
    ASSERT_LT(counter++, 1000) << "Test timed out";
    rtc::StringBuilder ss;
    ss << "counter = " << counter;
    SCOPED_TRACE(ss.str());  // Print out the loop iterator on failure.
    EXPECT_EQ(0, neteq_->GetAudio(&out_frame1, &muted));
    EXPECT_FALSE(muted);
    EXPECT_EQ(0, neteq2_->GetAudio(&out_frame2, &muted));
    if (muted) {
      EXPECT_TRUE(AudioFramesEqualExceptData(out_frame1, out_frame2));
    } else {
      EXPECT_TRUE(AudioFramesEqual(out_frame1, out_frame2));
    }
  }
  EXPECT_FALSE(muted);
}

TEST_F(NetEqDecodingTest, LastDecodedTimestampsEmpty) {
  EXPECT_TRUE(neteq_->LastDecodedTimestamps().empty());

  // Pull out data once.
  AudioFrame output;
  bool muted;
  ASSERT_EQ(0, neteq_->GetAudio(&output, &muted));

  EXPECT_TRUE(neteq_->LastDecodedTimestamps().empty());
}

TEST_F(NetEqDecodingTest, LastDecodedTimestampsOneDecoded) {
  // Insert one packet with PCM16b WB data (this is what PopulateRtpInfo does by
  // default). Make the length 10 ms.
  constexpr size_t kPayloadSamples = 16 * 10;
  constexpr size_t kPayloadBytes = 2 * kPayloadSamples;
  uint8_t payload[kPayloadBytes] = {0};

  RTPHeader rtp_info;
  constexpr uint32_t kRtpTimestamp = 0x1234;
  PopulateRtpInfo(0, kRtpTimestamp, &rtp_info);
  EXPECT_EQ(0, neteq_->InsertPacket(rtp_info, payload));

  // Pull out data once.
  AudioFrame output;
  bool muted;
  ASSERT_EQ(0, neteq_->GetAudio(&output, &muted));

  EXPECT_EQ(std::vector<uint32_t>({kRtpTimestamp}),
            neteq_->LastDecodedTimestamps());

  // Nothing decoded on the second call.
  ASSERT_EQ(0, neteq_->GetAudio(&output, &muted));
  EXPECT_TRUE(neteq_->LastDecodedTimestamps().empty());
}

TEST_F(NetEqDecodingTest, LastDecodedTimestampsTwoDecoded) {
  // Insert two packets with PCM16b WB data (this is what PopulateRtpInfo does
  // by default). Make the length 5 ms so that NetEq must decode them both in
  // the same GetAudio call.
  constexpr size_t kPayloadSamples = 16 * 5;
  constexpr size_t kPayloadBytes = 2 * kPayloadSamples;
  uint8_t payload[kPayloadBytes] = {0};

  RTPHeader rtp_info;
  constexpr uint32_t kRtpTimestamp1 = 0x1234;
  PopulateRtpInfo(0, kRtpTimestamp1, &rtp_info);
  EXPECT_EQ(0, neteq_->InsertPacket(rtp_info, payload));
  constexpr uint32_t kRtpTimestamp2 = kRtpTimestamp1 + kPayloadSamples;
  PopulateRtpInfo(1, kRtpTimestamp2, &rtp_info);
  EXPECT_EQ(0, neteq_->InsertPacket(rtp_info, payload));

  // Pull out data once.
  AudioFrame output;
  bool muted;
  ASSERT_EQ(0, neteq_->GetAudio(&output, &muted));

  EXPECT_EQ(std::vector<uint32_t>({kRtpTimestamp1, kRtpTimestamp2}),
            neteq_->LastDecodedTimestamps());
}

TEST_F(NetEqDecodingTest, TestConcealmentEvents) {
  const int kNumConcealmentEvents = 19;
  const size_t kSamples = 10 * 16;
  const size_t kPayloadBytes = kSamples * 2;
  int seq_no = 0;
  RTPHeader rtp_info;
  rtp_info.ssrc = 0x1234;     // Just an arbitrary SSRC.
  rtp_info.payloadType = 94;  // PCM16b WB codec.
  rtp_info.markerBit = 0;
  const uint8_t payload[kPayloadBytes] = {0};
  bool muted;

  for (int i = 0; i < kNumConcealmentEvents; i++) {
    // Insert some packets of 10 ms size.
    for (int j = 0; j < 10; j++) {
      rtp_info.sequenceNumber = seq_no++;
      rtp_info.timestamp = rtp_info.sequenceNumber * kSamples;
      neteq_->InsertPacket(rtp_info, payload);
      neteq_->GetAudio(&out_frame_, &muted);
    }

    // Lose a number of packets.
    int num_lost = 1 + i;
    for (int j = 0; j < num_lost; j++) {
      seq_no++;
      neteq_->GetAudio(&out_frame_, &muted);
    }
  }

  // Check number of concealment events.
  NetEqLifetimeStatistics stats = neteq_->GetLifetimeStatistics();
  EXPECT_EQ(kNumConcealmentEvents, static_cast<int>(stats.concealment_events));
}

// Test that the jitter buffer delay stat is computed correctly.
void NetEqDecodingTestFaxMode::TestJitterBufferDelay(bool apply_packet_loss) {
  const int kNumPackets = 10;
  const int kDelayInNumPackets = 2;
  const int kPacketLenMs = 10;  // All packets are of 10 ms size.
  const size_t kSamples = kPacketLenMs * 16;
  const size_t kPayloadBytes = kSamples * 2;
  RTPHeader rtp_info;
  rtp_info.ssrc = 0x1234;     // Just an arbitrary SSRC.
  rtp_info.payloadType = 94;  // PCM16b WB codec.
  rtp_info.markerBit = 0;
  const uint8_t payload[kPayloadBytes] = {0};
  bool muted;
  int packets_sent = 0;
  int packets_received = 0;
  int expected_delay = 0;
  int expected_target_delay = 0;
  uint64_t expected_emitted_count = 0;
  while (packets_received < kNumPackets) {
    // Insert packet.
    if (packets_sent < kNumPackets) {
      rtp_info.sequenceNumber = packets_sent++;
      rtp_info.timestamp = rtp_info.sequenceNumber * kSamples;
      neteq_->InsertPacket(rtp_info, payload);
    }

    // Get packet.
    if (packets_sent > kDelayInNumPackets) {
      neteq_->GetAudio(&out_frame_, &muted);
      packets_received++;

      // The delay reported by the jitter buffer never exceeds
      // the number of samples previously fetched with GetAudio
      // (hence the min()).
      int packets_delay = std::min(packets_received, kDelayInNumPackets + 1);

      // The increase of the expected delay is the product of
      // the current delay of the jitter buffer in ms * the
      // number of samples that are sent for play out.
      int current_delay_ms = packets_delay * kPacketLenMs;
      expected_delay += current_delay_ms * kSamples;
      expected_target_delay += neteq_->TargetDelayMs() * kSamples;
      expected_emitted_count += kSamples;
    }
  }

  if (apply_packet_loss) {
    // Extra call to GetAudio to cause concealment.
    neteq_->GetAudio(&out_frame_, &muted);
  }

  // Check jitter buffer delay.
  NetEqLifetimeStatistics stats = neteq_->GetLifetimeStatistics();
  EXPECT_EQ(expected_delay,
            rtc::checked_cast<int>(stats.jitter_buffer_delay_ms));
  EXPECT_EQ(expected_emitted_count, stats.jitter_buffer_emitted_count);
  EXPECT_EQ(expected_target_delay,
            rtc::checked_cast<int>(stats.jitter_buffer_target_delay_ms));
}

TEST_F(NetEqDecodingTestFaxMode, TestJitterBufferDelayWithoutLoss) {
  TestJitterBufferDelay(false);
}

TEST_F(NetEqDecodingTestFaxMode, TestJitterBufferDelayWithLoss) {
  TestJitterBufferDelay(true);
}

TEST_F(NetEqDecodingTestFaxMode, TestJitterBufferDelayWithAcceleration) {
  const int kPacketLenMs = 10;  // All packets are of 10 ms size.
  const size_t kSamples = kPacketLenMs * 16;
  const size_t kPayloadBytes = kSamples * 2;
  RTPHeader rtp_info;
  rtp_info.ssrc = 0x1234;     // Just an arbitrary SSRC.
  rtp_info.payloadType = 94;  // PCM16b WB codec.
  rtp_info.markerBit = 0;
  const uint8_t payload[kPayloadBytes] = {0};

  int expected_target_delay = neteq_->TargetDelayMs() * kSamples;
  neteq_->InsertPacket(rtp_info, payload);

  bool muted;
  neteq_->GetAudio(&out_frame_, &muted);

  rtp_info.sequenceNumber += 1;
  rtp_info.timestamp += kSamples;
  neteq_->InsertPacket(rtp_info, payload);
  rtp_info.sequenceNumber += 1;
  rtp_info.timestamp += kSamples;
  neteq_->InsertPacket(rtp_info, payload);

  expected_target_delay += neteq_->TargetDelayMs() * 2 * kSamples;
  // We have two packets in the buffer and kAccelerate operation will
  // extract 20 ms of data.
  neteq_->GetAudio(&out_frame_, &muted, nullptr, NetEq::Operation::kAccelerate);

  // Check jitter buffer delay.
  NetEqLifetimeStatistics stats = neteq_->GetLifetimeStatistics();
  EXPECT_EQ(10 * kSamples * 3, stats.jitter_buffer_delay_ms);
  EXPECT_EQ(kSamples * 3, stats.jitter_buffer_emitted_count);
  EXPECT_EQ(expected_target_delay,
            rtc::checked_cast<int>(stats.jitter_buffer_target_delay_ms));
}

namespace test {
TEST(NetEqNoTimeStretchingMode, RunTest) {
  NetEq::Config config;
  config.for_test_no_time_stretching = true;
  auto codecs = NetEqTest::StandardDecoderMap();
  NetEqPacketSourceInput::RtpHeaderExtensionMap rtp_ext_map = {
      {1, kRtpExtensionAudioLevel},
      {3, kRtpExtensionAbsoluteSendTime},
      {5, kRtpExtensionTransportSequenceNumber},
      {7, kRtpExtensionVideoContentType},
      {8, kRtpExtensionVideoTiming}};
  std::unique_ptr<NetEqInput> input(new NetEqRtpDumpInput(
      webrtc::test::ResourcePath("audio_coding/neteq_universal_new", "rtp"),
      rtp_ext_map, absl::nullopt /*No SSRC filter*/));
  std::unique_ptr<TimeLimitedNetEqInput> input_time_limit(
      new TimeLimitedNetEqInput(std::move(input), 20000));
  std::unique_ptr<AudioSink> output(new VoidAudioSink);
  NetEqTest::Callbacks callbacks;
  NetEqTest test(config, CreateBuiltinAudioDecoderFactory(), codecs,
                 /*text_log=*/nullptr, /*neteq_factory=*/nullptr,
                 /*input=*/std::move(input_time_limit), std::move(output),
                 callbacks);
  test.Run();
  const auto stats = test.SimulationStats();
  EXPECT_EQ(0, stats.accelerate_rate);
  EXPECT_EQ(0, stats.preemptive_rate);
}

namespace {
// Helper classes and data types and functions for NetEqOutputDelayTest.

class VectorAudioSink : public AudioSink {
 public:
  // Does not take ownership of the vector.
  VectorAudioSink(std::vector<int16_t>* output_vector) : v_(output_vector) {}

  virtual ~VectorAudioSink() = default;

  bool WriteArray(const int16_t* audio, size_t num_samples) override {
    v_->reserve(v_->size() + num_samples);
    for (size_t i = 0; i < num_samples; ++i) {
      v_->push_back(audio[i]);
    }
    return true;
  }

 private:
  std::vector<int16_t>* const v_;
};

struct TestResult {
  NetEqLifetimeStatistics lifetime_stats;
  NetEqNetworkStatistics network_stats;
  absl::optional<uint32_t> playout_timestamp;
  int target_delay_ms;
  int filtered_current_delay_ms;
  int sample_rate_hz;
};

// This class is used as callback object to NetEqTest to collect some stats
// at the end of the simulation.
class SimEndStatsCollector : public NetEqSimulationEndedCallback {
 public:
  SimEndStatsCollector(TestResult& result) : result_(result) {}

  void SimulationEnded(int64_t /*simulation_time_ms*/, NetEq* neteq) override {
    result_.playout_timestamp = neteq->GetPlayoutTimestamp();
    result_.target_delay_ms = neteq->TargetDelayMs();
    result_.filtered_current_delay_ms = neteq->FilteredCurrentDelayMs();
    result_.sample_rate_hz = neteq->last_output_sample_rate_hz();
  }

 private:
  TestResult& result_;
};

TestResult DelayLineNetEqTest(int delay_ms,
                              std::vector<int16_t>* output_vector) {
  NetEq::Config config;
  config.for_test_no_time_stretching = true;
  config.extra_output_delay_ms = delay_ms;
  auto codecs = NetEqTest::StandardDecoderMap();
  NetEqPacketSourceInput::RtpHeaderExtensionMap rtp_ext_map = {
      {1, kRtpExtensionAudioLevel},
      {3, kRtpExtensionAbsoluteSendTime},
      {5, kRtpExtensionTransportSequenceNumber},
      {7, kRtpExtensionVideoContentType},
      {8, kRtpExtensionVideoTiming}};
  std::unique_ptr<NetEqInput> input = std::make_unique<NetEqRtpDumpInput>(
      webrtc::test::ResourcePath("audio_coding/neteq_universal_new", "rtp"),
      rtp_ext_map, absl::nullopt /*No SSRC filter*/);
  std::unique_ptr<TimeLimitedNetEqInput> input_time_limit(
      new TimeLimitedNetEqInput(std::move(input), 10000));
  std::unique_ptr<AudioSink> output =
      std::make_unique<VectorAudioSink>(output_vector);

  TestResult result;
  SimEndStatsCollector stats_collector(result);
  NetEqTest::Callbacks callbacks;
  callbacks.simulation_ended_callback = &stats_collector;

  NetEqTest test(config, CreateBuiltinAudioDecoderFactory(), codecs,
                 /*text_log=*/nullptr, /*neteq_factory=*/nullptr,
                 /*input=*/std::move(input_time_limit), std::move(output),
                 callbacks);
  test.Run();
  result.lifetime_stats = test.LifetimeStats();
  result.network_stats = test.SimulationStats();
  return result;
}
}  // namespace

// Tests the extra output delay functionality of NetEq.
TEST(NetEqOutputDelayTest, RunTest) {
  std::vector<int16_t> output;
  const auto result_no_delay = DelayLineNetEqTest(0, &output);
  std::vector<int16_t> output_delayed;
  constexpr int kDelayMs = 100;
  const auto result_delay = DelayLineNetEqTest(kDelayMs, &output_delayed);

  // Verify that the loss concealment remains unchanged. The point of the delay
  // is to not affect the jitter buffering behavior.
  // First verify that there are concealments in the test.
  EXPECT_GT(result_no_delay.lifetime_stats.concealed_samples, 0u);
  // And that not all of the output is concealment.
  EXPECT_GT(result_no_delay.lifetime_stats.total_samples_received,
            result_no_delay.lifetime_stats.concealed_samples);
  // Now verify that they remain unchanged by the delay.
  EXPECT_EQ(result_no_delay.lifetime_stats.concealed_samples,
            result_delay.lifetime_stats.concealed_samples);
  // Accelerate and pre-emptive expand should also be unchanged.
  EXPECT_EQ(result_no_delay.lifetime_stats.inserted_samples_for_deceleration,
            result_delay.lifetime_stats.inserted_samples_for_deceleration);
  EXPECT_EQ(result_no_delay.lifetime_stats.removed_samples_for_acceleration,
            result_delay.lifetime_stats.removed_samples_for_acceleration);
  // Verify that delay stats are increased with the delay chain.
  EXPECT_EQ(
      result_no_delay.lifetime_stats.jitter_buffer_delay_ms +
          kDelayMs * result_no_delay.lifetime_stats.jitter_buffer_emitted_count,
      result_delay.lifetime_stats.jitter_buffer_delay_ms);
  EXPECT_EQ(
      result_no_delay.lifetime_stats.jitter_buffer_target_delay_ms +
          kDelayMs * result_no_delay.lifetime_stats.jitter_buffer_emitted_count,
      result_delay.lifetime_stats.jitter_buffer_target_delay_ms);
  EXPECT_EQ(result_no_delay.network_stats.current_buffer_size_ms + kDelayMs,
            result_delay.network_stats.current_buffer_size_ms);
  EXPECT_EQ(result_no_delay.network_stats.preferred_buffer_size_ms + kDelayMs,
            result_delay.network_stats.preferred_buffer_size_ms);
  EXPECT_EQ(result_no_delay.network_stats.mean_waiting_time_ms + kDelayMs,
            result_delay.network_stats.mean_waiting_time_ms);
  EXPECT_EQ(result_no_delay.network_stats.median_waiting_time_ms + kDelayMs,
            result_delay.network_stats.median_waiting_time_ms);
  EXPECT_EQ(result_no_delay.network_stats.min_waiting_time_ms + kDelayMs,
            result_delay.network_stats.min_waiting_time_ms);
  EXPECT_EQ(result_no_delay.network_stats.max_waiting_time_ms + kDelayMs,
            result_delay.network_stats.max_waiting_time_ms);

  ASSERT_TRUE(result_no_delay.playout_timestamp);
  ASSERT_TRUE(result_delay.playout_timestamp);
  EXPECT_EQ(*result_no_delay.playout_timestamp -
                static_cast<uint32_t>(
                    kDelayMs *
                    rtc::CheckedDivExact(result_no_delay.sample_rate_hz, 1000)),
            *result_delay.playout_timestamp);
  EXPECT_EQ(result_no_delay.target_delay_ms + kDelayMs,
            result_delay.target_delay_ms);
  EXPECT_EQ(result_no_delay.filtered_current_delay_ms + kDelayMs,
            result_delay.filtered_current_delay_ms);

  // Verify expected delay in decoded signal. The test vector uses 8 kHz sample
  // rate, so the delay will be 8 times the delay in ms.
  constexpr size_t kExpectedDelaySamples = kDelayMs * 8;
  for (size_t i = 0;
       i < output.size() && i + kExpectedDelaySamples < output_delayed.size();
       ++i) {
    EXPECT_EQ(output[i], output_delayed[i + kExpectedDelaySamples]);
  }
}

// Tests the extra output delay functionality of NetEq when configured via
// field trial.
TEST(NetEqOutputDelayTest, RunTestWithFieldTrial) {
  test::ScopedFieldTrials field_trial(
      "WebRTC-Audio-NetEqExtraDelay/Enabled-50/");
  constexpr int kExpectedDelayMs = 50;
  std::vector<int16_t> output;
  const auto result = DelayLineNetEqTest(0, &output);

  // The base delay values are taken from the resuts of the non-delayed case in
  // NetEqOutputDelayTest.RunTest above.
  EXPECT_EQ(20 + kExpectedDelayMs, result.target_delay_ms);
  EXPECT_EQ(24 + kExpectedDelayMs, result.filtered_current_delay_ms);
}

// Set a non-multiple-of-10 value in the field trial, and verify that we don't
// crash, and that the result is rounded down.
TEST(NetEqOutputDelayTest, RunTestWithFieldTrialOddValue) {
  test::ScopedFieldTrials field_trial(
      "WebRTC-Audio-NetEqExtraDelay/Enabled-103/");
  constexpr int kRoundedDelayMs = 100;
  std::vector<int16_t> output;
  const auto result = DelayLineNetEqTest(0, &output);

  // The base delay values are taken from the resuts of the non-delayed case in
  // NetEqOutputDelayTest.RunTest above.
  EXPECT_EQ(20 + kRoundedDelayMs, result.target_delay_ms);
  EXPECT_EQ(24 + kRoundedDelayMs, result.filtered_current_delay_ms);
}

}  // namespace test
}  // namespace webrtc
