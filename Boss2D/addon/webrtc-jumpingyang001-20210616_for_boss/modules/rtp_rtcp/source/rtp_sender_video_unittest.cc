/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_sender_video_h //original-code:"modules/rtp_rtcp/source/rtp_sender_video.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_api__test__mock_frame_encryptor_h //original-code:"api/test/mock_frame_encryptor.h"
#include BOSS_WEBRTC_U_api__transport__field_trial_based_config_h //original-code:"api/transport/field_trial_based_config.h"
#include BOSS_WEBRTC_U_api__transport__rtp__dependency_descriptor_h //original-code:"api/transport/rtp/dependency_descriptor.h"
#include BOSS_WEBRTC_U_api__video__video_codec_constants_h //original-code:"api/video/video_codec_constants.h"
#include BOSS_WEBRTC_U_api__video__video_timing_h //original-code:"api/video/video_timing.h"
#include BOSS_WEBRTC_U_common_video__generic_frame_descriptor__generic_frame_info_h //original-code:"common_video/generic_frame_descriptor/generic_frame_info.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_cvo_h //original-code:"modules/rtp_rtcp/include/rtp_cvo.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_header_extension_map_h //original-code:"modules/rtp_rtcp/include/rtp_header_extension_map.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_dependency_descriptor_extension_h //original-code:"modules/rtp_rtcp/source/rtp_dependency_descriptor_extension.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_descriptor_authentication_h //original-code:"modules/rtp_rtcp/source/rtp_descriptor_authentication.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_format_video_generic_h //original-code:"modules/rtp_rtcp/source/rtp_format_video_generic.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_generic_frame_descriptor_h //original-code:"modules/rtp_rtcp/source/rtp_generic_frame_descriptor.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_generic_frame_descriptor_extension_h //original-code:"modules/rtp_rtcp/source/rtp_generic_frame_descriptor_extension.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_header_extensions_h //original-code:"modules/rtp_rtcp/source/rtp_header_extensions.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_received_h //original-code:"modules/rtp_rtcp/source/rtp_packet_received.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_rtcp_impl2_h //original-code:"modules/rtp_rtcp/source/rtp_rtcp_impl2.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_video_layers_allocation_extension_h //original-code:"modules/rtp_rtcp/source/rtp_video_layers_allocation_extension.h"
#include BOSS_WEBRTC_U_rtc_base__arraysize_h //original-code:"rtc_base/arraysize.h"
#include BOSS_WEBRTC_U_rtc_base__rate_limiter_h //original-code:"rtc_base/rate_limiter.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_for_test_h //original-code:"rtc_base/task_queue_for_test.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include BOSS_WEBRTC_U_test__mock_frame_transformer_h //original-code:"test/mock_frame_transformer.h"

namespace webrtc {

namespace {

using ::testing::_;
using ::testing::ContainerEq;
using ::testing::ElementsAre;
using ::testing::ElementsAreArray;
using ::testing::IsEmpty;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::ReturnArg;
using ::testing::SaveArg;
using ::testing::SizeIs;
using ::testing::WithArgs;

enum : int {  // The first valid value is 1.
  kAbsoluteSendTimeExtensionId = 1,
  kGenericDescriptorId,
  kDependencyDescriptorId,
  kTransmissionTimeOffsetExtensionId,
  kTransportSequenceNumberExtensionId,
  kVideoRotationExtensionId,
  kVideoTimingExtensionId,
  kAbsoluteCaptureTimeExtensionId,
  kPlayoutDelayExtensionId,
  kVideoLayersAllocationExtensionId,
};

constexpr int kPayload = 100;
constexpr VideoCodecType kType = VideoCodecType::kVideoCodecGeneric;
constexpr uint32_t kTimestamp = 10;
constexpr uint16_t kSeqNum = 33;
constexpr uint32_t kSsrc = 725242;
constexpr int kMaxPacketLength = 1500;
constexpr uint64_t kStartTime = 123456789;
constexpr int64_t kDefaultExpectedRetransmissionTimeMs = 125;

class LoopbackTransportTest : public webrtc::Transport {
 public:
  LoopbackTransportTest() {
    receivers_extensions_.Register<TransmissionOffset>(
        kTransmissionTimeOffsetExtensionId);
    receivers_extensions_.Register<AbsoluteSendTime>(
        kAbsoluteSendTimeExtensionId);
    receivers_extensions_.Register<TransportSequenceNumber>(
        kTransportSequenceNumberExtensionId);
    receivers_extensions_.Register<VideoOrientation>(kVideoRotationExtensionId);
    receivers_extensions_.Register<VideoTimingExtension>(
        kVideoTimingExtensionId);
    receivers_extensions_.Register<RtpGenericFrameDescriptorExtension00>(
        kGenericDescriptorId);
    receivers_extensions_.Register<RtpDependencyDescriptorExtension>(
        kDependencyDescriptorId);
    receivers_extensions_.Register<AbsoluteCaptureTimeExtension>(
        kAbsoluteCaptureTimeExtensionId);
    receivers_extensions_.Register<PlayoutDelayLimits>(
        kPlayoutDelayExtensionId);
    receivers_extensions_.Register<RtpVideoLayersAllocationExtension>(
        kVideoLayersAllocationExtensionId);
  }

  bool SendRtp(const uint8_t* data,
               size_t len,
               const PacketOptions& options) override {
    sent_packets_.push_back(RtpPacketReceived(&receivers_extensions_));
    EXPECT_TRUE(sent_packets_.back().Parse(data, len));
    return true;
  }
  bool SendRtcp(const uint8_t* data, size_t len) override { return false; }
  const RtpPacketReceived& last_sent_packet() { return sent_packets_.back(); }
  int packets_sent() { return sent_packets_.size(); }
  const std::vector<RtpPacketReceived>& sent_packets() const {
    return sent_packets_;
  }

 private:
  RtpHeaderExtensionMap receivers_extensions_;
  std::vector<RtpPacketReceived> sent_packets_;
};

class TestRtpSenderVideo : public RTPSenderVideo {
 public:
  TestRtpSenderVideo(Clock* clock,
                     RTPSender* rtp_sender,
                     const WebRtcKeyValueConfig& field_trials)
      : RTPSenderVideo([&] {
          Config config;
          config.clock = clock;
          config.rtp_sender = rtp_sender;
          config.field_trials = &field_trials;
          return config;
        }()) {}
  ~TestRtpSenderVideo() override {}

  bool AllowRetransmission(const RTPVideoHeader& header,
                           int32_t retransmission_settings,
                           int64_t expected_retransmission_time_ms) {
    return RTPSenderVideo::AllowRetransmission(GetTemporalId(header),
                                               retransmission_settings,
                                               expected_retransmission_time_ms);
  }
};

class FieldTrials : public WebRtcKeyValueConfig {
 public:
  explicit FieldTrials(bool use_send_side_bwe_with_overhead)
      : use_send_side_bwe_with_overhead_(use_send_side_bwe_with_overhead),
        include_capture_clock_offset_(false) {}

  void set_include_capture_clock_offset(bool include_capture_clock_offset) {
    include_capture_clock_offset_ = include_capture_clock_offset;
  }

  std::string Lookup(absl::string_view key) const override {
    if (key == "WebRTC-SendSideBwe-WithOverhead") {
      return use_send_side_bwe_with_overhead_ ? "Enabled" : "";
    } else if (key == "WebRTC-IncludeCaptureClockOffset") {
      return include_capture_clock_offset_ ? "Enabled" : "";
    }
    return "";
  }

