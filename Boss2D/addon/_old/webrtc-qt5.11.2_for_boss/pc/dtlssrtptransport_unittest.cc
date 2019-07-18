/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__dtlssrtptransport_h //original-code:"pc/dtlssrtptransport.h"

#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_media__base__fakertp_h //original-code:"media/base/fakertp.h"
#include BOSS_WEBRTC_U_p2p__base__dtlstransportinternal_h //original-code:"p2p/base/dtlstransportinternal.h"
#include BOSS_WEBRTC_U_p2p__base__fakedtlstransport_h //original-code:"p2p/base/fakedtlstransport.h"
#include BOSS_WEBRTC_U_p2p__base__fakepackettransport_h //original-code:"p2p/base/fakepackettransport.h"
#include BOSS_WEBRTC_U_p2p__base__p2pconstants_h //original-code:"p2p/base/p2pconstants.h"
#include BOSS_WEBRTC_U_pc__rtptransport_h //original-code:"pc/rtptransport.h"
#include BOSS_WEBRTC_U_pc__rtptransporttestutil_h //original-code:"pc/rtptransporttestutil.h"
#include BOSS_WEBRTC_U_rtc_base__asyncpacketsocket_h //original-code:"rtc_base/asyncpacketsocket.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__ptr_util_h //original-code:"rtc_base/ptr_util.h"
#include BOSS_WEBRTC_U_rtc_base__sslstreamadapter_h //original-code:"rtc_base/sslstreamadapter.h"

using cricket::FakeDtlsTransport;
using cricket::FakeIceTransport;
using webrtc::DtlsSrtpTransport;
using webrtc::SrtpTransport;
using webrtc::RtpTransport;

const int kRtpAuthTagLen = 10;

class TransportObserver : public sigslot::has_slots<> {
 public:
  void OnPacketReceived(bool rtcp,
                        rtc::CopyOnWriteBuffer* packet,
                        const rtc::PacketTime& packet_time) {
    rtcp ? last_recv_rtcp_packet_ = *packet : last_recv_rtp_packet_ = *packet;
  }

  void OnReadyToSend(bool ready) { ready_to_send_ = ready; }

  rtc::CopyOnWriteBuffer last_recv_rtp_packet() {
    return last_recv_rtp_packet_;
  }

  rtc::CopyOnWriteBuffer last_recv_rtcp_packet() {
    return last_recv_rtcp_packet_;
  }

  bool ready_to_send() { return ready_to_send_; }

 private:
  rtc::CopyOnWriteBuffer last_recv_rtp_packet_;
  rtc::CopyOnWriteBuffer last_recv_rtcp_packet_;
  bool ready_to_send_ = false;
};

