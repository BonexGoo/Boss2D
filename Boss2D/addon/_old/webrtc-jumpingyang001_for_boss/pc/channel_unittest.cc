/*
 *  Copyright 2009 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_media__base__fakemediaengine_h //original-code:"media/base/fakemediaengine.h"
#include BOSS_WEBRTC_U_media__base__fakertp_h //original-code:"media/base/fakertp.h"
#include BOSS_WEBRTC_U_media__base__mediachannel_h //original-code:"media/base/mediachannel.h"
#include BOSS_WEBRTC_U_media__base__testutils_h //original-code:"media/base/testutils.h"
#include BOSS_WEBRTC_U_p2p__base__fakecandidatepair_h //original-code:"p2p/base/fakecandidatepair.h"
#include BOSS_WEBRTC_U_p2p__base__fakedtlstransport_h //original-code:"p2p/base/fakedtlstransport.h"
#include BOSS_WEBRTC_U_p2p__base__fakepackettransport_h //original-code:"p2p/base/fakepackettransport.h"
#include BOSS_WEBRTC_U_pc__channel_h //original-code:"pc/channel.h"
#include BOSS_WEBRTC_U_rtc_base__buffer_h //original-code:"rtc_base/buffer.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__fakeclock_h //original-code:"rtc_base/fakeclock.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__sslstreamadapter_h //original-code:"rtc_base/sslstreamadapter.h"

using cricket::DtlsTransportInternal;
using cricket::FakeVoiceMediaChannel;
using cricket::StreamParams;
using webrtc::RtpTransceiverDirection;
using webrtc::SdpType;

namespace {
const cricket::AudioCodec kPcmuCodec(0, "PCMU", 64000, 8000, 1);
const cricket::AudioCodec kPcmaCodec(8, "PCMA", 64000, 8000, 1);
const cricket::AudioCodec kIsacCodec(103, "ISAC", 40000, 16000, 1);
const cricket::VideoCodec kH264Codec(97, "H264");
const cricket::VideoCodec kH264SvcCodec(99, "H264-SVC");
const cricket::DataCodec kGoogleDataCodec(101, "google-data");
const uint32_t kSsrc1 = 0x1111;
const uint32_t kSsrc2 = 0x2222;
const uint32_t kSsrc3 = 0x3333;
const uint32_t kSsrc4 = 0x4444;
const int kAudioPts[] = {0, 8};
const int kVideoPts[] = {97, 99};
enum class NetworkIsWorker { Yes, No };

}  // namespace

template <class ChannelT,
          class MediaChannelT,
          class ContentT,
          class CodecT,
          class MediaInfoT,
          class OptionsT>
class Traits {
 public:
  typedef ChannelT Channel;
  typedef MediaChannelT MediaChannel;
  typedef ContentT Content;
  typedef CodecT Codec;
  typedef MediaInfoT MediaInfo;
  typedef OptionsT Options;
};

class VoiceTraits : public Traits<cricket::VoiceChannel,
                                  cricket::FakeVoiceMediaChannel,
                                  cricket::AudioContentDescription,
                                  cricket::AudioCodec,
                                  cricket::VoiceMediaInfo,
                                  cricket::AudioOptions> {};

class VideoTraits : public Traits<cricket::VideoChannel,
                                  cricket::FakeVideoMediaChannel,
                                  cricket::VideoContentDescription,
                                  cricket::VideoCodec,
                                  cricket::VideoMediaInfo,
                                  cricket::VideoOptions> {};

class DataTraits : public Traits<cricket::RtpDataChannel,
                                 cricket::FakeDataMediaChannel,
                                 cricket::DataContentDescription,
                                 cricket::DataCodec,
                                 cricket::DataMediaInfo,
                                 cricket::DataOptions> {};

// Base class for Voice/Video/RtpDataChannel tests
template <class T>
class ChannelTest : public testing::Test, public sigslot::has_slots<> {
 public:
  enum Flags {
    RTCP_MUX = 0x1,
    SSRC_MUX = 0x8,
    DTLS = 0x10,
    // Use BaseChannel with PacketTransportInternal rather than
    // DtlsTransportInternal.
    RAW_PACKET_TRANSPORT = 0x20,
  };

  ChannelTest(bool verify_playout,
              rtc::ArrayView<const uint8_t> rtp_data,
              rtc::ArrayView<const uint8_t> rtcp_data,
              NetworkIsWorker network_is_worker)
      : verify_playout_(verify_playout),
        rtp_packet_(rtp_data.data(), rtp_data.size()),
        rtcp_packet_(rtcp_data.data(), rtcp_data.size()) {
    if (network_is_worker == NetworkIsWorker::Yes) {
      network_thread_ = rtc::Thread::Current();
    } else {
      network_thread_keeper_ = rtc::Thread::Create();
      network_thread_keeper_->SetName("Network", nullptr);
      network_thread_ = network_thread_keeper_.get();
    }
  }

  void CreateChannels(int flags1, int flags2) {
    CreateChannels(absl::make_unique<typename T::MediaChannel>(
                       nullptr, typename T::Options()),
                   absl::make_unique<typename T::MediaChannel>(
                       nullptr, typename T::Options()),
                   flags1, flags2);
  }
  void CreateChannels(std::unique_ptr<typename T::MediaChannel> ch1,
                      std::unique_ptr<typename T::MediaChannel> ch2,
                      int flags1,
                      int flags2) {
    // Network thread is started in CreateChannels, to allow the test to
    // configure a fake clock before any threads are spawned and attempt to
    // access the time.
    if (network_thread_keeper_) {
      network_thread_keeper_->Start();
    }

    // Make sure if using raw packet transports, they're used for both
    // channels.
    RTC_DCHECK_EQ(flags1 & RAW_PACKET_TRANSPORT, flags2 & RAW_PACKET_TRANSPORT);
    rtc::Thread* worker_thread = rtc::Thread::Current();
    media_channel1_ = ch1.get();
    media_channel2_ = ch2.get();
    rtc::PacketTransportInternal* rtp1 = nullptr;
    rtc::PacketTransportInternal* rtcp1 = nullptr;
    rtc::PacketTransportInternal* rtp2 = nullptr;
    rtc::PacketTransportInternal* rtcp2 = nullptr;
    // Based on flags, create fake DTLS or raw packet transports.
    if (flags1 & RAW_PACKET_TRANSPORT) {
      fake_rtp_packet_transport1_.reset(
          new rtc::FakePacketTransport("channel1_rtp"));
      rtp1 = fake_rtp_packet_transport1_.get();
      if (!(flags1 & RTCP_MUX)) {
        fake_rtcp_packet_transport1_.reset(
            new rtc::FakePacketTransport("channel1_rtcp"));
        rtcp1 = fake_rtcp_packet_transport1_.get();
      }
    } else {
      // Confirmed to work with KT_RSA and KT_ECDSA.
      fake_rtp_dtls_transport1_.reset(new cricket::FakeDtlsTransport(
          "channel1", cricket::ICE_CANDIDATE_COMPONENT_RTP));
      rtp1 = fake_rtp_dtls_transport1_.get();
      if (!(flags1 & RTCP_MUX)) {
        fake_rtcp_dtls_transport1_.reset(new cricket::FakeDtlsTransport(
            "channel1", cricket::ICE_CANDIDATE_COMPONENT_RTCP));
        rtcp1 = fake_rtcp_dtls_transport1_.get();
      }
      if (flags1 & DTLS) {
        auto cert1 =
            rtc::RTCCertificate::Create(std::unique_ptr<rtc::SSLIdentity>(
                rtc::SSLIdentity::Generate("session1", rtc::KT_DEFAULT)));
        fake_rtp_dtls_transport1_->SetLocalCertificate(cert1);
        if (fake_rtcp_dtls_transport1_) {
          fake_rtcp_dtls_transport1_->SetLocalCertificate(cert1);
        }
      }
    }
    // Based on flags, create fake DTLS or raw packet transports.
    if (flags2 & RAW_PACKET_TRANSPORT) {
      fake_rtp_packet_transport2_.reset(
          new rtc::FakePacketTransport("channel2_rtp"));
      rtp2 = fake_rtp_packet_transport2_.get();
      if (!(flags2 & RTCP_MUX)) {
        fake_rtcp_packet_transport2_.reset(
            new rtc::FakePacketTransport("channel2_rtcp"));
        rtcp2 = fake_rtcp_packet_transport2_.get();
      }
    } else {
      // Confirmed to work with KT_RSA and KT_ECDSA.
      fake_rtp_dtls_transport2_.reset(new cricket::FakeDtlsTransport(
          "channel2", cricket::ICE_CANDIDATE_COMPONENT_RTP));
      rtp2 = fake_rtp_dtls_transport2_.get();
      if (!(flags2 & RTCP_MUX)) {
        fake_rtcp_dtls_transport2_.reset(new cricket::FakeDtlsTransport(
            "channel2", cricket::ICE_CANDIDATE_COMPONENT_RTCP));
        rtcp2 = fake_rtcp_dtls_transport2_.get();
      }
      if (flags2 & DTLS) {
        auto cert2 =
            rtc::RTCCertificate::Create(std::unique_ptr<rtc::SSLIdentity>(
                rtc::SSLIdentity::Generate("session2", rtc::KT_DEFAULT)));
        fake_rtp_dtls_transport2_->SetLocalCertificate(cert2);
        if (fake_rtcp_dtls_transport2_) {
          fake_rtcp_dtls_transport2_->SetLocalCertificate(cert2);
        }
      }
    }
    rtp_transport1_ = CreateRtpTransportBasedOnFlags(
        fake_rtp_packet_transport1_.get(), fake_rtcp_packet_transport1_.get(),
        fake_rtp_dtls_transport1_.get(), fake_rtcp_dtls_transport1_.get(),
        flags1);
    rtp_transport2_ = CreateRtpTransportBasedOnFlags(
        fake_rtp_packet_transport2_.get(), fake_rtcp_packet_transport2_.get(),
        fake_rtp_dtls_transport2_.get(), fake_rtcp_dtls_transport2_.get(),
        flags2);

    channel1_ = CreateChannel(worker_thread, network_thread_, &media_engine_,
                              std::move(ch1), rtp_transport1_.get(), flags1);
    channel2_ = CreateChannel(worker_thread, network_thread_, &media_engine_,
                              std::move(ch2), rtp_transport2_.get(), flags2);
    channel1_->SignalRtcpMuxFullyActive.connect(
        this, &ChannelTest<T>::OnRtcpMuxFullyActive1);
    channel2_->SignalRtcpMuxFullyActive.connect(
        this, &ChannelTest<T>::OnRtcpMuxFullyActive2);
    CreateContent(flags1, kPcmuCodec, kH264Codec, &local_media_content1_);
    CreateContent(flags2, kPcmuCodec, kH264Codec, &local_media_content2_);
    CopyContent(local_media_content1_, &remote_media_content1_);
    CopyContent(local_media_content2_, &remote_media_content2_);

    // Add stream information (SSRC) to the local content but not to the remote
    // content. This means that we per default know the SSRC of what we send but
    // not what we receive.
    AddLegacyStreamInContent(kSsrc1, flags1, &local_media_content1_);
    AddLegacyStreamInContent(kSsrc2, flags2, &local_media_content2_);

    // If SSRC_MUX is used we also need to know the SSRC of the incoming stream.
    if (flags1 & SSRC_MUX) {
      AddLegacyStreamInContent(kSsrc1, flags1, &remote_media_content1_);
    }
    if (flags2 & SSRC_MUX) {
      AddLegacyStreamInContent(kSsrc2, flags2, &remote_media_content2_);
    }
  }
  std::unique_ptr<typename T::Channel> CreateChannel(
      rtc::Thread* worker_thread,
      rtc::Thread* network_thread,
      cricket::MediaEngineInterface* engine,
      std::unique_ptr<typename T::MediaChannel> ch,
      webrtc::RtpTransportInternal* rtp_transport,
      int flags) {
    rtc::Thread* signaling_thread = rtc::Thread::Current();
    auto channel = absl::make_unique<typename T::Channel>(
        worker_thread, network_thread, signaling_thread, engine, std::move(ch),
        cricket::CN_AUDIO, (flags & DTLS) != 0, rtc::CryptoOptions());
    channel->Init_w(rtp_transport);
    return channel;
  }

  std::unique_ptr<webrtc::RtpTransportInternal> CreateRtpTransportBasedOnFlags(
      rtc::PacketTransportInternal* rtp_packet_transport,
      rtc::PacketTransportInternal* rtcp_packet_transport,
      DtlsTransportInternal* rtp_dtls_transport,
      DtlsTransportInternal* rtcp_dtls_transport,
      int flags) {
    if (flags & RTCP_MUX) {
      rtcp_packet_transport = nullptr;
      rtcp_dtls_transport = nullptr;
    }

    if (flags & DTLS) {
      return CreateDtlsSrtpTransport(rtp_dtls_transport, rtcp_dtls_transport);
    } else {
      if (flags & RAW_PACKET_TRANSPORT) {
        return CreateUnencryptedTransport(rtp_packet_transport,
                                          rtcp_packet_transport);
      } else {
        return CreateUnencryptedTransport(rtp_dtls_transport,
                                          rtcp_dtls_transport);
      }
    }
  }

  std::unique_ptr<webrtc::RtpTransport> CreateUnencryptedTransport(
      rtc::PacketTransportInternal* rtp_packet_transport,
      rtc::PacketTransportInternal* rtcp_packet_transport) {
    auto rtp_transport = absl::make_unique<webrtc::RtpTransport>(
        rtcp_packet_transport == nullptr);

    rtp_transport->SetRtpPacketTransport(rtp_packet_transport);
    if (rtcp_packet_transport) {
      rtp_transport->SetRtcpPacketTransport(rtcp_packet_transport);
    }
    return rtp_transport;
  }

  std::unique_ptr<webrtc::DtlsSrtpTransport> CreateDtlsSrtpTransport(
      cricket::DtlsTransportInternal* rtp_dtls_transport,
      cricket::DtlsTransportInternal* rtcp_dtls_transport) {
    auto dtls_srtp_transport = absl::make_unique<webrtc::DtlsSrtpTransport>(
        rtcp_dtls_transport == nullptr);

    dtls_srtp_transport->SetDtlsTransports(rtp_dtls_transport,
                                           rtcp_dtls_transport);
    return dtls_srtp_transport;
  }

  void ConnectFakeTransports() {
    network_thread_->Invoke<void>(RTC_FROM_HERE, [this] {
      bool asymmetric = false;
      // Depending on test flags, could be using DTLS or raw packet transport.
      if (fake_rtp_dtls_transport1_ && fake_rtp_dtls_transport2_) {
        fake_rtp_dtls_transport1_->SetDestination(
            fake_rtp_dtls_transport2_.get(), asymmetric);
      }
      if (fake_rtcp_dtls_transport1_ && fake_rtcp_dtls_transport2_) {
        fake_rtcp_dtls_transport1_->SetDestination(
            fake_rtcp_dtls_transport2_.get(), asymmetric);
      }
      if (fake_rtp_packet_transport1_ && fake_rtp_packet_transport2_) {
        fake_rtp_packet_transport1_->SetDestination(
            fake_rtp_packet_transport2_.get(), asymmetric);
      }
      if (fake_rtcp_packet_transport1_ && fake_rtcp_packet_transport2_) {
        fake_rtcp_packet_transport1_->SetDestination(
            fake_rtcp_packet_transport2_.get(), asymmetric);
      }
    });
  }

  bool SendInitiate() {
    bool result = channel1_->SetLocalContent(&local_media_content1_,
                                             SdpType::kOffer, NULL);
    if (result) {
      channel1_->Enable(true);
      result = channel2_->SetRemoteContent(&remote_media_content1_,
                                           SdpType::kOffer, NULL);
      if (result) {
        ConnectFakeTransports();
        result = channel2_->SetLocalContent(&local_media_content2_,
                                            SdpType::kAnswer, NULL);
      }
    }
    return result;
  }

  bool SendAccept() {
    channel2_->Enable(true);
    return channel1_->SetRemoteContent(&remote_media_content2_,
                                       SdpType::kAnswer, NULL);
  }

  bool SendOffer() {
    bool result = channel1_->SetLocalContent(&local_media_content1_,
                                             SdpType::kOffer, NULL);
    if (result) {
      channel1_->Enable(true);
      result = channel2_->SetRemoteContent(&remote_media_content1_,
                                           SdpType::kOffer, NULL);
    }
    return result;
  }

  bool SendProvisionalAnswer() {
    bool result = channel2_->SetLocalContent(&local_media_content2_,
                                             SdpType::kPrAnswer, NULL);
    if (result) {
      channel2_->Enable(true);
      result = channel1_->SetRemoteContent(&remote_media_content2_,
                                           SdpType::kPrAnswer, NULL);
      ConnectFakeTransports();
    }
    return result;
  }

  bool SendFinalAnswer() {
    bool result = channel2_->SetLocalContent(&local_media_content2_,
                                             SdpType::kAnswer, NULL);
    if (result)
      result = channel1_->SetRemoteContent(&remote_media_content2_,
                                           SdpType::kAnswer, NULL);
    return result;
  }

  bool Terminate() {
    channel1_.reset();
    channel2_.reset();
    fake_rtp_dtls_transport1_.reset();
    fake_rtcp_dtls_transport1_.reset();
    fake_rtp_dtls_transport2_.reset();
    fake_rtcp_dtls_transport2_.reset();
    fake_rtp_packet_transport1_.reset();
    fake_rtcp_packet_transport1_.reset();
    fake_rtp_packet_transport2_.reset();
    fake_rtcp_packet_transport2_.reset();
    if (network_thread_keeper_) {
      network_thread_keeper_.reset();
    }
    return true;
  }

  bool AddStream1(int id) {
    return channel1_->AddRecvStream(cricket::StreamParams::CreateLegacy(id));
  }
  bool RemoveStream1(int id) { return channel1_->RemoveRecvStream(id); }

  void SendRtp1() {
    media_channel1_->SendRtp(rtp_packet_.data(), rtp_packet_.size(),
                             rtc::PacketOptions());
  }
  void SendRtp2() {
    media_channel2_->SendRtp(rtp_packet_.data(), rtp_packet_.size(),
                             rtc::PacketOptions());
  }
  void SendRtcp1() {
    media_channel1_->SendRtcp(rtcp_packet_.data(), rtcp_packet_.size());
  }
  void SendRtcp2() {
    media_channel2_->SendRtcp(rtcp_packet_.data(), rtcp_packet_.size());
  }
  // Methods to send custom data.
  void SendCustomRtp1(uint32_t ssrc, int sequence_number, int pl_type = -1) {
    rtc::Buffer data = CreateRtpData(ssrc, sequence_number, pl_type);
    media_channel1_->SendRtp(data.data(), data.size(), rtc::PacketOptions());
  }
  void SendCustomRtp2(uint32_t ssrc, int sequence_number, int pl_type = -1) {
    rtc::Buffer data = CreateRtpData(ssrc, sequence_number, pl_type);
    media_channel2_->SendRtp(data.data(), data.size(), rtc::PacketOptions());
  }
  void SendCustomRtcp1(uint32_t ssrc) {
    rtc::Buffer data = CreateRtcpData(ssrc);
    media_channel1_->SendRtcp(data.data(), data.size());
  }
  void SendCustomRtcp2(uint32_t ssrc) {
    rtc::Buffer data = CreateRtcpData(ssrc);
    media_channel2_->SendRtcp(data.data(), data.size());
  }

  bool CheckRtp1() {
    return media_channel1_->CheckRtp(rtp_packet_.data(), rtp_packet_.size());
  }
  bool CheckRtp2() {
    return media_channel2_->CheckRtp(rtp_packet_.data(), rtp_packet_.size());
  }
  bool CheckRtcp1() {
    return media_channel1_->CheckRtcp(rtcp_packet_.data(), rtcp_packet_.size());
  }
  bool CheckRtcp2() {
    return media_channel2_->CheckRtcp(rtcp_packet_.data(), rtcp_packet_.size());
  }
  // Methods to check custom data.
  bool CheckCustomRtp1(uint32_t ssrc, int sequence_number, int pl_type = -1) {
    rtc::Buffer data = CreateRtpData(ssrc, sequence_number, pl_type);
    return media_channel1_->CheckRtp(data.data(), data.size());
  }
  bool CheckCustomRtp2(uint32_t ssrc, int sequence_number, int pl_type = -1) {
    rtc::Buffer data = CreateRtpData(ssrc, sequence_number, pl_type);
    return media_channel2_->CheckRtp(data.data(), data.size());
  }
  bool CheckCustomRtcp1(uint32_t ssrc) {
    rtc::Buffer data = CreateRtcpData(ssrc);
    return media_channel1_->CheckRtcp(data.data(), data.size());
  }
  bool CheckCustomRtcp2(uint32_t ssrc) {
    rtc::Buffer data = CreateRtcpData(ssrc);
    return media_channel2_->CheckRtcp(data.data(), data.size());
  }
  rtc::Buffer CreateRtpData(uint32_t ssrc, int sequence_number, int pl_type) {
    rtc::Buffer data(rtp_packet_.data(), rtp_packet_.size());
    // Set SSRC in the rtp packet copy.
    rtc::SetBE32(data.data() + 8, ssrc);
    rtc::SetBE16(data.data() + 2, sequence_number);
    if (pl_type >= 0) {
      rtc::Set8(data.data(), 1, static_cast<uint8_t>(pl_type));
    }
    return data;
  }
  rtc::Buffer CreateRtcpData(uint32_t ssrc) {
    rtc::Buffer data(rtcp_packet_.data(), rtcp_packet_.size());
    // Set SSRC in the rtcp packet copy.
    rtc::SetBE32(data.data() + 4, ssrc);
    return data;
  }

  bool CheckNoRtp1() { return media_channel1_->CheckNoRtp(); }
  bool CheckNoRtp2() { return media_channel2_->CheckNoRtp(); }
  bool CheckNoRtcp1() { return media_channel1_->CheckNoRtcp(); }
  bool CheckNoRtcp2() { return media_channel2_->CheckNoRtcp(); }

  void CreateContent(int flags,
                     const cricket::AudioCodec& audio_codec,
                     const cricket::VideoCodec& video_codec,
                     typename T::Content* content) {
    // overridden in specialized classes
  }
  void CopyContent(const typename T::Content& source,
                   typename T::Content* content) {
    // overridden in specialized classes
  }

  // Creates a MediaContent with one stream.
  // kPcmuCodec is used as audio codec and kH264Codec is used as video codec.
  typename T::Content* CreateMediaContentWithStream(uint32_t ssrc) {
    typename T::Content* content = new typename T::Content();
    CreateContent(0, kPcmuCodec, kH264Codec, content);
    AddLegacyStreamInContent(ssrc, 0, content);
    return content;
  }

  // Will manage the lifetime of a CallThread, making sure it's
  // destroyed before this object goes out of scope.
  class ScopedCallThread {
   public:
    template <class FunctorT>
    explicit ScopedCallThread(const FunctorT& functor)
        : thread_(rtc::Thread::Create()),
          task_(new rtc::FunctorMessageHandler<void, FunctorT>(functor)) {
      thread_->Start();
      thread_->Post(RTC_FROM_HERE, task_.get());
    }

    ~ScopedCallThread() { thread_->Stop(); }

    rtc::Thread* thread() { return thread_.get(); }

   private:
    std::unique_ptr<rtc::Thread> thread_;
    std::unique_ptr<rtc::MessageHandler> task_;
  };

  bool CodecMatches(const typename T::Codec& c1, const typename T::Codec& c2) {
    return false;  // overridden in specialized classes
  }

  void OnRtcpMuxFullyActive1(const std::string&) {
    rtcp_mux_activated_callbacks1_++;
  }
  void OnRtcpMuxFullyActive2(const std::string&) {
    rtcp_mux_activated_callbacks2_++;
  }

  cricket::CandidatePairInterface* last_selected_candidate_pair() {
    return last_selected_candidate_pair_;
  }

  void AddLegacyStreamInContent(uint32_t ssrc,
                                int flags,
                                typename T::Content* content) {
    // Base implementation.
  }

  // Tests that can be used by derived classes.

  // Basic sanity check.
  void TestInit() {
    CreateChannels(0, 0);
    EXPECT_FALSE(channel1_->srtp_active());
    EXPECT_FALSE(media_channel1_->sending());
    if (verify_playout_) {
      EXPECT_FALSE(media_channel1_->playout());
    }
    EXPECT_TRUE(media_channel1_->codecs().empty());
    EXPECT_TRUE(media_channel1_->recv_streams().empty());
    EXPECT_TRUE(media_channel1_->rtp_packets().empty());
    EXPECT_TRUE(media_channel1_->rtcp_packets().empty());
  }

  // Test that SetLocalContent and SetRemoteContent properly configure
  // the codecs.
  void TestSetContents() {
    CreateChannels(0, 0);
    typename T::Content content;
    CreateContent(0, kPcmuCodec, kH264Codec, &content);
    EXPECT_TRUE(channel1_->SetLocalContent(&content, SdpType::kOffer, NULL));
    EXPECT_EQ(0U, media_channel1_->codecs().size());
    EXPECT_TRUE(channel1_->SetRemoteContent(&content, SdpType::kAnswer, NULL));
    ASSERT_EQ(1U, media_channel1_->codecs().size());
    EXPECT_TRUE(
        CodecMatches(content.codecs()[0], media_channel1_->codecs()[0]));
  }

  // Test that SetLocalContent and SetRemoteContent properly deals
  // with an empty offer.
  void TestSetContentsNullOffer() {
    CreateChannels(0, 0);
    typename T::Content content;
    EXPECT_TRUE(channel1_->SetLocalContent(&content, SdpType::kOffer, NULL));
    CreateContent(0, kPcmuCodec, kH264Codec, &content);
    EXPECT_EQ(0U, media_channel1_->codecs().size());
    EXPECT_TRUE(channel1_->SetRemoteContent(&content, SdpType::kAnswer, NULL));
    ASSERT_EQ(1U, media_channel1_->codecs().size());
    EXPECT_TRUE(
        CodecMatches(content.codecs()[0], media_channel1_->codecs()[0]));
  }

  // Test that SetLocalContent and SetRemoteContent properly set RTCP
  // mux.
  void TestSetContentsRtcpMux() {
    CreateChannels(0, 0);
    typename T::Content content;
    CreateContent(0, kPcmuCodec, kH264Codec, &content);
    // Both sides agree on mux. Should no longer be a separate RTCP channel.
    content.set_rtcp_mux(true);
    EXPECT_TRUE(channel1_->SetLocalContent(&content, SdpType::kOffer, NULL));
    EXPECT_TRUE(channel1_->SetRemoteContent(&content, SdpType::kAnswer, NULL));
    // Only initiator supports mux. Should still have a separate RTCP channel.
    EXPECT_TRUE(channel2_->SetLocalContent(&content, SdpType::kOffer, NULL));
    content.set_rtcp_mux(false);
    EXPECT_TRUE(channel2_->SetRemoteContent(&content, SdpType::kAnswer, NULL));
  }

  // Test that SetLocalContent and SetRemoteContent properly set RTCP
  // mux when a provisional answer is received.
  void TestSetContentsRtcpMuxWithPrAnswer() {
    CreateChannels(0, 0);
    typename T::Content content;
    CreateContent(0, kPcmuCodec, kH264Codec, &content);
    content.set_rtcp_mux(true);
    EXPECT_TRUE(channel1_->SetLocalContent(&content, SdpType::kOffer, NULL));
    EXPECT_TRUE(
        channel1_->SetRemoteContent(&content, SdpType::kPrAnswer, NULL));
    // Both sides agree on mux. Should signal RTCP mux as fully activated.
    EXPECT_EQ(0, rtcp_mux_activated_callbacks1_);
    EXPECT_TRUE(channel1_->SetRemoteContent(&content, SdpType::kAnswer, NULL));
    EXPECT_EQ(1, rtcp_mux_activated_callbacks1_);
    // Only initiator supports mux. Should still have a separate RTCP channel.
    EXPECT_TRUE(channel2_->SetLocalContent(&content, SdpType::kOffer, NULL));
    content.set_rtcp_mux(false);
    EXPECT_TRUE(
        channel2_->SetRemoteContent(&content, SdpType::kPrAnswer, NULL));
    EXPECT_TRUE(channel2_->SetRemoteContent(&content, SdpType::kAnswer, NULL));
    EXPECT_EQ(0, rtcp_mux_activated_callbacks2_);
  }

  // Test that Add/RemoveStream properly forward to the media channel.
  void TestStreams() {
    CreateChannels(0, 0);
    EXPECT_TRUE(AddStream1(1));
    EXPECT_TRUE(AddStream1(2));
    EXPECT_EQ(2U, media_channel1_->recv_streams().size());
    EXPECT_TRUE(RemoveStream1(2));
    EXPECT_EQ(1U, media_channel1_->recv_streams().size());
    EXPECT_TRUE(RemoveStream1(1));
    EXPECT_EQ(0U, media_channel1_->recv_streams().size());
  }

  // Test that SetLocalContent and SetRemoteContent properly
  // handles adding and removing StreamParams when the action is a full
  // SdpType::kOffer / SdpType::kAnswer.
  void TestChangeStreamParamsInContent() {
    cricket::StreamParams stream1;
    stream1.groupid = "group1";
    stream1.id = "stream1";
    stream1.ssrcs.push_back(kSsrc1);
    stream1.cname = "stream1_cname";

    cricket::StreamParams stream2;
    stream2.groupid = "group1";
    stream2.id = "stream2";
    stream2.ssrcs.push_back(kSsrc2);
    stream2.cname = "stream2_cname";

    // Setup a call where channel 1 send |stream1| to channel 2.
    CreateChannels(0, 0);
    typename T::Content content1;
    CreateContent(0, kPcmuCodec, kH264Codec, &content1);
    content1.AddStream(stream1);
    EXPECT_TRUE(channel1_->SetLocalContent(&content1, SdpType::kOffer, NULL));
    EXPECT_TRUE(channel1_->Enable(true));
    EXPECT_EQ(1u, media_channel1_->send_streams().size());

    EXPECT_TRUE(channel2_->SetRemoteContent(&content1, SdpType::kOffer, NULL));
    EXPECT_EQ(1u, media_channel2_->recv_streams().size());
    ConnectFakeTransports();

    // Channel 2 do not send anything.
    typename T::Content content2;
    CreateContent(0, kPcmuCodec, kH264Codec, &content2);
    EXPECT_TRUE(channel1_->SetRemoteContent(&content2, SdpType::kAnswer, NULL));
    EXPECT_EQ(0u, media_channel1_->recv_streams().size());
    EXPECT_TRUE(channel2_->SetLocalContent(&content2, SdpType::kAnswer, NULL));
    EXPECT_TRUE(channel2_->Enable(true));
    EXPECT_EQ(0u, media_channel2_->send_streams().size());

    SendCustomRtp1(kSsrc1, 0);
    WaitForThreads();
    EXPECT_TRUE(CheckCustomRtp2(kSsrc1, 0));

    // Let channel 2 update the content by sending |stream2| and enable SRTP.
    typename T::Content content3;
    CreateContent(0, kPcmuCodec, kH264Codec, &content3);
    content3.AddStream(stream2);
    EXPECT_TRUE(channel2_->SetLocalContent(&content3, SdpType::kOffer, NULL));
    ASSERT_EQ(1u, media_channel2_->send_streams().size());
    EXPECT_EQ(stream2, media_channel2_->send_streams()[0]);

    EXPECT_TRUE(channel1_->SetRemoteContent(&content3, SdpType::kOffer, NULL));
    ASSERT_EQ(1u, media_channel1_->recv_streams().size());
    EXPECT_EQ(stream2, media_channel1_->recv_streams()[0]);

    // Channel 1 replies but stop sending stream1.
    typename T::Content content4;
    CreateContent(0, kPcmuCodec, kH264Codec, &content4);
    EXPECT_TRUE(channel1_->SetLocalContent(&content4, SdpType::kAnswer, NULL));
    EXPECT_EQ(0u, media_channel1_->send_streams().size());

    EXPECT_TRUE(channel2_->SetRemoteContent(&content4, SdpType::kAnswer, NULL));
    EXPECT_EQ(0u, media_channel2_->recv_streams().size());

    SendCustomRtp2(kSsrc2, 0);
    WaitForThreads();
    EXPECT_TRUE(CheckCustomRtp1(kSsrc2, 0));
  }

  // Test that we only start playout and sending at the right times.
  void TestPlayoutAndSendingStates() {
    CreateChannels(0, 0);
    if (verify_playout_) {
      EXPECT_FALSE(media_channel1_->playout());
    }
    EXPECT_FALSE(media_channel1_->sending());
    if (verify_playout_) {
      EXPECT_FALSE(media_channel2_->playout());
    }
    EXPECT_FALSE(media_channel2_->sending());
    EXPECT_TRUE(channel1_->Enable(true));
    if (verify_playout_) {
      EXPECT_FALSE(media_channel1_->playout());
    }
    EXPECT_FALSE(media_channel1_->sending());
    EXPECT_TRUE(channel1_->SetLocalContent(&local_media_content1_,
                                           SdpType::kOffer, NULL));
    if (verify_playout_) {
      EXPECT_TRUE(media_channel1_->playout());
    }
    EXPECT_FALSE(media_channel1_->sending());
    EXPECT_TRUE(channel2_->SetRemoteContent(&local_media_content1_,
                                            SdpType::kOffer, NULL));
    if (verify_playout_) {
      EXPECT_FALSE(media_channel2_->playout());
    }
    EXPECT_FALSE(media_channel2_->sending());
    EXPECT_TRUE(channel2_->SetLocalContent(&local_media_content2_,
                                           SdpType::kAnswer, NULL));
    if (verify_playout_) {
      EXPECT_FALSE(media_channel2_->playout());
    }
    EXPECT_FALSE(media_channel2_->sending());
    ConnectFakeTransports();
    if (verify_playout_) {
      EXPECT_TRUE(media_channel1_->playout());
    }
    EXPECT_FALSE(media_channel1_->sending());
    if (verify_playout_) {
      EXPECT_FALSE(media_channel2_->playout());
    }
    EXPECT_FALSE(media_channel2_->sending());
    EXPECT_TRUE(channel2_->Enable(true));
    if (verify_playout_) {
      EXPECT_TRUE(media_channel2_->playout());
    }
    EXPECT_TRUE(media_channel2_->sending());
    EXPECT_TRUE(channel1_->SetRemoteContent(&local_media_content2_,
                                            SdpType::kAnswer, NULL));
    if (verify_playout_) {
      EXPECT_TRUE(media_channel1_->playout());
    }
    EXPECT_TRUE(media_channel1_->sending());
  }

  // Test that changing the MediaContentDirection in the local and remote
  // session description start playout and sending at the right time.
  void TestMediaContentDirection() {
    CreateChannels(0, 0);
    typename T::Content content1;
    CreateContent(0, kPcmuCodec, kH264Codec, &content1);
    typename T::Content content2;
    CreateContent(0, kPcmuCodec, kH264Codec, &content2);
    // Set |content2| to be InActive.
    content2.set_direction(RtpTransceiverDirection::kInactive);

    EXPECT_TRUE(channel1_->Enable(true));
    EXPECT_TRUE(channel2_->Enable(true));
    if (verify_playout_) {
      EXPECT_FALSE(media_channel1_->playout());
    }
    EXPECT_FALSE(media_channel1_->sending());
    if (verify_playout_) {
      EXPECT_FALSE(media_channel2_->playout());
    }
    EXPECT_FALSE(media_channel2_->sending());

    EXPECT_TRUE(channel1_->SetLocalContent(&content1, SdpType::kOffer, NULL));
    EXPECT_TRUE(channel2_->SetRemoteContent(&content1, SdpType::kOffer, NULL));
    EXPECT_TRUE(
        channel2_->SetLocalContent(&content2, SdpType::kPrAnswer, NULL));
    EXPECT_TRUE(
        channel1_->SetRemoteContent(&content2, SdpType::kPrAnswer, NULL));
    ConnectFakeTransports();

    if (verify_playout_) {
      EXPECT_TRUE(media_channel1_->playout());
    }
    EXPECT_FALSE(media_channel1_->sending());  // remote InActive
    if (verify_playout_) {
      EXPECT_FALSE(media_channel2_->playout());  // local InActive
    }
    EXPECT_FALSE(media_channel2_->sending());  // local InActive

    // Update |content2| to be RecvOnly.
    content2.set_direction(RtpTransceiverDirection::kRecvOnly);
    EXPECT_TRUE(
        channel2_->SetLocalContent(&content2, SdpType::kPrAnswer, NULL));
    EXPECT_TRUE(
        channel1_->SetRemoteContent(&content2, SdpType::kPrAnswer, NULL));

    if (verify_playout_) {
      EXPECT_TRUE(media_channel1_->playout());
    }
    EXPECT_TRUE(media_channel1_->sending());
    if (verify_playout_) {
      EXPECT_TRUE(media_channel2_->playout());  // local RecvOnly
    }
    EXPECT_FALSE(media_channel2_->sending());  // local RecvOnly

    // Update |content2| to be SendRecv.
    content2.set_direction(RtpTransceiverDirection::kSendRecv);
    EXPECT_TRUE(channel2_->SetLocalContent(&content2, SdpType::kAnswer, NULL));
    EXPECT_TRUE(channel1_->SetRemoteContent(&content2, SdpType::kAnswer, NULL));

    if (verify_playout_) {
      EXPECT_TRUE(media_channel1_->playout());
    }
    EXPECT_TRUE(media_channel1_->sending());
    if (verify_playout_) {
      EXPECT_TRUE(media_channel2_->playout());
    }
    EXPECT_TRUE(media_channel2_->sending());
  }

  // Tests that when the transport channel signals a candidate pair change
  // event, the media channel will receive a call on the network route change.
  void TestNetworkRouteChanges() {
    static constexpr uint16_t kLocalNetId = 1;
    static constexpr uint16_t kRemoteNetId = 2;
    static constexpr int kLastPacketId = 100;
    // Ipv4(20) + UDP(8).
    static constexpr int kTransportOverheadPerPacket = 28;
    static constexpr int kSrtpOverheadPerPacket = 10;

    CreateChannels(DTLS, DTLS);
    SendInitiate();

    typename T::MediaChannel* media_channel1 =
        static_cast<typename T::MediaChannel*>(channel1_->media_channel());
    ASSERT_TRUE(media_channel1);

    // Need to wait for the threads before calling
    // |set_num_network_route_changes| because the network route would be set
    // when creating the channel.
    WaitForThreads();
    media_channel1->set_num_network_route_changes(0);
    network_thread_->Invoke<void>(RTC_FROM_HERE, [this] {
      rtc::NetworkRoute network_route;
      // The transport channel becomes disconnected.
      fake_rtp_dtls_transport1_->ice_transport()->SignalNetworkRouteChanged(

          absl::optional<rtc::NetworkRoute>(network_route));
    });
    WaitForThreads();
    EXPECT_EQ(1, media_channel1->num_network_route_changes());
    EXPECT_FALSE(media_channel1->last_network_route().connected);
    media_channel1->set_num_network_route_changes(0);

    network_thread_->Invoke<void>(RTC_FROM_HERE, [this] {
      rtc::NetworkRoute network_route;
      network_route.connected = true;
      network_route.local_network_id = kLocalNetId;
      network_route.remote_network_id = kRemoteNetId;
      network_route.last_sent_packet_id = kLastPacketId;
      network_route.packet_overhead = kTransportOverheadPerPacket;
      // The transport channel becomes connected.
      fake_rtp_dtls_transport1_->ice_transport()->SignalNetworkRouteChanged(

          absl::optional<rtc::NetworkRoute>(network_route));
    });
    WaitForThreads();
    EXPECT_EQ(1, media_channel1->num_network_route_changes());
    rtc::NetworkRoute expected_network_route;
    expected_network_route.connected = true;
    expected_network_route.local_network_id = kLocalNetId;
    expected_network_route.remote_network_id = kRemoteNetId;
    expected_network_route.last_sent_packet_id = kLastPacketId;
    EXPECT_EQ(expected_network_route, media_channel1->last_network_route());
    EXPECT_EQ(kLastPacketId,
              media_channel1->last_network_route().last_sent_packet_id);
    EXPECT_EQ(kTransportOverheadPerPacket + kSrtpOverheadPerPacket,
              media_channel1->transport_overhead_per_packet());
  }

  // Test setting up a call.
  void TestCallSetup() {
    CreateChannels(0, 0);
    EXPECT_FALSE(channel1_->srtp_active());
    EXPECT_TRUE(SendInitiate());
    if (verify_playout_) {
      EXPECT_TRUE(media_channel1_->playout());
    }
    EXPECT_FALSE(media_channel1_->sending());
    EXPECT_TRUE(SendAccept());
    EXPECT_FALSE(channel1_->srtp_active());
    EXPECT_TRUE(media_channel1_->sending());
    EXPECT_EQ(1U, media_channel1_->codecs().size());
    if (verify_playout_) {
      EXPECT_TRUE(media_channel2_->playout());
    }
    EXPECT_TRUE(media_channel2_->sending());
    EXPECT_EQ(1U, media_channel2_->codecs().size());
  }

  // Test that we don't crash if packets are sent during call teardown
  // when RTCP mux is enabled. This is a regression test against a specific
  // race condition that would only occur when a RTCP packet was sent during
  // teardown of a channel on which RTCP mux was enabled.
  void TestCallTeardownRtcpMux() {
    class LastWordMediaChannel : public T::MediaChannel {
     public:
      LastWordMediaChannel() : T::MediaChannel(NULL, typename T::Options()) {}
      ~LastWordMediaChannel() {
        T::MediaChannel::SendRtp(kPcmuFrame, sizeof(kPcmuFrame),
                                 rtc::PacketOptions());
        T::MediaChannel::SendRtcp(kRtcpReport, sizeof(kRtcpReport));
      }
    };
    CreateChannels(absl::make_unique<LastWordMediaChannel>(),
                   absl::make_unique<LastWordMediaChannel>(), RTCP_MUX,
                   RTCP_MUX);
    EXPECT_TRUE(SendInitiate());
    EXPECT_TRUE(SendAccept());
    EXPECT_TRUE(Terminate());
  }

  // Send voice RTP data to the other side and ensure it gets there.
  void SendRtpToRtp() {
    CreateChannels(RTCP_MUX, RTCP_MUX);
    EXPECT_TRUE(SendInitiate());
    EXPECT_TRUE(SendAccept());
    EXPECT_EQ(nullptr, channel1_->rtcp_packet_transport());
    EXPECT_EQ(nullptr, channel2_->rtcp_packet_transport());
    SendRtp1();
    SendRtp2();
    WaitForThreads();
    EXPECT_TRUE(CheckRtp1());
    EXPECT_TRUE(CheckRtp2());
    EXPECT_TRUE(CheckNoRtp1());
    EXPECT_TRUE(CheckNoRtp2());
  }

  void TestDeinit() {
    CreateChannels(0, 0);
    EXPECT_TRUE(SendInitiate());
    EXPECT_TRUE(SendAccept());
    SendRtp1();
    SendRtp2();
    SendRtcp1();
    SendRtcp2();
    // Do not wait, destroy channels.
    channel1_.reset(nullptr);
    channel2_.reset(nullptr);
  }

  // Check that RTCP can be transmitted between both sides.
  void SendRtcpToRtcp() {
    CreateChannels(0, 0);
    EXPECT_TRUE(SendInitiate());
    EXPECT_TRUE(SendAccept());
    EXPECT_NE(nullptr, channel1_->rtcp_packet_transport());
    EXPECT_NE(nullptr, channel2_->rtcp_packet_transport());
    SendRtcp1();
    SendRtcp2();
    WaitForThreads();
    EXPECT_TRUE(CheckRtcp1());
    EXPECT_TRUE(CheckRtcp2());
    EXPECT_TRUE(CheckNoRtcp1());
    EXPECT_TRUE(CheckNoRtcp2());
  }

  void SendDtlsSrtpToDtlsSrtp(int flags1, int flags2) {
    CreateChannels(flags1 | DTLS, flags2 | DTLS);
    EXPECT_FALSE(channel1_->srtp_active());
    EXPECT_FALSE(channel2_->srtp_active());
    EXPECT_TRUE(SendInitiate());
    WaitForThreads();
    EXPECT_TRUE(channel1_->writable());
    EXPECT_TRUE(channel2_->writable());
    EXPECT_TRUE(SendAccept());
    EXPECT_TRUE(channel1_->srtp_active());
    EXPECT_TRUE(channel2_->srtp_active());
    SendRtp1();
    SendRtp2();
    SendRtcp1();
    SendRtcp2();
    WaitForThreads();
    EXPECT_TRUE(CheckRtp1());
    EXPECT_TRUE(CheckRtp2());
    EXPECT_TRUE(CheckNoRtp1());
    EXPECT_TRUE(CheckNoRtp2());
    EXPECT_TRUE(CheckRtcp1());
    EXPECT_TRUE(CheckRtcp2());
    EXPECT_TRUE(CheckNoRtcp1());
    EXPECT_TRUE(CheckNoRtcp2());
  }

  // Test that we can send and receive early media when a provisional answer is
  // sent and received. The test uses SRTP, RTCP mux and SSRC mux.
  void SendEarlyMediaUsingRtcpMuxSrtp() {
    int sequence_number1_1 = 0, sequence_number2_2 = 0;

    CreateChannels(SSRC_MUX | RTCP_MUX | DTLS, SSRC_MUX | RTCP_MUX | DTLS);
    EXPECT_TRUE(SendOffer());
    EXPECT_TRUE(SendProvisionalAnswer());
    EXPECT_TRUE(channel1_->srtp_active());
    EXPECT_TRUE(channel2_->srtp_active());
    EXPECT_EQ(nullptr, channel1_->rtcp_packet_transport());
    EXPECT_EQ(nullptr, channel2_->rtcp_packet_transport());
    WaitForThreads();  // Wait for 'sending' flag go through network thread.
    SendCustomRtcp1(kSsrc1);
    SendCustomRtp1(kSsrc1, ++sequence_number1_1);
    WaitForThreads();
    EXPECT_TRUE(CheckCustomRtcp2(kSsrc1));
    EXPECT_TRUE(CheckCustomRtp2(kSsrc1, sequence_number1_1));

    // Send packets from callee and verify that it is received.
    SendCustomRtcp2(kSsrc2);
    SendCustomRtp2(kSsrc2, ++sequence_number2_2);
    WaitForThreads();
    EXPECT_TRUE(CheckCustomRtcp1(kSsrc2));
    EXPECT_TRUE(CheckCustomRtp1(kSsrc2, sequence_number2_2));

    // Complete call setup and ensure everything is still OK.
    EXPECT_TRUE(SendFinalAnswer());
    EXPECT_TRUE(channel1_->srtp_active());
    EXPECT_TRUE(channel2_->srtp_active());
    SendCustomRtcp1(kSsrc1);
    SendCustomRtp1(kSsrc1, ++sequence_number1_1);
    SendCustomRtcp2(kSsrc2);
    SendCustomRtp2(kSsrc2, ++sequence_number2_2);
    WaitForThreads();
    EXPECT_TRUE(CheckCustomRtcp2(kSsrc1));
    EXPECT_TRUE(CheckCustomRtp2(kSsrc1, sequence_number1_1));
    EXPECT_TRUE(CheckCustomRtcp1(kSsrc2));
    EXPECT_TRUE(CheckCustomRtp1(kSsrc2, sequence_number2_2));
  }

  // Test that we properly send RTP without SRTP from a thread.
  void SendRtpToRtpOnThread() {
    CreateChannels(0, 0);
    EXPECT_TRUE(SendInitiate());
    EXPECT_TRUE(SendAccept());
    ScopedCallThread send_rtp1([this] { SendRtp1(); });
    ScopedCallThread send_rtp2([this] { SendRtp2(); });
    ScopedCallThread send_rtcp1([this] { SendRtcp1(); });
    ScopedCallThread send_rtcp2([this] { SendRtcp2(); });
    rtc::Thread* involved_threads[] = {send_rtp1.thread(), send_rtp2.thread(),
                                       send_rtcp1.thread(),
                                       send_rtcp2.thread()};
    WaitForThreads(involved_threads);
    EXPECT_TRUE(CheckRtp1());
    EXPECT_TRUE(CheckRtp2());
    EXPECT_TRUE(CheckNoRtp1());
    EXPECT_TRUE(CheckNoRtp2());
    EXPECT_TRUE(CheckRtcp1());
    EXPECT_TRUE(CheckRtcp2());
    EXPECT_TRUE(CheckNoRtcp1());
    EXPECT_TRUE(CheckNoRtcp2());
  }

  // Test that the mediachannel retains its sending state after the transport
  // becomes non-writable.
  void SendWithWritabilityLoss() {
    CreateChannels(RTCP_MUX, RTCP_MUX);
    EXPECT_TRUE(SendInitiate());
    EXPECT_TRUE(SendAccept());
    EXPECT_EQ(nullptr, channel1_->rtcp_packet_transport());
    EXPECT_EQ(nullptr, channel2_->rtcp_packet_transport());
    SendRtp1();
    SendRtp2();
    WaitForThreads();
    EXPECT_TRUE(CheckRtp1());
    EXPECT_TRUE(CheckRtp2());
    EXPECT_TRUE(CheckNoRtp1());
    EXPECT_TRUE(CheckNoRtp2());

    // Lose writability, which should fail.
    network_thread_->Invoke<void>(RTC_FROM_HERE, [this] {
      fake_rtp_dtls_transport1_->SetWritable(false);
    });
    SendRtp1();
    SendRtp2();
    WaitForThreads();
    EXPECT_TRUE(CheckRtp1());
    EXPECT_TRUE(CheckNoRtp2());

    // Regain writability
    network_thread_->Invoke<void>(RTC_FROM_HERE, [this] {
      fake_rtp_dtls_transport1_->SetWritable(true);
    });
    EXPECT_TRUE(media_channel1_->sending());
    SendRtp1();
    SendRtp2();
    WaitForThreads();
    EXPECT_TRUE(CheckRtp1());
    EXPECT_TRUE(CheckRtp2());
    EXPECT_TRUE(CheckNoRtp1());
    EXPECT_TRUE(CheckNoRtp2());

    // Lose writability completely
    network_thread_->Invoke<void>(RTC_FROM_HERE, [this] {
      bool asymmetric = true;
      fake_rtp_dtls_transport1_->SetDestination(nullptr, asymmetric);
    });
    EXPECT_TRUE(media_channel1_->sending());

    // Should fail also.
    SendRtp1();
    SendRtp2();
    WaitForThreads();
    EXPECT_TRUE(CheckRtp1());
    EXPECT_TRUE(CheckNoRtp2());
    EXPECT_TRUE(CheckNoRtp1());

    // Gain writability back
    network_thread_->Invoke<void>(RTC_FROM_HERE, [this] {
      bool asymmetric = true;
      fake_rtp_dtls_transport1_->SetDestination(fake_rtp_dtls_transport2_.get(),
                                                asymmetric);
    });
    EXPECT_TRUE(media_channel1_->sending());
    SendRtp1();
    SendRtp2();
    WaitForThreads();
    EXPECT_TRUE(CheckRtp1());
    EXPECT_TRUE(CheckRtp2());
    EXPECT_TRUE(CheckNoRtp1());
    EXPECT_TRUE(CheckNoRtp2());
  }

  void SendBundleToBundle(const int* pl_types,
                          int len,
                          bool rtcp_mux,
                          bool secure) {
    ASSERT_EQ(2, len);
    int sequence_number1_1 = 0, sequence_number2_2 = 0;
    // Only pl_type1 was added to the bundle filter for both |channel1_|
    // and |channel2_|.
    int pl_type1 = pl_types[0];
    int pl_type2 = pl_types[1];
    int flags = SSRC_MUX;
    if (secure)
      flags |= DTLS;
    if (rtcp_mux) {
      flags |= RTCP_MUX;
    }
    CreateChannels(flags, flags);
    EXPECT_TRUE(SendInitiate());
    EXPECT_TRUE(SendAccept());

    // Both channels can receive pl_type1 only.
    SendCustomRtp1(kSsrc1, ++sequence_number1_1, pl_type1);
    SendCustomRtp2(kSsrc2, ++sequence_number2_2, pl_type1);
    WaitForThreads();
    EXPECT_TRUE(CheckCustomRtp2(kSsrc1, sequence_number1_1, pl_type1));
    EXPECT_TRUE(CheckCustomRtp1(kSsrc2, sequence_number2_2, pl_type1));
    EXPECT_TRUE(CheckNoRtp1());
    EXPECT_TRUE(CheckNoRtp2());

    SendCustomRtp1(kSsrc3, ++sequence_number1_1, pl_type2);
    SendCustomRtp2(kSsrc4, ++sequence_number2_2, pl_type2);
    WaitForThreads();
    EXPECT_FALSE(CheckCustomRtp2(kSsrc3, sequence_number1_1, pl_type2));
    EXPECT_FALSE(CheckCustomRtp1(kSsrc4, sequence_number2_2, pl_type2));

    // RTCP test
    SendCustomRtcp1(kSsrc1);
    SendCustomRtcp2(kSsrc2);
    WaitForThreads();
    EXPECT_TRUE(CheckCustomRtcp1(kSsrc2));
    EXPECT_TRUE(CheckNoRtcp1());
    EXPECT_TRUE(CheckCustomRtcp2(kSsrc1));
    EXPECT_TRUE(CheckNoRtcp2());

    SendCustomRtcp1(kSsrc2);
    SendCustomRtcp2(kSsrc1);
    WaitForThreads();
    // Bundle filter shouldn't filter out any RTCP.
    EXPECT_TRUE(CheckCustomRtcp1(kSsrc1));
    EXPECT_TRUE(CheckCustomRtcp2(kSsrc2));
  }

  void TestSetContentFailure() {
    CreateChannels(0, 0);

    std::string err;
    std::unique_ptr<typename T::Content> content(
        CreateMediaContentWithStream(1));

    media_channel1_->set_fail_set_recv_codecs(true);
    EXPECT_FALSE(
        channel1_->SetLocalContent(content.get(), SdpType::kOffer, &err));
    EXPECT_FALSE(
        channel1_->SetLocalContent(content.get(), SdpType::kAnswer, &err));

    media_channel1_->set_fail_set_send_codecs(true);
    EXPECT_FALSE(
        channel1_->SetRemoteContent(content.get(), SdpType::kOffer, &err));

    media_channel1_->set_fail_set_send_codecs(true);
    EXPECT_FALSE(
        channel1_->SetRemoteContent(content.get(), SdpType::kAnswer, &err));
  }

  void TestSendTwoOffers() {
    CreateChannels(0, 0);

    std::string err;
    std::unique_ptr<typename T::Content> content1(
        CreateMediaContentWithStream(1));
    EXPECT_TRUE(
        channel1_->SetLocalContent(content1.get(), SdpType::kOffer, &err));
    EXPECT_TRUE(media_channel1_->HasSendStream(1));

    std::unique_ptr<typename T::Content> content2(
        CreateMediaContentWithStream(2));
    EXPECT_TRUE(
        channel1_->SetLocalContent(content2.get(), SdpType::kOffer, &err));
    EXPECT_FALSE(media_channel1_->HasSendStream(1));
    EXPECT_TRUE(media_channel1_->HasSendStream(2));
  }

  void TestReceiveTwoOffers() {
    CreateChannels(0, 0);

    std::string err;
    std::unique_ptr<typename T::Content> content1(
        CreateMediaContentWithStream(1));
    EXPECT_TRUE(
        channel1_->SetRemoteContent(content1.get(), SdpType::kOffer, &err));
    EXPECT_TRUE(media_channel1_->HasRecvStream(1));

    std::unique_ptr<typename T::Content> content2(
        CreateMediaContentWithStream(2));
    EXPECT_TRUE(
        channel1_->SetRemoteContent(content2.get(), SdpType::kOffer, &err));
    EXPECT_FALSE(media_channel1_->HasRecvStream(1));
    EXPECT_TRUE(media_channel1_->HasRecvStream(2));
  }

  void TestSendPrAnswer() {
    CreateChannels(0, 0);

    std::string err;
    // Receive offer
    std::unique_ptr<typename T::Content> content1(
        CreateMediaContentWithStream(1));
    EXPECT_TRUE(
        channel1_->SetRemoteContent(content1.get(), SdpType::kOffer, &err));
    EXPECT_TRUE(media_channel1_->HasRecvStream(1));

    // Send PR answer
    std::unique_ptr<typename T::Content> content2(
        CreateMediaContentWithStream(2));
    EXPECT_TRUE(
        channel1_->SetLocalContent(content2.get(), SdpType::kPrAnswer, &err));
    EXPECT_TRUE(media_channel1_->HasRecvStream(1));
    EXPECT_TRUE(media_channel1_->HasSendStream(2));

    // Send answer
    std::unique_ptr<typename T::Content> content3(
        CreateMediaContentWithStream(3));
    EXPECT_TRUE(
        channel1_->SetLocalContent(content3.get(), SdpType::kAnswer, &err));
    EXPECT_TRUE(media_channel1_->HasRecvStream(1));
    EXPECT_FALSE(media_channel1_->HasSendStream(2));
    EXPECT_TRUE(media_channel1_->HasSendStream(3));
  }

  void TestReceivePrAnswer() {
    CreateChannels(0, 0);

    std::string err;
    // Send offer
    std::unique_ptr<typename T::Content> content1(
        CreateMediaContentWithStream(1));
    EXPECT_TRUE(
        channel1_->SetLocalContent(content1.get(), SdpType::kOffer, &err));
    EXPECT_TRUE(media_channel1_->HasSendStream(1));

    // Receive PR answer
    std::unique_ptr<typename T::Content> content2(
        CreateMediaContentWithStream(2));
    EXPECT_TRUE(
        channel1_->SetRemoteContent(content2.get(), SdpType::kPrAnswer, &err));
    EXPECT_TRUE(media_channel1_->HasSendStream(1));
    EXPECT_TRUE(media_channel1_->HasRecvStream(2));

    // Receive answer
    std::unique_ptr<typename T::Content> content3(
        CreateMediaContentWithStream(3));
    EXPECT_TRUE(
        channel1_->SetRemoteContent(content3.get(), SdpType::kAnswer, &err));
    EXPECT_TRUE(media_channel1_->HasSendStream(1));
    EXPECT_FALSE(media_channel1_->HasRecvStream(2));
    EXPECT_TRUE(media_channel1_->HasRecvStream(3));
  }

  void TestFlushRtcp() {
    CreateChannels(0, 0);
    EXPECT_TRUE(SendInitiate());
    EXPECT_TRUE(SendAccept());
    EXPECT_NE(nullptr, channel1_->rtcp_packet_transport());
    EXPECT_NE(nullptr, channel2_->rtcp_packet_transport());

    // Send RTCP1 from a different thread.
    ScopedCallThread send_rtcp([this] { SendRtcp1(); });
    // The sending message is only posted.  channel2_ should be empty.
    EXPECT_TRUE(CheckNoRtcp2());
    rtc::Thread* wait_for[] = {send_rtcp.thread()};
    WaitForThreads(wait_for);  // Ensure rtcp was posted

    // When channel1_ is deleted, the RTCP packet should be sent out to
    // channel2_.
    channel1_.reset();
    WaitForThreads();
    EXPECT_TRUE(CheckRtcp2());
  }

  void TestOnTransportReadyToSend() {
    CreateChannels(0, 0);
    EXPECT_FALSE(media_channel1_->ready_to_send());

    channel1_->OnTransportReadyToSend(true);
    WaitForThreads();
    EXPECT_TRUE(media_channel1_->ready_to_send());

    channel1_->OnTransportReadyToSend(false);
    WaitForThreads();
    EXPECT_FALSE(media_channel1_->ready_to_send());
  }

  bool SetRemoteContentWithBitrateLimit(int remote_limit) {
    typename T::Content content;
    CreateContent(0, kPcmuCodec, kH264Codec, &content);
    content.set_bandwidth(remote_limit);
    return channel1_->SetRemoteContent(&content, SdpType::kOffer, NULL);
  }

  webrtc::RtpParameters BitrateLimitedParameters(absl::optional<int> limit) {
    webrtc::RtpParameters parameters;
    webrtc::RtpEncodingParameters encoding;
    encoding.max_bitrate_bps = std::move(limit);
    parameters.encodings.push_back(encoding);
    return parameters;
  }

  void VerifyMaxBitrate(const webrtc::RtpParameters& parameters,
                        absl::optional<int> expected_bitrate) {
    EXPECT_EQ(1UL, parameters.encodings.size());
    EXPECT_EQ(expected_bitrate, parameters.encodings[0].max_bitrate_bps);
  }

  void DefaultMaxBitrateIsUnlimited() {
    CreateChannels(0, 0);
    EXPECT_TRUE(channel1_->SetLocalContent(&local_media_content1_,
                                           SdpType::kOffer, NULL));
    EXPECT_EQ(media_channel1_->max_bps(), -1);
    VerifyMaxBitrate(media_channel1_->GetRtpSendParameters(kSsrc1),
                     absl::nullopt);
  }

  // Test that when a channel gets new RtpTransport with a call to
  // |SetRtpTransport|, the socket options from the old RtpTransport is merged
  // with the options on the new one.

  // For example, audio and video may use separate socket options, but initially
  // be unbundled, then later become bundled. When this happens, their preferred
  // socket options should be merged to the underlying transport they share.
  void SocketOptionsMergedOnSetTransport() {
    constexpr int kSndBufSize = 4000;
    constexpr int kRcvBufSize = 8000;

    CreateChannels(DTLS, DTLS);

    channel1_->SetOption(cricket::BaseChannel::ST_RTP,
                         rtc::Socket::Option::OPT_SNDBUF, kSndBufSize);
    channel2_->SetOption(cricket::BaseChannel::ST_RTP,
                         rtc::Socket::Option::OPT_RCVBUF, kRcvBufSize);

    new_rtp_transport_ = CreateDtlsSrtpTransport(
        static_cast<DtlsTransportInternal*>(channel2_->rtp_packet_transport()),
        static_cast<DtlsTransportInternal*>(
            channel2_->rtcp_packet_transport()));
    channel1_->SetRtpTransport(new_rtp_transport_.get());

    int option_val;
    ASSERT_TRUE(
        static_cast<DtlsTransportInternal*>(channel1_->rtp_packet_transport())
            ->GetOption(rtc::Socket::Option::OPT_SNDBUF, &option_val));
    EXPECT_EQ(kSndBufSize, option_val);
    ASSERT_TRUE(
        static_cast<DtlsTransportInternal*>(channel1_->rtp_packet_transport())
            ->GetOption(rtc::Socket::Option::OPT_RCVBUF, &option_val));
    EXPECT_EQ(kRcvBufSize, option_val);
  }

 protected:
  void WaitForThreads() { WaitForThreads(rtc::ArrayView<rtc::Thread*>()); }
  static void ProcessThreadQueue(rtc::Thread* thread) {
    RTC_DCHECK(thread->IsCurrent());
    while (!thread->empty()) {
      thread->ProcessMessages(0);
    }
  }
  void WaitForThreads(rtc::ArrayView<rtc::Thread*> threads) {
    // |threads| and current thread post packets to network thread.
    for (rtc::Thread* thread : threads) {
      thread->Invoke<void>(RTC_FROM_HERE,
                           [thread] { ProcessThreadQueue(thread); });
    }
    ProcessThreadQueue(rtc::Thread::Current());
    // Network thread move them around and post back to worker = current thread.
    if (!network_thread_->IsCurrent()) {
      network_thread_->Invoke<void>(
          RTC_FROM_HERE, [this] { ProcessThreadQueue(network_thread_); });
    }
    // Worker thread = current Thread process received messages.
    ProcessThreadQueue(rtc::Thread::Current());
  }
  // TODO(pbos): Remove playout from all media channels and let renderers mute
  // themselves.
  const bool verify_playout_;
  std::unique_ptr<rtc::Thread> network_thread_keeper_;
  rtc::Thread* network_thread_;
  std::unique_ptr<cricket::FakeDtlsTransport> fake_rtp_dtls_transport1_;
  std::unique_ptr<cricket::FakeDtlsTransport> fake_rtcp_dtls_transport1_;
  std::unique_ptr<cricket::FakeDtlsTransport> fake_rtp_dtls_transport2_;
  std::unique_ptr<cricket::FakeDtlsTransport> fake_rtcp_dtls_transport2_;
  std::unique_ptr<rtc::FakePacketTransport> fake_rtp_packet_transport1_;
  std::unique_ptr<rtc::FakePacketTransport> fake_rtcp_packet_transport1_;
  std::unique_ptr<rtc::FakePacketTransport> fake_rtp_packet_transport2_;
  std::unique_ptr<rtc::FakePacketTransport> fake_rtcp_packet_transport2_;
  std::unique_ptr<webrtc::RtpTransportInternal> rtp_transport1_;
  std::unique_ptr<webrtc::RtpTransportInternal> rtp_transport2_;
  std::unique_ptr<webrtc::RtpTransportInternal> new_rtp_transport_;
  cricket::FakeMediaEngine media_engine_;
  // The media channels are owned by the voice channel objects below.
  typename T::MediaChannel* media_channel1_ = nullptr;
  typename T::MediaChannel* media_channel2_ = nullptr;
  std::unique_ptr<typename T::Channel> channel1_;
  std::unique_ptr<typename T::Channel> channel2_;
  typename T::Content local_media_content1_;
  typename T::Content local_media_content2_;
  typename T::Content remote_media_content1_;
  typename T::Content remote_media_content2_;
  // The RTP and RTCP packets to send in the tests.
  rtc::Buffer rtp_packet_;
  rtc::Buffer rtcp_packet_;
  int rtcp_mux_activated_callbacks1_ = 0;
  int rtcp_mux_activated_callbacks2_ = 0;
  cricket::CandidatePairInterface* last_selected_candidate_pair_;
};

template <>
void ChannelTest<VoiceTraits>::CreateContent(
    int flags,
    const cricket::AudioCodec& audio_codec,
    const cricket::VideoCodec& video_codec,
    cricket::AudioContentDescription* audio) {
  audio->AddCodec(audio_codec);
  audio->set_rtcp_mux((flags & RTCP_MUX) != 0);
}

template <>
void ChannelTest<VoiceTraits>::CopyContent(
    const cricket::AudioContentDescription& source,
    cricket::AudioContentDescription* audio) {
  *audio = source;
}

template <>
bool ChannelTest<VoiceTraits>::CodecMatches(const cricket::AudioCodec& c1,
                                            const cricket::AudioCodec& c2) {
  return c1.name == c2.name && c1.clockrate == c2.clockrate &&
         c1.bitrate == c2.bitrate && c1.channels == c2.channels;
}

template <>
void ChannelTest<VoiceTraits>::AddLegacyStreamInContent(
    uint32_t ssrc,
    int flags,
    cricket::AudioContentDescription* audio) {
  audio->AddLegacyStream(ssrc);
}

class VoiceChannelSingleThreadTest : public ChannelTest<VoiceTraits> {
 public:
  typedef ChannelTest<VoiceTraits> Base;
  VoiceChannelSingleThreadTest()
      : Base(true, kPcmuFrame, kRtcpReport, NetworkIsWorker::Yes) {}
};

class VoiceChannelDoubleThreadTest : public ChannelTest<VoiceTraits> {
 public:
  typedef ChannelTest<VoiceTraits> Base;
  VoiceChannelDoubleThreadTest()
      : Base(true, kPcmuFrame, kRtcpReport, NetworkIsWorker::No) {}
};

class VoiceChannelWithEncryptedRtpHeaderExtensionsSingleThreadTest
    : public ChannelTest<VoiceTraits> {
 public:
  typedef ChannelTest<VoiceTraits> Base;
  VoiceChannelWithEncryptedRtpHeaderExtensionsSingleThreadTest()
      : Base(true,
             kPcmuFrameWithExtensions,
             kRtcpReport,
             NetworkIsWorker::Yes) {}
};

class VoiceChannelWithEncryptedRtpHeaderExtensionsDoubleThreadTest
    : public ChannelTest<VoiceTraits> {
 public:
  typedef ChannelTest<VoiceTraits> Base;
  VoiceChannelWithEncryptedRtpHeaderExtensionsDoubleThreadTest()
      : Base(true, kPcmuFrameWithExtensions, kRtcpReport, NetworkIsWorker::No) {
  }
};

// override to add NULL parameter
template <>
std::unique_ptr<cricket::VideoChannel> ChannelTest<VideoTraits>::CreateChannel(
    rtc::Thread* worker_thread,
    rtc::Thread* network_thread,
    cricket::MediaEngineInterface* engine,
    std::unique_ptr<cricket::FakeVideoMediaChannel> ch,
    webrtc::RtpTransportInternal* rtp_transport,
    int flags) {
  rtc::Thread* signaling_thread = rtc::Thread::Current();
  auto channel = absl::make_unique<cricket::VideoChannel>(
      worker_thread, network_thread, signaling_thread, std::move(ch),
      cricket::CN_VIDEO, (flags & DTLS) != 0, rtc::CryptoOptions());
  channel->Init_w(rtp_transport);
  return channel;
}

// override to add 0 parameter
template <>
bool ChannelTest<VideoTraits>::AddStream1(int id) {
  return channel1_->AddRecvStream(cricket::StreamParams::CreateLegacy(id));
}

template <>
void ChannelTest<VideoTraits>::CreateContent(
    int flags,
    const cricket::AudioCodec& audio_codec,
    const cricket::VideoCodec& video_codec,
    cricket::VideoContentDescription* video) {
  video->AddCodec(video_codec);
  video->set_rtcp_mux((flags & RTCP_MUX) != 0);
}

template <>
void ChannelTest<VideoTraits>::CopyContent(
    const cricket::VideoContentDescription& source,
    cricket::VideoContentDescription* video) {
  *video = source;
}

template <>
bool ChannelTest<VideoTraits>::CodecMatches(const cricket::VideoCodec& c1,
                                            const cricket::VideoCodec& c2) {
  return c1.name == c2.name;
}

template <>
void ChannelTest<VideoTraits>::AddLegacyStreamInContent(
    uint32_t ssrc,
    int flags,
    cricket::VideoContentDescription* video) {
  video->AddLegacyStream(ssrc);
}

class VideoChannelSingleThreadTest : public ChannelTest<VideoTraits> {
 public:
  typedef ChannelTest<VideoTraits> Base;
  VideoChannelSingleThreadTest()
      : Base(false, kH264Packet, kRtcpReport, NetworkIsWorker::Yes) {}
};

class VideoChannelDoubleThreadTest : public ChannelTest<VideoTraits> {
 public:
  typedef ChannelTest<VideoTraits> Base;
  VideoChannelDoubleThreadTest()
      : Base(false, kH264Packet, kRtcpReport, NetworkIsWorker::No) {}
};

TEST_F(VoiceChannelSingleThreadTest, TestInit) {
  Base::TestInit();
  EXPECT_FALSE(media_channel1_->IsStreamMuted(0));
  EXPECT_TRUE(media_channel1_->dtmf_info_queue().empty());
}

TEST_F(VoiceChannelSingleThreadTest, TestDeinit) {
  Base::TestDeinit();
}

TEST_F(VoiceChannelSingleThreadTest, TestSetContents) {
  Base::TestSetContents();
}

TEST_F(VoiceChannelSingleThreadTest, TestSetContentsNullOffer) {
  Base::TestSetContentsNullOffer();
}

TEST_F(VoiceChannelSingleThreadTest, TestSetContentsRtcpMux) {
  Base::TestSetContentsRtcpMux();
}

TEST_F(VoiceChannelSingleThreadTest, TestSetContentsRtcpMuxWithPrAnswer) {
  Base::TestSetContentsRtcpMux();
}

TEST_F(VoiceChannelSingleThreadTest, TestStreams) {
  Base::TestStreams();
}

TEST_F(VoiceChannelSingleThreadTest, TestChangeStreamParamsInContent) {
  Base::TestChangeStreamParamsInContent();
}

TEST_F(VoiceChannelSingleThreadTest, TestPlayoutAndSendingStates) {
  Base::TestPlayoutAndSendingStates();
}

TEST_F(VoiceChannelSingleThreadTest, TestMediaContentDirection) {
  Base::TestMediaContentDirection();
}

TEST_F(VoiceChannelSingleThreadTest, TestNetworkRouteChanges) {
  Base::TestNetworkRouteChanges();
}

TEST_F(VoiceChannelSingleThreadTest, TestCallSetup) {
  Base::TestCallSetup();
}

TEST_F(VoiceChannelSingleThreadTest, TestCallTeardownRtcpMux) {
  Base::TestCallTeardownRtcpMux();
}

TEST_F(VoiceChannelSingleThreadTest, SendRtpToRtp) {
  Base::SendRtpToRtp();
}

TEST_F(VoiceChannelSingleThreadTest, SendRtcpToRtcp) {
  Base::SendRtcpToRtcp();
}

TEST_F(VoiceChannelSingleThreadTest, SendDtlsSrtpToDtlsSrtp) {
  Base::SendDtlsSrtpToDtlsSrtp(0, 0);
}

TEST_F(VoiceChannelSingleThreadTest, SendDtlsSrtpToDtlsSrtpRtcpMux) {
  Base::SendDtlsSrtpToDtlsSrtp(RTCP_MUX, RTCP_MUX);
}

TEST_F(VoiceChannelSingleThreadTest, SendEarlyMediaUsingRtcpMuxSrtp) {
  Base::SendEarlyMediaUsingRtcpMuxSrtp();
}

TEST_F(VoiceChannelSingleThreadTest, SendRtpToRtpOnThread) {
  Base::SendRtpToRtpOnThread();
}

TEST_F(VoiceChannelSingleThreadTest, SendWithWritabilityLoss) {
  Base::SendWithWritabilityLoss();
}

TEST_F(VoiceChannelSingleThreadTest, TestSetContentFailure) {
  Base::TestSetContentFailure();
}

TEST_F(VoiceChannelSingleThreadTest, TestSendTwoOffers) {
  Base::TestSendTwoOffers();
}

TEST_F(VoiceChannelSingleThreadTest, TestReceiveTwoOffers) {
  Base::TestReceiveTwoOffers();
}

TEST_F(VoiceChannelSingleThreadTest, TestSendPrAnswer) {
  Base::TestSendPrAnswer();
}

TEST_F(VoiceChannelSingleThreadTest, TestReceivePrAnswer) {
  Base::TestReceivePrAnswer();
}

TEST_F(VoiceChannelSingleThreadTest, TestFlushRtcp) {
  Base::TestFlushRtcp();
}

TEST_F(VoiceChannelSingleThreadTest, TestOnTransportReadyToSend) {
  Base::TestOnTransportReadyToSend();
}

TEST_F(VoiceChannelSingleThreadTest, SendBundleToBundle) {
  Base::SendBundleToBundle(kAudioPts, arraysize(kAudioPts), false, false);
}

TEST_F(VoiceChannelSingleThreadTest, SendBundleToBundleSecure) {
  Base::SendBundleToBundle(kAudioPts, arraysize(kAudioPts), false, true);
}

TEST_F(VoiceChannelSingleThreadTest, SendBundleToBundleWithRtcpMux) {
  Base::SendBundleToBundle(kAudioPts, arraysize(kAudioPts), true, false);
}

TEST_F(VoiceChannelSingleThreadTest, SendBundleToBundleWithRtcpMuxSecure) {
  Base::SendBundleToBundle(kAudioPts, arraysize(kAudioPts), true, true);
}

TEST_F(VoiceChannelSingleThreadTest, DefaultMaxBitrateIsUnlimited) {
  Base::DefaultMaxBitrateIsUnlimited();
}

TEST_F(VoiceChannelSingleThreadTest, SocketOptionsMergedOnSetTransport) {
  Base::SocketOptionsMergedOnSetTransport();
}

// VoiceChannelDoubleThreadTest
TEST_F(VoiceChannelDoubleThreadTest, TestInit) {
  Base::TestInit();
  EXPECT_FALSE(media_channel1_->IsStreamMuted(0));
  EXPECT_TRUE(media_channel1_->dtmf_info_queue().empty());
}

TEST_F(VoiceChannelDoubleThreadTest, TestDeinit) {
  Base::TestDeinit();
}

TEST_F(VoiceChannelDoubleThreadTest, TestSetContents) {
  Base::TestSetContents();
}

TEST_F(VoiceChannelDoubleThreadTest, TestSetContentsNullOffer) {
  Base::TestSetContentsNullOffer();
}

TEST_F(VoiceChannelDoubleThreadTest, TestSetContentsRtcpMux) {
  Base::TestSetContentsRtcpMux();
}

TEST_F(VoiceChannelDoubleThreadTest, TestSetContentsRtcpMuxWithPrAnswer) {
  Base::TestSetContentsRtcpMux();
}

TEST_F(VoiceChannelDoubleThreadTest, TestStreams) {
  Base::TestStreams();
}

TEST_F(VoiceChannelDoubleThreadTest, TestChangeStreamParamsInContent) {
  Base::TestChangeStreamParamsInContent();
}

TEST_F(VoiceChannelDoubleThreadTest, TestPlayoutAndSendingStates) {
  Base::TestPlayoutAndSendingStates();
}

TEST_F(VoiceChannelDoubleThreadTest, TestMediaContentDirection) {
  Base::TestMediaContentDirection();
}

TEST_F(VoiceChannelDoubleThreadTest, TestNetworkRouteChanges) {
  Base::TestNetworkRouteChanges();
}

TEST_F(VoiceChannelDoubleThreadTest, TestCallSetup) {
  Base::TestCallSetup();
}

TEST_F(VoiceChannelDoubleThreadTest, TestCallTeardownRtcpMux) {
  Base::TestCallTeardownRtcpMux();
}

TEST_F(VoiceChannelDoubleThreadTest, SendRtpToRtp) {
  Base::SendRtpToRtp();
}

TEST_F(VoiceChannelDoubleThreadTest, SendRtcpToRtcp) {
  Base::SendRtcpToRtcp();
}

TEST_F(VoiceChannelDoubleThreadTest, SendDtlsSrtpToDtlsSrtp) {
  Base::SendDtlsSrtpToDtlsSrtp(0, 0);
}

TEST_F(VoiceChannelDoubleThreadTest, SendDtlsSrtpToDtlsSrtpRtcpMux) {
  Base::SendDtlsSrtpToDtlsSrtp(RTCP_MUX, RTCP_MUX);
}

TEST_F(VoiceChannelDoubleThreadTest, SendEarlyMediaUsingRtcpMuxSrtp) {
  Base::SendEarlyMediaUsingRtcpMuxSrtp();
}

TEST_F(VoiceChannelDoubleThreadTest, SendRtpToRtpOnThread) {
  Base::SendRtpToRtpOnThread();
}

TEST_F(VoiceChannelDoubleThreadTest, SendWithWritabilityLoss) {
  Base::SendWithWritabilityLoss();
}

TEST_F(VoiceChannelDoubleThreadTest, TestSetContentFailure) {
  Base::TestSetContentFailure();
}

TEST_F(VoiceChannelDoubleThreadTest, TestSendTwoOffers) {
  Base::TestSendTwoOffers();
}

TEST_F(VoiceChannelDoubleThreadTest, TestReceiveTwoOffers) {
  Base::TestReceiveTwoOffers();
}

TEST_F(VoiceChannelDoubleThreadTest, TestSendPrAnswer) {
  Base::TestSendPrAnswer();
}

TEST_F(VoiceChannelDoubleThreadTest, TestReceivePrAnswer) {
  Base::TestReceivePrAnswer();
}

TEST_F(VoiceChannelDoubleThreadTest, TestFlushRtcp) {
  Base::TestFlushRtcp();
}

TEST_F(VoiceChannelDoubleThreadTest, TestOnTransportReadyToSend) {
  Base::TestOnTransportReadyToSend();
}

TEST_F(VoiceChannelDoubleThreadTest, SendBundleToBundle) {
  Base::SendBundleToBundle(kAudioPts, arraysize(kAudioPts), false, false);
}

TEST_F(VoiceChannelDoubleThreadTest, SendBundleToBundleSecure) {
  Base::SendBundleToBundle(kAudioPts, arraysize(kAudioPts), false, true);
}

TEST_F(VoiceChannelDoubleThreadTest, SendBundleToBundleWithRtcpMux) {
  Base::SendBundleToBundle(kAudioPts, arraysize(kAudioPts), true, false);
}

TEST_F(VoiceChannelDoubleThreadTest, SendBundleToBundleWithRtcpMuxSecure) {
  Base::SendBundleToBundle(kAudioPts, arraysize(kAudioPts), true, true);
}

TEST_F(VoiceChannelDoubleThreadTest, DefaultMaxBitrateIsUnlimited) {
  Base::DefaultMaxBitrateIsUnlimited();
}

TEST_F(VoiceChannelDoubleThreadTest, SocketOptionsMergedOnSetTransport) {
  Base::SocketOptionsMergedOnSetTransport();
}

// VideoChannelSingleThreadTest
TEST_F(VideoChannelSingleThreadTest, TestInit) {
  Base::TestInit();
}

TEST_F(VideoChannelSingleThreadTest, TestDeinit) {
  Base::TestDeinit();
}

TEST_F(VideoChannelSingleThreadTest, TestSetContents) {
  Base::TestSetContents();
}

TEST_F(VideoChannelSingleThreadTest, TestSetContentsNullOffer) {
  Base::TestSetContentsNullOffer();
}

TEST_F(VideoChannelSingleThreadTest, TestSetContentsRtcpMux) {
  Base::TestSetContentsRtcpMux();
}

TEST_F(VideoChannelSingleThreadTest, TestSetContentsRtcpMuxWithPrAnswer) {
  Base::TestSetContentsRtcpMux();
}

TEST_F(VideoChannelSingleThreadTest, TestStreams) {
  Base::TestStreams();
}

TEST_F(VideoChannelSingleThreadTest, TestChangeStreamParamsInContent) {
  Base::TestChangeStreamParamsInContent();
}

TEST_F(VideoChannelSingleThreadTest, TestPlayoutAndSendingStates) {
  Base::TestPlayoutAndSendingStates();
}

TEST_F(VideoChannelSingleThreadTest, TestMediaContentDirection) {
  Base::TestMediaContentDirection();
}

TEST_F(VideoChannelSingleThreadTest, TestNetworkRouteChanges) {
  Base::TestNetworkRouteChanges();
}

TEST_F(VideoChannelSingleThreadTest, TestCallSetup) {
  Base::TestCallSetup();
}

TEST_F(VideoChannelSingleThreadTest, TestCallTeardownRtcpMux) {
  Base::TestCallTeardownRtcpMux();
}

TEST_F(VideoChannelSingleThreadTest, SendRtpToRtp) {
  Base::SendRtpToRtp();
}

TEST_F(VideoChannelSingleThreadTest, SendRtcpToRtcp) {
  Base::SendRtcpToRtcp();
}

TEST_F(VideoChannelSingleThreadTest, SendDtlsSrtpToDtlsSrtp) {
  Base::SendDtlsSrtpToDtlsSrtp(0, 0);
}

TEST_F(VideoChannelSingleThreadTest, SendDtlsSrtpToDtlsSrtpRtcpMux) {
  Base::SendDtlsSrtpToDtlsSrtp(RTCP_MUX, RTCP_MUX);
}

TEST_F(VideoChannelSingleThreadTest, SendEarlyMediaUsingRtcpMuxSrtp) {
  Base::SendEarlyMediaUsingRtcpMuxSrtp();
}

TEST_F(VideoChannelSingleThreadTest, SendRtpToRtpOnThread) {
  Base::SendRtpToRtpOnThread();
}

TEST_F(VideoChannelSingleThreadTest, SendWithWritabilityLoss) {
  Base::SendWithWritabilityLoss();
}

TEST_F(VideoChannelSingleThreadTest, TestSetContentFailure) {
  Base::TestSetContentFailure();
}

TEST_F(VideoChannelSingleThreadTest, TestSendTwoOffers) {
  Base::TestSendTwoOffers();
}

TEST_F(VideoChannelSingleThreadTest, TestReceiveTwoOffers) {
  Base::TestReceiveTwoOffers();
}

TEST_F(VideoChannelSingleThreadTest, TestSendPrAnswer) {
  Base::TestSendPrAnswer();
}

TEST_F(VideoChannelSingleThreadTest, TestReceivePrAnswer) {
  Base::TestReceivePrAnswer();
}

TEST_F(VideoChannelSingleThreadTest, TestFlushRtcp) {
  Base::TestFlushRtcp();
}

TEST_F(VideoChannelSingleThreadTest, SendBundleToBundle) {
  Base::SendBundleToBundle(kVideoPts, arraysize(kVideoPts), false, false);
}

TEST_F(VideoChannelSingleThreadTest, SendBundleToBundleSecure) {
  Base::SendBundleToBundle(kVideoPts, arraysize(kVideoPts), false, true);
}

TEST_F(VideoChannelSingleThreadTest, SendBundleToBundleWithRtcpMux) {
  Base::SendBundleToBundle(kVideoPts, arraysize(kVideoPts), true, false);
}

TEST_F(VideoChannelSingleThreadTest, SendBundleToBundleWithRtcpMuxSecure) {
  Base::SendBundleToBundle(kVideoPts, arraysize(kVideoPts), true, true);
}

TEST_F(VideoChannelSingleThreadTest, TestOnTransportReadyToSend) {
  Base::TestOnTransportReadyToSend();
}

TEST_F(VideoChannelSingleThreadTest, DefaultMaxBitrateIsUnlimited) {
  Base::DefaultMaxBitrateIsUnlimited();
}

TEST_F(VideoChannelSingleThreadTest, SocketOptionsMergedOnSetTransport) {
  Base::SocketOptionsMergedOnSetTransport();
}

// VideoChannelDoubleThreadTest
TEST_F(VideoChannelDoubleThreadTest, TestInit) {
  Base::TestInit();
}

TEST_F(VideoChannelDoubleThreadTest, TestDeinit) {
  Base::TestDeinit();
}

TEST_F(VideoChannelDoubleThreadTest, TestSetContents) {
  Base::TestSetContents();
}

TEST_F(VideoChannelDoubleThreadTest, TestSetContentsNullOffer) {
  Base::TestSetContentsNullOffer();
}

TEST_F(VideoChannelDoubleThreadTest, TestSetContentsRtcpMux) {
  Base::TestSetContentsRtcpMux();
}

TEST_F(VideoChannelDoubleThreadTest, TestSetContentsRtcpMuxWithPrAnswer) {
  Base::TestSetContentsRtcpMux();
}

TEST_F(VideoChannelDoubleThreadTest, TestStreams) {
  Base::TestStreams();
}

TEST_F(VideoChannelDoubleThreadTest, TestChangeStreamParamsInContent) {
  Base::TestChangeStreamParamsInContent();
}

TEST_F(VideoChannelDoubleThreadTest, TestPlayoutAndSendingStates) {
  Base::TestPlayoutAndSendingStates();
}

TEST_F(VideoChannelDoubleThreadTest, TestMediaContentDirection) {
  Base::TestMediaContentDirection();
}

TEST_F(VideoChannelDoubleThreadTest, TestNetworkRouteChanges) {
  Base::TestNetworkRouteChanges();
}

TEST_F(VideoChannelDoubleThreadTest, TestCallSetup) {
  Base::TestCallSetup();
}

TEST_F(VideoChannelDoubleThreadTest, TestCallTeardownRtcpMux) {
  Base::TestCallTeardownRtcpMux();
}

TEST_F(VideoChannelDoubleThreadTest, SendRtpToRtp) {
  Base::SendRtpToRtp();
}

TEST_F(VideoChannelDoubleThreadTest, SendRtcpToRtcp) {
  Base::SendRtcpToRtcp();
}

TEST_F(VideoChannelDoubleThreadTest, SendDtlsSrtpToDtlsSrtp) {
  Base::SendDtlsSrtpToDtlsSrtp(0, 0);
}

TEST_F(VideoChannelDoubleThreadTest, SendDtlsSrtpToDtlsSrtpRtcpMux) {
  Base::SendDtlsSrtpToDtlsSrtp(RTCP_MUX, RTCP_MUX);
}

TEST_F(VideoChannelDoubleThreadTest, SendEarlyMediaUsingRtcpMuxSrtp) {
  Base::SendEarlyMediaUsingRtcpMuxSrtp();
}

TEST_F(VideoChannelDoubleThreadTest, SendRtpToRtpOnThread) {
  Base::SendRtpToRtpOnThread();
}

TEST_F(VideoChannelDoubleThreadTest, SendWithWritabilityLoss) {
  Base::SendWithWritabilityLoss();
}

TEST_F(VideoChannelDoubleThreadTest, TestSetContentFailure) {
  Base::TestSetContentFailure();
}

TEST_F(VideoChannelDoubleThreadTest, TestSendTwoOffers) {
  Base::TestSendTwoOffers();
}

TEST_F(VideoChannelDoubleThreadTest, TestReceiveTwoOffers) {
  Base::TestReceiveTwoOffers();
}

TEST_F(VideoChannelDoubleThreadTest, TestSendPrAnswer) {
  Base::TestSendPrAnswer();
}

TEST_F(VideoChannelDoubleThreadTest, TestReceivePrAnswer) {
  Base::TestReceivePrAnswer();
}

TEST_F(VideoChannelDoubleThreadTest, TestFlushRtcp) {
  Base::TestFlushRtcp();
}

TEST_F(VideoChannelDoubleThreadTest, SendBundleToBundle) {
  Base::SendBundleToBundle(kVideoPts, arraysize(kVideoPts), false, false);
}

TEST_F(VideoChannelDoubleThreadTest, SendBundleToBundleSecure) {
  Base::SendBundleToBundle(kVideoPts, arraysize(kVideoPts), false, true);
}

TEST_F(VideoChannelDoubleThreadTest, SendBundleToBundleWithRtcpMux) {
  Base::SendBundleToBundle(kVideoPts, arraysize(kVideoPts), true, false);
}

TEST_F(VideoChannelDoubleThreadTest, SendBundleToBundleWithRtcpMuxSecure) {
  Base::SendBundleToBundle(kVideoPts, arraysize(kVideoPts), true, true);
}

TEST_F(VideoChannelDoubleThreadTest, TestOnTransportReadyToSend) {
  Base::TestOnTransportReadyToSend();
}

TEST_F(VideoChannelDoubleThreadTest, DefaultMaxBitrateIsUnlimited) {
  Base::DefaultMaxBitrateIsUnlimited();
}

TEST_F(VideoChannelDoubleThreadTest, SocketOptionsMergedOnSetTransport) {
  Base::SocketOptionsMergedOnSetTransport();
}

// RtpDataChannelSingleThreadTest
class RtpDataChannelSingleThreadTest : public ChannelTest<DataTraits> {
 public:
  typedef ChannelTest<DataTraits> Base;
  RtpDataChannelSingleThreadTest()
      : Base(true, kDataPacket, kRtcpReport, NetworkIsWorker::Yes) {}
};

// RtpDataChannelDoubleThreadTest
class RtpDataChannelDoubleThreadTest : public ChannelTest<DataTraits> {
 public:
  typedef ChannelTest<DataTraits> Base;
  RtpDataChannelDoubleThreadTest()
      : Base(true, kDataPacket, kRtcpReport, NetworkIsWorker::No) {}
};

// Override to avoid engine channel parameter.
template <>
std::unique_ptr<cricket::RtpDataChannel> ChannelTest<DataTraits>::CreateChannel(
    rtc::Thread* worker_thread,
    rtc::Thread* network_thread,
    cricket::MediaEngineInterface* engine,
    std::unique_ptr<cricket::FakeDataMediaChannel> ch,
    webrtc::RtpTransportInternal* rtp_transport,
    int flags) {
  rtc::Thread* signaling_thread = rtc::Thread::Current();
  auto channel = absl::make_unique<cricket::RtpDataChannel>(
      worker_thread, network_thread, signaling_thread, std::move(ch),
      cricket::CN_DATA, (flags & DTLS) != 0, rtc::CryptoOptions());
  channel->Init_w(rtp_transport);
  return channel;
}

template <>
void ChannelTest<DataTraits>::CreateContent(
    int flags,
    const cricket::AudioCodec& audio_codec,
    const cricket::VideoCodec& video_codec,
    cricket::DataContentDescription* data) {
  data->AddCodec(kGoogleDataCodec);
  data->set_rtcp_mux((flags & RTCP_MUX) != 0);
}

template <>
void ChannelTest<DataTraits>::CopyContent(
    const cricket::DataContentDescription& source,
    cricket::DataContentDescription* data) {
  *data = source;
}

template <>
bool ChannelTest<DataTraits>::CodecMatches(const cricket::DataCodec& c1,
                                           const cricket::DataCodec& c2) {
  return c1.name == c2.name;
}

template <>
void ChannelTest<DataTraits>::AddLegacyStreamInContent(
    uint32_t ssrc,
    int flags,
    cricket::DataContentDescription* data) {
  data->AddLegacyStream(ssrc);
}

TEST_F(RtpDataChannelSingleThreadTest, TestInit) {
  Base::TestInit();
  EXPECT_FALSE(media_channel1_->IsStreamMuted(0));
}

TEST_F(RtpDataChannelSingleThreadTest, TestDeinit) {
  Base::TestDeinit();
}

TEST_F(RtpDataChannelSingleThreadTest, TestSetContents) {
  Base::TestSetContents();
}

TEST_F(RtpDataChannelSingleThreadTest, TestSetContentsNullOffer) {
  Base::TestSetContentsNullOffer();
}

TEST_F(RtpDataChannelSingleThreadTest, TestSetContentsRtcpMux) {
  Base::TestSetContentsRtcpMux();
}

TEST_F(RtpDataChannelSingleThreadTest, TestStreams) {
  Base::TestStreams();
}

TEST_F(RtpDataChannelSingleThreadTest, TestChangeStreamParamsInContent) {
  Base::TestChangeStreamParamsInContent();
}

TEST_F(RtpDataChannelSingleThreadTest, TestPlayoutAndSendingStates) {
  Base::TestPlayoutAndSendingStates();
}

TEST_F(RtpDataChannelSingleThreadTest, TestMediaContentDirection) {
  Base::TestMediaContentDirection();
}

TEST_F(RtpDataChannelSingleThreadTest, TestCallSetup) {
  Base::TestCallSetup();
}

TEST_F(RtpDataChannelSingleThreadTest, TestCallTeardownRtcpMux) {
  Base::TestCallTeardownRtcpMux();
}

TEST_F(RtpDataChannelSingleThreadTest, TestOnTransportReadyToSend) {
  Base::TestOnTransportReadyToSend();
}

TEST_F(RtpDataChannelSingleThreadTest, SendRtpToRtp) {
  Base::SendRtpToRtp();
}

TEST_F(RtpDataChannelSingleThreadTest, SendRtcpToRtcp) {
  Base::SendRtcpToRtcp();
}

TEST_F(RtpDataChannelSingleThreadTest, SendRtpToRtpOnThread) {
  Base::SendRtpToRtpOnThread();
}

TEST_F(RtpDataChannelSingleThreadTest, SendWithWritabilityLoss) {
  Base::SendWithWritabilityLoss();
}

TEST_F(RtpDataChannelSingleThreadTest, SocketOptionsMergedOnSetTransport) {
  Base::SocketOptionsMergedOnSetTransport();
}

TEST_F(RtpDataChannelSingleThreadTest, TestSendData) {
  CreateChannels(0, 0);
  EXPECT_TRUE(SendInitiate());
  EXPECT_TRUE(SendAccept());

  cricket::SendDataParams params;
  params.ssrc = 42;
  unsigned char data[] = {'f', 'o', 'o'};
  rtc::CopyOnWriteBuffer payload(data, 3);
  cricket::SendDataResult result;
  ASSERT_TRUE(media_channel1_->SendData(params, payload, &result));
  EXPECT_EQ(params.ssrc, media_channel1_->last_sent_data_params().ssrc);
  EXPECT_EQ("foo", media_channel1_->last_sent_data());
}

TEST_F(RtpDataChannelDoubleThreadTest, TestInit) {
  Base::TestInit();
  EXPECT_FALSE(media_channel1_->IsStreamMuted(0));
}

TEST_F(RtpDataChannelDoubleThreadTest, TestDeinit) {
  Base::TestDeinit();
}

TEST_F(RtpDataChannelDoubleThreadTest, TestSetContents) {
  Base::TestSetContents();
}

TEST_F(RtpDataChannelDoubleThreadTest, TestSetContentsNullOffer) {
  Base::TestSetContentsNullOffer();
}

TEST_F(RtpDataChannelDoubleThreadTest, TestSetContentsRtcpMux) {
  Base::TestSetContentsRtcpMux();
}

TEST_F(RtpDataChannelDoubleThreadTest, TestStreams) {
  Base::TestStreams();
}

TEST_F(RtpDataChannelDoubleThreadTest, TestChangeStreamParamsInContent) {
  Base::TestChangeStreamParamsInContent();
}

TEST_F(RtpDataChannelDoubleThreadTest, TestPlayoutAndSendingStates) {
  Base::TestPlayoutAndSendingStates();
}

TEST_F(RtpDataChannelDoubleThreadTest, TestMediaContentDirection) {
  Base::TestMediaContentDirection();
}

TEST_F(RtpDataChannelDoubleThreadTest, TestCallSetup) {
  Base::TestCallSetup();
}

TEST_F(RtpDataChannelDoubleThreadTest, TestCallTeardownRtcpMux) {
  Base::TestCallTeardownRtcpMux();
}

TEST_F(RtpDataChannelDoubleThreadTest, TestOnTransportReadyToSend) {
  Base::TestOnTransportReadyToSend();
}

TEST_F(RtpDataChannelDoubleThreadTest, SendRtpToRtp) {
  Base::SendRtpToRtp();
}

TEST_F(RtpDataChannelDoubleThreadTest, SendRtcpToRtcp) {
  Base::SendRtcpToRtcp();
}

TEST_F(RtpDataChannelDoubleThreadTest, SendRtpToRtpOnThread) {
  Base::SendRtpToRtpOnThread();
}

TEST_F(RtpDataChannelDoubleThreadTest, SendWithWritabilityLoss) {
  Base::SendWithWritabilityLoss();
}

TEST_F(RtpDataChannelDoubleThreadTest, SocketOptionsMergedOnSetTransport) {
  Base::SocketOptionsMergedOnSetTransport();
}

TEST_F(RtpDataChannelDoubleThreadTest, TestSendData) {
  CreateChannels(0, 0);
  EXPECT_TRUE(SendInitiate());
  EXPECT_TRUE(SendAccept());

  cricket::SendDataParams params;
  params.ssrc = 42;
  unsigned char data[] = {'f', 'o', 'o'};
  rtc::CopyOnWriteBuffer payload(data, 3);
  cricket::SendDataResult result;
  ASSERT_TRUE(media_channel1_->SendData(params, payload, &result));
  EXPECT_EQ(params.ssrc, media_channel1_->last_sent_data_params().ssrc);
  EXPECT_EQ("foo", media_channel1_->last_sent_data());
}

// TODO(pthatcher): TestSetReceiver?
