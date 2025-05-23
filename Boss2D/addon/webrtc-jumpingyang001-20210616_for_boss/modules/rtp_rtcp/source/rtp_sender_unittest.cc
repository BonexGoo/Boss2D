/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_sender_h //original-code:"modules/rtp_rtcp/source/rtp_sender.h"

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_h //original-code:"api/rtc_event_log/rtc_event.h"
#include BOSS_WEBRTC_U_api__transport__field_trial_based_config_h //original-code:"api/transport/field_trial_based_config.h"
#include BOSS_WEBRTC_U_api__video__video_codec_constants_h //original-code:"api/video/video_codec_constants.h"
#include BOSS_WEBRTC_U_api__video__video_timing_h //original-code:"api/video/video_timing.h"
#include "logging/rtc_event_log/mock/mock_rtc_event_log.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_cvo_h //original-code:"modules/rtp_rtcp/include/rtp_cvo.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_header_extension_map_h //original-code:"modules/rtp_rtcp/include/rtp_header_extension_map.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_packet_sender_h //original-code:"modules/rtp_rtcp/include/rtp_packet_sender.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_format_video_generic_h //original-code:"modules/rtp_rtcp/source/rtp_format_video_generic.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_generic_frame_descriptor_h //original-code:"modules/rtp_rtcp/source/rtp_generic_frame_descriptor.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_generic_frame_descriptor_extension_h //original-code:"modules/rtp_rtcp/source/rtp_generic_frame_descriptor_extension.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_header_extensions_h //original-code:"modules/rtp_rtcp/source/rtp_header_extensions.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_received_h //original-code:"modules/rtp_rtcp/source/rtp_packet_received.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_to_send_h //original-code:"modules/rtp_rtcp/source/rtp_packet_to_send.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_sender_video_h //original-code:"modules/rtp_rtcp/source/rtp_sender_video.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_utility_h //original-code:"modules/rtp_rtcp/source/rtp_utility.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_fec_generator_h //original-code:"modules/rtp_rtcp/source/video_fec_generator.h"
#include BOSS_WEBRTC_U_rtc_base__arraysize_h //original-code:"rtc_base/arraysize.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__rate_limiter_h //original-code:"rtc_base/rate_limiter.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__to_queued_task_h //original-code:"rtc_base/task_utils/to_queued_task.h"
#include BOSS_WEBRTC_U_test__field_trial_h //original-code:"test/field_trial.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include BOSS_WEBRTC_U_test__mock_transport_h //original-code:"test/mock_transport.h"
#include BOSS_WEBRTC_U_test__rtp_header_parser_h //original-code:"test/rtp_header_parser.h"
#include "test/time_controller/simulated_time_controller.h"

namespace webrtc {

namespace {
enum : int {  // The first valid value is 1.
  kAbsoluteSendTimeExtensionId = 1,
  kAudioLevelExtensionId,
  kGenericDescriptorId,
  kMidExtensionId,
  kRepairedRidExtensionId,
  kRidExtensionId,
  kTransmissionTimeOffsetExtensionId,
  kTransportSequenceNumberExtensionId,
  kVideoRotationExtensionId,
  kVideoTimingExtensionId,
};

const int kPayload = 100;
const int kRtxPayload = 98;
const uint32_t kTimestamp = 10;
const uint16_t kSeqNum = 33;
const uint32_t kSsrc = 725242;
const uint32_t kRtxSsrc = 12345;
const uint32_t kFlexFecSsrc = 45678;
const uint64_t kStartTime = 123456789;
const size_t kMaxPaddingSize = 224u;
const uint8_t kPayloadData[] = {47, 11, 32, 93, 89};
const int64_t kDefaultExpectedRetransmissionTimeMs = 125;
const size_t kMaxPaddingLength = 224;      // Value taken from rtp_sender.cc.
const uint32_t kTimestampTicksPerMs = 90;  // 90kHz clock.

using ::testing::_;
using ::testing::AllOf;
using ::testing::AtLeast;
using ::testing::Contains;
using ::testing::Each;
using ::testing::ElementsAre;
using ::testing::Eq;
using ::testing::Field;
using ::testing::Gt;
using ::testing::IsEmpty;
using ::testing::NiceMock;
using ::testing::Not;
using ::testing::Pointee;
using ::testing::Property;
using ::testing::Return;
using ::testing::SizeIs;

class MockRtpPacketPacer : public RtpPacketSender {
 public:
  MockRtpPacketPacer() {}
  virtual ~MockRtpPacketPacer() {}

  MOCK_METHOD(void,
              EnqueuePackets,
              (std::vector<std::unique_ptr<RtpPacketToSend>>),
              (override));
};

class FieldTrialConfig : public WebRtcKeyValueConfig {
 public:
  FieldTrialConfig() : max_padding_factor_(1200) {}
  ~FieldTrialConfig() override {}

  void SetMaxPaddingFactor(double factor) { max_padding_factor_ = factor; }

  std::string Lookup(absl::string_view key) const override {
    if (key == "WebRTC-LimitPaddingSize") {
      char string_buf[32];
      rtc::SimpleStringBuilder ssb(string_buf);
      ssb << "factor:" << max_padding_factor_;
      return ssb.str();
    }
    return "";
  }

 private:
  double max_padding_factor_;
};

}  // namespace

class RtpSenderTest : public ::testing::Test {
 protected:
  RtpSenderTest()
      : time_controller_(Timestamp::Millis(kStartTime)),
        clock_(time_controller_.GetClock()),
        retransmission_rate_limiter_(clock_, 1000),
        flexfec_sender_(0,
                        kFlexFecSsrc,
                        kSsrc,
                        "",
                        std::vector<RtpExtension>(),
                        std::vector<RtpExtensionSize>(),
                        nullptr,
                        clock_),
        kMarkerBit(true) {
  }

  void SetUp() override { SetUpRtpSender(true, false, nullptr); }

  void SetUpRtpSender(bool populate_network2,
                      bool always_send_mid_and_rid,
                      VideoFecGenerator* fec_generator) {
    RtpRtcpInterface::Configuration config = GetDefaultConfig();
    config.fec_generator = fec_generator;
    config.populate_network2_timestamp = populate_network2;
    config.always_send_mid_and_rid = always_send_mid_and_rid;
    CreateSender(config);
  }

  RtpRtcpInterface::Configuration GetDefaultConfig() {
    RtpRtcpInterface::Configuration config;
    config.clock = clock_;
    config.local_media_ssrc = kSsrc;
    config.rtx_send_ssrc = kRtxSsrc;
    config.event_log = &mock_rtc_event_log_;
    config.retransmission_rate_limiter = &retransmission_rate_limiter_;
    config.paced_sender = &mock_paced_sender_;
    config.field_trials = &field_trials_;
    return config;
  }

  void CreateSender(const RtpRtcpInterface::Configuration& config) {
    packet_history_ = std::make_unique<RtpPacketHistory>(
        config.clock, config.enable_rtx_padding_prioritization);
    rtp_sender_ = std::make_unique<RTPSender>(config, packet_history_.get(),
                                              config.paced_sender);
    rtp_sender_->SetSequenceNumber(kSeqNum);
    rtp_sender_->SetTimestampOffset(0);
  }

  GlobalSimulatedTimeController time_controller_;
  Clock* const clock_;
  NiceMock<MockRtcEventLog> mock_rtc_event_log_;
  MockRtpPacketPacer mock_paced_sender_;
  RateLimiter retransmission_rate_limiter_;
  FlexfecSender flexfec_sender_;

  std::unique_ptr<RtpPacketHistory> packet_history_;
  std::unique_ptr<RTPSender> rtp_sender_;

  const bool kMarkerBit;
  FieldTrialConfig field_trials_;