class DtlsSrtpTransportTest : public testing::Test,
                              public sigslot::has_slots<> {
 protected:
  DtlsSrtpTransportTest() {}

  std::unique_ptr<DtlsSrtpTransport> MakeDtlsSrtpTransport(
      FakeDtlsTransport* rtp_dtls,
      FakeDtlsTransport* rtcp_dtls,
      bool rtcp_mux_enabled) {
    auto rtp_transport = rtc::MakeUnique<RtpTransport>(rtcp_mux_enabled);

    rtp_transport->AddHandledPayloadType(0x00);
    rtp_transport->AddHandledPayloadType(0xc9);

    auto srtp_transport =
        rtc::MakeUnique<SrtpTransport>(std::move(rtp_transport));
    auto dtls_srtp_transport =
        rtc::MakeUnique<DtlsSrtpTransport>(std::move(srtp_transport));

    dtls_srtp_transport->SetDtlsTransports(rtp_dtls, rtcp_dtls);

    return dtls_srtp_transport;
  }

  void MakeDtlsSrtpTransports(FakeDtlsTransport* rtp_dtls1,
                              FakeDtlsTransport* rtcp_dtls1,
                              FakeDtlsTransport* rtp_dtls2,
                              FakeDtlsTransport* rtcp_dtls2,
                              bool rtcp_mux_enabled) {
    dtls_srtp_transport1_ =
        MakeDtlsSrtpTransport(rtp_dtls1, rtcp_dtls1, rtcp_mux_enabled);
    dtls_srtp_transport2_ =
        MakeDtlsSrtpTransport(rtp_dtls2, rtcp_dtls2, rtcp_mux_enabled);

    dtls_srtp_transport1_->SignalPacketReceived.connect(
        &transport_observer1_, &TransportObserver::OnPacketReceived);
    dtls_srtp_transport1_->SignalReadyToSend.connect(
        &transport_observer1_, &TransportObserver::OnReadyToSend);

    dtls_srtp_transport2_->SignalPacketReceived.connect(
        &transport_observer2_, &TransportObserver::OnPacketReceived);
    dtls_srtp_transport2_->SignalReadyToSend.connect(
        &transport_observer2_, &TransportObserver::OnReadyToSend);
  }

  void CompleteDtlsHandshake(FakeDtlsTransport* fake_dtls1,
                             FakeDtlsTransport* fake_dtls2) {
    auto cert1 = rtc::RTCCertificate::Create(std::unique_ptr<rtc::SSLIdentity>(
        rtc::SSLIdentity::Generate("session1", rtc::KT_DEFAULT)));
    fake_dtls1->SetLocalCertificate(cert1);
    auto cert2 = rtc::RTCCertificate::Create(std::unique_ptr<rtc::SSLIdentity>(
        rtc::SSLIdentity::Generate("session1", rtc::KT_DEFAULT)));
    fake_dtls2->SetLocalCertificate(cert2);
    fake_dtls1->SetDestination(fake_dtls2);
  }

  void SendRecvRtpPackets() {
    ASSERT_TRUE(dtls_srtp_transport1_);
    ASSERT_TRUE(dtls_srtp_transport2_);
    ASSERT_TRUE(dtls_srtp_transport1_->IsActive());
    ASSERT_TRUE(dtls_srtp_transport2_->IsActive());

    size_t rtp_len = sizeof(kPcmuFrame);
    size_t packet_size = rtp_len + kRtpAuthTagLen;
    rtc::Buffer rtp_packet_buffer(packet_size);
    char* rtp_packet_data = rtp_packet_buffer.data<char>();
    memcpy(rtp_packet_data, kPcmuFrame, rtp_len);
    // In order to be able to run this test function multiple times we can not
    // use the same sequence number twice. Increase the sequence number by one.
    rtc::SetBE16(reinterpret_cast<uint8_t*>(rtp_packet_data) + 2,
                 ++sequence_number_);
    rtc::CopyOnWriteBuffer rtp_packet1to2(rtp_packet_data, rtp_len,
                                          packet_size);
    rtc::CopyOnWriteBuffer rtp_packet2to1(rtp_packet_data, rtp_len,
                                          packet_size);

    rtc::PacketOptions options;
    // Send a packet from |srtp_transport1_| to |srtp_transport2_| and verify
    // that the packet can be successfully received and decrypted.
    ASSERT_TRUE(dtls_srtp_transport1_->SendRtpPacket(&rtp_packet1to2, options,
                                                     cricket::PF_SRTP_BYPASS));
    ASSERT_TRUE(transport_observer2_.last_recv_rtp_packet().data());
    EXPECT_EQ(0, memcmp(transport_observer2_.last_recv_rtp_packet().data(),
                        kPcmuFrame, rtp_len));
    ASSERT_TRUE(dtls_srtp_transport2_->SendRtpPacket(&rtp_packet2to1, options,
                                                     cricket::PF_SRTP_BYPASS));
    ASSERT_TRUE(transport_observer1_.last_recv_rtp_packet().data());
    EXPECT_EQ(0, memcmp(transport_observer1_.last_recv_rtp_packet().data(),
                        kPcmuFrame, rtp_len));
  }

  void SendRecvRtcpPackets() {
    size_t rtcp_len = sizeof(kRtcpReport);
    size_t packet_size = rtcp_len + 4 + kRtpAuthTagLen;
    rtc::Buffer rtcp_packet_buffer(packet_size);

    // TODO(zhihuang): Remove the extra copy when the SendRtpPacket method
    // doesn't take the CopyOnWriteBuffer by pointer.
    rtc::CopyOnWriteBuffer rtcp_packet1to2(kRtcpReport, rtcp_len, packet_size);
    rtc::CopyOnWriteBuffer rtcp_packet2to1(kRtcpReport, rtcp_len, packet_size);

    rtc::PacketOptions options;
    // Send a packet from |srtp_transport1_| to |srtp_transport2_| and verify
    // that the packet can be successfully received and decrypted.
    ASSERT_TRUE(dtls_srtp_transport1_->SendRtcpPacket(&rtcp_packet1to2, options,
                                                      cricket::PF_SRTP_BYPASS));
    ASSERT_TRUE(transport_observer2_.last_recv_rtcp_packet().data());
    EXPECT_EQ(0, memcmp(transport_observer2_.last_recv_rtcp_packet().data(),
                        kRtcpReport, rtcp_len));

    // Do the same thing in the opposite direction;
    ASSERT_TRUE(dtls_srtp_transport2_->SendRtcpPacket(&rtcp_packet2to1, options,
                                                      cricket::PF_SRTP_BYPASS));
    ASSERT_TRUE(transport_observer1_.last_recv_rtcp_packet().data());
    EXPECT_EQ(0, memcmp(transport_observer1_.last_recv_rtcp_packet().data(),
                        kRtcpReport, rtcp_len));
  }

  void SendRecvRtpPacketsWithHeaderExtension(
      const std::vector<int>& encrypted_header_ids) {
    ASSERT_TRUE(dtls_srtp_transport1_);
    ASSERT_TRUE(dtls_srtp_transport2_);
    ASSERT_TRUE(dtls_srtp_transport1_->IsActive());
    ASSERT_TRUE(dtls_srtp_transport2_->IsActive());

    size_t rtp_len = sizeof(kPcmuFrameWithExtensions);
    size_t packet_size = rtp_len + kRtpAuthTagLen;
    rtc::Buffer rtp_packet_buffer(packet_size);
    char* rtp_packet_data = rtp_packet_buffer.data<char>();
    memcpy(rtp_packet_data, kPcmuFrameWithExtensions, rtp_len);
    // In order to be able to run this test function multiple times we can not
    // use the same sequence number twice. Increase the sequence number by one.
    rtc::SetBE16(reinterpret_cast<uint8_t*>(rtp_packet_data) + 2,
                 ++sequence_number_);
    rtc::CopyOnWriteBuffer rtp_packet1to2(rtp_packet_data, rtp_len,
                                          packet_size);
    rtc::CopyOnWriteBuffer rtp_packet2to1(rtp_packet_data, rtp_len,
                                          packet_size);

    char original_rtp_data[sizeof(kPcmuFrameWithExtensions)];
    memcpy(original_rtp_data, rtp_packet_data, rtp_len);

    rtc::PacketOptions options;
    // Send a packet from |srtp_transport1_| to |srtp_transport2_| and verify
    // that the packet can be successfully received and decrypted.
    ASSERT_TRUE(dtls_srtp_transport1_->SendRtpPacket(&rtp_packet1to2, options,
                                                     cricket::PF_SRTP_BYPASS));
    ASSERT_TRUE(transport_observer2_.last_recv_rtp_packet().data());
    EXPECT_EQ(0, memcmp(transport_observer2_.last_recv_rtp_packet().data(),
                        original_rtp_data, rtp_len));
    // Get the encrypted packet from underneath packet transport and verify the
    // data and header extension are actually encrypted.
    auto fake_dtls_transport = static_cast<FakeDtlsTransport*>(
        dtls_srtp_transport1_->rtp_packet_transport());
    auto fake_ice_transport =
        static_cast<FakeIceTransport*>(fake_dtls_transport->ice_transport());
    EXPECT_NE(0, memcmp(fake_ice_transport->last_sent_packet().data(),
                        original_rtp_data, rtp_len));
    CompareHeaderExtensions(reinterpret_cast<const char*>(
                                fake_ice_transport->last_sent_packet().data()),
                            fake_ice_transport->last_sent_packet().size(),
                            original_rtp_data, rtp_len, encrypted_header_ids,
                            false);

    // Do the same thing in the opposite direction.
    ASSERT_TRUE(dtls_srtp_transport2_->SendRtpPacket(&rtp_packet2to1, options,
                                                     cricket::PF_SRTP_BYPASS));
    ASSERT_TRUE(transport_observer1_.last_recv_rtp_packet().data());
    EXPECT_EQ(0, memcmp(transport_observer1_.last_recv_rtp_packet().data(),
                        original_rtp_data, rtp_len));
    // Get the encrypted packet from underneath packet transport and verify the
    // data and header extension are actually encrypted.
    fake_dtls_transport = static_cast<FakeDtlsTransport*>(
        dtls_srtp_transport2_->rtp_packet_transport());
    fake_ice_transport =
        static_cast<FakeIceTransport*>(fake_dtls_transport->ice_transport());
    EXPECT_NE(0, memcmp(fake_ice_transport->last_sent_packet().data(),
                        original_rtp_data, rtp_len));
    CompareHeaderExtensions(reinterpret_cast<const char*>(
                                fake_ice_transport->last_sent_packet().data()),
                            fake_ice_transport->last_sent_packet().size(),
                            original_rtp_data, rtp_len, encrypted_header_ids,
                            false);
  }

  void SendRecvPackets() {
    SendRecvRtpPackets();
    SendRecvRtcpPackets();
  }

  std::unique_ptr<DtlsSrtpTransport> dtls_srtp_transport1_;
  std::unique_ptr<DtlsSrtpTransport> dtls_srtp_transport2_;
  TransportObserver transport_observer1_;
  TransportObserver transport_observer2_;

  int sequence_number_ = 0;
};