 private:
  bool use_send_side_bwe_with_overhead_;
  bool include_capture_clock_offset_;
};

class RtpSenderVideoTest : public ::testing::TestWithParam<bool> {
 public:
  RtpSenderVideoTest()
      : field_trials_(GetParam()),
        fake_clock_(kStartTime),
        retransmission_rate_limiter_(&fake_clock_, 1000),
        rtp_module_(ModuleRtpRtcpImpl2::Create([&] {
          RtpRtcpInterface::Configuration config;
          config.clock = &fake_clock_;
          config.outgoing_transport = &transport_;
          config.retransmission_rate_limiter = &retransmission_rate_limiter_;
          config.field_trials = &field_trials_;
          config.local_media_ssrc = kSsrc;
          return config;
        }())),
        rtp_sender_video_(
            std::make_unique<TestRtpSenderVideo>(&fake_clock_,
                                                 rtp_module_->RtpSender(),
                                                 field_trials_)) {
    rtp_module_->SetSequenceNumber(kSeqNum);
    rtp_module_->SetStartTimestamp(0);
  }

  void UsesMinimalVp8DescriptorWhenGenericFrameDescriptorExtensionIsUsed(
      int version);

 protected:
  const RtpRtcpInterface::Configuration config_;
  FieldTrials field_trials_;
  SimulatedClock fake_clock_;
  LoopbackTransportTest transport_;
  RateLimiter retransmission_rate_limiter_;
  std::unique_ptr<ModuleRtpRtcpImpl2> rtp_module_;
  std::unique_ptr<TestRtpSenderVideo> rtp_sender_video_;
};

TEST_P(RtpSenderVideoTest, KeyFrameHasCVO) {
  uint8_t kFrame[kMaxPacketLength];
  rtp_module_->RegisterRtpHeaderExtension(VideoOrientation::kUri,
                                          kVideoRotationExtensionId);

  RTPVideoHeader hdr;
  hdr.rotation = kVideoRotation_0;
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  VideoRotation rotation;
  EXPECT_TRUE(
      transport_.last_sent_packet().GetExtension<VideoOrientation>(&rotation));
  EXPECT_EQ(kVideoRotation_0, rotation);
}

TEST_P(RtpSenderVideoTest, TimingFrameHasPacketizationTimstampSet) {
  uint8_t kFrame[kMaxPacketLength];
  const int64_t kPacketizationTimeMs = 100;
  const int64_t kEncodeStartDeltaMs = 10;
  const int64_t kEncodeFinishDeltaMs = 50;
  rtp_module_->RegisterRtpHeaderExtension(VideoTimingExtension::kUri,
                                          kVideoTimingExtensionId);

  const int64_t kCaptureTimestamp = fake_clock_.TimeInMilliseconds();

  RTPVideoHeader hdr;
  hdr.video_timing.flags = VideoSendTiming::kTriggeredByTimer;
  hdr.video_timing.encode_start_delta_ms = kEncodeStartDeltaMs;
  hdr.video_timing.encode_finish_delta_ms = kEncodeFinishDeltaMs;

  fake_clock_.AdvanceTimeMilliseconds(kPacketizationTimeMs);
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, kCaptureTimestamp,
                               kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  VideoSendTiming timing;
  EXPECT_TRUE(transport_.last_sent_packet().GetExtension<VideoTimingExtension>(
      &timing));
  EXPECT_EQ(kPacketizationTimeMs, timing.packetization_finish_delta_ms);
  EXPECT_EQ(kEncodeStartDeltaMs, timing.encode_start_delta_ms);
  EXPECT_EQ(kEncodeFinishDeltaMs, timing.encode_finish_delta_ms);
}

TEST_P(RtpSenderVideoTest, DeltaFrameHasCVOWhenChanged) {
  uint8_t kFrame[kMaxPacketLength];
  rtp_module_->RegisterRtpHeaderExtension(VideoOrientation::kUri,
                                          kVideoRotationExtensionId);

  RTPVideoHeader hdr;
  hdr.rotation = kVideoRotation_90;
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  EXPECT_TRUE(
      rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                                   kDefaultExpectedRetransmissionTimeMs));

  hdr.rotation = kVideoRotation_0;
  hdr.frame_type = VideoFrameType::kVideoFrameDelta;
  EXPECT_TRUE(
      rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp + 1, 0, kFrame,
                                   hdr, kDefaultExpectedRetransmissionTimeMs));

  VideoRotation rotation;
  EXPECT_TRUE(
      transport_.last_sent_packet().GetExtension<VideoOrientation>(&rotation));
  EXPECT_EQ(kVideoRotation_0, rotation);
}

TEST_P(RtpSenderVideoTest, DeltaFrameHasCVOWhenNonZero) {
  uint8_t kFrame[kMaxPacketLength];
  rtp_module_->RegisterRtpHeaderExtension(VideoOrientation::kUri,
                                          kVideoRotationExtensionId);

  RTPVideoHeader hdr;
  hdr.rotation = kVideoRotation_90;
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  EXPECT_TRUE(
      rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                                   kDefaultExpectedRetransmissionTimeMs));

  hdr.frame_type = VideoFrameType::kVideoFrameDelta;
  EXPECT_TRUE(
      rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp + 1, 0, kFrame,
                                   hdr, kDefaultExpectedRetransmissionTimeMs));

  VideoRotation rotation;
  EXPECT_TRUE(
      transport_.last_sent_packet().GetExtension<VideoOrientation>(&rotation));
  EXPECT_EQ(kVideoRotation_90, rotation);
}

// Make sure rotation is parsed correctly when the Camera (C) and Flip (F) bits
// are set in the CVO byte.
TEST_P(RtpSenderVideoTest, SendVideoWithCameraAndFlipCVO) {
  // Test extracting rotation when Camera (C) and Flip (F) bits are zero.
  EXPECT_EQ(kVideoRotation_0, ConvertCVOByteToVideoRotation(0));
  EXPECT_EQ(kVideoRotation_90, ConvertCVOByteToVideoRotation(1));
  EXPECT_EQ(kVideoRotation_180, ConvertCVOByteToVideoRotation(2));
  EXPECT_EQ(kVideoRotation_270, ConvertCVOByteToVideoRotation(3));
  // Test extracting rotation when Camera (C) and Flip (F) bits are set.
  const int flip_bit = 1 << 2;
  const int camera_bit = 1 << 3;
  EXPECT_EQ(kVideoRotation_0,
            ConvertCVOByteToVideoRotation(flip_bit | camera_bit | 0));
  EXPECT_EQ(kVideoRotation_90,
            ConvertCVOByteToVideoRotation(flip_bit | camera_bit | 1));
  EXPECT_EQ(kVideoRotation_180,
            ConvertCVOByteToVideoRotation(flip_bit | camera_bit | 2));
  EXPECT_EQ(kVideoRotation_270,
            ConvertCVOByteToVideoRotation(flip_bit | camera_bit | 3));
}

TEST_P(RtpSenderVideoTest, RetransmissionTypesGeneric) {
  RTPVideoHeader header;
  header.codec = kVideoCodecGeneric;

  EXPECT_FALSE(rtp_sender_video_->AllowRetransmission(
      header, kRetransmitOff, kDefaultExpectedRetransmissionTimeMs));
  EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
      header, kRetransmitBaseLayer, kDefaultExpectedRetransmissionTimeMs));
  EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
      header, kRetransmitHigherLayers, kDefaultExpectedRetransmissionTimeMs));
  EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
      header, kConditionallyRetransmitHigherLayers,
      kDefaultExpectedRetransmissionTimeMs));
}

TEST_P(RtpSenderVideoTest, RetransmissionTypesH264) {
  RTPVideoHeader header;
  header.video_type_header.emplace<RTPVideoHeaderH264>().packetization_mode =
      H264PacketizationMode::NonInterleaved;
  header.codec = kVideoCodecH264;

  EXPECT_FALSE(rtp_sender_video_->AllowRetransmission(
      header, kRetransmitOff, kDefaultExpectedRetransmissionTimeMs));
  EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
      header, kRetransmitBaseLayer, kDefaultExpectedRetransmissionTimeMs));
  EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
      header, kRetransmitHigherLayers, kDefaultExpectedRetransmissionTimeMs));
  EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
      header, kConditionallyRetransmitHigherLayers,
      kDefaultExpectedRetransmissionTimeMs));
}