  std::unique_ptr<RtpPacketToSend> BuildRtpPacket(int payload_type,
                                                  bool marker_bit,
                                                  uint32_t timestamp,
                                                  int64_t capture_time_ms) {
    auto packet = rtp_sender_->AllocatePacket();
    packet->SetPayloadType(payload_type);
    packet->set_packet_type(RtpPacketMediaType::kVideo);
    packet->SetMarker(marker_bit);
    packet->SetTimestamp(timestamp);
    packet->set_capture_time_ms(capture_time_ms);
    EXPECT_TRUE(rtp_sender_->AssignSequenceNumber(packet.get()));
    return packet;
  }

  std::unique_ptr<RtpPacketToSend> SendPacket(int64_t capture_time_ms,
                                              int payload_length) {
    uint32_t timestamp = capture_time_ms * 90;
    auto packet =
        BuildRtpPacket(kPayload, kMarkerBit, timestamp, capture_time_ms);
    packet->AllocatePayload(payload_length);
    packet->set_allow_retransmission(true);

    // Packet should be stored in a send bucket.
    EXPECT_TRUE(
        rtp_sender_->SendToNetwork(std::make_unique<RtpPacketToSend>(*packet)));
    return packet;
  }

  std::unique_ptr<RtpPacketToSend> SendGenericPacket() {
    const int64_t kCaptureTimeMs = clock_->TimeInMilliseconds();
    // Use maximum allowed size to catch corner cases when packet is dropped
    // because of lack of capacity for the media packet, or for an rtx packet
    // containing the media packet.
    return SendPacket(kCaptureTimeMs,
                      /*payload_length=*/rtp_sender_->MaxRtpPacketSize() -
                          rtp_sender_->ExpectedPerPacketOverhead());
  }

  size_t GenerateAndSendPadding(size_t target_size_bytes) {
    size_t generated_bytes = 0;
    for (auto& packet : rtp_sender_->GeneratePadding(target_size_bytes, true)) {
      generated_bytes += packet->payload_size() + packet->padding_size();
      rtp_sender_->SendToNetwork(std::move(packet));
    }
    return generated_bytes;
  }

  // The following are helpers for configuring the RTPSender. They must be
  // called before sending any packets.

  // Enable the retransmission stream with sizable packet storage.
  void EnableRtx() {
    // RTX needs to be able to read the source packets from the packet store.
    // Pick a number of packets to store big enough for any unit test.
    constexpr uint16_t kNumberOfPacketsToStore = 100;
    packet_history_->SetStorePacketsStatus(
        RtpPacketHistory::StorageMode::kStoreAndCull, kNumberOfPacketsToStore);
    rtp_sender_->SetRtxPayloadType(kRtxPayload, kPayload);
    rtp_sender_->SetRtxStatus(kRtxRetransmitted | kRtxRedundantPayloads);
  }

  // Enable sending of the MID header extension for both the primary SSRC and
  // the RTX SSRC.
  void EnableMidSending(const std::string& mid) {
    rtp_sender_->RegisterRtpHeaderExtension(RtpMid::kUri, kMidExtensionId);
    rtp_sender_->SetMid(mid);
  }

  // Enable sending of the RSID header extension for the primary SSRC and the
  // RRSID header extension for the RTX SSRC.
  void EnableRidSending(const std::string& rid) {
    rtp_sender_->RegisterRtpHeaderExtension(RtpStreamId::kUri, kRidExtensionId);
    rtp_sender_->RegisterRtpHeaderExtension(RepairedRtpStreamId::kUri,
                                            kRepairedRidExtensionId);
    rtp_sender_->SetRid(rid);
  }
};

TEST_F(RtpSenderTest, AllocatePacketSetCsrc) {
  // Configure rtp_sender with csrc.
  std::vector<uint32_t> csrcs;
  csrcs.push_back(0x23456789);
  rtp_sender_->SetCsrcs(csrcs);

  auto packet = rtp_sender_->AllocatePacket();

  ASSERT_TRUE(packet);
  EXPECT_EQ(rtp_sender_->SSRC(), packet->Ssrc());
  EXPECT_EQ(csrcs, packet->Csrcs());
}

TEST_F(RtpSenderTest, AllocatePacketReserveExtensions) {
  // Configure rtp_sender with extensions.
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      TransmissionOffset::kUri, kTransmissionTimeOffsetExtensionId));
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      AbsoluteSendTime::kUri, kAbsoluteSendTimeExtensionId));
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(AudioLevel::kUri,
                                                      kAudioLevelExtensionId));
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      TransportSequenceNumber::kUri, kTransportSequenceNumberExtensionId));
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      VideoOrientation::kUri, kVideoRotationExtensionId));

  auto packet = rtp_sender_->AllocatePacket();

  ASSERT_TRUE(packet);
  // Preallocate BWE extensions RtpSender set itself.
  EXPECT_TRUE(packet->HasExtension<TransmissionOffset>());
  EXPECT_TRUE(packet->HasExtension<AbsoluteSendTime>());
  EXPECT_TRUE(packet->HasExtension<TransportSequenceNumber>());
  // Do not allocate media specific extensions.
  EXPECT_FALSE(packet->HasExtension<AudioLevel>());
  EXPECT_FALSE(packet->HasExtension<VideoOrientation>());
}

TEST_F(RtpSenderTest, PaddingAlwaysAllowedOnAudio) {
  RtpRtcpInterface::Configuration config = GetDefaultConfig();
  config.audio = true;
  CreateSender(config);

  std::unique_ptr<RtpPacketToSend> audio_packet = rtp_sender_->AllocatePacket();
  // Padding on audio stream allowed regardless of marker in the last packet.
  audio_packet->SetMarker(false);
  audio_packet->SetPayloadType(kPayload);
  rtp_sender_->AssignSequenceNumber(audio_packet.get());

  const size_t kPaddingSize = 59;

  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(AllOf(
          Pointee(Property(&RtpPacketToSend::packet_type,
                           RtpPacketMediaType::kPadding)),
          Pointee(Property(&RtpPacketToSend::padding_size, kPaddingSize))))));
  EXPECT_EQ(kPaddingSize, GenerateAndSendPadding(kPaddingSize));

  // Requested padding size is too small, will send a larger one.
  const size_t kMinPaddingSize = 50;
  EXPECT_CALL(mock_paced_sender_,
              EnqueuePackets(ElementsAre(
                  AllOf(Pointee(Property(&RtpPacketToSend::packet_type,
                                         RtpPacketMediaType::kPadding)),
                        Pointee(Property(&RtpPacketToSend::padding_size,
                                         kMinPaddingSize))))));
  EXPECT_EQ(kMinPaddingSize, GenerateAndSendPadding(kMinPaddingSize - 5));
}

TEST_F(RtpSenderTest, SendToNetworkForwardsPacketsToPacer) {
  auto packet = BuildRtpPacket(kPayload, kMarkerBit, kTimestamp, 0);
  int64_t now_ms = clock_->TimeInMilliseconds();

  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(AllOf(
          Pointee(Property(&RtpPacketToSend::Ssrc, kSsrc)),
          Pointee(Property(&RtpPacketToSend::SequenceNumber, kSeqNum)),
          Pointee(Property(&RtpPacketToSend::capture_time_ms, now_ms))))));
  EXPECT_TRUE(
      rtp_sender_->SendToNetwork(std::make_unique<RtpPacketToSend>(*packet)));
}

TEST_F(RtpSenderTest, ReSendPacketForwardsPacketsToPacer) {
  packet_history_->SetStorePacketsStatus(
      RtpPacketHistory::StorageMode::kStoreAndCull, 10);
  int64_t now_ms = clock_->TimeInMilliseconds();
  auto packet = BuildRtpPacket(kPayload, kMarkerBit, kTimestamp, now_ms);
  uint16_t seq_no = packet->SequenceNumber();
  packet->set_allow_retransmission(true);
  packet_history_->PutRtpPacket(std::move(packet), now_ms);

  EXPECT_CALL(mock_paced_sender_,
              EnqueuePackets(ElementsAre(AllOf(
                  Pointee(Property(&RtpPacketToSend::Ssrc, kSsrc)),
                  Pointee(Property(&RtpPacketToSend::SequenceNumber, kSeqNum)),
                  Pointee(Property(&RtpPacketToSend::capture_time_ms, now_ms)),
                  Pointee(Property(&RtpPacketToSend::packet_type,
                                   RtpPacketMediaType::kRetransmission))))));
  EXPECT_TRUE(rtp_sender_->ReSendPacket(seq_no));
}