// Tests that if RTCP muxing is enabled and transports are set after RTP
// transport finished the handshake, SRTP is set up.
TEST_F(DtlsSrtpTransportTest, SetTransportsAfterHandshakeCompleteWithRtcpMux) {
  auto rtp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "video", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "video", cricket::ICE_CANDIDATE_COMPONENT_RTP);

  MakeDtlsSrtpTransports(rtp_dtls1.get(), nullptr, rtp_dtls2.get(), nullptr,
                         /*rtcp_mux_enabled=*/true);

  auto rtp_dtls3 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtp_dtls4 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);

  CompleteDtlsHandshake(rtp_dtls3.get(), rtp_dtls4.get());

  dtls_srtp_transport1_->SetDtlsTransports(rtp_dtls3.get(), nullptr);
  dtls_srtp_transport2_->SetDtlsTransports(rtp_dtls4.get(), nullptr);

  SendRecvPackets();
}

// Tests that if RTCP muxing is not enabled and transports are set after both
// RTP and RTCP transports finished the handshake, SRTP is set up.
TEST_F(DtlsSrtpTransportTest,
       SetTransportsAfterHandshakeCompleteWithoutRtcpMux) {
  auto rtp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "video", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "video", cricket::ICE_CANDIDATE_COMPONENT_RTCP);
  auto rtp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "video", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "video", cricket::ICE_CANDIDATE_COMPONENT_RTCP);

  MakeDtlsSrtpTransports(rtp_dtls1.get(), rtcp_dtls1.get(), rtp_dtls2.get(),
                         rtcp_dtls2.get(), /*rtcp_mux_enabled=*/false);

  auto rtp_dtls3 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls3 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTCP);
  auto rtp_dtls4 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls4 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTCP);
  CompleteDtlsHandshake(rtp_dtls3.get(), rtp_dtls4.get());
  CompleteDtlsHandshake(rtcp_dtls3.get(), rtcp_dtls4.get());

  dtls_srtp_transport1_->SetDtlsTransports(rtp_dtls3.get(), rtcp_dtls3.get());
  dtls_srtp_transport2_->SetDtlsTransports(rtp_dtls4.get(), rtcp_dtls4.get());

  SendRecvPackets();
}

