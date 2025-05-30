/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>

#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_decoder_factory_h //original-code:"api/audio_codecs/builtin_audio_decoder_factory.h"
#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_encoder_factory_h //original-code:"api/audio_codecs/builtin_audio_encoder_factory.h"
#include BOSS_WEBRTC_U_media__base__fakemediaengine_h //original-code:"media/base/fakemediaengine.h"
#include "ortc/ortcfactory.h"
#include "ortc/testrtpparameters.h"
#include BOSS_WEBRTC_U_p2p__base__fakepackettransport_h //original-code:"p2p/base/fakepackettransport.h"
#include BOSS_WEBRTC_U_rtc_base__fakenetwork_h //original-code:"rtc_base/fakenetwork.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__virtualsocketserver_h //original-code:"rtc_base/virtualsocketserver.h"

namespace webrtc {

// This test uses a virtual network and fake media engine, in order to test the
// OrtcFactory at only an API level. Any end-to-end test should go in
// ortcfactory_integrationtest.cc instead.
class OrtcFactoryTest : public testing::Test {
 public:
  OrtcFactoryTest()
      : thread_(&virtual_socket_server_),
        fake_packet_transport_("fake transport") {
    ortc_factory_ =
        OrtcFactory::Create(&thread_, nullptr, &fake_network_manager_, nullptr,
                            nullptr,
                            std::unique_ptr<cricket::MediaEngineInterface>(
                                new cricket::FakeMediaEngine()),
                            CreateBuiltinAudioEncoderFactory(),
                            CreateBuiltinAudioDecoderFactory())
            .MoveValue();
  }

 protected:
  // Uses a single pre-made FakePacketTransport, so shouldn't be called twice in
  // the same test.
  std::unique_ptr<RtpTransportInterface>
  CreateRtpTransportWithFakePacketTransport() {
    return ortc_factory_
        ->CreateRtpTransport(MakeRtcpMuxParameters(), &fake_packet_transport_,
                             nullptr, nullptr)
        .MoveValue();
  }