// This test sends 1 regular video packet, then 4 padding packets, and then
// 1 more regular packet.
TEST_F(RtpSenderTest, SendPadding) {
  constexpr int kNumPaddingPackets = 4;
  EXPECT_CALL(mock_paced_sender_, EnqueuePackets);
  std::unique_ptr<RtpPacketToSend> media_packet =
      SendPacket(/*capture_time_ms=*/clock_->TimeInMilliseconds(),
                 /*payload_size=*/100);

  // Wait 50 ms before generating each padding packet.
  for (int i = 0; i < kNumPaddingPackets; ++i) {
    time_controller_.AdvanceTime(TimeDelta::Millis(50));
    const size_t kPaddingTargetBytes = 100;  // Request 100 bytes of padding.

    // Padding should be sent on the media ssrc, with a continous sequence
    // number range. Size will be forced to full pack size and the timestamp
    // shall be that of the last media packet.
    EXPECT_CALL(mock_paced_sender_,
                EnqueuePackets(ElementsAre(AllOf(
                    Pointee(Property(&RtpPacketToSend::Ssrc, kSsrc)),
                    Pointee(Property(&RtpPacketToSend::SequenceNumber,
                                     media_packet->SequenceNumber() + i + 1)),
                    Pointee(Property(&RtpPacketToSend::padding_size,
                                     kMaxPaddingLength)),
                    Pointee(Property(&RtpPacketToSend::Timestamp,
                                     media_packet->Timestamp()))))));
    std::vector<std::unique_ptr<RtpPacketToSend>> padding_packets =
        rtp_sender_->GeneratePadding(kPaddingTargetBytes,
                                     /*media_has_been_sent=*/true);
    ASSERT_THAT(padding_packets, SizeIs(1));
    rtp_sender_->SendToNetwork(std::move(padding_packets[0]));
  }

  // Send a regular video packet again.
  EXPECT_CALL(mock_paced_sender_,
              EnqueuePackets(ElementsAre(AllOf(
                  Pointee(Property(
                      &RtpPacketToSend::SequenceNumber,
                      media_packet->SequenceNumber() + kNumPaddingPackets + 1)),
                  Pointee(Property(&RtpPacketToSend::Timestamp,
                                   Gt(media_packet->Timestamp())))))));

  std::unique_ptr<RtpPacketToSend> next_media_packet =
      SendPacket(/*capture_time_ms=*/clock_->TimeInMilliseconds(),
                 /*payload_size=*/100);
}

TEST_F(RtpSenderTest, NoPaddingAsFirstPacketWithoutBweExtensions) {
  EXPECT_THAT(rtp_sender_->GeneratePadding(/*target_size_bytes=*/100,
                                           /*media_has_been_sent=*/false),
              IsEmpty());

  // Don't send padding before media even with RTX.
  EnableRtx();
  EXPECT_THAT(rtp_sender_->GeneratePadding(/*target_size_bytes=*/100,
                                           /*media_has_been_sent=*/false),
              IsEmpty());
}

TEST_F(RtpSenderTest, AllowPaddingAsFirstPacketOnRtxWithTransportCc) {
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      TransportSequenceNumber::kUri, kTransportSequenceNumberExtensionId));

  // Padding can't be sent as first packet on media SSRC since we don't know
  // what payload type to assign.
  EXPECT_THAT(rtp_sender_->GeneratePadding(/*target_size_bytes=*/100,
                                           /*media_has_been_sent=*/false),
              IsEmpty());

  // With transportcc padding can be sent as first packet on the RTX SSRC.
  EnableRtx();
  EXPECT_THAT(rtp_sender_->GeneratePadding(/*target_size_bytes=*/100,
                                           /*media_has_been_sent=*/false),
              Not(IsEmpty()));
}

TEST_F(RtpSenderTest, AllowPaddingAsFirstPacketOnRtxWithAbsSendTime) {
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      AbsoluteSendTime::kUri, kAbsoluteSendTimeExtensionId));

  // Padding can't be sent as first packet on media SSRC since we don't know
  // what payload type to assign.
  EXPECT_THAT(rtp_sender_->GeneratePadding(/*target_size_bytes=*/100,
                                           /*media_has_been_sent=*/false),
              IsEmpty());

  // With abs send time, padding can be sent as first packet on the RTX SSRC.
  EnableRtx();
  EXPECT_THAT(rtp_sender_->GeneratePadding(/*target_size_bytes=*/100,
                                           /*media_has_been_sent=*/false),
              Not(IsEmpty()));
}

TEST_F(RtpSenderTest, UpdatesTimestampsOnPlainRtxPadding) {
  EnableRtx();
  // Timestamps as set based on capture time in RtpSenderTest.
  const int64_t start_time = clock_->TimeInMilliseconds();
  const uint32_t start_timestamp = start_time * kTimestampTicksPerMs;

  // Start by sending one media packet.
  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(AllOf(
          Pointee(Property(&RtpPacketToSend::padding_size, 0u)),
          Pointee(Property(&RtpPacketToSend::Timestamp, start_timestamp)),
          Pointee(Property(&RtpPacketToSend::capture_time_ms, start_time))))));
  std::unique_ptr<RtpPacketToSend> media_packet =
      SendPacket(start_time, /*payload_size=*/600);

  // Advance time before sending padding.
  const TimeDelta kTimeDiff = TimeDelta::Millis(17);
  time_controller_.AdvanceTime(kTimeDiff);

  // Timestamps on padding should be offset from the sent media.
  EXPECT_THAT(
      rtp_sender_->GeneratePadding(/*target_size_bytes=*/100,
                                   /*media_has_been_sent=*/true),
      Each(AllOf(
          Pointee(Property(&RtpPacketToSend::padding_size, kMaxPaddingLength)),
          Pointee(Property(
              &RtpPacketToSend::Timestamp,
              start_timestamp + (kTimestampTicksPerMs * kTimeDiff.ms()))),
          Pointee(Property(&RtpPacketToSend::capture_time_ms,
                           start_time + kTimeDiff.ms())))));
}

TEST_F(RtpSenderTest, KeepsTimestampsOnPayloadPadding) {
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      TransportSequenceNumber::kUri, kTransportSequenceNumberExtensionId));
  EnableRtx();
  // Timestamps as set based on capture time in RtpSenderTest.
  const int64_t start_time = clock_->TimeInMilliseconds();
  const uint32_t start_timestamp = start_time * kTimestampTicksPerMs;
  const size_t kPayloadSize = 600;
  const size_t kRtxHeaderSize = 2;

  // Start by sending one media packet and putting in the packet history.
  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(AllOf(
          Pointee(Property(&RtpPacketToSend::padding_size, 0u)),
          Pointee(Property(&RtpPacketToSend::Timestamp, start_timestamp)),
          Pointee(Property(&RtpPacketToSend::capture_time_ms, start_time))))));
  std::unique_ptr<RtpPacketToSend> media_packet =
      SendPacket(start_time, kPayloadSize);
  packet_history_->PutRtpPacket(std::move(media_packet), start_time);

  // Advance time before sending padding.
  const TimeDelta kTimeDiff = TimeDelta::Millis(17);
  time_controller_.AdvanceTime(kTimeDiff);

  // Timestamps on payload padding should be set to original.
  EXPECT_THAT(
      rtp_sender_->GeneratePadding(/*target_size_bytes=*/100,
                                   /*media_has_been_sent=*/true),
      Each(AllOf(
          Pointee(Property(&RtpPacketToSend::padding_size, 0u)),
          Pointee(Property(&RtpPacketToSend::payload_size,
                           kPayloadSize + kRtxHeaderSize)),
          Pointee(Property(&RtpPacketToSend::Timestamp, start_timestamp)),
          Pointee(Property(&RtpPacketToSend::capture_time_ms, start_time)))));
}