// Tests if RTCP muxing is enabled, SRTP is set up as soon as the RTP DTLS
// handshake is finished.
TEST_F(DtlsSrtpTransportTest, SetTransportsBeforeHandshakeCompleteWithRtcpMux) {
  auto rtp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTCP);
  auto rtp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTCP);

  MakeDtlsSrtpTransports(rtp_dtls1.get(), rtcp_dtls1.get(), rtp_dtls2.get(),
                         rtcp_dtls2.get(),
                         /*rtcp_mux_enabled=*/false);

  dtls_srtp_transport1_->SetRtcpMuxEnabled(true);
  dtls_srtp_transport2_->SetRtcpMuxEnabled(true);
  CompleteDtlsHandshake(rtp_dtls1.get(), rtp_dtls2.get());
  SendRecvPackets();
}

// Tests if RTCP muxing is not enabled, SRTP is set up when both the RTP and
// RTCP DTLS handshake are finished.
TEST_F(DtlsSrtpTransportTest,
       SetTransportsBeforeHandshakeCompleteWithoutRtcpMux) {
  auto rtp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTCP);
  auto rtp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTCP);

  MakeDtlsSrtpTransports(rtp_dtls1.get(), rtcp_dtls1.get(), rtp_dtls2.get(),
                         rtcp_dtls2.get(), /*rtcp_mux_enabled=*/false);

  CompleteDtlsHandshake(rtp_dtls1.get(), rtp_dtls2.get());
  EXPECT_FALSE(dtls_srtp_transport1_->IsActive());
  EXPECT_FALSE(dtls_srtp_transport2_->IsActive());
  CompleteDtlsHandshake(rtcp_dtls1.get(), rtcp_dtls2.get());
  SendRecvPackets();
}

