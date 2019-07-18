/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_p2p__base__fakeportallocator_h //original-code:"p2p/base/fakeportallocator.h"
#include BOSS_WEBRTC_U_p2p__base__teststunserver_h //original-code:"p2p/base/teststunserver.h"
#include BOSS_WEBRTC_U_p2p__client__basicportallocator_h //original-code:"p2p/client/basicportallocator.h"
#include BOSS_WEBRTC_U_pc__mediasession_h //original-code:"pc/mediasession.h"
#include BOSS_WEBRTC_U_pc__peerconnection_h //original-code:"pc/peerconnection.h"
#include BOSS_WEBRTC_U_pc__peerconnectionwrapper_h //original-code:"pc/peerconnectionwrapper.h"
#include BOSS_WEBRTC_U_pc__sdputils_h //original-code:"pc/sdputils.h"
#ifdef WEBRTC_ANDROID
#include "pc/test/androidtestinitializer.h"
#endif
#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_decoder_factory_h //original-code:"api/audio_codecs/builtin_audio_decoder_factory.h"
#include BOSS_WEBRTC_U_api__audio_codecs__builtin_audio_encoder_factory_h //original-code:"api/audio_codecs/builtin_audio_encoder_factory.h"
#include BOSS_WEBRTC_U_api__peerconnectionproxy_h //original-code:"api/peerconnectionproxy.h"
#include "pc/test/fakeaudiocapturemodule.h"
#include BOSS_WEBRTC_U_rtc_base__fakenetwork_h //original-code:"rtc_base/fakenetwork.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__ptr_util_h //original-code:"rtc_base/ptr_util.h"
#include BOSS_WEBRTC_U_rtc_base__virtualsocketserver_h //original-code:"rtc_base/virtualsocketserver.h"

namespace webrtc {

using RTCConfiguration = PeerConnectionInterface::RTCConfiguration;
using RTCOfferAnswerOptions = PeerConnectionInterface::RTCOfferAnswerOptions;
using rtc::SocketAddress;
using ::testing::Values;

constexpr int kIceCandidatesTimeout = 10000;

class PeerConnectionWrapperForIceUnitTest : public PeerConnectionWrapper {
 public:
  using PeerConnectionWrapper::PeerConnectionWrapper;

  // Adds a new ICE candidate to the first transport.
  bool AddIceCandidate(cricket::Candidate* candidate) {
    RTC_DCHECK(pc()->remote_description());
    const auto* desc = pc()->remote_description()->description();
    RTC_DCHECK(desc->contents().size() > 0);
    const auto& first_content = desc->contents()[0];
    candidate->set_transport_name(first_content.name);
    JsepIceCandidate jsep_candidate(first_content.name, 0, *candidate);
    return pc()->AddIceCandidate(&jsep_candidate);
  }

  // Returns ICE candidates from the remote session description.
  std::vector<const IceCandidateInterface*>
  GetIceCandidatesFromRemoteDescription() {
    const SessionDescriptionInterface* sdesc = pc()->remote_description();
    RTC_DCHECK(sdesc);
    std::vector<const IceCandidateInterface*> candidates;
    for (size_t mline_index = 0; mline_index < sdesc->number_of_mediasections();
         mline_index++) {
      const auto* candidate_collection = sdesc->candidates(mline_index);
      for (size_t i = 0; i < candidate_collection->count(); i++) {
        candidates.push_back(candidate_collection->at(i));
      }
    }
    return candidates;
  }

  rtc::FakeNetworkManager* network() { return network_; }

  void set_network(rtc::FakeNetworkManager* network) { network_ = network; }

 private:
  rtc::FakeNetworkManager* network_;
};

class PeerConnectionIceUnitTest : public ::testing::Test {
 protected:
  typedef std::unique_ptr<PeerConnectionWrapperForIceUnitTest> WrapperPtr;

  PeerConnectionIceUnitTest()
      : vss_(new rtc::VirtualSocketServer()), main_(vss_.get()) {
#ifdef WEBRTC_ANDROID
    InitializeAndroidObjects();
#endif
    pc_factory_ = CreatePeerConnectionFactory(
        rtc::Thread::Current(), rtc::Thread::Current(), rtc::Thread::Current(),
        FakeAudioCaptureModule::Create(), CreateBuiltinAudioEncoderFactory(),
        CreateBuiltinAudioDecoderFactory(), nullptr, nullptr);
  }

  WrapperPtr CreatePeerConnection() {
    return CreatePeerConnection(RTCConfiguration());
  }

