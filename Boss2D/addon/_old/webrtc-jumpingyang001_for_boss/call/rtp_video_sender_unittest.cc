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
#include <string>

#include BOSS_WEBRTC_U_call__rtp_transport_controller_send_h //original-code:"call/rtp_transport_controller_send.h"
#include BOSS_WEBRTC_U_call__rtp_video_sender_h //original-code:"call/rtp_video_sender.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"
#include BOSS_WEBRTC_U_rtc_base__rate_limiter_h //original-code:"rtc_base/rate_limiter.h"
#include "test/field_trial.h"
#include "test/gmock.h"
#include "test/gtest.h"
#include "test/mock_transport.h"
#include BOSS_WEBRTC_U_video__call_stats_h //original-code:"video/call_stats.h"
#include BOSS_WEBRTC_U_video__send_delay_stats_h //original-code:"video/send_delay_stats.h"
#include BOSS_WEBRTC_U_video__send_statistics_proxy_h //original-code:"video/send_statistics_proxy.h"

using ::testing::_;
using ::testing::AnyNumber;
using ::testing::Invoke;
using ::testing::NiceMock;
using ::testing::Return;
using ::testing::Unused;

namespace webrtc {
namespace {
const int8_t kPayloadType = 96;
const uint32_t kSsrc1 = 12345;
const uint32_t kSsrc2 = 23456;
const int16_t kInitialPictureId1 = 222;
const int16_t kInitialPictureId2 = 44;
const int16_t kInitialTl0PicIdx1 = 99;
const int16_t kInitialTl0PicIdx2 = 199;
const int64_t kRetransmitWindowSizeMs = 500;

class MockRtcpIntraFrameObserver : public RtcpIntraFrameObserver {
 public:
  MOCK_METHOD1(OnReceivedIntraFrameRequest, void(uint32_t));
};

class MockOverheadObserver : public OverheadObserver {
 public:
  MOCK_METHOD1(OnOverheadChanged, void(size_t overhead_bytes_per_packet));
};

class MockCongestionObserver : public NetworkChangedObserver {
 public:
  MOCK_METHOD4(OnNetworkChanged,
               void(uint32_t bitrate_bps,
                    uint8_t fraction_loss,
                    int64_t rtt_ms,
                    int64_t probing_interval_ms));
};

RtpSenderObservers CreateObservers(
    RtcpRttStats* rtcp_rtt_stats,
    RtcpIntraFrameObserver* intra_frame_callback,
    RtcpStatisticsCallback* rtcp_stats,
    StreamDataCountersCallback* rtp_stats,
    BitrateStatisticsObserver* bitrate_observer,
    FrameCountObserver* frame_count_observer,
    RtcpPacketTypeCounterObserver* rtcp_type_observer,
    SendSideDelayObserver* send_delay_observer,
    SendPacketObserver* send_packet_observer,
    OverheadObserver* overhead_observer) {
  RtpSenderObservers observers;
  observers.rtcp_rtt_stats = rtcp_rtt_stats;
  observers.intra_frame_callback = intra_frame_callback;
  observers.rtcp_stats = rtcp_stats;
  observers.rtp_stats = rtp_stats;
  observers.bitrate_observer = bitrate_observer;
  observers.frame_count_observer = frame_count_observer;
  observers.rtcp_type_observer = rtcp_type_observer;
  observers.send_delay_observer = send_delay_observer;
  observers.send_packet_observer = send_packet_observer;
  observers.overhead_observer = overhead_observer;
  return observers;
}

class RtpVideoSenderTestFixture {
 public:
  RtpVideoSenderTestFixture(
      const std::vector<uint32_t>& ssrcs,
      int payload_type,
      const std::map<uint32_t, RtpPayloadState>& suspended_payload_states)
      : clock_(0),
        config_(&transport_),
        send_delay_stats_(&clock_),
        transport_controller_(&clock_, &event_log_, nullptr, bitrate_config_),
        process_thread_(ProcessThread::Create("test_thread")),
        call_stats_(&clock_, process_thread_.get()),
        stats_proxy_(&clock_,
                     config_,
                     VideoEncoderConfig::ContentType::kRealtimeVideo),
        retransmission_rate_limiter_(&clock_, kRetransmitWindowSizeMs) {
    for (uint32_t ssrc : ssrcs) {
      config_.rtp.ssrcs.push_back(ssrc);
    }
    config_.rtp.payload_type = payload_type;
    std::map<uint32_t, RtpState> suspended_ssrcs;
    router_ = absl::make_unique<RtpVideoSender>(
        config_.rtp.ssrcs, suspended_ssrcs, suspended_payload_states,
        config_.rtp, config_.rtcp, &transport_,
        CreateObservers(&call_stats_, &encoder_feedback_, &stats_proxy_,
                        &stats_proxy_, &stats_proxy_, &stats_proxy_,
                        &stats_proxy_, &stats_proxy_, &send_delay_stats_,
                        &overhead_observer_),
        &transport_controller_, &event_log_, &retransmission_rate_limiter_);
  }