// Test that the MID header extension is included on sent packets when
// configured.
TEST_F(RtpSenderTest, MidIncludedOnSentPackets) {
  const char kMid[] = "mid";
  EnableMidSending(kMid);

  // Send a couple packets, expect both packets to have the MID set.
  EXPECT_CALL(mock_paced_sender_,
              EnqueuePackets(ElementsAre(Pointee(
                  Property(&RtpPacketToSend::GetExtension<RtpMid>, kMid)))))
      .Times(2);
  SendGenericPacket();
  SendGenericPacket();
}

TEST_F(RtpSenderTest, RidIncludedOnSentPackets) {
  const char kRid[] = "f";
  EnableRidSending(kRid);

  EXPECT_CALL(mock_paced_sender_,
              EnqueuePackets(ElementsAre(Pointee(Property(
                  &RtpPacketToSend::GetExtension<RtpStreamId>, kRid)))));
  SendGenericPacket();
}

TEST_F(RtpSenderTest, RidIncludedOnRtxSentPackets) {
  const char kRid[] = "f";
  EnableRtx();
  EnableRidSending(kRid);

  EXPECT_CALL(mock_paced_sender_,
              EnqueuePackets(ElementsAre(Pointee(AllOf(
                  Property(&RtpPacketToSend::GetExtension<RtpStreamId>, kRid),
                  Property(&RtpPacketToSend::HasExtension<RepairedRtpStreamId>,
                           false))))))
      .WillOnce([&](std::vector<std::unique_ptr<RtpPacketToSend>> packets) {
        packet_history_->PutRtpPacket(std::move(packets[0]),
                                      clock_->TimeInMilliseconds());
      });
  SendGenericPacket();

  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(Pointee(AllOf(
          Property(&RtpPacketToSend::GetExtension<RepairedRtpStreamId>, kRid),
          Property(&RtpPacketToSend::HasExtension<RtpStreamId>, false))))));
  rtp_sender_->ReSendPacket(kSeqNum);
}

TEST_F(RtpSenderTest, MidAndRidNotIncludedOnSentPacketsAfterAck) {
  const char kMid[] = "mid";
  const char kRid[] = "f";

  EnableMidSending(kMid);
  EnableRidSending(kRid);

  // This first packet should include both MID and RID.
  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(Pointee(AllOf(
          Property(&RtpPacketToSend::GetExtension<RtpMid>, kMid),
          Property(&RtpPacketToSend::GetExtension<RtpStreamId>, kRid))))));
  auto first_built_packet = SendGenericPacket();
  rtp_sender_->OnReceivedAckOnSsrc(first_built_packet->SequenceNumber());

  // The second packet should include neither since an ack was received.
  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(Pointee(AllOf(
          Property(&RtpPacketToSend::HasExtension<RtpMid>, false),
          Property(&RtpPacketToSend::HasExtension<RtpStreamId>, false))))));
  SendGenericPacket();
}

TEST_F(RtpSenderTest, MidAndRidAlwaysIncludedOnSentPacketsWhenConfigured) {
  SetUpRtpSender(false, /*always_send_mid_and_rid=*/true, nullptr);
  const char kMid[] = "mid";
  const char kRid[] = "f";
  EnableMidSending(kMid);
  EnableRidSending(kRid);

  // Send two media packets: one before and one after the ack.
  // Due to the configuration, both sent packets should contain MID and RID.
  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(Pointee(
          AllOf(Property(&RtpPacketToSend::GetExtension<RtpMid>, kMid),
                Property(&RtpPacketToSend::GetExtension<RtpStreamId>, kRid))))))
      .Times(2);
  auto first_built_packet = SendGenericPacket();
  rtp_sender_->OnReceivedAckOnSsrc(first_built_packet->SequenceNumber());
  SendGenericPacket();
}

// Test that the first RTX packet includes both MID and RRID even if the packet
// being retransmitted did not have MID or RID. The MID and RID are needed on
// the first packets for a given SSRC, and RTX packets are sent on a separate
// SSRC.
TEST_F(RtpSenderTest, MidAndRidIncludedOnFirstRtxPacket) {
  const char kMid[] = "mid";
  const char kRid[] = "f";

  EnableRtx();
  EnableMidSending(kMid);
  EnableRidSending(kRid);

  // This first packet will include both MID and RID.
  EXPECT_CALL(mock_paced_sender_, EnqueuePackets);
  auto first_built_packet = SendGenericPacket();
  rtp_sender_->OnReceivedAckOnSsrc(first_built_packet->SequenceNumber());

  // The second packet will include neither since an ack was received, put
  // it in the packet history for retransmission.
  EXPECT_CALL(mock_paced_sender_, EnqueuePackets(SizeIs(1)))
      .WillOnce([&](std::vector<std::unique_ptr<RtpPacketToSend>> packets) {
        packet_history_->PutRtpPacket(std::move(packets[0]),
                                      clock_->TimeInMilliseconds());
      });
  auto second_built_packet = SendGenericPacket();

  // The first RTX packet should include MID and RRID.
  EXPECT_CALL(mock_paced_sender_,
              EnqueuePackets(ElementsAre(Pointee(AllOf(
                  Property(&RtpPacketToSend::GetExtension<RtpMid>, kMid),
                  Property(&RtpPacketToSend::GetExtension<RepairedRtpStreamId>,
                           kRid))))));
  rtp_sender_->ReSendPacket(second_built_packet->SequenceNumber());
}

// Test that the RTX packets sent after receving an ACK on the RTX SSRC does
// not include either MID or RRID even if the packet being retransmitted did
// had a MID or RID.
TEST_F(RtpSenderTest, MidAndRidNotIncludedOnRtxPacketsAfterAck) {
  const char kMid[] = "mid";
  const char kRid[] = "f";

  EnableRtx();
  EnableMidSending(kMid);
  EnableRidSending(kRid);

  // This first packet will include both MID and RID.
  EXPECT_CALL(mock_paced_sender_, EnqueuePackets(SizeIs(1)))
      .WillOnce([&](std::vector<std::unique_ptr<RtpPacketToSend>> packets) {
        packet_history_->PutRtpPacket(std::move(packets[0]),
                                      clock_->TimeInMilliseconds());
      });
  auto first_built_packet = SendGenericPacket();
  rtp_sender_->OnReceivedAckOnSsrc(first_built_packet->SequenceNumber());

  // The second packet will include neither since an ack was received.
  EXPECT_CALL(mock_paced_sender_, EnqueuePackets(SizeIs(1)))
      .WillOnce([&](std::vector<std::unique_ptr<RtpPacketToSend>> packets) {
        packet_history_->PutRtpPacket(std::move(packets[0]),
                                      clock_->TimeInMilliseconds());
      });
  auto second_built_packet = SendGenericPacket();

  // The first RTX packet will include MID and RRID.
  EXPECT_CALL(mock_paced_sender_, EnqueuePackets(SizeIs(1)))
      .WillOnce([&](std::vector<std::unique_ptr<RtpPacketToSend>> packets) {
        rtp_sender_->OnReceivedAckOnRtxSsrc(packets[0]->SequenceNumber());
        packet_history_->MarkPacketAsSent(
            *packets[0]->retransmitted_sequence_number());
      });
  rtp_sender_->ReSendPacket(second_built_packet->SequenceNumber());

  // The second and third RTX packets should not include MID nor RRID.
  EXPECT_CALL(mock_paced_sender_,
              EnqueuePackets(ElementsAre(Pointee(AllOf(
                  Property(&RtpPacketToSend::HasExtension<RtpMid>, false),
                  Property(&RtpPacketToSend::HasExtension<RepairedRtpStreamId>,
                           false))))))
      .Times(2);
  rtp_sender_->ReSendPacket(first_built_packet->SequenceNumber());
  rtp_sender_->ReSendPacket(second_built_packet->SequenceNumber());
}