  WrapperPtr CreatePeerConnection(const RTCConfiguration& config) {
    auto* fake_network = NewFakeNetwork();
    auto port_allocator =
        rtc::MakeUnique<cricket::BasicPortAllocator>(fake_network);
    port_allocator->set_flags(cricket::PORTALLOCATOR_DISABLE_TCP |
                              cricket::PORTALLOCATOR_DISABLE_RELAY);
    port_allocator->set_step_delay(cricket::kMinimumStepDelay);
    auto observer = rtc::MakeUnique<MockPeerConnectionObserver>();
    auto pc = pc_factory_->CreatePeerConnection(
        config, std::move(port_allocator), nullptr, observer.get());
    if (!pc) {
      return nullptr;
    }

    auto wrapper = rtc::MakeUnique<PeerConnectionWrapperForIceUnitTest>(
        pc_factory_, pc, std::move(observer));
    wrapper->set_network(fake_network);
    return wrapper;
  }

  // Accepts the same arguments as CreatePeerConnection and adds default audio
  // and video tracks.
  template <typename... Args>
  WrapperPtr CreatePeerConnectionWithAudioVideo(Args&&... args) {
    auto wrapper = CreatePeerConnection(std::forward<Args>(args)...);
    if (!wrapper) {
      return nullptr;
    }
    wrapper->AddAudioTrack("a");
    wrapper->AddVideoTrack("v");
    return wrapper;
  }

  cricket::Candidate CreateLocalUdpCandidate(
      const rtc::SocketAddress& address) {
    cricket::Candidate candidate;
    candidate.set_component(cricket::ICE_CANDIDATE_COMPONENT_DEFAULT);
    candidate.set_protocol(cricket::UDP_PROTOCOL_NAME);
    candidate.set_address(address);
    candidate.set_type(cricket::LOCAL_PORT_TYPE);
    return candidate;
  }

  // Remove all ICE ufrag/pwd lines from the given session description.
  void RemoveIceUfragPwd(SessionDescriptionInterface* sdesc) {
    SetIceUfragPwd(sdesc, "", "");
  }

  // Sets all ICE ufrag/pwds on the given session description.
  void SetIceUfragPwd(SessionDescriptionInterface* sdesc,
                      const std::string& ufrag,
                      const std::string& pwd) {
    auto* desc = sdesc->description();
    for (const auto& content : desc->contents()) {
      auto* transport_info = desc->GetTransportInfoByName(content.name);
      transport_info->description.ice_ufrag = ufrag;
      transport_info->description.ice_pwd = pwd;
    }
  }

  // Set ICE mode on the given session description.
  void SetIceMode(SessionDescriptionInterface* sdesc,
                  const cricket::IceMode ice_mode) {
    auto* desc = sdesc->description();
    for (const auto& content : desc->contents()) {
      auto* transport_info = desc->GetTransportInfoByName(content.name);
      transport_info->description.ice_mode = ice_mode;
    }
  }

  cricket::TransportDescription* GetFirstTransportDescription(
      SessionDescriptionInterface* sdesc) {
    auto* desc = sdesc->description();
    RTC_DCHECK(desc->contents().size() > 0);
    auto* transport_info =
        desc->GetTransportInfoByName(desc->contents()[0].name);
    RTC_DCHECK(transport_info);
    return &transport_info->description;
  }

  const cricket::TransportDescription* GetFirstTransportDescription(
      const SessionDescriptionInterface* sdesc) {
    auto* desc = sdesc->description();
    RTC_DCHECK(desc->contents().size() > 0);
    auto* transport_info =
        desc->GetTransportInfoByName(desc->contents()[0].name);
    RTC_DCHECK(transport_info);
    return &transport_info->description;
  }

  // TODO(qingsi): Rewrite this method in terms of the standard IceTransport
  // after it is implemented.
  cricket::IceRole GetIceRole(const WrapperPtr& pc_wrapper_ptr) {
    auto* pc_proxy =
        static_cast<PeerConnectionProxyWithInternal<PeerConnectionInterface>*>(
            pc_wrapper_ptr->pc());
    PeerConnection* pc = static_cast<PeerConnection*>(pc_proxy->internal());
    return pc->voice_channel()
        ->rtp_dtls_transport()
        ->ice_transport()
        ->GetIceRole();
  }

  bool AddCandidateToFirstTransport(cricket::Candidate* candidate,
                                    SessionDescriptionInterface* sdesc) {
    auto* desc = sdesc->description();
    RTC_DCHECK(desc->contents().size() > 0);
    const auto& first_content = desc->contents()[0];
    candidate->set_transport_name(first_content.name);
    JsepIceCandidate jsep_candidate(first_content.name, 0, *candidate);
    return sdesc->AddCandidate(&jsep_candidate);
  }