TEST_P(RtpSenderVideoTest, RetransmissionTypesVP8BaseLayer) {
  RTPVideoHeader header;
  header.codec = kVideoCodecVP8;
  auto& vp8_header = header.video_type_header.emplace<RTPVideoHeaderVP8>();
  vp8_header.temporalIdx = 0;

  EXPECT_FALSE(rtp_sender_video_->AllowRetransmission(
      header, kRetransmitOff, kDefaultExpectedRetransmissionTimeMs));
  EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
      header, kRetransmitBaseLayer, kDefaultExpectedRetransmissionTimeMs));
  EXPECT_FALSE(rtp_sender_video_->AllowRetransmission(
      header, kRetransmitHigherLayers, kDefaultExpectedRetransmissionTimeMs));
  EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
      header, kRetransmitHigherLayers | kRetransmitBaseLayer,
      kDefaultExpectedRetransmissionTimeMs));
  EXPECT_FALSE(rtp_sender_video_->AllowRetransmission(
      header, kConditionallyRetransmitHigherLayers,
      kDefaultExpectedRetransmissionTimeMs));
  EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
      header, kRetransmitBaseLayer | kConditionallyRetransmitHigherLayers,
      kDefaultExpectedRetransmissionTimeMs));
}

TEST_P(RtpSenderVideoTest, RetransmissionTypesVP8HigherLayers) {
  RTPVideoHeader header;
  header.codec = kVideoCodecVP8;

  auto& vp8_header = header.video_type_header.emplace<RTPVideoHeaderVP8>();
  for (int tid = 1; tid <= kMaxTemporalStreams; ++tid) {
    vp8_header.temporalIdx = tid;

    EXPECT_FALSE(rtp_sender_video_->AllowRetransmission(
        header, kRetransmitOff, kDefaultExpectedRetransmissionTimeMs));
    EXPECT_FALSE(rtp_sender_video_->AllowRetransmission(
        header, kRetransmitBaseLayer, kDefaultExpectedRetransmissionTimeMs));
    EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
        header, kRetransmitHigherLayers, kDefaultExpectedRetransmissionTimeMs));
    EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
        header, kRetransmitHigherLayers | kRetransmitBaseLayer,
        kDefaultExpectedRetransmissionTimeMs));
  }
}

TEST_P(RtpSenderVideoTest, RetransmissionTypesVP9) {
  RTPVideoHeader header;
  header.codec = kVideoCodecVP9;

  auto& vp9_header = header.video_type_header.emplace<RTPVideoHeaderVP9>();
  for (int tid = 1; tid <= kMaxTemporalStreams; ++tid) {
    vp9_header.temporal_idx = tid;

    EXPECT_FALSE(rtp_sender_video_->AllowRetransmission(
        header, kRetransmitOff, kDefaultExpectedRetransmissionTimeMs));
    EXPECT_FALSE(rtp_sender_video_->AllowRetransmission(
        header, kRetransmitBaseLayer, kDefaultExpectedRetransmissionTimeMs));
    EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
        header, kRetransmitHigherLayers, kDefaultExpectedRetransmissionTimeMs));
    EXPECT_TRUE(rtp_sender_video_->AllowRetransmission(
        header, kRetransmitHigherLayers | kRetransmitBaseLayer,
        kDefaultExpectedRetransmissionTimeMs));
  }
}

TEST_P(RtpSenderVideoTest, ConditionalRetransmit) {
  const int64_t kFrameIntervalMs = 33;
  const int64_t kRttMs = (kFrameIntervalMs * 3) / 2;
  const uint8_t kSettings =
      kRetransmitBaseLayer | kConditionallyRetransmitHigherLayers;

  // Insert VP8 frames for all temporal layers, but stop before the final index.
  RTPVideoHeader header;
  header.codec = kVideoCodecVP8;

  // Fill averaging window to prevent rounding errors.
  constexpr int kNumRepetitions =
      (RTPSenderVideo::kTLRateWindowSizeMs + (kFrameIntervalMs / 2)) /
      kFrameIntervalMs;
  constexpr int kPattern[] = {0, 2, 1, 2};
  auto& vp8_header = header.video_type_header.emplace<RTPVideoHeaderVP8>();
  for (size_t i = 0; i < arraysize(kPattern) * kNumRepetitions; ++i) {
    vp8_header.temporalIdx = kPattern[i % arraysize(kPattern)];
    rtp_sender_video_->AllowRetransmission(header, kSettings, kRttMs);
    fake_clock_.AdvanceTimeMilliseconds(kFrameIntervalMs);
  }

  // Since we're at the start of the pattern, the next expected frame in TL0 is
  // right now. We will wait at most one expected retransmission time before
  // acknowledging that it did not arrive, which means this frame and the next
  // will not be retransmitted.
  vp8_header.temporalIdx = 1;
  EXPECT_FALSE(
      rtp_sender_video_->AllowRetransmission(header, kSettings, kRttMs));
  fake_clock_.AdvanceTimeMilliseconds(kFrameIntervalMs);
  EXPECT_FALSE(
      rtp_sender_video_->AllowRetransmission(header, kSettings, kRttMs));
  fake_clock_.AdvanceTimeMilliseconds(kFrameIntervalMs);

  // The TL0 frame did not arrive. So allow retransmission.
  EXPECT_TRUE(
      rtp_sender_video_->AllowRetransmission(header, kSettings, kRttMs));
  fake_clock_.AdvanceTimeMilliseconds(kFrameIntervalMs);

  // Insert a frame for TL2. We just had frame in TL1, so the next one there is
  // in three frames away. TL0 is still too far in the past. So, allow
  // retransmission.
  vp8_header.temporalIdx = 2;
  EXPECT_TRUE(
      rtp_sender_video_->AllowRetransmission(header, kSettings, kRttMs));
  fake_clock_.AdvanceTimeMilliseconds(kFrameIntervalMs);

  // Another TL2, next in TL1 is two frames away. Allow again.
  EXPECT_TRUE(
      rtp_sender_video_->AllowRetransmission(header, kSettings, kRttMs));
  fake_clock_.AdvanceTimeMilliseconds(kFrameIntervalMs);

  // Yet another TL2, next in TL1 is now only one frame away, so don't store
  // for retransmission.
  EXPECT_FALSE(
      rtp_sender_video_->AllowRetransmission(header, kSettings, kRttMs));
}

TEST_P(RtpSenderVideoTest, ConditionalRetransmitLimit) {
  const int64_t kFrameIntervalMs = 200;
  const int64_t kRttMs = (kFrameIntervalMs * 3) / 2;
  const int32_t kSettings =
      kRetransmitBaseLayer | kConditionallyRetransmitHigherLayers;

  // Insert VP8 frames for all temporal layers, but stop before the final index.
  RTPVideoHeader header;
  header.codec = kVideoCodecVP8;

  // Fill averaging window to prevent rounding errors.
  constexpr int kNumRepetitions =
      (RTPSenderVideo::kTLRateWindowSizeMs + (kFrameIntervalMs / 2)) /
      kFrameIntervalMs;
  constexpr int kPattern[] = {0, 2, 2, 2};
  auto& vp8_header = header.video_type_header.emplace<RTPVideoHeaderVP8>();
  for (size_t i = 0; i < arraysize(kPattern) * kNumRepetitions; ++i) {
    vp8_header.temporalIdx = kPattern[i % arraysize(kPattern)];

    rtp_sender_video_->AllowRetransmission(header, kSettings, kRttMs);
    fake_clock_.AdvanceTimeMilliseconds(kFrameIntervalMs);
  }

  // Since we're at the start of the pattern, the next expected frame will be
  // right now in TL0. Put it in TL1 instead. Regular rules would dictate that
  // we don't store for retransmission because we expect a frame in a lower
  // layer, but that last frame in TL1 was a long time ago in absolute terms,
  // so allow retransmission anyway.
  vp8_header.temporalIdx = 1;
  EXPECT_TRUE(
      rtp_sender_video_->AllowRetransmission(header, kSettings, kRttMs));
}