TEST_F(RtpSenderTest, MidAndRidAlwaysIncludedOnRtxPacketsWhenConfigured) {
  SetUpRtpSender(false, /*always_send_mid_and_rid=*/true, nullptr);
  const char kMid[] = "mid";
  const char kRid[] = "f";
  EnableRtx();
  EnableMidSending(kMid);
  EnableRidSending(kRid);

  // Send two media packets: one before and one after the ack.
  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(Pointee(
          AllOf(Property(&RtpPacketToSend::GetExtension<RtpMid>, kMid),
                Property(&RtpPacketToSend::GetExtension<RtpStreamId>, kRid))))))
      .Times(2)
      .WillRepeatedly(
          [&](std::vector<std::unique_ptr<RtpPacketToSend>> packets) {
            packet_history_->PutRtpPacket(std::move(packets[0]),
                                          clock_->TimeInMilliseconds());
          });
  auto media_packet1 = SendGenericPacket();
  rtp_sender_->OnReceivedAckOnSsrc(media_packet1->SequenceNumber());
  auto media_packet2 = SendGenericPacket();

  // Send three RTX packets with different combinations of orders w.r.t. the
  // media and RTX acks.
  // Due to the configuration, all sent packets should contain MID
  // and either RID (media) or RRID (RTX).
  EXPECT_CALL(mock_paced_sender_,
              EnqueuePackets(ElementsAre(Pointee(AllOf(
                  Property(&RtpPacketToSend::GetExtension<RtpMid>, kMid),
                  Property(&RtpPacketToSend::GetExtension<RepairedRtpStreamId>,
                           kRid))))))
      .Times(3)
      .WillRepeatedly(
          [&](std::vector<std::unique_ptr<RtpPacketToSend>> packets) {
            rtp_sender_->OnReceivedAckOnRtxSsrc(packets[0]->SequenceNumber());
            packet_history_->MarkPacketAsSent(
                *packets[0]->retransmitted_sequence_number());
          });
  rtp_sender_->ReSendPacket(media_packet2->SequenceNumber());
  rtp_sender_->ReSendPacket(media_packet1->SequenceNumber());
  rtp_sender_->ReSendPacket(media_packet2->SequenceNumber());
}

// Test that if the RtpState indicates an ACK has been received on that SSRC
// then neither the MID nor RID header extensions will be sent.
TEST_F(RtpSenderTest, MidAndRidNotIncludedOnSentPacketsAfterRtpStateRestored) {
  const char kMid[] = "mid";
  const char kRid[] = "f";

  EnableMidSending(kMid);
  EnableRidSending(kRid);

  RtpState state = rtp_sender_->GetRtpState();
  EXPECT_FALSE(state.ssrc_has_acked);
  state.ssrc_has_acked = true;
  rtp_sender_->SetRtpState(state);

  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(Pointee(AllOf(
          Property(&RtpPacketToSend::HasExtension<RtpMid>, false),
          Property(&RtpPacketToSend::HasExtension<RtpStreamId>, false))))));
  SendGenericPacket();
}

// Test that if the RTX RtpState indicates an ACK has been received on that
// RTX SSRC then neither the MID nor RRID header extensions will be sent on
// RTX packets.
TEST_F(RtpSenderTest, MidAndRridNotIncludedOnRtxPacketsAfterRtpStateRestored) {
  const char kMid[] = "mid";
  const char kRid[] = "f";

  EnableRtx();
  EnableMidSending(kMid);
  EnableRidSending(kRid);

  RtpState rtx_state = rtp_sender_->GetRtxRtpState();
  EXPECT_FALSE(rtx_state.ssrc_has_acked);
  rtx_state.ssrc_has_acked = true;
  rtp_sender_->SetRtxRtpState(rtx_state);

  EXPECT_CALL(mock_paced_sender_, EnqueuePackets(SizeIs(1)))
      .WillOnce([&](std::vector<std::unique_ptr<RtpPacketToSend>> packets) {
        packet_history_->PutRtpPacket(std::move(packets[0]),
                                      clock_->TimeInMilliseconds());
      });
  auto built_packet = SendGenericPacket();

  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(Pointee(AllOf(
          Property(&RtpPacketToSend::HasExtension<RtpMid>, false),
          Property(&RtpPacketToSend::HasExtension<RtpStreamId>, false))))));
  ASSERT_LT(0, rtp_sender_->ReSendPacket(built_packet->SequenceNumber()));
}

TEST_F(RtpSenderTest, RespectsNackBitrateLimit) {
  const int32_t kPacketSize = 1400;
  const int32_t kNumPackets = 30;

  retransmission_rate_limiter_.SetMaxRate(kPacketSize * kNumPackets * 8);

  packet_history_->SetStorePacketsStatus(
      RtpPacketHistory::StorageMode::kStoreAndCull, kNumPackets);
  const uint16_t kStartSequenceNumber = rtp_sender_->SequenceNumber();
  std::vector<uint16_t> sequence_numbers;
  for (int32_t i = 0; i < kNumPackets; ++i) {
    sequence_numbers.push_back(kStartSequenceNumber + i);
    time_controller_.AdvanceTime(TimeDelta::Millis(1));
    EXPECT_CALL(mock_paced_sender_, EnqueuePackets(SizeIs(1)))
        .WillOnce([&](std::vector<std::unique_ptr<RtpPacketToSend>> packets) {
          packet_history_->PutRtpPacket(std::move(packets[0]),
                                        clock_->TimeInMilliseconds());
        });
    SendPacket(clock_->TimeInMilliseconds(), kPacketSize);
  }

  time_controller_.AdvanceTime(TimeDelta::Millis(1000 - kNumPackets));

  // Resending should work - brings the bandwidth up to the limit.
  // NACK bitrate is capped to the same bitrate as the encoder, since the max
  // protection overhead is 50% (see MediaOptimization::SetTargetRates).
  EXPECT_CALL(mock_paced_sender_, EnqueuePackets(ElementsAre(Pointee(Property(
                                      &RtpPacketToSend::packet_type,
                                      RtpPacketMediaType::kRetransmission)))))
      .Times(kNumPackets)
      .WillRepeatedly(
          [&](std::vector<std::unique_ptr<RtpPacketToSend>> packets) {
            for (const auto& packet : packets) {
              packet_history_->MarkPacketAsSent(
                  *packet->retransmitted_sequence_number());
            }
          });
  rtp_sender_->OnReceivedNack(sequence_numbers, 0);

  // Must be at least 5ms in between retransmission attempts.
  time_controller_.AdvanceTime(TimeDelta::Millis(5));

  // Resending should not work, bandwidth exceeded.
  EXPECT_CALL(mock_paced_sender_, EnqueuePackets).Times(0);
  rtp_sender_->OnReceivedNack(sequence_numbers, 0);
}

TEST_F(RtpSenderTest, UpdatingCsrcsUpdatedOverhead) {
  RtpRtcpInterface::Configuration config = GetDefaultConfig();
  config.rtx_send_ssrc = {};
  CreateSender(config);

  // Base RTP overhead is 12B.
  EXPECT_EQ(rtp_sender_->ExpectedPerPacketOverhead(), 12u);

  // Adding two csrcs adds 2*4 bytes to the header.
  rtp_sender_->SetCsrcs({1, 2});
  EXPECT_EQ(rtp_sender_->ExpectedPerPacketOverhead(), 20u);
}