  rtc::FakeNetworkManager* NewFakeNetwork() {
    // The PeerConnection's port allocator is tied to the PeerConnection's
    // lifetime and expects the underlying NetworkManager to outlive it. That
    // prevents us from having the PeerConnectionWrapper own the fake network.
    // Therefore, the test fixture will own all the fake networks even though
    // tests should access the fake network through the PeerConnectionWrapper.
    auto* fake_network = new rtc::FakeNetworkManager();
    fake_networks_.emplace_back(fake_network);
    return fake_network;
  }

  std::unique_ptr<rtc::VirtualSocketServer> vss_;
  rtc::AutoSocketServerThread main_;
  rtc::scoped_refptr<PeerConnectionFactoryInterface> pc_factory_;
  std::vector<std::unique_ptr<rtc::FakeNetworkManager>> fake_networks_;
};

::testing::AssertionResult AssertCandidatesEqual(const char* a_expr,
                                                 const char* b_expr,
                                                 const cricket::Candidate& a,
                                                 const cricket::Candidate& b) {
  std::stringstream failure_info;
  if (a.component() != b.component()) {
    failure_info << "\ncomponent: " << a.component() << " != " << b.component();
  }
  if (a.protocol() != b.protocol()) {
    failure_info << "\nprotocol: " << a.protocol() << " != " << b.protocol();
  }
  if (a.address() != b.address()) {
    failure_info << "\naddress: " << a.address().ToString()
                 << " != " << b.address().ToString();
  }
  if (a.type() != b.type()) {
    failure_info << "\ntype: " << a.type() << " != " << b.type();
  }
  std::string failure_info_str = failure_info.str();
  if (failure_info_str.empty()) {
    return ::testing::AssertionSuccess();
  } else {
    return ::testing::AssertionFailure()
           << a_expr << " and " << b_expr << " are not equal"
           << failure_info_str;
  }
}

TEST_F(PeerConnectionIceUnitTest, OfferContainsGatheredCandidates) {
  const SocketAddress kLocalAddress("1.1.1.1", 0);

  auto caller = CreatePeerConnectionWithAudioVideo();
  caller->network()->AddInterface(kLocalAddress);

  // Start ICE candidate gathering by setting the local offer.
  ASSERT_TRUE(caller->SetLocalDescription(caller->CreateOffer()));

  EXPECT_TRUE_WAIT(caller->IsIceGatheringDone(), kIceCandidatesTimeout);

  auto offer = caller->CreateOffer();
  EXPECT_LT(0u, caller->observer()->GetCandidatesByMline(0).size());
  EXPECT_EQ(caller->observer()->GetCandidatesByMline(0).size(),
            offer->candidates(0)->count());
  EXPECT_LT(0u, caller->observer()->GetCandidatesByMline(1).size());
  EXPECT_EQ(caller->observer()->GetCandidatesByMline(1).size(),
            offer->candidates(1)->count());
}

TEST_F(PeerConnectionIceUnitTest, AnswerContainsGatheredCandidates) {
  const SocketAddress kCallerAddress("1.1.1.1", 0);

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();
  caller->network()->AddInterface(kCallerAddress);

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(callee->SetLocalDescription(callee->CreateAnswer()));

  EXPECT_TRUE_WAIT(callee->IsIceGatheringDone(), kIceCandidatesTimeout);

  auto answer = callee->CreateAnswer();
  EXPECT_LT(0u, caller->observer()->GetCandidatesByMline(0).size());
  EXPECT_EQ(callee->observer()->GetCandidatesByMline(0).size(),
            answer->candidates(0)->count());
  EXPECT_LT(0u, caller->observer()->GetCandidatesByMline(1).size());
  EXPECT_EQ(callee->observer()->GetCandidatesByMline(1).size(),
            answer->candidates(1)->count());
}

TEST_F(PeerConnectionIceUnitTest,
       CanSetRemoteSessionDescriptionWithRemoteCandidates) {
  const SocketAddress kCallerAddress("1.1.1.1", 1111);

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  auto offer = caller->CreateOfferAndSetAsLocal();
  cricket::Candidate candidate = CreateLocalUdpCandidate(kCallerAddress);
  AddCandidateToFirstTransport(&candidate, offer.get());

  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));
  auto remote_candidates = callee->GetIceCandidatesFromRemoteDescription();
  ASSERT_EQ(1u, remote_candidates.size());
  EXPECT_PRED_FORMAT2(AssertCandidatesEqual, candidate,
                      remote_candidates[0]->candidate());
}