TEST_P(RtpSenderVideoTest, SendsDependencyDescriptorWhenVideoStructureIsSet) {
  const int64_t kFrameId = 100000;
  uint8_t kFrame[100];
  rtp_module_->RegisterRtpHeaderExtension(
      RtpDependencyDescriptorExtension::kUri, kDependencyDescriptorId);
  FrameDependencyStructure video_structure;
  video_structure.num_decode_targets = 2;
  video_structure.templates = {
      FrameDependencyTemplate().S(0).T(0).Dtis("SS"),
      FrameDependencyTemplate().S(1).T(0).Dtis("-S"),
      FrameDependencyTemplate().S(1).T(1).Dtis("-D"),
  };
  rtp_sender_video_->SetVideoStructure(&video_structure);

  // Send key frame.
  RTPVideoHeader hdr;
  RTPVideoHeader::GenericDescriptorInfo& generic = hdr.generic.emplace();
  generic.frame_id = kFrameId;
  generic.temporal_index = 0;
  generic.spatial_index = 0;
  generic.decode_target_indications = {DecodeTargetIndication::kSwitch,
                                       DecodeTargetIndication::kSwitch};
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  ASSERT_EQ(transport_.packets_sent(), 1);
  DependencyDescriptor descriptor_key;
  ASSERT_TRUE(transport_.last_sent_packet()
                  .GetExtension<RtpDependencyDescriptorExtension>(
                      nullptr, &descriptor_key));
  ASSERT_TRUE(descriptor_key.attached_structure);
  EXPECT_EQ(descriptor_key.attached_structure->num_decode_targets, 2);
  EXPECT_THAT(descriptor_key.attached_structure->templates, SizeIs(3));
  EXPECT_EQ(descriptor_key.frame_number, kFrameId & 0xFFFF);
  EXPECT_EQ(descriptor_key.frame_dependencies.spatial_id, 0);
  EXPECT_EQ(descriptor_key.frame_dependencies.temporal_id, 0);
  EXPECT_EQ(descriptor_key.frame_dependencies.decode_target_indications,
            generic.decode_target_indications);
  EXPECT_THAT(descriptor_key.frame_dependencies.frame_diffs, IsEmpty());

  // Send delta frame.
  generic.frame_id = kFrameId + 1;
  generic.temporal_index = 1;
  generic.spatial_index = 1;
  generic.dependencies = {kFrameId, kFrameId - 500};
  generic.decode_target_indications = {DecodeTargetIndication::kNotPresent,
                                       DecodeTargetIndication::kRequired};
  hdr.frame_type = VideoFrameType::kVideoFrameDelta;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  EXPECT_EQ(transport_.packets_sent(), 2);
  DependencyDescriptor descriptor_delta;
  ASSERT_TRUE(
      transport_.last_sent_packet()
          .GetExtension<RtpDependencyDescriptorExtension>(
              descriptor_key.attached_structure.get(), &descriptor_delta));
  EXPECT_EQ(descriptor_delta.attached_structure, nullptr);
  EXPECT_EQ(descriptor_delta.frame_number, (kFrameId + 1) & 0xFFFF);
  EXPECT_EQ(descriptor_delta.frame_dependencies.spatial_id, 1);
  EXPECT_EQ(descriptor_delta.frame_dependencies.temporal_id, 1);
  EXPECT_EQ(descriptor_delta.frame_dependencies.decode_target_indications,
            generic.decode_target_indications);
  EXPECT_THAT(descriptor_delta.frame_dependencies.frame_diffs,
              ElementsAre(1, 501));
}

TEST_P(RtpSenderVideoTest, PropagatesChainDiffsIntoDependencyDescriptor) {
  const int64_t kFrameId = 100000;
  uint8_t kFrame[100];
  rtp_module_->RegisterRtpHeaderExtension(
      RtpDependencyDescriptorExtension::kUri, kDependencyDescriptorId);
  FrameDependencyStructure video_structure;
  video_structure.num_decode_targets = 2;
  video_structure.num_chains = 1;
  video_structure.decode_target_protected_by_chain = {0, 0};
  video_structure.templates = {
      FrameDependencyTemplate().S(0).T(0).Dtis("SS").ChainDiffs({1}),
  };
  rtp_sender_video_->SetVideoStructure(&video_structure);

  RTPVideoHeader hdr;
  RTPVideoHeader::GenericDescriptorInfo& generic = hdr.generic.emplace();
  generic.frame_id = kFrameId;
  generic.decode_target_indications = {DecodeTargetIndication::kSwitch,
                                       DecodeTargetIndication::kSwitch};
  generic.chain_diffs = {2};
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  ASSERT_EQ(transport_.packets_sent(), 1);
  DependencyDescriptor descriptor_key;
  ASSERT_TRUE(transport_.last_sent_packet()
                  .GetExtension<RtpDependencyDescriptorExtension>(
                      nullptr, &descriptor_key));
  EXPECT_THAT(descriptor_key.frame_dependencies.chain_diffs,
              ContainerEq(generic.chain_diffs));
}

TEST_P(RtpSenderVideoTest,
       PropagatesActiveDecodeTargetsIntoDependencyDescriptor) {
  const int64_t kFrameId = 100000;
  uint8_t kFrame[100];
  rtp_module_->RegisterRtpHeaderExtension(
      RtpDependencyDescriptorExtension::kUri, kDependencyDescriptorId);
  FrameDependencyStructure video_structure;
  video_structure.num_decode_targets = 2;
  video_structure.num_chains = 1;
  video_structure.decode_target_protected_by_chain = {0, 0};
  video_structure.templates = {
      FrameDependencyTemplate().S(0).T(0).Dtis("SS").ChainDiffs({1}),
  };
  rtp_sender_video_->SetVideoStructure(&video_structure);

  RTPVideoHeader hdr;
  RTPVideoHeader::GenericDescriptorInfo& generic = hdr.generic.emplace();
  generic.frame_id = kFrameId;
  generic.decode_target_indications = {DecodeTargetIndication::kSwitch,
                                       DecodeTargetIndication::kSwitch};
  generic.active_decode_targets = 0b01;
  generic.chain_diffs = {1};
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  ASSERT_EQ(transport_.packets_sent(), 1);
  DependencyDescriptor descriptor_key;
  ASSERT_TRUE(transport_.last_sent_packet()
                  .GetExtension<RtpDependencyDescriptorExtension>(
                      nullptr, &descriptor_key));
  EXPECT_EQ(descriptor_key.active_decode_targets_bitmask, 0b01u);
}