TEST_F(RtpSenderTest, OnOverheadChanged) {
  RtpRtcpInterface::Configuration config = GetDefaultConfig();
  config.rtx_send_ssrc = {};
  CreateSender(config);

  // Base RTP overhead is 12B.
  EXPECT_EQ(rtp_sender_->ExpectedPerPacketOverhead(), 12u);

  rtp_sender_->RegisterRtpHeaderExtension(TransmissionOffset::kUri,
                                          kTransmissionTimeOffsetExtensionId);

  // TransmissionTimeOffset extension has a size of 3B, but with the addition
  // of header index and rounding to 4 byte boundary we end up with 20B total.
  EXPECT_EQ(rtp_sender_->ExpectedPerPacketOverhead(), 20u);
}

TEST_F(RtpSenderTest, CountMidOnlyUntilAcked) {
  RtpRtcpInterface::Configuration config = GetDefaultConfig();
  config.rtx_send_ssrc = {};
  CreateSender(config);

  // Base RTP overhead is 12B.
  EXPECT_EQ(rtp_sender_->ExpectedPerPacketOverhead(), 12u);

  rtp_sender_->RegisterRtpHeaderExtension(RtpMid::kUri, kMidExtensionId);
  rtp_sender_->RegisterRtpHeaderExtension(RtpStreamId::kUri, kRidExtensionId);

  // Counted only if set.
  EXPECT_EQ(rtp_sender_->ExpectedPerPacketOverhead(), 12u);
  rtp_sender_->SetMid("foo");
  EXPECT_EQ(rtp_sender_->ExpectedPerPacketOverhead(), 36u);
  rtp_sender_->SetRid("bar");
  EXPECT_EQ(rtp_sender_->ExpectedPerPacketOverhead(), 52u);

  // Ack received, mid/rid no longer sent.
  rtp_sender_->OnReceivedAckOnSsrc(0);
  EXPECT_EQ(rtp_sender_->ExpectedPerPacketOverhead(), 12u);
}

TEST_F(RtpSenderTest, DontCountVolatileExtensionsIntoOverhead) {
  RtpRtcpInterface::Configuration config = GetDefaultConfig();
  config.rtx_send_ssrc = {};
  CreateSender(config);

  // Base RTP overhead is 12B.
  EXPECT_EQ(rtp_sender_->ExpectedPerPacketOverhead(), 12u);

  rtp_sender_->RegisterRtpHeaderExtension(InbandComfortNoiseExtension::kUri, 1);
  rtp_sender_->RegisterRtpHeaderExtension(AbsoluteCaptureTimeExtension::kUri,
                                          2);
  rtp_sender_->RegisterRtpHeaderExtension(VideoOrientation::kUri, 3);
  rtp_sender_->RegisterRtpHeaderExtension(PlayoutDelayLimits::kUri, 4);
  rtp_sender_->RegisterRtpHeaderExtension(VideoContentTypeExtension::kUri, 5);
  rtp_sender_->RegisterRtpHeaderExtension(VideoTimingExtension::kUri, 6);
  rtp_sender_->RegisterRtpHeaderExtension(RepairedRtpStreamId::kUri, 7);
  rtp_sender_->RegisterRtpHeaderExtension(ColorSpaceExtension::kUri, 8);

  // Still only 12B counted since can't count on above being sent.
  EXPECT_EQ(rtp_sender_->ExpectedPerPacketOverhead(), 12u);
}

TEST_F(RtpSenderTest, SendPacketHandlesRetransmissionHistory) {
  packet_history_->SetStorePacketsStatus(
      RtpPacketHistory::StorageMode::kStoreAndCull, 10);

  // Ignore calls to EnqueuePackets() for this test.
  EXPECT_CALL(mock_paced_sender_, EnqueuePackets).WillRepeatedly(Return());

  // Build a media packet and put in the packet history.
  std::unique_ptr<RtpPacketToSend> packet =
      BuildRtpPacket(kPayload, true, 0, clock_->TimeInMilliseconds());
  const uint16_t media_sequence_number = packet->SequenceNumber();
  packet->set_allow_retransmission(true);
  packet_history_->PutRtpPacket(std::move(packet),
                                clock_->TimeInMilliseconds());

  // Simulate successful retransmission request.
  time_controller_.AdvanceTime(TimeDelta::Millis(30));
  EXPECT_THAT(rtp_sender_->ReSendPacket(media_sequence_number), Gt(0));

  // Packet already pending, retransmission not allowed.
  time_controller_.AdvanceTime(TimeDelta::Millis(30));
  EXPECT_THAT(rtp_sender_->ReSendPacket(media_sequence_number), Eq(0));

  // Simulate packet exiting pacer, mark as not longer pending.
  packet_history_->MarkPacketAsSent(media_sequence_number);

  // Retransmissions allowed again.
  time_controller_.AdvanceTime(TimeDelta::Millis(30));
  EXPECT_THAT(rtp_sender_->ReSendPacket(media_sequence_number), Gt(0));
}

TEST_F(RtpSenderTest, MarksRetransmittedPackets) {
  packet_history_->SetStorePacketsStatus(
      RtpPacketHistory::StorageMode::kStoreAndCull, 10);

  // Build a media packet and put in the packet history.
  std::unique_ptr<RtpPacketToSend> packet =
      BuildRtpPacket(kPayload, true, 0, clock_->TimeInMilliseconds());
  const uint16_t media_sequence_number = packet->SequenceNumber();
  packet->set_allow_retransmission(true);
  packet_history_->PutRtpPacket(std::move(packet),
                                clock_->TimeInMilliseconds());

  // Expect a retransmission packet marked with which packet it is a
  // retransmit of.
  EXPECT_CALL(
      mock_paced_sender_,
      EnqueuePackets(ElementsAre(AllOf(
          Pointee(Property(&RtpPacketToSend::packet_type,
                           RtpPacketMediaType::kRetransmission)),
          Pointee(Property(&RtpPacketToSend::retransmitted_sequence_number,
                           Eq(media_sequence_number)))))));
  EXPECT_THAT(rtp_sender_->ReSendPacket(media_sequence_number), Gt(0));
}

TEST_F(RtpSenderTest, GeneratedPaddingHasBweExtensions) {
  // Min requested size in order to use RTX payload.
  const size_t kMinPaddingSize = 50;
  EnableRtx();

  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      TransmissionOffset::kUri, kTransmissionTimeOffsetExtensionId));
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      AbsoluteSendTime::kUri, kAbsoluteSendTimeExtensionId));
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      TransportSequenceNumber::kUri, kTransportSequenceNumberExtensionId));

  // Put a packet in the history, in order to facilitate payload padding.
  std::unique_ptr<RtpPacketToSend> packet =
      BuildRtpPacket(kPayload, true, 0, clock_->TimeInMilliseconds());
  packet->set_allow_retransmission(true);
  packet->SetPayloadSize(kMinPaddingSize);
  packet->set_packet_type(RtpPacketMediaType::kVideo);
  packet_history_->PutRtpPacket(std::move(packet),
                                clock_->TimeInMilliseconds());

  // Generate a plain padding packet, check that extensions are registered.
  std::vector<std::unique_ptr<RtpPacketToSend>> generated_packets =
      rtp_sender_->GeneratePadding(/*target_size_bytes=*/1, true);
  ASSERT_THAT(generated_packets, SizeIs(1));
  auto& plain_padding = generated_packets.front();
  EXPECT_GT(plain_padding->padding_size(), 0u);
  EXPECT_TRUE(plain_padding->HasExtension<TransportSequenceNumber>());
  EXPECT_TRUE(plain_padding->HasExtension<AbsoluteSendTime>());
  EXPECT_TRUE(plain_padding->HasExtension<TransmissionOffset>());
  EXPECT_GT(plain_padding->padding_size(), 0u);

  // Generate a payload padding packets, check that extensions are registered.
  generated_packets = rtp_sender_->GeneratePadding(kMinPaddingSize, true);
  ASSERT_EQ(generated_packets.size(), 1u);
  auto& payload_padding = generated_packets.front();
  EXPECT_EQ(payload_padding->padding_size(), 0u);
  EXPECT_TRUE(payload_padding->HasExtension<TransportSequenceNumber>());
  EXPECT_TRUE(payload_padding->HasExtension<AbsoluteSendTime>());
  EXPECT_TRUE(payload_padding->HasExtension<TransmissionOffset>());
  EXPECT_GT(payload_padding->payload_size(), 0u);
}