  rtc::VirtualSocketServer virtual_socket_server_;
  rtc::AutoSocketServerThread thread_;
  rtc::FakeNetworkManager fake_network_manager_;
  rtc::FakePacketTransport fake_packet_transport_;
  std::unique_ptr<OrtcFactoryInterface> ortc_factory_;
};

TEST_F(OrtcFactoryTest, CanCreateMultipleRtpTransportControllers) {
  auto controller_result1 = ortc_factory_->CreateRtpTransportController();
  EXPECT_TRUE(controller_result1.ok());
  auto controller_result2 = ortc_factory_->CreateRtpTransportController();
  EXPECT_TRUE(controller_result1.ok());
}

// Simple test for the successful cases of CreateRtpTransport.
TEST_F(OrtcFactoryTest, CreateRtpTransportWithAndWithoutMux) {
  rtc::FakePacketTransport rtp("rtp");
  rtc::FakePacketTransport rtcp("rtcp");
  // With muxed RTCP.
  RtpTransportParameters parameters = MakeRtcpMuxParameters();
  auto result =
      ortc_factory_->CreateRtpTransport(parameters, &rtp, nullptr, nullptr);
  EXPECT_TRUE(result.ok());
  result.MoveValue().reset();
  // With non-muxed RTCP.
  parameters.rtcp.mux = false;
  result = ortc_factory_->CreateRtpTransport(parameters, &rtp, &rtcp, nullptr);
  EXPECT_TRUE(result.ok());
}

// Simple test for the successful cases of CreateSrtpTransport.
TEST_F(OrtcFactoryTest, CreateSrtpTransport) {
  rtc::FakePacketTransport rtp("rtp");
  rtc::FakePacketTransport rtcp("rtcp");
  // With muxed RTCP.
  RtpTransportParameters parameters = MakeRtcpMuxParameters();
  auto result =
      ortc_factory_->CreateSrtpTransport(parameters, &rtp, nullptr, nullptr);
  EXPECT_TRUE(result.ok());
  result.MoveValue().reset();
  // With non-muxed RTCP.
  parameters.rtcp.mux = false;
  result = ortc_factory_->CreateSrtpTransport(parameters, &rtp, &rtcp, nullptr);
  EXPECT_TRUE(result.ok());
}

// If no CNAME is provided, one should be generated and returned by
// GetRtpParameters.
TEST_F(OrtcFactoryTest, CreateRtpTransportGeneratesCname) {
  rtc::FakePacketTransport rtp("rtp");
  auto result = ortc_factory_->CreateRtpTransport(MakeRtcpMuxParameters(), &rtp,
                                                  nullptr, nullptr);
  ASSERT_TRUE(result.ok());
  EXPECT_FALSE(result.value()->GetParameters().rtcp.cname.empty());
}

// Extension of the above test; multiple transports created by the same factory
// should use the same generated CNAME.
TEST_F(OrtcFactoryTest, MultipleRtpTransportsUseSameGeneratedCname) {
  rtc::FakePacketTransport packet_transport1("1");
  rtc::FakePacketTransport packet_transport2("2");
  RtpTransportParameters parameters = MakeRtcpMuxParameters();
  // Sanity check.
  ASSERT_TRUE(parameters.rtcp.cname.empty());
  auto result = ortc_factory_->CreateRtpTransport(
      parameters, &packet_transport1, nullptr, nullptr);
  ASSERT_TRUE(result.ok());
  auto rtp_transport1 = result.MoveValue();
  result = ortc_factory_->CreateRtpTransport(parameters, &packet_transport2,
                                             nullptr, nullptr);
  ASSERT_TRUE(result.ok());
  auto rtp_transport2 = result.MoveValue();
  RtcpParameters params1 = rtp_transport1->GetParameters().rtcp;
  RtcpParameters params2 = rtp_transport2->GetParameters().rtcp;
  EXPECT_FALSE(params1.cname.empty());
  EXPECT_EQ(params1.cname, params2.cname);
}

TEST_F(OrtcFactoryTest, CreateRtpTransportWithNoPacketTransport) {
  auto result = ortc_factory_->CreateRtpTransport(MakeRtcpMuxParameters(),
                                                  nullptr, nullptr, nullptr);
  EXPECT_EQ(RTCErrorType::INVALID_PARAMETER, result.error().type());
}

// If the |mux| member of the RtcpParameters is false, both an RTP and RTCP
// packet transport are needed.
TEST_F(OrtcFactoryTest, CreateRtpTransportWithMissingRtcpTransport) {
  rtc::FakePacketTransport rtp("rtp");
  RtpTransportParameters parameters;
  parameters.rtcp.mux = false;
  auto result =
      ortc_factory_->CreateRtpTransport(parameters, &rtp, nullptr, nullptr);
  EXPECT_EQ(RTCErrorType::INVALID_PARAMETER, result.error().type());
}

// If the |mux| member of the RtcpParameters is true, only an RTP packet
// transport is necessary. So, passing in an RTCP transport is most likely
// an accident, and thus should be treated as an error.
TEST_F(OrtcFactoryTest, CreateRtpTransportWithExtraneousRtcpTransport) {
  rtc::FakePacketTransport rtp("rtp");
  rtc::FakePacketTransport rtcp("rtcp");
  auto result = ortc_factory_->CreateRtpTransport(MakeRtcpMuxParameters(), &rtp,
                                                  &rtcp, nullptr);
  EXPECT_EQ(RTCErrorType::INVALID_PARAMETER, result.error().type());
}

// Basic test that CreateUdpTransport works with AF_INET and AF_INET6.
TEST_F(OrtcFactoryTest, CreateUdpTransport) {
  auto result = ortc_factory_->CreateUdpTransport(AF_INET);
  EXPECT_TRUE(result.ok());
  result = ortc_factory_->CreateUdpTransport(AF_INET6);
  EXPECT_TRUE(result.ok());
}

// Test CreateUdpPort with the |min_port| and |max_port| arguments.
TEST_F(OrtcFactoryTest, CreateUdpTransportWithPortRange) {
  auto socket_result1 = ortc_factory_->CreateUdpTransport(AF_INET, 2000, 2002);
  ASSERT_TRUE(socket_result1.ok());
  EXPECT_EQ(2000, socket_result1.value()->GetLocalAddress().port());
  auto socket_result2 = ortc_factory_->CreateUdpTransport(AF_INET, 2000, 2002);
  ASSERT_TRUE(socket_result2.ok());
  EXPECT_EQ(2001, socket_result2.value()->GetLocalAddress().port());
  auto socket_result3 = ortc_factory_->CreateUdpTransport(AF_INET, 2000, 2002);
  ASSERT_TRUE(socket_result3.ok());
  EXPECT_EQ(2002, socket_result3.value()->GetLocalAddress().port());

  // All sockets in the range have been exhausted, so the next call should
  // fail.
  auto failed_result = ortc_factory_->CreateUdpTransport(AF_INET, 2000, 2002);
  EXPECT_EQ(RTCErrorType::RESOURCE_EXHAUSTED, failed_result.error().type());

  // If one socket is destroyed, that port should be freed up again.
  socket_result2.MoveValue().reset();
  auto socket_result4 = ortc_factory_->CreateUdpTransport(AF_INET, 2000, 2002);
  ASSERT_TRUE(socket_result4.ok());
  EXPECT_EQ(2001, socket_result4.value()->GetLocalAddress().port());
}

// Basic test that CreateUdpTransport works with AF_INET and AF_INET6.
TEST_F(OrtcFactoryTest, CreateUdpTransportWithInvalidAddressFamily) {
  auto result = ortc_factory_->CreateUdpTransport(12345);
  EXPECT_EQ(RTCErrorType::INVALID_PARAMETER, result.error().type());
}

TEST_F(OrtcFactoryTest, CreateUdpTransportWithInvalidPortRange) {
  auto result = ortc_factory_->CreateUdpTransport(AF_INET, 3000, 2000);
  EXPECT_EQ(RTCErrorType::INVALID_RANGE, result.error().type());
}

// Just sanity check that each "GetCapabilities" method returns some codecs.
TEST_F(OrtcFactoryTest, GetSenderAndReceiverCapabilities) {
  RtpCapabilities audio_send_caps =
      ortc_factory_->GetRtpSenderCapabilities(cricket::MEDIA_TYPE_AUDIO);
  EXPECT_GT(audio_send_caps.codecs.size(), 0u);
  RtpCapabilities video_send_caps =
      ortc_factory_->GetRtpSenderCapabilities(cricket::MEDIA_TYPE_VIDEO);
  EXPECT_GT(video_send_caps.codecs.size(), 0u);
  RtpCapabilities audio_receive_caps =
      ortc_factory_->GetRtpReceiverCapabilities(cricket::MEDIA_TYPE_AUDIO);
  EXPECT_GT(audio_receive_caps.codecs.size(), 0u);
  RtpCapabilities video_receive_caps =
      ortc_factory_->GetRtpReceiverCapabilities(cricket::MEDIA_TYPE_VIDEO);
  EXPECT_GT(video_receive_caps.codecs.size(), 0u);
}

// Calling CreateRtpSender with a null track should fail, since that makes it
// impossible to know whether to create an audio or video sender. The
// application should be using the method that takes a cricket::MediaType
// instead.
TEST_F(OrtcFactoryTest, CreateSenderWithNullTrack) {
  auto rtp_transport = CreateRtpTransportWithFakePacketTransport();
  auto result = ortc_factory_->CreateRtpSender(nullptr, rtp_transport.get());
  EXPECT_EQ(RTCErrorType::INVALID_PARAMETER, result.error().type());
}

// Calling CreateRtpSender or CreateRtpReceiver with MEDIA_TYPE_DATA should
// fail.
TEST_F(OrtcFactoryTest, CreateSenderOrReceieverWithInvalidKind) {
  auto rtp_transport = CreateRtpTransportWithFakePacketTransport();
  auto sender_result = ortc_factory_->CreateRtpSender(cricket::MEDIA_TYPE_DATA,
                                                      rtp_transport.get());
  EXPECT_EQ(RTCErrorType::INVALID_PARAMETER, sender_result.error().type());
  auto receiver_result = ortc_factory_->CreateRtpReceiver(
      cricket::MEDIA_TYPE_DATA, rtp_transport.get());
  EXPECT_EQ(RTCErrorType::INVALID_PARAMETER, receiver_result.error().type());
}

TEST_F(OrtcFactoryTest, CreateSendersOrReceieversWithNullTransport) {
  auto sender_result =
      ortc_factory_->CreateRtpSender(cricket::MEDIA_TYPE_AUDIO, nullptr);
  EXPECT_EQ(RTCErrorType::INVALID_PARAMETER, sender_result.error().type());
  auto receiver_result =
      ortc_factory_->CreateRtpReceiver(cricket::MEDIA_TYPE_AUDIO, nullptr);
  EXPECT_EQ(RTCErrorType::INVALID_PARAMETER, receiver_result.error().type());
}

}  // namespace webrtc