TEST_F(PeerConnectionIceUnitTest, SetLocalDescriptionFailsIfNoIceCredentials) {
  auto caller = CreatePeerConnectionWithAudioVideo();

  auto offer = caller->CreateOffer();
  RemoveIceUfragPwd(offer.get());

  EXPECT_FALSE(caller->SetLocalDescription(std::move(offer)));
}

TEST_F(PeerConnectionIceUnitTest, SetRemoteDescriptionFailsIfNoIceCredentials) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  auto offer = caller->CreateOfferAndSetAsLocal();
  RemoveIceUfragPwd(offer.get());

  EXPECT_FALSE(callee->SetRemoteDescription(std::move(offer)));
}

// The following group tests that ICE candidates are not generated before
// SetLocalDescription is called on a PeerConnection.

TEST_F(PeerConnectionIceUnitTest, NoIceCandidatesBeforeSetLocalDescription) {
  const SocketAddress kLocalAddress("1.1.1.1", 0);

  auto caller = CreatePeerConnectionWithAudioVideo();
  caller->network()->AddInterface(kLocalAddress);

  // Pump for 1 second and verify that no candidates are generated.
  rtc::Thread::Current()->ProcessMessages(1000);

  EXPECT_EQ(0u, caller->observer()->candidates_.size());
}
TEST_F(PeerConnectionIceUnitTest,
       NoIceCandidatesBeforeAnswerSetAsLocalDescription) {
  const SocketAddress kCallerAddress("1.1.1.1", 1111);

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();
  caller->network()->AddInterface(kCallerAddress);

  auto offer = caller->CreateOfferAndSetAsLocal();
  cricket::Candidate candidate = CreateLocalUdpCandidate(kCallerAddress);
  AddCandidateToFirstTransport(&candidate, offer.get());
  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));

  // Pump for 1 second and verify that no candidates are generated.
  rtc::Thread::Current()->ProcessMessages(1000);

  EXPECT_EQ(0u, callee->observer()->candidates_.size());
}

TEST_F(PeerConnectionIceUnitTest,
       CannotAddCandidateWhenRemoteDescriptionNotSet) {
  const SocketAddress kCalleeAddress("1.1.1.1", 1111);

  auto caller = CreatePeerConnectionWithAudioVideo();
  cricket::Candidate candidate = CreateLocalUdpCandidate(kCalleeAddress);
  JsepIceCandidate jsep_candidate(cricket::CN_AUDIO, 0, candidate);

  EXPECT_FALSE(caller->pc()->AddIceCandidate(&jsep_candidate));

  caller->CreateOfferAndSetAsLocal();

  EXPECT_FALSE(caller->pc()->AddIceCandidate(&jsep_candidate));
}

TEST_F(PeerConnectionIceUnitTest, DuplicateIceCandidateIgnoredWhenAdded) {
  const SocketAddress kCalleeAddress("1.1.1.1", 1111);

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  cricket::Candidate candidate = CreateLocalUdpCandidate(kCalleeAddress);
  caller->AddIceCandidate(&candidate);
  EXPECT_TRUE(caller->AddIceCandidate(&candidate));
  EXPECT_EQ(1u, caller->GetIceCandidatesFromRemoteDescription().size());
}

TEST_F(PeerConnectionIceUnitTest,
       AddRemoveCandidateWithEmptyTransportDoesNotCrash) {
  const SocketAddress kCalleeAddress("1.1.1.1", 1111);

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  // |candidate.transport_name()| is empty.
  cricket::Candidate candidate = CreateLocalUdpCandidate(kCalleeAddress);
  JsepIceCandidate ice_candidate(cricket::CN_AUDIO, 0, candidate);
  EXPECT_TRUE(caller->pc()->AddIceCandidate(&ice_candidate));
  EXPECT_TRUE(caller->pc()->RemoveIceCandidates({candidate}));
}

TEST_F(PeerConnectionIceUnitTest, RemoveCandidateRemovesFromRemoteDescription) {
  const SocketAddress kCalleeAddress("1.1.1.1", 1111);

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  cricket::Candidate candidate = CreateLocalUdpCandidate(kCalleeAddress);
  ASSERT_TRUE(caller->AddIceCandidate(&candidate));
  EXPECT_TRUE(caller->pc()->RemoveIceCandidates({candidate}));
  EXPECT_EQ(0u, caller->GetIceCandidatesFromRemoteDescription().size());
}