TEST_F(RtpSenderTest, GeneratePaddingResendsOldPacketsWithRtx) {
  // Min requested size in order to use RTX payload.
  const size_t kMinPaddingSize = 50;

  rtp_sender_->SetRtxStatus(kRtxRetransmitted | kRtxRedundantPayloads);
  rtp_sender_->SetRtxPayloadType(kRtxPayload, kPayload);
  packet_history_->SetStorePacketsStatus(
      RtpPacketHistory::StorageMode::kStoreAndCull, 1);

  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      TransportSequenceNumber::kUri, kTransportSequenceNumberExtensionId));

  const size_t kPayloadPacketSize = kMinPaddingSize;
  std::unique_ptr<RtpPacketToSend> packet =
      BuildRtpPacket(kPayload, true, 0, clock_->TimeInMilliseconds());
  packet->set_allow_retransmission(true);
  packet->SetPayloadSize(kPayloadPacketSize);
  packet->set_packet_type(RtpPacketMediaType::kVideo);
  packet_history_->PutRtpPacket(std::move(packet),
                                clock_->TimeInMilliseconds());

  // Generated padding has large enough budget that the video packet should be
  // retransmitted as padding.
  std::vector<std::unique_ptr<RtpPacketToSend>> generated_packets =
      rtp_sender_->GeneratePadding(kMinPaddingSize, true);
  ASSERT_EQ(generated_packets.size(), 1u);
  auto& padding_packet = generated_packets.front();
  EXPECT_EQ(padding_packet->packet_type(), RtpPacketMediaType::kPadding);
  EXPECT_EQ(padding_packet->Ssrc(), kRtxSsrc);
  EXPECT_EQ(padding_packet->payload_size(),
            kPayloadPacketSize + kRtxHeaderSize);

  // Not enough budged for payload padding, use plain padding instead.
  const size_t kPaddingBytesRequested = kMinPaddingSize - 1;

  size_t padding_bytes_generated = 0;
  generated_packets =
      rtp_sender_->GeneratePadding(kPaddingBytesRequested, true);
  EXPECT_EQ(generated_packets.size(), 1u);
  for (auto& packet : generated_packets) {
    EXPECT_EQ(packet->packet_type(), RtpPacketMediaType::kPadding);
    EXPECT_EQ(packet->Ssrc(), kRtxSsrc);
    EXPECT_EQ(packet->payload_size(), 0u);
    EXPECT_GT(packet->padding_size(), 0u);
    padding_bytes_generated += packet->padding_size();
  }

  EXPECT_EQ(padding_bytes_generated, kMaxPaddingSize);
}

TEST_F(RtpSenderTest, LimitsPayloadPaddingSize) {
  // Limit RTX payload padding to 2x target size.
  const double kFactor = 2.0;
  field_trials_.SetMaxPaddingFactor(kFactor);
  SetUpRtpSender(false, false, nullptr);
  rtp_sender_->SetRtxStatus(kRtxRetransmitted | kRtxRedundantPayloads);
  rtp_sender_->SetRtxPayloadType(kRtxPayload, kPayload);
  packet_history_->SetStorePacketsStatus(
      RtpPacketHistory::StorageMode::kStoreAndCull, 1);

  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      TransportSequenceNumber::kUri, kTransportSequenceNumberExtensionId));

  // Send a dummy video packet so it ends up in the packet history.
  const size_t kPayloadPacketSize = 1234u;
  std::unique_ptr<RtpPacketToSend> packet =
      BuildRtpPacket(kPayload, true, 0, clock_->TimeInMilliseconds());
  packet->set_allow_retransmission(true);
  packet->SetPayloadSize(kPayloadPacketSize);
  packet->set_packet_type(RtpPacketMediaType::kVideo);
  packet_history_->PutRtpPacket(std::move(packet),
                                clock_->TimeInMilliseconds());

  // Smallest target size that will result in the sent packet being returned as
  // padding.
  const size_t kMinTargerSizeForPayload =
      (kPayloadPacketSize + kRtxHeaderSize) / kFactor;

  // Generated padding has large enough budget that the video packet should be
  // retransmitted as padding.
  EXPECT_THAT(
      rtp_sender_->GeneratePadding(kMinTargerSizeForPayload, true),
      AllOf(Not(IsEmpty()),
            Each(Pointee(Property(&RtpPacketToSend::padding_size, Eq(0u))))));

  // If payload padding is > 2x requested size, plain padding is returned
  // instead.
  EXPECT_THAT(
      rtp_sender_->GeneratePadding(kMinTargerSizeForPayload - 1, true),
      AllOf(Not(IsEmpty()),
            Each(Pointee(Property(&RtpPacketToSend::padding_size, Gt(0u))))));
}

TEST_F(RtpSenderTest, GeneratePaddingCreatesPurePaddingWithoutRtx) {
  packet_history_->SetStorePacketsStatus(
      RtpPacketHistory::StorageMode::kStoreAndCull, 1);
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      TransmissionOffset::kUri, kTransmissionTimeOffsetExtensionId));
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      AbsoluteSendTime::kUri, kAbsoluteSendTimeExtensionId));
  ASSERT_TRUE(rtp_sender_->RegisterRtpHeaderExtension(
      TransportSequenceNumber::kUri, kTransportSequenceNumberExtensionId));

  const size_t kPayloadPacketSize = 1234;
  // Send a dummy video packet so it ends up in the packet history. Since we
  // are not using RTX, it should never be used as padding.
  std::unique_ptr<RtpPacketToSend> packet =
      BuildRtpPacket(kPayload, true, 0, clock_->TimeInMilliseconds());
  packet->set_allow_retransmission(true);
  packet->SetPayloadSize(kPayloadPacketSize);
  packet->set_packet_type(RtpPacketMediaType::kVideo);
  packet_history_->PutRtpPacket(std::move(packet),
                                clock_->TimeInMilliseconds());

  // Payload padding not available without RTX, only generate plain padding on
  // the media SSRC.
  // Number of padding packets is the requested padding size divided by max
  // padding packet size, rounded up. Pure padding packets are always of the
  // maximum size.
  const size_t kPaddingBytesRequested = kPayloadPacketSize + kRtxHeaderSize;
  const size_t kExpectedNumPaddingPackets =
      (kPaddingBytesRequested + kMaxPaddingSize - 1) / kMaxPaddingSize;
  size_t padding_bytes_generated = 0;
  std::vector<std::unique_ptr<RtpPacketToSend>> padding_packets =
      rtp_sender_->GeneratePadding(kPaddingBytesRequested, true);
  EXPECT_EQ(padding_packets.size(), kExpectedNumPaddingPackets);
  for (auto& packet : padding_packets) {
    EXPECT_EQ(packet->packet_type(), RtpPacketMediaType::kPadding);
    EXPECT_EQ(packet->Ssrc(), kSsrc);
    EXPECT_EQ(packet->payload_size(), 0u);
    EXPECT_GT(packet->padding_size(), 0u);
    padding_bytes_generated += packet->padding_size();
    EXPECT_TRUE(packet->HasExtension<TransportSequenceNumber>());
    EXPECT_TRUE(packet->HasExtension<AbsoluteSendTime>());
    EXPECT_TRUE(packet->HasExtension<TransmissionOffset>());
  }

  EXPECT_EQ(padding_bytes_generated,
            kExpectedNumPaddingPackets * kMaxPaddingSize);
}