// Tests that if the DtlsTransport underneath is changed, the previous DTLS-SRTP
// context will be reset and will be re-setup once the new transports' handshake
// complete.
TEST_F(DtlsSrtpTransportTest, DtlsSrtpResetAfterDtlsTransportChange) {
  auto rtp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);

  MakeDtlsSrtpTransports(rtp_dtls1.get(), nullptr, rtp_dtls2.get(), nullptr,
                         /*rtcp_mux_enabled=*/true);

  CompleteDtlsHandshake(rtp_dtls1.get(), rtp_dtls2.get());
  EXPECT_TRUE(dtls_srtp_transport1_->IsActive());
  EXPECT_TRUE(dtls_srtp_transport2_->IsActive());

  auto rtp_dtls3 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtp_dtls4 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);

  // The previous context is reset.
  dtls_srtp_transport1_->SetDtlsTransports(rtp_dtls3.get(), nullptr);
  dtls_srtp_transport2_->SetDtlsTransports(rtp_dtls4.get(), nullptr);
  EXPECT_FALSE(dtls_srtp_transport1_->IsActive());
  EXPECT_FALSE(dtls_srtp_transport2_->IsActive());

  // Re-setup.
  CompleteDtlsHandshake(rtp_dtls3.get(), rtp_dtls4.get());
  SendRecvPackets();
}