// Test that if a candidate is added via AddIceCandidate and via an updated
// remote description, then both candidates appear in the stored remote
// description.
TEST_F(PeerConnectionIceUnitTest,
       CandidateInSubsequentOfferIsAddedToRemoteDescription) {
  const SocketAddress kCallerAddress1("1.1.1.1", 1111);
  const SocketAddress kCallerAddress2("2.2.2.2", 2222);

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  // Add one candidate via |AddIceCandidate|.
  cricket::Candidate candidate1 = CreateLocalUdpCandidate(kCallerAddress1);
  ASSERT_TRUE(callee->AddIceCandidate(&candidate1));

  // Add the second candidate via a reoffer.
  auto offer = caller->CreateOffer();
  cricket::Candidate candidate2 = CreateLocalUdpCandidate(kCallerAddress2);
  AddCandidateToFirstTransport(&candidate2, offer.get());

  // Expect both candidates to appear in the callee's remote description.
  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));
  EXPECT_EQ(2u, callee->GetIceCandidatesFromRemoteDescription().size());
}

// The follow test verifies that SetLocal/RemoteDescription fails when an offer
// has either ICE ufrag/pwd too short or too long and succeeds otherwise.
// The standard (https://tools.ietf.org/html/rfc5245#section-15.4) says that
// pwd must be 22-256 characters and ufrag must be 4-256 characters.
TEST_F(PeerConnectionIceUnitTest, VerifyUfragPwdLength) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  auto set_local_description_with_ufrag_pwd_length =
      [this, &caller](int ufrag_len, int pwd_len) {
        auto offer = caller->CreateOffer();
        SetIceUfragPwd(offer.get(), std::string(ufrag_len, 'x'),
                       std::string(pwd_len, 'x'));
        return caller->SetLocalDescription(std::move(offer));
      };

  auto set_remote_description_with_ufrag_pwd_length =
      [this, &caller, &callee](int ufrag_len, int pwd_len) {
        auto offer = caller->CreateOffer();
        SetIceUfragPwd(offer.get(), std::string(ufrag_len, 'x'),
                       std::string(pwd_len, 'x'));
        return callee->SetRemoteDescription(std::move(offer));
      };

  EXPECT_FALSE(set_local_description_with_ufrag_pwd_length(3, 22));
  EXPECT_FALSE(set_remote_description_with_ufrag_pwd_length(3, 22));
  EXPECT_FALSE(set_local_description_with_ufrag_pwd_length(257, 22));
  EXPECT_FALSE(set_remote_description_with_ufrag_pwd_length(257, 22));
  EXPECT_FALSE(set_local_description_with_ufrag_pwd_length(4, 21));
  EXPECT_FALSE(set_remote_description_with_ufrag_pwd_length(4, 21));
  EXPECT_FALSE(set_local_description_with_ufrag_pwd_length(4, 257));
  EXPECT_FALSE(set_remote_description_with_ufrag_pwd_length(4, 257));
  EXPECT_TRUE(set_local_description_with_ufrag_pwd_length(4, 22));
  EXPECT_TRUE(set_remote_description_with_ufrag_pwd_length(4, 22));
  EXPECT_TRUE(set_local_description_with_ufrag_pwd_length(256, 256));
  EXPECT_TRUE(set_remote_description_with_ufrag_pwd_length(256, 256));
}

::testing::AssertionResult AssertIpInCandidates(
    const char* address_expr,
    const char* candidates_expr,
    const SocketAddress& address,
    const std::vector<IceCandidateInterface*> candidates) {
  std::stringstream candidate_hosts;
  for (const auto* candidate : candidates) {
    const auto& candidate_ip = candidate->candidate().address().ipaddr();
    if (candidate_ip == address.ipaddr()) {
      return ::testing::AssertionSuccess();
    }
    candidate_hosts << "\n" << candidate_ip;
  }
  return ::testing::AssertionFailure()
         << address_expr << " (host " << address.HostAsURIString()
         << ") not in " << candidates_expr
         << " which have the following address hosts:" << candidate_hosts.str();
}

TEST_F(PeerConnectionIceUnitTest, CandidatesGeneratedForEachLocalInterface) {
  const SocketAddress kLocalAddress1("1.1.1.1", 0);
  const SocketAddress kLocalAddress2("2.2.2.2", 0);

  auto caller = CreatePeerConnectionWithAudioVideo();
  caller->network()->AddInterface(kLocalAddress1);
  caller->network()->AddInterface(kLocalAddress2);

  caller->CreateOfferAndSetAsLocal();
  EXPECT_TRUE_WAIT(caller->IsIceGatheringDone(), kIceCandidatesTimeout);

  auto candidates = caller->observer()->GetCandidatesByMline(0);
  EXPECT_PRED_FORMAT2(AssertIpInCandidates, kLocalAddress1, candidates);
  EXPECT_PRED_FORMAT2(AssertIpInCandidates, kLocalAddress2, candidates);
}