TEST_F(RtpSenderTest, SupportsPadding) {
  bool kSendingMediaStats[] = {true, false};
  bool kEnableRedundantPayloads[] = {true, false};
  absl::string_view kBweExtensionUris[] = {
      TransportSequenceNumber::kUri, TransportSequenceNumberV2::kUri,
      AbsoluteSendTime::kUri, TransmissionOffset::kUri};
  const int kExtensionsId = 7;

  for (bool sending_media : kSendingMediaStats) {
    rtp_sender_->SetSendingMediaStatus(sending_media);
    for (bool redundant_payloads : kEnableRedundantPayloads) {
      int rtx_mode = kRtxRetransmitted;
      if (redundant_payloads) {
        rtx_mode |= kRtxRedundantPayloads;
      }
      rtp_sender_->SetRtxStatus(rtx_mode);

      for (auto extension_uri : kBweExtensionUris) {
        EXPECT_FALSE(rtp_sender_->SupportsPadding());
        rtp_sender_->RegisterRtpHeaderExtension(extension_uri, kExtensionsId);
        if (!sending_media) {
          EXPECT_FALSE(rtp_sender_->SupportsPadding());
        } else {
          EXPECT_TRUE(rtp_sender_->SupportsPadding());
          if (redundant_payloads) {
            EXPECT_TRUE(rtp_sender_->SupportsRtxPayloadPadding());
          } else {
            EXPECT_FALSE(rtp_sender_->SupportsRtxPayloadPadding());
          }
        }
        rtp_sender_->DeregisterRtpHeaderExtension(extension_uri);
        EXPECT_FALSE(rtp_sender_->SupportsPadding());
      }
    }
  }
}

TEST_F(RtpSenderTest, SetsCaptureTimeOnRtxRetransmissions) {
  EnableRtx();

  // Put a packet in the packet history, with current time as capture time.
  const int64_t start_time_ms = clock_->TimeInMilliseconds();
  std::unique_ptr<RtpPacketToSend> packet =
      BuildRtpPacket(kPayload, kMarkerBit, start_time_ms,
                     /*capture_time_ms=*/start_time_ms);
  packet->set_allow_retransmission(true);
  packet_history_->PutRtpPacket(std::move(packet), start_time_ms);

  // Advance time, request an RTX retransmission. Capture timestamp should be
  // preserved.
  time_controller_.AdvanceTime(TimeDelta::Millis(10));

  EXPECT_CALL(mock_paced_sender_,
              EnqueuePackets(ElementsAre(Pointee(Property(
                  &RtpPacketToSend::capture_time_ms, start_time_ms)))));
  EXPECT_GT(rtp_sender_->ReSendPacket(kSeqNum), 0);
}

TEST_F(RtpSenderTest, ClearHistoryOnSequenceNumberCange) {
  EnableRtx();

  // Put a packet in the packet history.
  const int64_t now_ms = clock_->TimeInMilliseconds();
  std::unique_ptr<RtpPacketToSend> packet =
      BuildRtpPacket(kPayload, kMarkerBit, now_ms, now_ms);
  packet->set_allow_retransmission(true);
  packet_history_->PutRtpPacket(std::move(packet), now_ms);

  EXPECT_TRUE(packet_history_->GetPacketState(kSeqNum));

  // Update the sequence number of the RTP module, verify packet has been
  // removed.
  rtp_sender_->SetSequenceNumber(rtp_sender_->SequenceNumber() - 1);
  EXPECT_FALSE(packet_history_->GetPacketState(kSeqNum));
}

TEST_F(RtpSenderTest, IgnoresNackAfterDisablingMedia) {
  const int64_t kRtt = 10;

  EnableRtx();
  packet_history_->SetRtt(kRtt);

  // Put a packet in the history.
  const int64_t start_time_ms = clock_->TimeInMilliseconds();
  std::unique_ptr<RtpPacketToSend> packet =
      BuildRtpPacket(kPayload, kMarkerBit, start_time_ms,
                     /*capture_time_ms=*/start_time_ms);
  packet->set_allow_retransmission(true);
  packet_history_->PutRtpPacket(std::move(packet), start_time_ms);

  // Disable media sending and try to retransmit the packet, it should fail.
  rtp_sender_->SetSendingMediaStatus(false);
  time_controller_.AdvanceTime(TimeDelta::Millis(kRtt));
  EXPECT_LT(rtp_sender_->ReSendPacket(kSeqNum), 0);
}

TEST_F(RtpSenderTest, DoesntFecProtectRetransmissions) {
  // Set up retranmission without RTX, so that a plain copy of the old packet is
  // re-sent instead.
  const int64_t kRtt = 10;
  rtp_sender_->SetSendingMediaStatus(true);
  rtp_sender_->SetRtxStatus(kRtxOff);
  packet_history_->SetStorePacketsStatus(
      RtpPacketHistory::StorageMode::kStoreAndCull, 10);
  packet_history_->SetRtt(kRtt);

  // Put a fec protected packet in the history.
  const int64_t start_time_ms = clock_->TimeInMilliseconds();
  std::unique_ptr<RtpPacketToSend> packet =
      BuildRtpPacket(kPayload, kMarkerBit, start_time_ms,
                     /*capture_time_ms=*/start_time_ms);
  packet->set_allow_retransmission(true);
  packet->set_fec_protect_packet(true);
  packet_history_->PutRtpPacket(std::move(packet), start_time_ms);

  // Re-send packet, the retransmitted packet should not have the FEC protection
  // flag set.
  EXPECT_CALL(mock_paced_sender_,
              EnqueuePackets(ElementsAre(Pointee(
                  Property(&RtpPacketToSend::fec_protect_packet, false)))));

  time_controller_.AdvanceTime(TimeDelta::Millis(kRtt));
  EXPECT_GT(rtp_sender_->ReSendPacket(kSeqNum), 0);
}

TEST_F(RtpSenderTest, MarksPacketsWithKeyframeStatus) {
  FieldTrialBasedConfig field_trials;
  RTPSenderVideo::Config video_config;
  video_config.clock = clock_;
  video_config.rtp_sender = rtp_sender_.get();
  video_config.field_trials = &field_trials;
  RTPSenderVideo rtp_sender_video(video_config);

  const uint8_t kPayloadType = 127;
  const absl::optional<VideoCodecType> kCodecType =
      VideoCodecType::kVideoCodecGeneric;

  const uint32_t kCaptureTimeMsToRtpTimestamp = 90;  // 90 kHz clock

  {
    EXPECT_CALL(mock_paced_sender_,
                EnqueuePackets(Each(
                    Pointee(Property(&RtpPacketToSend::is_key_frame, true)))))
        .Times(AtLeast(1));
    RTPVideoHeader video_header;
    video_header.frame_type = VideoFrameType::kVideoFrameKey;
    int64_t capture_time_ms = clock_->TimeInMilliseconds();
    EXPECT_TRUE(rtp_sender_video.SendVideo(
        kPayloadType, kCodecType,
        capture_time_ms * kCaptureTimeMsToRtpTimestamp, capture_time_ms,
        kPayloadData, video_header, kDefaultExpectedRetransmissionTimeMs));

    time_controller_.AdvanceTime(TimeDelta::Millis(33));
  }

  {
    EXPECT_CALL(mock_paced_sender_,
                EnqueuePackets(Each(
                    Pointee(Property(&RtpPacketToSend::is_key_frame, false)))))
        .Times(AtLeast(1));
    RTPVideoHeader video_header;
    video_header.frame_type = VideoFrameType::kVideoFrameDelta;
    int64_t capture_time_ms = clock_->TimeInMilliseconds();
    EXPECT_TRUE(rtp_sender_video.SendVideo(
        kPayloadType, kCodecType,
        capture_time_ms * kCaptureTimeMsToRtpTimestamp, capture_time_ms,
        kPayloadData, video_header, kDefaultExpectedRetransmissionTimeMs));

    time_controller_.AdvanceTime(TimeDelta::Millis(33));
  }
}

}  // namespace webrtc