// Tests if only the RTP DTLS handshake complete, and then RTCP muxing is
// enabled, SRTP is set up.
TEST_F(DtlsSrtpTransportTest,
       RtcpMuxEnabledAfterRtpTransportHandshakeComplete) {
  auto rtp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTCP);
  auto rtp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTCP);

  MakeDtlsSrtpTransports(rtp_dtls1.get(), rtcp_dtls1.get(), rtp_dtls2.get(),
                         rtcp_dtls2.get(), /*rtcp_mux_enabled=*/false);

  CompleteDtlsHandshake(rtp_dtls1.get(), rtp_dtls2.get());
  // Inactive because the RTCP transport handshake didn't complete.
  EXPECT_FALSE(dtls_srtp_transport1_->IsActive());
  EXPECT_FALSE(dtls_srtp_transport2_->IsActive());

  dtls_srtp_transport1_->SetRtcpMuxEnabled(true);
  dtls_srtp_transport2_->SetRtcpMuxEnabled(true);
  // The transports should be active and be able to send packets when the
  // RTCP muxing is enabled.
  SendRecvPackets();
}

// Tests that when SetSend/RecvEncryptedHeaderExtensionIds is called, the SRTP
// sessions are updated with new encryped header extension IDs immediately.
TEST_F(DtlsSrtpTransportTest, EncryptedHeaderExtensionIdUpdated) {
  auto rtp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);

  MakeDtlsSrtpTransports(rtp_dtls1.get(), nullptr, rtp_dtls2.get(), nullptr,
                         /*rtcp_mux_enabled=*/true);
  CompleteDtlsHandshake(rtp_dtls1.get(), rtp_dtls2.get());

  std::vector<int> encrypted_headers;
  encrypted_headers.push_back(kHeaderExtensionIDs[0]);
  encrypted_headers.push_back(kHeaderExtensionIDs[1]);

  dtls_srtp_transport1_->UpdateSendEncryptedHeaderExtensionIds(
      encrypted_headers);
  dtls_srtp_transport1_->UpdateRecvEncryptedHeaderExtensionIds(
      encrypted_headers);
  dtls_srtp_transport2_->UpdateSendEncryptedHeaderExtensionIds(
      encrypted_headers);
  dtls_srtp_transport2_->UpdateRecvEncryptedHeaderExtensionIds(
      encrypted_headers);
}

// Tests if RTCP muxing is enabled. DtlsSrtpTransport is ready to send once the
// RTP DtlsTransport is ready.
TEST_F(DtlsSrtpTransportTest, SignalReadyToSendFiredWithRtcpMux) {
  auto rtp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);

  MakeDtlsSrtpTransports(rtp_dtls1.get(), nullptr, rtp_dtls2.get(), nullptr,
                         /*rtcp_mux_enabled=*/true);

  rtp_dtls1->SetDestination(rtp_dtls2.get());
  EXPECT_TRUE(transport_observer1_.ready_to_send());
  EXPECT_TRUE(transport_observer2_.ready_to_send());
}

// Tests if RTCP muxing is not enabled. DtlsSrtpTransport is ready to send once
// both the RTP and RTCP DtlsTransport are ready.
TEST_F(DtlsSrtpTransportTest, SignalReadyToSendFiredWithoutRtcpMux) {
  auto rtp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls1 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTCP);
  auto rtp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTP);
  auto rtcp_dtls2 = rtc::MakeUnique<FakeDtlsTransport>(
      "audio", cricket::ICE_CANDIDATE_COMPONENT_RTCP);

  MakeDtlsSrtpTransports(rtp_dtls1.get(), rtcp_dtls1.get(), rtp_dtls2.get(),
                         rtcp_dtls2.get(), /*rtcp_mux_enabled=*/false);

  rtp_dtls1->SetDestination(rtp_dtls2.get());
  EXPECT_FALSE(transport_observer1_.ready_to_send());
  EXPECT_FALSE(transport_observer2_.ready_to_send());

  rtcp_dtls1->SetDestination(rtcp_dtls2.get());
  EXPECT_TRUE(transport_observer1_.ready_to_send());
  EXPECT_TRUE(transport_observer2_.ready_to_send());
}