TEST_F(PeerConnectionIceUnitTest,
       TrickledSingleCandidateAddedToRemoteDescription) {
  const SocketAddress kCallerAddress("1.1.1.1", 1111);

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));

  cricket::Candidate candidate = CreateLocalUdpCandidate(kCallerAddress);
  callee->AddIceCandidate(&candidate);
  auto candidates = callee->GetIceCandidatesFromRemoteDescription();
  ASSERT_EQ(1u, candidates.size());
  EXPECT_PRED_FORMAT2(AssertCandidatesEqual, candidate,
                      candidates[0]->candidate());
}

TEST_F(PeerConnectionIceUnitTest,
       TwoTrickledCandidatesAddedToRemoteDescription) {
  const SocketAddress kCalleeAddress1("1.1.1.1", 1111);
  const SocketAddress kCalleeAddress2("2.2.2.2", 2222);

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  cricket::Candidate candidate1 = CreateLocalUdpCandidate(kCalleeAddress1);
  caller->AddIceCandidate(&candidate1);

  cricket::Candidate candidate2 = CreateLocalUdpCandidate(kCalleeAddress2);
  caller->AddIceCandidate(&candidate2);

  auto candidates = caller->GetIceCandidatesFromRemoteDescription();
  ASSERT_EQ(2u, candidates.size());
  EXPECT_PRED_FORMAT2(AssertCandidatesEqual, candidate1,
                      candidates[0]->candidate());
  EXPECT_PRED_FORMAT2(AssertCandidatesEqual, candidate2,
                      candidates[1]->candidate());
}

TEST_F(PeerConnectionIceUnitTest,
       LocalDescriptionUpdatedWhenContinualGathering) {
  const SocketAddress kLocalAddress("1.1.1.1", 0);

  RTCConfiguration config;
  config.continual_gathering_policy =
      PeerConnectionInterface::GATHER_CONTINUALLY;
  auto caller = CreatePeerConnectionWithAudioVideo(config);
  caller->network()->AddInterface(kLocalAddress);

  // Start ICE candidate gathering by setting the local offer.
  ASSERT_TRUE(caller->SetLocalDescription(caller->CreateOffer()));

  // Since we're using continual gathering, we won't get "gathering done".
  EXPECT_TRUE_WAIT(
      caller->pc()->local_description()->candidates(0)->count() > 0,
      kIceCandidatesTimeout);
}

// Test that when continual gathering is enabled, and a network interface goes
// down, the candidate is signaled as removed and removed from the local
// description.
TEST_F(PeerConnectionIceUnitTest,
       LocalCandidatesRemovedWhenNetworkDownIfGatheringContinually) {
  const SocketAddress kLocalAddress("1.1.1.1", 0);

  RTCConfiguration config;
  config.continual_gathering_policy =
      PeerConnectionInterface::GATHER_CONTINUALLY;
  auto caller = CreatePeerConnectionWithAudioVideo(config);
  caller->network()->AddInterface(kLocalAddress);

  // Start ICE candidate gathering by setting the local offer.
  ASSERT_TRUE(caller->SetLocalDescription(caller->CreateOffer()));

  EXPECT_TRUE_WAIT(
      caller->pc()->local_description()->candidates(0)->count() > 0,
      kIceCandidatesTimeout);

  // Remove the only network interface, causing the PeerConnection to signal
  // the removal of all candidates derived from this interface.
  caller->network()->RemoveInterface(kLocalAddress);

  EXPECT_EQ_WAIT(0u, caller->pc()->local_description()->candidates(0)->count(),
                 kIceCandidatesTimeout);
  EXPECT_LT(0, caller->observer()->num_candidates_removed_);
}

TEST_F(PeerConnectionIceUnitTest,
       LocalCandidatesNotRemovedWhenNetworkDownIfGatheringOnce) {
  const SocketAddress kLocalAddress("1.1.1.1", 0);

  RTCConfiguration config;
  config.continual_gathering_policy = PeerConnectionInterface::GATHER_ONCE;
  auto caller = CreatePeerConnectionWithAudioVideo(config);
  caller->network()->AddInterface(kLocalAddress);

  // Start ICE candidate gathering by setting the local offer.
  ASSERT_TRUE(caller->SetLocalDescription(caller->CreateOffer()));

  EXPECT_TRUE_WAIT(caller->IsIceGatheringDone(), kIceCandidatesTimeout);

  caller->network()->RemoveInterface(kLocalAddress);

  // Verify that the local candidates are not removed;
  rtc::Thread::Current()->ProcessMessages(1000);
  EXPECT_EQ(0, caller->observer()->num_candidates_removed_);
}