TEST_P(RtpSenderVideoTest,
       SetDiffentVideoStructureAvoidsCollisionWithThePreviousStructure) {
  const int64_t kFrameId = 100000;
  uint8_t kFrame[100];
  rtp_module_->RegisterRtpHeaderExtension(
      RtpDependencyDescriptorExtension::kUri, kDependencyDescriptorId);
  FrameDependencyStructure video_structure1;
  video_structure1.num_decode_targets = 2;
  video_structure1.templates = {
      FrameDependencyTemplate().S(0).T(0).Dtis("SS"),
      FrameDependencyTemplate().S(0).T(1).Dtis("D-"),
  };
  FrameDependencyStructure video_structure2;
  video_structure2.num_decode_targets = 2;
  video_structure2.templates = {
      FrameDependencyTemplate().S(0).T(0).Dtis("SS"),
      FrameDependencyTemplate().S(0).T(1).Dtis("R-"),
  };

  // Send 1st key frame.
  RTPVideoHeader hdr;
  RTPVideoHeader::GenericDescriptorInfo& generic = hdr.generic.emplace();
  generic.frame_id = kFrameId;
  generic.decode_target_indications = {DecodeTargetIndication::kSwitch,
                                       DecodeTargetIndication::kSwitch};
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SetVideoStructure(&video_structure1);
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  // Parse 1st extension.
  ASSERT_EQ(transport_.packets_sent(), 1);
  DependencyDescriptor descriptor_key1;
  ASSERT_TRUE(transport_.last_sent_packet()
                  .GetExtension<RtpDependencyDescriptorExtension>(
                      nullptr, &descriptor_key1));
  ASSERT_TRUE(descriptor_key1.attached_structure);

  // Send the delta frame.
  generic.frame_id = kFrameId + 1;
  generic.temporal_index = 1;
  generic.decode_target_indications = {DecodeTargetIndication::kDiscardable,
                                       DecodeTargetIndication::kNotPresent};
  hdr.frame_type = VideoFrameType::kVideoFrameDelta;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  ASSERT_EQ(transport_.packets_sent(), 2);
  RtpPacket delta_packet = transport_.last_sent_packet();

  // Send 2nd key frame.
  generic.frame_id = kFrameId + 2;
  generic.decode_target_indications = {DecodeTargetIndication::kSwitch,
                                       DecodeTargetIndication::kSwitch};
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SetVideoStructure(&video_structure2);
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  // Parse the 2nd key frame.
  ASSERT_EQ(transport_.packets_sent(), 3);
  DependencyDescriptor descriptor_key2;
  ASSERT_TRUE(transport_.last_sent_packet()
                  .GetExtension<RtpDependencyDescriptorExtension>(
                      nullptr, &descriptor_key2));
  ASSERT_TRUE(descriptor_key2.attached_structure);

  // Try to parse the 1st delta frame. It should parseble using the structure
  // from the 1st key frame, but not using the structure from the 2nd key frame.
  DependencyDescriptor descriptor_delta;
  EXPECT_TRUE(delta_packet.GetExtension<RtpDependencyDescriptorExtension>(
      descriptor_key1.attached_structure.get(), &descriptor_delta));
  EXPECT_FALSE(delta_packet.GetExtension<RtpDependencyDescriptorExtension>(
      descriptor_key2.attached_structure.get(), &descriptor_delta));
}

TEST_P(RtpSenderVideoTest,
       AuthenticateVideoHeaderWhenDependencyDescriptorExtensionIsUsed) {
  static constexpr size_t kFrameSize = 100;
  uint8_t kFrame[kFrameSize] = {1, 2, 3, 4};

  rtp_module_->RegisterRtpHeaderExtension(
      RtpDependencyDescriptorExtension::kUri, kDependencyDescriptorId);
  rtc::scoped_refptr<MockFrameEncryptor> encryptor(
      new rtc::RefCountedObject<NiceMock<MockFrameEncryptor>>);
  ON_CALL(*encryptor, GetMaxCiphertextByteSize).WillByDefault(ReturnArg<1>());
  ON_CALL(*encryptor, Encrypt)
      .WillByDefault(WithArgs<3, 5>(
          [](rtc::ArrayView<const uint8_t> frame, size_t* bytes_written) {
            *bytes_written = frame.size();
            return 0;
          }));
  RTPSenderVideo::Config config;
  config.clock = &fake_clock_;
  config.rtp_sender = rtp_module_->RtpSender();
  config.field_trials = &field_trials_;
  config.frame_encryptor = encryptor;
  RTPSenderVideo rtp_sender_video(config);

  FrameDependencyStructure video_structure;
  video_structure.num_decode_targets = 1;
  video_structure.templates = {FrameDependencyTemplate().Dtis("S")};
  rtp_sender_video.SetVideoStructure(&video_structure);

  // Send key frame.
  RTPVideoHeader hdr;
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  hdr.generic.emplace().decode_target_indications =
      video_structure.templates[0].decode_target_indications;

  EXPECT_CALL(*encryptor,
              Encrypt(_, _, Not(IsEmpty()), ElementsAreArray(kFrame), _, _));
  rtp_sender_video.SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                             kDefaultExpectedRetransmissionTimeMs);
  // Double check packet with the dependency descriptor is sent.
  ASSERT_EQ(transport_.packets_sent(), 1);
  EXPECT_TRUE(transport_.last_sent_packet()
                  .HasExtension<RtpDependencyDescriptorExtension>());
}

