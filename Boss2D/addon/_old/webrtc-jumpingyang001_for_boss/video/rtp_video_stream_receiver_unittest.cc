/*
 *  Copyright 2017 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "test/gmock.h"
#include "test/gtest.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_common_video__h264__h264_common_h //original-code:"common_video/h264/h264_common.h"
#include BOSS_WEBRTC_U_media__base__mediaconstants_h //original-code:"media/base/mediaconstants.h"
#include BOSS_WEBRTC_U_modules__pacing__packet_router_h //original-code:"modules/pacing/packet_router.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_received_h //original-code:"modules/rtp_rtcp/source/rtp_packet_received.h"
#include BOSS_WEBRTC_U_modules__utility__include__process_thread_h //original-code:"modules/utility/include/process_thread.h"
#include BOSS_WEBRTC_U_modules__video_coding__frame_object_h //original-code:"modules/video_coding/frame_object.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_coding_defines_h //original-code:"modules/video_coding/include/video_coding_defines.h"
#include BOSS_WEBRTC_U_modules__video_coding__packet_h //original-code:"modules/video_coding/packet.h"
#include BOSS_WEBRTC_U_modules__video_coding__rtp_frame_reference_finder_h //original-code:"modules/video_coding/rtp_frame_reference_finder.h"
#include BOSS_WEBRTC_U_rtc_base__bytebuffer_h //original-code:"rtc_base/bytebuffer.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_default_h //original-code:"system_wrappers/include/field_trial_default.h"
#include "test/field_trial.h"
#include BOSS_WEBRTC_U_video__rtp_video_stream_receiver_h //original-code:"video/rtp_video_stream_receiver.h"

using testing::_;

namespace webrtc {

namespace {

const uint8_t kH264StartCode[] = {0x00, 0x00, 0x00, 0x01};

class MockTransport : public Transport {
 public:
  MOCK_METHOD3(SendRtp,
               bool(const uint8_t* packet,
                    size_t length,
                    const PacketOptions& options));
  MOCK_METHOD2(SendRtcp, bool(const uint8_t* packet, size_t length));
};

class MockNackSender : public NackSender {
 public:
  MOCK_METHOD1(SendNack, void(const std::vector<uint16_t>& sequence_numbers));
};

class MockKeyFrameRequestSender : public KeyFrameRequestSender {
 public:
  MOCK_METHOD0(RequestKeyFrame, void());
};

class MockOnCompleteFrameCallback
    : public video_coding::OnCompleteFrameCallback {
 public:
  MockOnCompleteFrameCallback() : buffer_(rtc::ByteBuffer::ORDER_NETWORK) {}

  MOCK_METHOD1(DoOnCompleteFrame, void(video_coding::EncodedFrame* frame));
  MOCK_METHOD1(DoOnCompleteFrameFailNullptr,
               void(video_coding::EncodedFrame* frame));
  MOCK_METHOD1(DoOnCompleteFrameFailLength,
               void(video_coding::EncodedFrame* frame));
  MOCK_METHOD1(DoOnCompleteFrameFailBitstream,
               void(video_coding::EncodedFrame* frame));
  void OnCompleteFrame(std::unique_ptr<video_coding::EncodedFrame> frame) {
    if (!frame) {
      DoOnCompleteFrameFailNullptr(nullptr);
      return;
    }
    EXPECT_EQ(buffer_.Length(), frame->size());
    if (buffer_.Length() != frame->size()) {
      DoOnCompleteFrameFailLength(frame.get());
      return;
    }
    std::vector<uint8_t> actual_data(frame->size());
    frame->GetBitstream(actual_data.data());
    if (memcmp(buffer_.Data(), actual_data.data(), buffer_.Length()) != 0) {
      DoOnCompleteFrameFailBitstream(frame.get());
      return;
    }
    DoOnCompleteFrame(frame.get());
  }
  void AppendExpectedBitstream(const uint8_t data[], size_t size_in_bytes) {
    // TODO(Johan): Let rtc::ByteBuffer handle uint8_t* instead of char*.
    buffer_.WriteBytes(reinterpret_cast<const char*>(data), size_in_bytes);
  }
  rtc::ByteBufferWriter buffer_;
};

class MockRtpPacketSink : public RtpPacketSinkInterface {
 public:
  MOCK_METHOD1(OnRtpPacket, void(const RtpPacketReceived&));
};

constexpr uint32_t kSsrc = 111;
constexpr uint16_t kSequenceNumber = 222;
std::unique_ptr<RtpPacketReceived> CreateRtpPacketReceived(
    uint32_t ssrc = kSsrc,
    uint16_t sequence_number = kSequenceNumber) {
  auto packet = absl::make_unique<RtpPacketReceived>();
  packet->SetSsrc(ssrc);
  packet->SetSequenceNumber(sequence_number);
  return packet;
}

MATCHER_P(SamePacketAs, other, "") {
  return arg.Ssrc() == other.Ssrc() &&
         arg.SequenceNumber() == other.SequenceNumber();
}

}  // namespace

class RtpVideoStreamReceiverTest : public testing::Test {
 public:
  RtpVideoStreamReceiverTest() : RtpVideoStreamReceiverTest("") {}
  explicit RtpVideoStreamReceiverTest(std::string field_trials)
      : override_field_trials_(field_trials),
        config_(CreateConfig()),
        process_thread_(ProcessThread::Create("TestThread")) {}

  void SetUp() {
    rtp_receive_statistics_ =
        absl::WrapUnique(ReceiveStatistics::Create(Clock::GetRealTimeClock()));
    rtp_video_stream_receiver_ = absl::make_unique<RtpVideoStreamReceiver>(
        &mock_transport_, nullptr, &packet_router_, &config_,
        rtp_receive_statistics_.get(), nullptr, process_thread_.get(),
        &mock_nack_sender_, &mock_key_frame_request_sender_,
        &mock_on_complete_frame_callback_);
  }

  WebRtcRTPHeader GetDefaultPacket() {
    WebRtcRTPHeader packet = {};
    packet.video_header().codec = kVideoCodecH264;
    packet.video_header().video_type_header.emplace<RTPVideoHeaderH264>();
    return packet;
  }

  // TODO(Johan): refactor h264_sps_pps_tracker_unittests.cc to avoid duplicate
  // code.
  void AddSps(WebRtcRTPHeader* packet,
              uint8_t sps_id,
              std::vector<uint8_t>* data) {
    NaluInfo info;
    info.type = H264::NaluType::kSps;
    info.sps_id = sps_id;
    info.pps_id = -1;
    data->push_back(H264::NaluType::kSps);
    data->push_back(sps_id);
    auto& h264 =
        absl::get<RTPVideoHeaderH264>(packet->video_header().video_type_header);
    h264.nalus[h264.nalus_length++] = info;
  }

  void AddPps(WebRtcRTPHeader* packet,
              uint8_t sps_id,
              uint8_t pps_id,
              std::vector<uint8_t>* data) {
    NaluInfo info;
    info.type = H264::NaluType::kPps;
    info.sps_id = sps_id;
    info.pps_id = pps_id;
    data->push_back(H264::NaluType::kPps);
    data->push_back(pps_id);
    auto& h264 =
        absl::get<RTPVideoHeaderH264>(packet->video_header().video_type_header);
    h264.nalus[h264.nalus_length++] = info;
  }

  void AddIdr(WebRtcRTPHeader* packet, int pps_id) {
    NaluInfo info;
    info.type = H264::NaluType::kIdr;
    info.sps_id = -1;
    info.pps_id = pps_id;
    auto& h264 =
        absl::get<RTPVideoHeaderH264>(packet->video_header().video_type_header);
    h264.nalus[h264.nalus_length++] = info;
  }

 protected:
  static VideoReceiveStream::Config CreateConfig() {
    VideoReceiveStream::Config config(nullptr);
    config.rtp.remote_ssrc = 1111;
    config.rtp.local_ssrc = 2222;
    return config;
  }

  const webrtc::test::ScopedFieldTrials override_field_trials_;
  VideoReceiveStream::Config config_;
  MockNackSender mock_nack_sender_;
  MockKeyFrameRequestSender mock_key_frame_request_sender_;
  MockTransport mock_transport_;
  MockOnCompleteFrameCallback mock_on_complete_frame_callback_;
  PacketRouter packet_router_;
  std::unique_ptr<ProcessThread> process_thread_;
  std::unique_ptr<ReceiveStatistics> rtp_receive_statistics_;
  std::unique_ptr<RtpVideoStreamReceiver> rtp_video_stream_receiver_;
};

TEST_F(RtpVideoStreamReceiverTest, GenericKeyFrame) {
  WebRtcRTPHeader rtp_header = {};
  const std::vector<uint8_t> data({1, 2, 3, 4});
  rtp_header.header.sequenceNumber = 1;
  rtp_header.header.markerBit = 1;
  rtp_header.video_header().is_first_packet_in_frame = true;
  rtp_header.frameType = kVideoFrameKey;
  rtp_header.video_header().codec = kVideoCodecGeneric;
  mock_on_complete_frame_callback_.AppendExpectedBitstream(data.data(),
                                                           data.size());
  EXPECT_CALL(mock_on_complete_frame_callback_, DoOnCompleteFrame(_));
  rtp_video_stream_receiver_->OnReceivedPayloadData(data.data(), data.size(),
                                                    &rtp_header);
}

TEST_F(RtpVideoStreamReceiverTest, NoInfiniteRecursionOnEncapsulatedRedPacket) {
  const uint8_t kRedPayloadType = 125;
  VideoCodec codec;
  codec.plType = kRedPayloadType;
  rtp_video_stream_receiver_->AddReceiveCodec(codec, {});
  const std::vector<uint8_t> data({
      0x80,              // RTP version.
      kRedPayloadType,   // Payload type.
      0, 0, 0, 0, 0, 0,  // Don't care.
      0, 0, 0x4, 0x57,   // SSRC
      kRedPayloadType,   // RED header.
      0, 0, 0, 0, 0      // Don't care.
  });
  RtpPacketReceived packet;
  EXPECT_TRUE(packet.Parse(data.data(), data.size()));
  rtp_video_stream_receiver_->StartReceive();
  rtp_video_stream_receiver_->OnRtpPacket(packet);
}

TEST_F(RtpVideoStreamReceiverTest,
       DropsPacketWithRedPayloadTypeAndEmptyPayload) {
  const uint8_t kRedPayloadType = 125;
  config_.rtp.red_payload_type = kRedPayloadType;
  SetUp();  // re-create rtp_video_stream_receiver with red payload type.
  // clang-format off
  const uint8_t data[] = {
      0x80,              // RTP version.
      kRedPayloadType,   // Payload type.
      0, 0, 0, 0, 0, 0,  // Don't care.
      0, 0, 0x4, 0x57,   // SSRC
      // Empty rtp payload.
  };
  // clang-format on
  RtpPacketReceived packet;
  // Manually convert to CopyOnWriteBuffer to be sure capacity == size
  // and asan bot can catch read buffer overflow.
  EXPECT_TRUE(packet.Parse(rtc::CopyOnWriteBuffer(data)));
  rtp_video_stream_receiver_->StartReceive();
  rtp_video_stream_receiver_->OnRtpPacket(packet);
  // Expect asan doesn't find anything.
}

TEST_F(RtpVideoStreamReceiverTest, GenericKeyFrameBitstreamError) {
  WebRtcRTPHeader rtp_header = {};
  const std::vector<uint8_t> data({1, 2, 3, 4});
  rtp_header.header.sequenceNumber = 1;
  rtp_header.header.markerBit = 1;
  rtp_header.video_header().is_first_packet_in_frame = true;
  rtp_header.frameType = kVideoFrameKey;
  rtp_header.video_header().codec = kVideoCodecGeneric;
  constexpr uint8_t expected_bitsteam[] = {1, 2, 3, 0xff};
  mock_on_complete_frame_callback_.AppendExpectedBitstream(
      expected_bitsteam, sizeof(expected_bitsteam));
  EXPECT_CALL(mock_on_complete_frame_callback_,
              DoOnCompleteFrameFailBitstream(_));
  rtp_video_stream_receiver_->OnReceivedPayloadData(data.data(), data.size(),
                                                    &rtp_header);
}

class RtpVideoStreamReceiverTestH264
    : public RtpVideoStreamReceiverTest,
      public testing::WithParamInterface<std::string> {
 protected:
  RtpVideoStreamReceiverTestH264() : RtpVideoStreamReceiverTest(GetParam()) {}
};

INSTANTIATE_TEST_CASE_P(
    SpsPpsIdrIsKeyframe,
    RtpVideoStreamReceiverTestH264,
    ::testing::Values("", "WebRTC-SpsPpsIdrIsH264Keyframe/Enabled/"));

TEST_P(RtpVideoStreamReceiverTestH264, InBandSpsPps) {
  std::vector<uint8_t> sps_data;
  WebRtcRTPHeader sps_packet = GetDefaultPacket();
  AddSps(&sps_packet, 0, &sps_data);
  sps_packet.header.sequenceNumber = 0;
  sps_packet.video_header().is_first_packet_in_frame = true;
  mock_on_complete_frame_callback_.AppendExpectedBitstream(
      kH264StartCode, sizeof(kH264StartCode));
  mock_on_complete_frame_callback_.AppendExpectedBitstream(sps_data.data(),
                                                           sps_data.size());
  rtp_video_stream_receiver_->OnReceivedPayloadData(
      sps_data.data(), sps_data.size(), &sps_packet);

  std::vector<uint8_t> pps_data;
  WebRtcRTPHeader pps_packet = GetDefaultPacket();
  AddPps(&pps_packet, 0, 1, &pps_data);
  pps_packet.header.sequenceNumber = 1;
  pps_packet.video_header().is_first_packet_in_frame = true;
  mock_on_complete_frame_callback_.AppendExpectedBitstream(
      kH264StartCode, sizeof(kH264StartCode));
  mock_on_complete_frame_callback_.AppendExpectedBitstream(pps_data.data(),
                                                           pps_data.size());
  rtp_video_stream_receiver_->OnReceivedPayloadData(
      pps_data.data(), pps_data.size(), &pps_packet);

  std::vector<uint8_t> idr_data;
  WebRtcRTPHeader idr_packet = GetDefaultPacket();
  AddIdr(&idr_packet, 1);
  idr_packet.video_header().is_first_packet_in_frame = true;
  idr_packet.header.sequenceNumber = 2;
  idr_packet.header.markerBit = 1;
  idr_packet.frameType = kVideoFrameKey;
  idr_data.insert(idr_data.end(), {0x65, 1, 2, 3});
  mock_on_complete_frame_callback_.AppendExpectedBitstream(
      kH264StartCode, sizeof(kH264StartCode));
  mock_on_complete_frame_callback_.AppendExpectedBitstream(idr_data.data(),
                                                           idr_data.size());
  EXPECT_CALL(mock_on_complete_frame_callback_, DoOnCompleteFrame(_));
  rtp_video_stream_receiver_->OnReceivedPayloadData(
      idr_data.data(), idr_data.size(), &idr_packet);
}

TEST_P(RtpVideoStreamReceiverTestH264, OutOfBandFmtpSpsPps) {
  constexpr int kPayloadType = 99;
  VideoCodec codec;
  codec.plType = kPayloadType;
  std::map<std::string, std::string> codec_params;
  // Example parameter sets from https://tools.ietf.org/html/rfc3984#section-8.2
  // .
  codec_params.insert(
      {cricket::kH264FmtpSpropParameterSets, "Z0IACpZTBYmI,aMljiA=="});
  rtp_video_stream_receiver_->AddReceiveCodec(codec, codec_params);
  const uint8_t binary_sps[] = {0x67, 0x42, 0x00, 0x0a, 0x96,
                                0x53, 0x05, 0x89, 0x88};
  mock_on_complete_frame_callback_.AppendExpectedBitstream(
      kH264StartCode, sizeof(kH264StartCode));
  mock_on_complete_frame_callback_.AppendExpectedBitstream(binary_sps,
                                                           sizeof(binary_sps));
  const uint8_t binary_pps[] = {0x68, 0xc9, 0x63, 0x88};
  mock_on_complete_frame_callback_.AppendExpectedBitstream(
      kH264StartCode, sizeof(kH264StartCode));
  mock_on_complete_frame_callback_.AppendExpectedBitstream(binary_pps,
                                                           sizeof(binary_pps));

  std::vector<uint8_t> data;
  WebRtcRTPHeader idr_packet = GetDefaultPacket();
  AddIdr(&idr_packet, 0);
  idr_packet.header.payloadType = kPayloadType;
  idr_packet.video_header().is_first_packet_in_frame = true;
  idr_packet.header.sequenceNumber = 2;
  idr_packet.header.markerBit = 1;
  idr_packet.video_header().is_first_packet_in_frame = true;
  idr_packet.frameType = kVideoFrameKey;
  idr_packet.video_header().codec = kVideoCodecH264;
  data.insert(data.end(), {1, 2, 3});
  mock_on_complete_frame_callback_.AppendExpectedBitstream(
      kH264StartCode, sizeof(kH264StartCode));
  mock_on_complete_frame_callback_.AppendExpectedBitstream(data.data(),
                                                           data.size());
  EXPECT_CALL(mock_on_complete_frame_callback_, DoOnCompleteFrame(_));
  rtp_video_stream_receiver_->OnReceivedPayloadData(data.data(), data.size(),
                                                    &idr_packet);
}

TEST_F(RtpVideoStreamReceiverTest, PaddingInMediaStream) {
  WebRtcRTPHeader header = GetDefaultPacket();
  std::vector<uint8_t> data;
  data.insert(data.end(), {1, 2, 3});
  header.header.payloadType = 99;
  header.video_header().is_first_packet_in_frame = true;
  header.header.sequenceNumber = 2;
  header.header.markerBit = true;
  header.frameType = kVideoFrameKey;
  header.video_header().codec = kVideoCodecGeneric;
  mock_on_complete_frame_callback_.AppendExpectedBitstream(data.data(),
                                                           data.size());

  EXPECT_CALL(mock_on_complete_frame_callback_, DoOnCompleteFrame(_));
  rtp_video_stream_receiver_->OnReceivedPayloadData(data.data(), data.size(),
                                                    &header);

  header.header.sequenceNumber = 3;
  rtp_video_stream_receiver_->OnReceivedPayloadData(nullptr, 0, &header);

  header.frameType = kVideoFrameDelta;
  header.header.sequenceNumber = 4;
  EXPECT_CALL(mock_on_complete_frame_callback_, DoOnCompleteFrame(_));
  rtp_video_stream_receiver_->OnReceivedPayloadData(data.data(), data.size(),
                                                    &header);

  header.header.sequenceNumber = 6;
  rtp_video_stream_receiver_->OnReceivedPayloadData(data.data(), data.size(),
                                                    &header);

  EXPECT_CALL(mock_on_complete_frame_callback_, DoOnCompleteFrame(_));
  header.header.sequenceNumber = 5;
  rtp_video_stream_receiver_->OnReceivedPayloadData(nullptr, 0, &header);
}

TEST_F(RtpVideoStreamReceiverTest, RequestKeyframeIfFirstFrameIsDelta) {
  WebRtcRTPHeader rtp_header = {};
  const std::vector<uint8_t> data({1, 2, 3, 4});
  rtp_header.header.sequenceNumber = 1;
  rtp_header.header.markerBit = 1;
  rtp_header.video_header().is_first_packet_in_frame = true;
  rtp_header.frameType = kVideoFrameDelta;
  rtp_header.video_header().codec = kVideoCodecGeneric;

  EXPECT_CALL(mock_key_frame_request_sender_, RequestKeyFrame());
  rtp_video_stream_receiver_->OnReceivedPayloadData(data.data(), data.size(),
                                                    &rtp_header);
}

TEST_F(RtpVideoStreamReceiverTest, SecondarySinksGetRtpNotifications) {
  rtp_video_stream_receiver_->StartReceive();

  MockRtpPacketSink secondary_sink_1;
  MockRtpPacketSink secondary_sink_2;

  rtp_video_stream_receiver_->AddSecondarySink(&secondary_sink_1);
  rtp_video_stream_receiver_->AddSecondarySink(&secondary_sink_2);

  auto rtp_packet = CreateRtpPacketReceived();
  EXPECT_CALL(secondary_sink_1, OnRtpPacket(SamePacketAs(*rtp_packet)));
  EXPECT_CALL(secondary_sink_2, OnRtpPacket(SamePacketAs(*rtp_packet)));

  rtp_video_stream_receiver_->OnRtpPacket(*rtp_packet);

  // Test tear-down.
  rtp_video_stream_receiver_->StopReceive();
  rtp_video_stream_receiver_->RemoveSecondarySink(&secondary_sink_1);
  rtp_video_stream_receiver_->RemoveSecondarySink(&secondary_sink_2);
}

TEST_F(RtpVideoStreamReceiverTest, RemovedSecondarySinksGetNoRtpNotifications) {
  rtp_video_stream_receiver_->StartReceive();

  MockRtpPacketSink secondary_sink;

  rtp_video_stream_receiver_->AddSecondarySink(&secondary_sink);
  rtp_video_stream_receiver_->RemoveSecondarySink(&secondary_sink);

  auto rtp_packet = CreateRtpPacketReceived();

  EXPECT_CALL(secondary_sink, OnRtpPacket(_)).Times(0);

  rtp_video_stream_receiver_->OnRtpPacket(*rtp_packet);

  // Test tear-down.
  rtp_video_stream_receiver_->StopReceive();
}

TEST_F(RtpVideoStreamReceiverTest,
       OnlyRemovedSecondarySinksExcludedFromNotifications) {
  rtp_video_stream_receiver_->StartReceive();

  MockRtpPacketSink kept_secondary_sink;
  MockRtpPacketSink removed_secondary_sink;

  rtp_video_stream_receiver_->AddSecondarySink(&kept_secondary_sink);
  rtp_video_stream_receiver_->AddSecondarySink(&removed_secondary_sink);
  rtp_video_stream_receiver_->RemoveSecondarySink(&removed_secondary_sink);

  auto rtp_packet = CreateRtpPacketReceived();
  EXPECT_CALL(kept_secondary_sink, OnRtpPacket(SamePacketAs(*rtp_packet)));

  rtp_video_stream_receiver_->OnRtpPacket(*rtp_packet);

  // Test tear-down.
  rtp_video_stream_receiver_->StopReceive();
  rtp_video_stream_receiver_->RemoveSecondarySink(&kept_secondary_sink);
}

TEST_F(RtpVideoStreamReceiverTest,
       SecondariesOfNonStartedStreamGetNoNotifications) {
  // Explicitly showing that the stream is not in the |started| state,
  // regardless of whether streams start out |started| or |stopped|.
  rtp_video_stream_receiver_->StopReceive();

  MockRtpPacketSink secondary_sink;
  rtp_video_stream_receiver_->AddSecondarySink(&secondary_sink);

  auto rtp_packet = CreateRtpPacketReceived();
  EXPECT_CALL(secondary_sink, OnRtpPacket(_)).Times(0);

  rtp_video_stream_receiver_->OnRtpPacket(*rtp_packet);

  // Test tear-down.
  rtp_video_stream_receiver_->RemoveSecondarySink(&secondary_sink);
}

#if RTC_DCHECK_IS_ON && GTEST_HAS_DEATH_TEST && !defined(WEBRTC_ANDROID)
TEST_F(RtpVideoStreamReceiverTest, RepeatedSecondarySinkDisallowed) {
  MockRtpPacketSink secondary_sink;

  rtp_video_stream_receiver_->AddSecondarySink(&secondary_sink);
  EXPECT_DEATH(rtp_video_stream_receiver_->AddSecondarySink(&secondary_sink),
               "");

  // Test tear-down.
  rtp_video_stream_receiver_->RemoveSecondarySink(&secondary_sink);
}
#endif

}  // namespace webrtc