// The following group tests that when an offer includes a new ufrag or pwd
// (indicating an ICE restart) the old candidates are removed and new candidates
// added to the remote description.

TEST_F(PeerConnectionIceUnitTest, IceRestartOfferClearsExistingCandidate) {
  const SocketAddress kCallerAddress("1.1.1.1", 1111);

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  auto offer = caller->CreateOffer();
  cricket::Candidate candidate = CreateLocalUdpCandidate(kCallerAddress);
  AddCandidateToFirstTransport(&candidate, offer.get());

  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));

  RTCOfferAnswerOptions options;
  options.ice_restart = true;
  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOffer(options)));

  EXPECT_EQ(0u, callee->GetIceCandidatesFromRemoteDescription().size());
}
TEST_F(PeerConnectionIceUnitTest,
       IceRestartOfferCandidateReplacesExistingCandidate) {
  const SocketAddress kFirstCallerAddress("1.1.1.1", 1111);
  const SocketAddress kRestartedCallerAddress("2.2.2.2", 2222);

  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  auto offer = caller->CreateOffer();
  cricket::Candidate old_candidate =
      CreateLocalUdpCandidate(kFirstCallerAddress);
  AddCandidateToFirstTransport(&old_candidate, offer.get());

  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));

  RTCOfferAnswerOptions options;
  options.ice_restart = true;
  auto restart_offer = caller->CreateOffer(options);
  cricket::Candidate new_candidate =
      CreateLocalUdpCandidate(kRestartedCallerAddress);
  AddCandidateToFirstTransport(&new_candidate, restart_offer.get());

  ASSERT_TRUE(callee->SetRemoteDescription(std::move(restart_offer)));

  auto remote_candidates = callee->GetIceCandidatesFromRemoteDescription();
  ASSERT_EQ(1u, remote_candidates.size());
  EXPECT_PRED_FORMAT2(AssertCandidatesEqual, new_candidate,
                      remote_candidates[0]->candidate());
}

// Test that if there is not an ICE restart (i.e., nothing changes), then the
// answer to a later offer should have the same ufrag/pwd as the first answer.
TEST_F(PeerConnectionIceUnitTest,
       LaterAnswerHasSameIceCredentialsIfNoIceRestart) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  // Re-offer.
  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));

  auto answer = callee->CreateAnswer();
  auto* answer_transport_desc = GetFirstTransportDescription(answer.get());
  auto* local_transport_desc =
      GetFirstTransportDescription(callee->pc()->local_description());

  EXPECT_EQ(answer_transport_desc->ice_ufrag, local_transport_desc->ice_ufrag);
  EXPECT_EQ(answer_transport_desc->ice_pwd, local_transport_desc->ice_pwd);
}

// The following parameterized test verifies that if an offer is sent with a
// modified ICE ufrag and/or ICE pwd, then the answer should identify that the
// other side has initiated an ICE restart and generate a new ufrag and pwd.
// RFC 5245 says: "If the offer contained a change in the a=ice-ufrag or
// a=ice-pwd attributes compared to the previous SDP from the peer, it
// indicates that ICE is restarting for this media stream."

class PeerConnectionIceUfragPwdAnswerUnitTest
    : public PeerConnectionIceUnitTest,
      public ::testing::WithParamInterface<std::pair<bool, bool>> {
 protected:
  PeerConnectionIceUfragPwdAnswerUnitTest() {
    offer_new_ufrag_ = GetParam().first;
    offer_new_pwd_ = GetParam().second;
  }

  bool offer_new_ufrag_;
  bool offer_new_pwd_;
};

TEST_P(PeerConnectionIceUfragPwdAnswerUnitTest, TestIncludedInAnswer) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  auto offer = caller->CreateOffer();
  auto* offer_transport_desc = GetFirstTransportDescription(offer.get());
  if (offer_new_ufrag_) {
    offer_transport_desc->ice_ufrag += "_new";
  }
  if (offer_new_pwd_) {
    offer_transport_desc->ice_pwd += "_new";
  }

  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));

  auto answer = callee->CreateAnswer();
  auto* answer_transport_desc = GetFirstTransportDescription(answer.get());
  auto* local_transport_desc =
      GetFirstTransportDescription(callee->pc()->local_description());

  EXPECT_NE(answer_transport_desc->ice_ufrag, local_transport_desc->ice_ufrag);
  EXPECT_NE(answer_transport_desc->ice_pwd, local_transport_desc->ice_pwd);
}