  RtpVideoSender* router() { return router_.get(); }

 private:
  NiceMock<MockTransport> transport_;
  NiceMock<MockCongestionObserver> congestion_observer_;
  NiceMock<MockOverheadObserver> overhead_observer_;
  NiceMock<MockRtcpIntraFrameObserver> encoder_feedback_;
  SimulatedClock clock_;
  RtcEventLogNullImpl event_log_;
  VideoSendStream::Config config_;
  SendDelayStats send_delay_stats_;
  BitrateConstraints bitrate_config_;
  RtpTransportControllerSend transport_controller_;
  std::unique_ptr<ProcessThread> process_thread_;
  CallStats call_stats_;
  SendStatisticsProxy stats_proxy_;
  RateLimiter retransmission_rate_limiter_;
  std::unique_ptr<RtpVideoSender> router_;
};
}  // namespace

TEST(RtpVideoSenderTest, SendOnOneModule) {
  uint8_t payload = 'a';
  EncodedImage encoded_image;
  encoded_image._timeStamp = 1;
  encoded_image.capture_time_ms_ = 2;
  encoded_image._frameType = kVideoFrameKey;
  encoded_image._buffer = &payload;
  encoded_image._length = 1;

  RtpVideoSenderTestFixture test({kSsrc1}, kPayloadType, {});
  EXPECT_NE(
      EncodedImageCallback::Result::OK,
      test.router()->OnEncodedImage(encoded_image, nullptr, nullptr).error);

  test.router()->SetActive(true);
  EXPECT_EQ(
      EncodedImageCallback::Result::OK,
      test.router()->OnEncodedImage(encoded_image, nullptr, nullptr).error);

  test.router()->SetActive(false);
  EXPECT_NE(
      EncodedImageCallback::Result::OK,
      test.router()->OnEncodedImage(encoded_image, nullptr, nullptr).error);

  test.router()->SetActive(true);
  EXPECT_EQ(
      EncodedImageCallback::Result::OK,
      test.router()->OnEncodedImage(encoded_image, nullptr, nullptr).error);
}

TEST(RtpVideoSenderTest, SendSimulcastSetActive) {
  uint8_t payload = 'a';
  EncodedImage encoded_image;
  encoded_image._timeStamp = 1;
  encoded_image.capture_time_ms_ = 2;
  encoded_image._frameType = kVideoFrameKey;
  encoded_image._buffer = &payload;
  encoded_image._length = 1;

  RtpVideoSenderTestFixture test({kSsrc1, kSsrc2}, kPayloadType, {});

  CodecSpecificInfo codec_info_1;
  memset(&codec_info_1, 0, sizeof(CodecSpecificInfo));
  codec_info_1.codecType = kVideoCodecVP8;
  codec_info_1.codecSpecific.VP8.simulcastIdx = 0;

  test.router()->SetActive(true);
  EXPECT_EQ(EncodedImageCallback::Result::OK,
            test.router()
                ->OnEncodedImage(encoded_image, &codec_info_1, nullptr)
                .error);

  CodecSpecificInfo codec_info_2;
  memset(&codec_info_2, 0, sizeof(CodecSpecificInfo));
  codec_info_2.codecType = kVideoCodecVP8;
  codec_info_2.codecSpecific.VP8.simulcastIdx = 1;
  EXPECT_EQ(EncodedImageCallback::Result::OK,
            test.router()
                ->OnEncodedImage(encoded_image, &codec_info_2, nullptr)
                .error);

  // Inactive.
  test.router()->SetActive(false);
  EXPECT_NE(EncodedImageCallback::Result::OK,
            test.router()
                ->OnEncodedImage(encoded_image, &codec_info_1, nullptr)
                .error);
  EXPECT_NE(EncodedImageCallback::Result::OK,
            test.router()
                ->OnEncodedImage(encoded_image, &codec_info_2, nullptr)
                .error);
}

// Tests how setting individual rtp modules to active affects the overall
// behavior of the payload router. First sets one module to active and checks
// that outgoing data can be sent on this module, and checks that no data can
// be sent if both modules are inactive.
TEST(RtpVideoSenderTest, SendSimulcastSetActiveModules) {
  uint8_t payload = 'a';
  EncodedImage encoded_image;
  encoded_image._timeStamp = 1;
  encoded_image.capture_time_ms_ = 2;
  encoded_image._frameType = kVideoFrameKey;
  encoded_image._buffer = &payload;
  encoded_image._length = 1;

  RtpVideoSenderTestFixture test({kSsrc1, kSsrc2}, kPayloadType, {});
  CodecSpecificInfo codec_info_1;
  memset(&codec_info_1, 0, sizeof(CodecSpecificInfo));
  codec_info_1.codecType = kVideoCodecVP8;
  codec_info_1.codecSpecific.VP8.simulcastIdx = 0;
  CodecSpecificInfo codec_info_2;
  memset(&codec_info_2, 0, sizeof(CodecSpecificInfo));
  codec_info_2.codecType = kVideoCodecVP8;
  codec_info_2.codecSpecific.VP8.simulcastIdx = 1;

  // Only setting one stream to active will still set the payload router to
  // active and allow sending data on the active stream.
  std::vector<bool> active_modules({true, false});
  test.router()->SetActiveModules(active_modules);
  EXPECT_EQ(EncodedImageCallback::Result::OK,
            test.router()
                ->OnEncodedImage(encoded_image, &codec_info_1, nullptr)
                .error);

  // Setting both streams to inactive will turn the payload router to
  // inactive.
  active_modules = {false, false};
  test.router()->SetActiveModules(active_modules);
  // An incoming encoded image will not ask the module to send outgoing data
  // because the payload router is inactive.
  EXPECT_NE(EncodedImageCallback::Result::OK,
            test.router()
                ->OnEncodedImage(encoded_image, &codec_info_1, nullptr)
                .error);
  EXPECT_NE(EncodedImageCallback::Result::OK,
            test.router()
                ->OnEncodedImage(encoded_image, &codec_info_2, nullptr)
                .error);
}

TEST(RtpVideoSenderTest, CreateWithNoPreviousStates) {
  RtpVideoSenderTestFixture test({kSsrc1, kSsrc2}, kPayloadType, {});
  test.router()->SetActive(true);

  std::map<uint32_t, RtpPayloadState> initial_states =
      test.router()->GetRtpPayloadStates();
  EXPECT_EQ(2u, initial_states.size());
  EXPECT_NE(initial_states.find(kSsrc1), initial_states.end());
  EXPECT_NE(initial_states.find(kSsrc2), initial_states.end());
}

TEST(RtpVideoSenderTest, CreateWithPreviousStates) {
  const int64_t kState1SharedFrameId = 123;
  const int64_t kState2SharedFrameId = 234;
  RtpPayloadState state1;
  state1.picture_id = kInitialPictureId1;
  state1.tl0_pic_idx = kInitialTl0PicIdx1;
  state1.shared_frame_id = kState1SharedFrameId;
  RtpPayloadState state2;
  state2.picture_id = kInitialPictureId2;
  state2.tl0_pic_idx = kInitialTl0PicIdx2;
  state2.shared_frame_id = kState2SharedFrameId;
  std::map<uint32_t, RtpPayloadState> states = {{kSsrc1, state1},
                                                {kSsrc2, state2}};

  RtpVideoSenderTestFixture test({kSsrc1, kSsrc2}, kPayloadType, states);
  test.router()->SetActive(true);

  std::map<uint32_t, RtpPayloadState> initial_states =
      test.router()->GetRtpPayloadStates();
  EXPECT_EQ(2u, initial_states.size());
  EXPECT_EQ(kInitialPictureId1, initial_states[kSsrc1].picture_id);
  EXPECT_EQ(kInitialTl0PicIdx1, initial_states[kSsrc1].tl0_pic_idx);
  EXPECT_EQ(kInitialPictureId2, initial_states[kSsrc2].picture_id);
  EXPECT_EQ(kInitialTl0PicIdx2, initial_states[kSsrc2].tl0_pic_idx);
  EXPECT_EQ(kState2SharedFrameId, initial_states[kSsrc1].shared_frame_id);
  EXPECT_EQ(kState2SharedFrameId, initial_states[kSsrc2].shared_frame_id);
}
}  // namespace webrtc