TEST_P(RtpSenderVideoTest, PopulateGenericFrameDescriptor) {
  const int64_t kFrameId = 100000;
  uint8_t kFrame[100];
  rtp_module_->RegisterRtpHeaderExtension(
      RtpGenericFrameDescriptorExtension00::kUri, kGenericDescriptorId);

  RTPVideoHeader hdr;
  RTPVideoHeader::GenericDescriptorInfo& generic = hdr.generic.emplace();
  generic.frame_id = kFrameId;
  generic.temporal_index = 3;
  generic.spatial_index = 2;
  generic.dependencies.push_back(kFrameId - 1);
  generic.dependencies.push_back(kFrameId - 500);
  hdr.frame_type = VideoFrameType::kVideoFrameDelta;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  RtpGenericFrameDescriptor descriptor_wire;
  EXPECT_EQ(1, transport_.packets_sent());
  ASSERT_TRUE(transport_.last_sent_packet()
                  .GetExtension<RtpGenericFrameDescriptorExtension00>(
                      &descriptor_wire));
  EXPECT_EQ(static_cast<uint16_t>(generic.frame_id), descriptor_wire.FrameId());
  EXPECT_EQ(generic.temporal_index, descriptor_wire.TemporalLayer());
  EXPECT_THAT(descriptor_wire.FrameDependenciesDiffs(), ElementsAre(1, 500));
  EXPECT_EQ(descriptor_wire.SpatialLayersBitmask(), 0b0000'0100);
}

void RtpSenderVideoTest::
    UsesMinimalVp8DescriptorWhenGenericFrameDescriptorExtensionIsUsed(
        int version) {
  const int64_t kFrameId = 100000;
  const size_t kFrameSize = 100;
  uint8_t kFrame[kFrameSize];

  rtp_module_->RegisterRtpHeaderExtension(
      RtpGenericFrameDescriptorExtension00::kUri, kGenericDescriptorId);

  RTPVideoHeader hdr;
  hdr.codec = kVideoCodecVP8;
  RTPVideoHeaderVP8& vp8 = hdr.video_type_header.emplace<RTPVideoHeaderVP8>();
  vp8.pictureId = kFrameId % 0X7FFF;
  vp8.tl0PicIdx = 13;
  vp8.temporalIdx = 1;
  vp8.keyIdx = 2;
  RTPVideoHeader::GenericDescriptorInfo& generic = hdr.generic.emplace();
  generic.frame_id = kFrameId;
  hdr.frame_type = VideoFrameType::kVideoFrameDelta;
  rtp_sender_video_->SendVideo(kPayload, VideoCodecType::kVideoCodecVP8,
                               kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  ASSERT_EQ(transport_.packets_sent(), 1);
  // Expect only minimal 1-byte vp8 descriptor was generated.
  EXPECT_EQ(transport_.last_sent_packet().payload_size(), 1 + kFrameSize);
}

TEST_P(RtpSenderVideoTest,
       UsesMinimalVp8DescriptorWhenGenericFrameDescriptorExtensionIsUsed00) {
  UsesMinimalVp8DescriptorWhenGenericFrameDescriptorExtensionIsUsed(0);
}

TEST_P(RtpSenderVideoTest,
       UsesMinimalVp8DescriptorWhenGenericFrameDescriptorExtensionIsUsed01) {
  UsesMinimalVp8DescriptorWhenGenericFrameDescriptorExtensionIsUsed(1);
}

TEST_P(RtpSenderVideoTest, VideoLayersAllocationWithResolutionSentOnKeyFrames) {
  const size_t kFrameSize = 100;
  uint8_t kFrame[kFrameSize];
  rtp_module_->RegisterRtpHeaderExtension(
      RtpVideoLayersAllocationExtension::kUri,
      kVideoLayersAllocationExtensionId);

  VideoLayersAllocation allocation;
  VideoLayersAllocation::SpatialLayer layer;
  layer.width = 360;
  layer.height = 180;
  layer.target_bitrate_per_temporal_layer.push_back(
      DataRate::KilobitsPerSec(50));
  allocation.resolution_and_frame_rate_is_valid = true;
  allocation.active_spatial_layers.push_back(layer);
  rtp_sender_video_->SetVideoLayersAllocation(allocation);

  RTPVideoHeader hdr;
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  VideoLayersAllocation sent_allocation;
  EXPECT_TRUE(
      transport_.last_sent_packet()
          .GetExtension<RtpVideoLayersAllocationExtension>(&sent_allocation));
  EXPECT_THAT(sent_allocation.active_spatial_layers, ElementsAre(layer));

  // Next key frame also have the allocation.
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  EXPECT_TRUE(
      transport_.last_sent_packet()
          .GetExtension<RtpVideoLayersAllocationExtension>(&sent_allocation));
}

TEST_P(RtpSenderVideoTest,
       VideoLayersAllocationWithoutResolutionSentOnDeltaWhenUpdated) {
  const size_t kFrameSize = 100;
  uint8_t kFrame[kFrameSize];
  rtp_module_->RegisterRtpHeaderExtension(
      RtpVideoLayersAllocationExtension::kUri,
      kVideoLayersAllocationExtensionId);

  VideoLayersAllocation allocation;
  VideoLayersAllocation::SpatialLayer layer;
  layer.width = 360;
  layer.height = 180;
  allocation.resolution_and_frame_rate_is_valid = true;
  layer.target_bitrate_per_temporal_layer.push_back(
      DataRate::KilobitsPerSec(50));
  allocation.active_spatial_layers.push_back(layer);
  rtp_sender_video_->SetVideoLayersAllocation(allocation);

  RTPVideoHeader hdr;
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  EXPECT_TRUE(transport_.last_sent_packet()
                  .HasExtension<RtpVideoLayersAllocationExtension>());

  // No allocation sent on delta frame unless it has been updated.
  hdr.frame_type = VideoFrameType::kVideoFrameDelta;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  EXPECT_FALSE(transport_.last_sent_packet()
                   .HasExtension<RtpVideoLayersAllocationExtension>());

  // Update the allocation.
  rtp_sender_video_->SetVideoLayersAllocation(allocation);
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  VideoLayersAllocation sent_allocation;
  EXPECT_TRUE(
      transport_.last_sent_packet()
          .GetExtension<RtpVideoLayersAllocationExtension>(&sent_allocation));
  ASSERT_THAT(sent_allocation.active_spatial_layers, SizeIs(1));
  EXPECT_FALSE(sent_allocation.resolution_and_frame_rate_is_valid);
  EXPECT_THAT(sent_allocation.active_spatial_layers[0]
                  .target_bitrate_per_temporal_layer,
              SizeIs(1));
}

TEST_P(RtpSenderVideoTest,
       VideoLayersAllocationWithResolutionSentOnDeltaWhenSpatialLayerAdded) {
  const size_t kFrameSize = 100;
  uint8_t kFrame[kFrameSize];
  rtp_module_->RegisterRtpHeaderExtension(
      RtpVideoLayersAllocationExtension::kUri,
      kVideoLayersAllocationExtensionId);

  VideoLayersAllocation allocation;
  allocation.resolution_and_frame_rate_is_valid = true;
  VideoLayersAllocation::SpatialLayer layer;
  layer.width = 360;
  layer.height = 180;
  layer.spatial_id = 0;
  layer.target_bitrate_per_temporal_layer.push_back(
      DataRate::KilobitsPerSec(50));
  allocation.active_spatial_layers.push_back(layer);
  rtp_sender_video_->SetVideoLayersAllocation(allocation);

  RTPVideoHeader hdr;
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  ASSERT_TRUE(transport_.last_sent_packet()
                  .HasExtension<RtpVideoLayersAllocationExtension>());

  // Update the allocation.
  layer.width = 640;
  layer.height = 320;
  layer.spatial_id = 1;
  layer.target_bitrate_per_temporal_layer.push_back(
      DataRate::KilobitsPerSec(100));
  allocation.active_spatial_layers.push_back(layer);
  rtp_sender_video_->SetVideoLayersAllocation(allocation);
  hdr.frame_type = VideoFrameType::kVideoFrameDelta;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  VideoLayersAllocation sent_allocation;
  EXPECT_TRUE(
      transport_.last_sent_packet()
          .GetExtension<RtpVideoLayersAllocationExtension>(&sent_allocation));
  EXPECT_THAT(sent_allocation.active_spatial_layers, SizeIs(2));
  EXPECT_TRUE(sent_allocation.resolution_and_frame_rate_is_valid);
}

TEST_P(RtpSenderVideoTest, VideoLayersAllocationSentOnDeltaFramesOnlyOnUpdate) {
  const size_t kFrameSize = 100;
  uint8_t kFrame[kFrameSize];
  rtp_module_->RegisterRtpHeaderExtension(
      RtpVideoLayersAllocationExtension::kUri,
      kVideoLayersAllocationExtensionId);

  VideoLayersAllocation allocation;
  VideoLayersAllocation::SpatialLayer layer;
  layer.width = 360;
  layer.height = 180;
  layer.target_bitrate_per_temporal_layer.push_back(
      DataRate::KilobitsPerSec(50));
  allocation.active_spatial_layers.push_back(layer);
  rtp_sender_video_->SetVideoLayersAllocation(allocation);

  RTPVideoHeader hdr;
  hdr.frame_type = VideoFrameType::kVideoFrameDelta;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  VideoLayersAllocation sent_allocation;
  EXPECT_TRUE(
      transport_.last_sent_packet()
          .GetExtension<RtpVideoLayersAllocationExtension>(&sent_allocation));
  EXPECT_THAT(sent_allocation.active_spatial_layers, SizeIs(1));

  // VideoLayersAllocation not sent on the next delta frame.
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  EXPECT_FALSE(transport_.last_sent_packet()
                   .HasExtension<RtpVideoLayersAllocationExtension>());

  // Update allocation. VideoLayesAllocation should be sent on the next frame.
  rtp_sender_video_->SetVideoLayersAllocation(allocation);
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  EXPECT_TRUE(
      transport_.last_sent_packet()
          .GetExtension<RtpVideoLayersAllocationExtension>(&sent_allocation));
}

TEST_P(RtpSenderVideoTest, VideoLayersAllocationNotSentOnHigherTemporalLayers) {
  const size_t kFrameSize = 100;
  uint8_t kFrame[kFrameSize];
  rtp_module_->RegisterRtpHeaderExtension(
      RtpVideoLayersAllocationExtension::kUri,
      kVideoLayersAllocationExtensionId);

  VideoLayersAllocation allocation;
  allocation.resolution_and_frame_rate_is_valid = true;
  VideoLayersAllocation::SpatialLayer layer;
  layer.width = 360;
  layer.height = 180;
  layer.target_bitrate_per_temporal_layer.push_back(
      DataRate::KilobitsPerSec(50));
  allocation.active_spatial_layers.push_back(layer);
  rtp_sender_video_->SetVideoLayersAllocation(allocation);

  RTPVideoHeader hdr;
  hdr.frame_type = VideoFrameType::kVideoFrameDelta;
  hdr.codec = VideoCodecType::kVideoCodecVP8;
  auto& vp8_header = hdr.video_type_header.emplace<RTPVideoHeaderVP8>();
  vp8_header.temporalIdx = 1;

  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  EXPECT_FALSE(transport_.last_sent_packet()
                   .HasExtension<RtpVideoLayersAllocationExtension>());

  // Send a delta frame on tl0.
  vp8_header.temporalIdx = 0;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  EXPECT_TRUE(transport_.last_sent_packet()
                  .HasExtension<RtpVideoLayersAllocationExtension>());
}

TEST_P(RtpSenderVideoTest, AbsoluteCaptureTime) {
  constexpr int64_t kAbsoluteCaptureTimestampMs = 12345678;
  uint8_t kFrame[kMaxPacketLength];
  rtp_module_->RegisterRtpHeaderExtension(AbsoluteCaptureTimeExtension::kUri,
                                          kAbsoluteCaptureTimeExtensionId);

  RTPVideoHeader hdr;
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp,
                               kAbsoluteCaptureTimestampMs, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);

  // It is expected that one and only one of the packets sent on this video
  // frame has absolute capture time header extension. And no absolute capture
  // time header extensions include capture clock offset.
  int packets_with_abs_capture_time = 0;
  for (const RtpPacketReceived& packet : transport_.sent_packets()) {
    auto absolute_capture_time =
        packet.GetExtension<AbsoluteCaptureTimeExtension>();
    if (absolute_capture_time) {
      ++packets_with_abs_capture_time;
      EXPECT_EQ(
          absolute_capture_time->absolute_capture_timestamp,
          Int64MsToUQ32x32(fake_clock_.ConvertTimestampToNtpTimeInMilliseconds(
              kAbsoluteCaptureTimestampMs)));
      EXPECT_FALSE(
          absolute_capture_time->estimated_capture_clock_offset.has_value());
    }
  }
  EXPECT_EQ(packets_with_abs_capture_time, 1);
}

// Essentially the same test as AbsoluteCaptureTime but with a field trial.
// After the field trial is experimented, we will remove AbsoluteCaptureTime.
TEST_P(RtpSenderVideoTest, AbsoluteCaptureTimeWithCaptureClockOffset) {
  field_trials_.set_include_capture_clock_offset(true);
  rtp_sender_video_ = std::make_unique<TestRtpSenderVideo>(
      &fake_clock_, rtp_module_->RtpSender(), field_trials_);

  constexpr int64_t kAbsoluteCaptureTimestampMs = 12345678;
  uint8_t kFrame[kMaxPacketLength];
  rtp_module_->RegisterRtpHeaderExtension(AbsoluteCaptureTimeExtension::kUri,
                                          kAbsoluteCaptureTimeExtensionId);

  RTPVideoHeader hdr;
  const absl::optional<int64_t> kExpectedCaptureClockOffset =
      absl::make_optional(1234);
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SendVideo(
      kPayload, kType, kTimestamp, kAbsoluteCaptureTimestampMs, kFrame, hdr,
      kDefaultExpectedRetransmissionTimeMs, kExpectedCaptureClockOffset);

  // It is expected that one and only one of the packets sent on this video
  // frame has absolute capture time header extension. And it includes capture
  // clock offset.
  int packets_with_abs_capture_time = 0;
  for (const RtpPacketReceived& packet : transport_.sent_packets()) {
    auto absolute_capture_time =
        packet.GetExtension<AbsoluteCaptureTimeExtension>();
    if (absolute_capture_time) {
      ++packets_with_abs_capture_time;
      EXPECT_EQ(
          absolute_capture_time->absolute_capture_timestamp,
          Int64MsToUQ32x32(fake_clock_.ConvertTimestampToNtpTimeInMilliseconds(
              kAbsoluteCaptureTimestampMs)));
      EXPECT_EQ(kExpectedCaptureClockOffset,
                absolute_capture_time->estimated_capture_clock_offset);
    }
  }
  EXPECT_EQ(packets_with_abs_capture_time, 1);
}

TEST_P(RtpSenderVideoTest, PopulatesPlayoutDelay) {
  // Single packet frames.
  constexpr size_t kPacketSize = 123;
  uint8_t kFrame[kPacketSize];
  rtp_module_->RegisterRtpHeaderExtension(PlayoutDelayLimits::kUri,
                                          kPlayoutDelayExtensionId);
  const VideoPlayoutDelay kExpectedDelay = {10, 20};

  // Send initial key-frame without playout delay.
  RTPVideoHeader hdr;
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  hdr.codec = VideoCodecType::kVideoCodecVP8;
  auto& vp8_header = hdr.video_type_header.emplace<RTPVideoHeaderVP8>();
  vp8_header.temporalIdx = 0;

  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  EXPECT_FALSE(
      transport_.last_sent_packet().HasExtension<PlayoutDelayLimits>());

  // Set playout delay on a discardable frame.
  hdr.playout_delay = kExpectedDelay;
  hdr.frame_type = VideoFrameType::kVideoFrameDelta;
  vp8_header.temporalIdx = 1;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  VideoPlayoutDelay received_delay = VideoPlayoutDelay();
  ASSERT_TRUE(transport_.last_sent_packet().GetExtension<PlayoutDelayLimits>(
      &received_delay));
  EXPECT_EQ(received_delay, kExpectedDelay);

  // Set playout delay on a non-discardable frame, the extension should still
  // be populated since dilvery wasn't guaranteed on the last one.
  hdr.playout_delay = VideoPlayoutDelay();  // Indicates "no change".
  vp8_header.temporalIdx = 0;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  ASSERT_TRUE(transport_.last_sent_packet().GetExtension<PlayoutDelayLimits>(
      &received_delay));
  EXPECT_EQ(received_delay, kExpectedDelay);

  // The next frame does not need the extensions since it's delivery has
  // already been guaranteed.
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  EXPECT_FALSE(
      transport_.last_sent_packet().HasExtension<PlayoutDelayLimits>());

  // Insert key-frame, we need to refresh the state here.
  hdr.frame_type = VideoFrameType::kVideoFrameKey;
  rtp_sender_video_->SendVideo(kPayload, kType, kTimestamp, 0, kFrame, hdr,
                               kDefaultExpectedRetransmissionTimeMs);
  ASSERT_TRUE(transport_.last_sent_packet().GetExtension<PlayoutDelayLimits>(
      &received_delay));
  EXPECT_EQ(received_delay, kExpectedDelay);
}

TEST_P(RtpSenderVideoTest, SendGenericVideo) {
  const uint8_t kPayloadType = 127;
  const VideoCodecType kCodecType = VideoCodecType::kVideoCodecGeneric;
  const uint8_t kPayload[] = {47, 11, 32, 93, 89};

  // Send keyframe.
  RTPVideoHeader video_header;
  video_header.frame_type = VideoFrameType::kVideoFrameKey;
  ASSERT_TRUE(rtp_sender_video_->SendVideo(kPayloadType, kCodecType, 1234, 4321,
                                           kPayload, video_header,
                                           absl::nullopt));

  rtc::ArrayView<const uint8_t> sent_payload =
      transport_.last_sent_packet().payload();
  uint8_t generic_header = sent_payload[0];
  EXPECT_TRUE(generic_header & RtpFormatVideoGeneric::kKeyFrameBit);
  EXPECT_TRUE(generic_header & RtpFormatVideoGeneric::kFirstPacketBit);
  EXPECT_THAT(sent_payload.subview(1), ElementsAreArray(kPayload));

  // Send delta frame.
  const uint8_t kDeltaPayload[] = {13, 42, 32, 93, 13};
  video_header.frame_type = VideoFrameType::kVideoFrameDelta;
  ASSERT_TRUE(rtp_sender_video_->SendVideo(kPayloadType, kCodecType, 1234, 4321,
                                           kDeltaPayload, video_header,
                                           absl::nullopt));

  sent_payload = sent_payload = transport_.last_sent_packet().payload();
  generic_header = sent_payload[0];
  EXPECT_FALSE(generic_header & RtpFormatVideoGeneric::kKeyFrameBit);
  EXPECT_TRUE(generic_header & RtpFormatVideoGeneric::kFirstPacketBit);
  EXPECT_THAT(sent_payload.subview(1), ElementsAreArray(kDeltaPayload));
}

TEST_P(RtpSenderVideoTest, SendRawVideo) {
  const uint8_t kPayloadType = 111;
  const uint8_t kPayload[] = {11, 22, 33, 44, 55};

  // Send a frame.
  RTPVideoHeader video_header;
  video_header.frame_type = VideoFrameType::kVideoFrameKey;
  ASSERT_TRUE(rtp_sender_video_->SendVideo(kPayloadType, absl::nullopt, 1234,
                                           4321, kPayload, video_header,
                                           absl::nullopt));

  rtc::ArrayView<const uint8_t> sent_payload =
      transport_.last_sent_packet().payload();
  EXPECT_THAT(sent_payload, ElementsAreArray(kPayload));
}

INSTANTIATE_TEST_SUITE_P(WithAndWithoutOverhead,
                         RtpSenderVideoTest,
                         ::testing::Bool());

class RtpSenderVideoWithFrameTransformerTest : public ::testing::Test {
 public:
  RtpSenderVideoWithFrameTransformerTest()
      : fake_clock_(kStartTime),
        retransmission_rate_limiter_(&fake_clock_, 1000),
        rtp_module_(ModuleRtpRtcpImpl2::Create([&] {
          RtpRtcpInterface::Configuration config;
          config.clock = &fake_clock_;
          config.outgoing_transport = &transport_;
          config.retransmission_rate_limiter = &retransmission_rate_limiter_;
          config.field_trials = &field_trials_;
          config.local_media_ssrc = kSsrc;
          return config;
        }())) {
    rtp_module_->SetSequenceNumber(kSeqNum);
    rtp_module_->SetStartTimestamp(0);
  }

  std::unique_ptr<RTPSenderVideo> CreateSenderWithFrameTransformer(
      rtc::scoped_refptr<FrameTransformerInterface> transformer) {
    RTPSenderVideo::Config config;
    config.clock = &fake_clock_;
    config.rtp_sender = rtp_module_->RtpSender();
    config.field_trials = &field_trials_;
    config.frame_transformer = transformer;
    return std::make_unique<RTPSenderVideo>(config);
  }

 protected:
  FieldTrialBasedConfig field_trials_;
  SimulatedClock fake_clock_;
  LoopbackTransportTest transport_;
  RateLimiter retransmission_rate_limiter_;
  std::unique_ptr<ModuleRtpRtcpImpl2> rtp_module_;
};

std::unique_ptr<EncodedImage> CreateDefaultEncodedImage() {
  const uint8_t data[] = {1, 2, 3, 4};
  auto encoded_image = std::make_unique<EncodedImage>();
  encoded_image->SetEncodedData(
      webrtc::EncodedImageBuffer::Create(data, sizeof(data)));
  return encoded_image;
}

TEST_F(RtpSenderVideoWithFrameTransformerTest,
       CreateSenderRegistersFrameTransformer) {
  rtc::scoped_refptr<MockFrameTransformer> mock_frame_transformer =
      new rtc::RefCountedObject<NiceMock<MockFrameTransformer>>();
  EXPECT_CALL(*mock_frame_transformer,
              RegisterTransformedFrameSinkCallback(_, kSsrc));
  std::unique_ptr<RTPSenderVideo> rtp_sender_video =
      CreateSenderWithFrameTransformer(mock_frame_transformer);
}

TEST_F(RtpSenderVideoWithFrameTransformerTest,
       DestroySenderUnregistersFrameTransformer) {
  rtc::scoped_refptr<MockFrameTransformer> mock_frame_transformer =
      new rtc::RefCountedObject<NiceMock<MockFrameTransformer>>();
  std::unique_ptr<RTPSenderVideo> rtp_sender_video =
      CreateSenderWithFrameTransformer(mock_frame_transformer);
  EXPECT_CALL(*mock_frame_transformer,
              UnregisterTransformedFrameSinkCallback(kSsrc));
  rtp_sender_video = nullptr;
}

TEST_F(RtpSenderVideoWithFrameTransformerTest,
       SendEncodedImageTransformsFrame) {
  rtc::scoped_refptr<MockFrameTransformer> mock_frame_transformer =
      new rtc::RefCountedObject<NiceMock<MockFrameTransformer>>();
  std::unique_ptr<RTPSenderVideo> rtp_sender_video =
      CreateSenderWithFrameTransformer(mock_frame_transformer);
  auto encoded_image = CreateDefaultEncodedImage();
  RTPVideoHeader video_header;

  EXPECT_CALL(*mock_frame_transformer, Transform);
  rtp_sender_video->SendEncodedImage(kPayload, kType, kTimestamp,
                                     *encoded_image, video_header,
                                     kDefaultExpectedRetransmissionTimeMs);
}

TEST_F(RtpSenderVideoWithFrameTransformerTest, OnTransformedFrameSendsVideo) {
  rtc::scoped_refptr<MockFrameTransformer> mock_frame_transformer =
      new rtc::RefCountedObject<NiceMock<MockFrameTransformer>>();
  rtc::scoped_refptr<TransformedFrameCallback> callback;
  EXPECT_CALL(*mock_frame_transformer, RegisterTransformedFrameSinkCallback)
      .WillOnce(SaveArg<0>(&callback));
  std::unique_ptr<RTPSenderVideo> rtp_sender_video =
      CreateSenderWithFrameTransformer(mock_frame_transformer);
  ASSERT_TRUE(callback);

  auto encoded_image = CreateDefaultEncodedImage();
  RTPVideoHeader video_header;
  video_header.frame_type = VideoFrameType::kVideoFrameKey;
  ON_CALL(*mock_frame_transformer, Transform)
      .WillByDefault(
          [&callback](std::unique_ptr<TransformableFrameInterface> frame) {
            callback->OnTransformedFrame(std::move(frame));
          });
  TaskQueueForTest encoder_queue;
  encoder_queue.SendTask(
      [&] {
        rtp_sender_video->SendEncodedImage(
            kPayload, kType, kTimestamp, *encoded_image, video_header,
            kDefaultExpectedRetransmissionTimeMs);
      },
      RTC_FROM_HERE);
  encoder_queue.WaitForPreviouslyPostedTasks();
  EXPECT_EQ(transport_.packets_sent(), 1);
}

TEST_F(RtpSenderVideoWithFrameTransformerTest,
       TransformableFrameMetadataHasCorrectValue) {
  rtc::scoped_refptr<MockFrameTransformer> mock_frame_transformer =
      new rtc::RefCountedObject<NiceMock<MockFrameTransformer>>();
  std::unique_ptr<RTPSenderVideo> rtp_sender_video =
      CreateSenderWithFrameTransformer(mock_frame_transformer);
  auto encoded_image = CreateDefaultEncodedImage();
  RTPVideoHeader video_header;
  video_header.width = 1280u;
  video_header.height = 720u;
  RTPVideoHeader::GenericDescriptorInfo& generic =
      video_header.generic.emplace();
  generic.frame_id = 10;
  generic.temporal_index = 3;
  generic.spatial_index = 2;
  generic.decode_target_indications = {DecodeTargetIndication::kSwitch};
  generic.dependencies = {5};

  // Check that the transformable frame passed to the frame transformer has the
  // correct metadata.
  EXPECT_CALL(*mock_frame_transformer, Transform)
      .WillOnce(
          [](std::unique_ptr<TransformableFrameInterface> transformable_frame) {
            auto frame =
                absl::WrapUnique(static_cast<TransformableVideoFrameInterface*>(
                    transformable_frame.release()));
            ASSERT_TRUE(frame);
            auto metadata = frame->GetMetadata();
            EXPECT_EQ(metadata.GetWidth(), 1280u);
            EXPECT_EQ(metadata.GetHeight(), 720u);
            EXPECT_EQ(metadata.GetFrameId(), 10);
            EXPECT_EQ(metadata.GetTemporalIndex(), 3);
            EXPECT_EQ(metadata.GetSpatialIndex(), 2);
            EXPECT_THAT(metadata.GetFrameDependencies(), ElementsAre(5));
            EXPECT_THAT(metadata.GetDecodeTargetIndications(),
                        ElementsAre(DecodeTargetIndication::kSwitch));
          });
  rtp_sender_video->SendEncodedImage(kPayload, kType, kTimestamp,
                                     *encoded_image, video_header,
                                     kDefaultExpectedRetransmissionTimeMs);
}

}  // namespace
}  // namespace webrtc