INSTANTIATE_TEST_CASE_P(
    PeerConnectionIceUnitTest,
    PeerConnectionIceUfragPwdAnswerUnitTest,
    Values(std::make_pair(true, true),     // Both changed.
           std::make_pair(true, false),    // Only ufrag changed.
           std::make_pair(false, true)));  // Only pwd changed.

// Test that if an ICE restart is offered on one media section, then the answer
// will only change ICE ufrag/pwd for that section and keep the other sections
// the same.
// Note that this only works if we have disabled BUNDLE, otherwise all media
// sections will share the same transport.
TEST_F(PeerConnectionIceUnitTest,
       CreateAnswerHasNewUfragPwdForOnlyMediaSectionWhichRestarted) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  ASSERT_TRUE(callee->SetRemoteDescription(caller->CreateOfferAndSetAsLocal()));
  ASSERT_TRUE(
      caller->SetRemoteDescription(callee->CreateAnswerAndSetAsLocal()));

  RTCOfferAnswerOptions disable_bundle_options;
  disable_bundle_options.use_rtp_mux = false;

  auto offer = caller->CreateOffer(disable_bundle_options);

  // Signal ICE restart on the first media section.
  auto* offer_transport_desc = GetFirstTransportDescription(offer.get());
  offer_transport_desc->ice_ufrag += "_new";
  offer_transport_desc->ice_pwd += "_new";

  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));

  auto answer = callee->CreateAnswer(disable_bundle_options);
  const auto& answer_transports = answer->description()->transport_infos();
  const auto& local_transports =
      callee->pc()->local_description()->description()->transport_infos();

  EXPECT_NE(answer_transports[0].description.ice_ufrag,
            local_transports[0].description.ice_ufrag);
  EXPECT_NE(answer_transports[0].description.ice_pwd,
            local_transports[0].description.ice_pwd);
  EXPECT_EQ(answer_transports[1].description.ice_ufrag,
            local_transports[1].description.ice_ufrag);
  EXPECT_EQ(answer_transports[1].description.ice_pwd,
            local_transports[1].description.ice_pwd);
}

// Test that when the initial offerer (caller) uses the lite implementation of
// ICE and the callee uses the full implementation, the caller takes the
// CONTROLLED role and the callee takes the CONTROLLING role. This is specified
// in RFC5245 Section 5.1.1.
TEST_F(PeerConnectionIceUnitTest,
       OfferFromLiteIceControlledAndAnswerFromFullIceControlling) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  auto offer = caller->CreateOffer();
  SetIceMode(offer.get(), cricket::IceMode::ICEMODE_LITE);
  ASSERT_TRUE(
      caller->SetLocalDescription(CloneSessionDescription(offer.get())));
  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));

  auto answer = callee->CreateAnswer();
  SetIceMode(answer.get(), cricket::IceMode::ICEMODE_FULL);
  ASSERT_TRUE(
      callee->SetLocalDescription(CloneSessionDescription(answer.get())));
  ASSERT_TRUE(caller->SetRemoteDescription(std::move(answer)));

  EXPECT_EQ(cricket::ICEROLE_CONTROLLED, GetIceRole(caller));
  EXPECT_EQ(cricket::ICEROLE_CONTROLLING, GetIceRole(callee));
}

// Test that when the caller and the callee both use the lite implementation of
// ICE, the initial offerer (caller) takes the CONTROLLING role and the callee
// takes the CONTROLLED role. This is specified in RFC5245 Section 5.1.1.
TEST_F(PeerConnectionIceUnitTest,
       OfferFromLiteIceControllingAndAnswerFromLiteIceControlled) {
  auto caller = CreatePeerConnectionWithAudioVideo();
  auto callee = CreatePeerConnectionWithAudioVideo();

  auto offer = caller->CreateOffer();
  SetIceMode(offer.get(), cricket::IceMode::ICEMODE_LITE);
  ASSERT_TRUE(
      caller->SetLocalDescription(CloneSessionDescription(offer.get())));
  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));

  auto answer = callee->CreateAnswer();
  SetIceMode(answer.get(), cricket::IceMode::ICEMODE_LITE);
  ASSERT_TRUE(
      callee->SetLocalDescription(CloneSessionDescription(answer.get())));
  ASSERT_TRUE(caller->SetRemoteDescription(std::move(answer)));

  EXPECT_EQ(cricket::ICEROLE_CONTROLLING, GetIceRole(caller));
  EXPECT_EQ(cricket::ICEROLE_CONTROLLED, GetIceRole(callee));
}

}  // namespace webrtc
