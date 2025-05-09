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
#include <set>
#include <string>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__call__call_factory_interface_h //original-code:"api/call/call_factory_interface.h"
#include BOSS_WEBRTC_U_api__jsep_h //original-code:"api/jsep.h"
#include BOSS_WEBRTC_U_api__jsep_session_description_h //original-code:"api/jsep_session_description.h"
#include BOSS_WEBRTC_U_api__peer_connection_interface_h //original-code:"api/peer_connection_interface.h"
#include BOSS_WEBRTC_U_api__rtc_error_h //original-code:"api/rtc_error.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__task_queue__default_task_queue_factory_h //original-code:"api/task_queue/default_task_queue_factory.h"
#include BOSS_WEBRTC_U_media__base__fake_media_engine_h //original-code:"media/base/fake_media_engine.h"
#include BOSS_WEBRTC_U_p2p__base__mock_async_resolver_h //original-code:"p2p/base/mock_async_resolver.h"
#include BOSS_WEBRTC_U_p2p__base__port_allocator_h //original-code:"p2p/base/port_allocator.h"
#include BOSS_WEBRTC_U_p2p__client__basic_port_allocator_h //original-code:"p2p/client/basic_port_allocator.h"
#include BOSS_WEBRTC_U_pc__peer_connection_h //original-code:"pc/peer_connection.h"
#include BOSS_WEBRTC_U_pc__peer_connection_factory_h //original-code:"pc/peer_connection_factory.h"
#include BOSS_WEBRTC_U_pc__peer_connection_proxy_h //original-code:"pc/peer_connection_proxy.h"
#include BOSS_WEBRTC_U_pc__peer_connection_wrapper_h //original-code:"pc/peer_connection_wrapper.h"
#include BOSS_WEBRTC_U_pc__sdp_utils_h //original-code:"pc/sdp_utils.h"
#include "pc/test/mock_peer_connection_observers.h"
#include BOSS_WEBRTC_U_pc__usage_pattern_h //original-code:"pc/usage_pattern.h"
#include BOSS_WEBRTC_U_pc__webrtc_sdp_h //original-code:"pc/webrtc_sdp.h"
#include BOSS_WEBRTC_U_rtc_base__arraysize_h //original-code:"rtc_base/arraysize.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__fake_mdns_responder_h //original-code:"rtc_base/fake_mdns_responder.h"
#include BOSS_WEBRTC_U_rtc_base__fake_network_h //original-code:"rtc_base/fake_network.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__rtc_certificate_generator_h //original-code:"rtc_base/rtc_certificate_generator.h"
#include BOSS_WEBRTC_U_rtc_base__socket_address_h //original-code:"rtc_base/socket_address.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__virtual_socket_server_h //original-code:"rtc_base/virtual_socket_server.h"
#include BOSS_WEBRTC_U_system_wrappers__include__metrics_h //original-code:"system_wrappers/include/metrics.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

using RTCConfiguration = PeerConnectionInterface::RTCConfiguration;
using RTCOfferAnswerOptions = PeerConnectionInterface::RTCOfferAnswerOptions;
using ::testing::NiceMock;
using ::testing::Values;

static const char kUsagePatternMetric[] = "WebRTC.PeerConnection.UsagePattern";
static constexpr int kDefaultTimeout = 10000;
static const rtc::SocketAddress kLocalAddrs[2] = {
    rtc::SocketAddress("1.1.1.1", 0), rtc::SocketAddress("2.2.2.2", 0)};
static const rtc::SocketAddress kPrivateLocalAddress("10.1.1.1", 0);
static const rtc::SocketAddress kPrivateIpv6LocalAddress("fd12:3456:789a:1::1",
                                                         0);

int MakeUsageFingerprint(std::set<UsageEvent> events) {
  int signature = 0;
  for (const auto it : events) {
    signature |= static_cast<int>(it);
  }
  return signature;
}

class PeerConnectionFactoryForUsageHistogramTest
    : public rtc::RefCountedObject<PeerConnectionFactory> {
 public:
  PeerConnectionFactoryForUsageHistogramTest()
      : rtc::RefCountedObject<PeerConnectionFactory>([] {
          PeerConnectionFactoryDependencies dependencies;
          dependencies.network_thread = rtc::Thread::Current();
          dependencies.worker_thread = rtc::Thread::Current();
          dependencies.signaling_thread = rtc::Thread::Current();
          dependencies.task_queue_factory = CreateDefaultTaskQueueFactory();
          dependencies.media_engine =
              std::make_unique<cricket::FakeMediaEngine>();
          dependencies.call_factory = CreateCallFactory();
          return dependencies;
        }()) {}
};

class PeerConnectionWrapperForUsageHistogramTest;

typedef PeerConnectionWrapperForUsageHistogramTest* RawWrapperPtr;

class ObserverForUsageHistogramTest : public MockPeerConnectionObserver {
 public:
  void OnIceCandidate(const webrtc::IceCandidateInterface* candidate) override;

  void OnInterestingUsage(int usage_pattern) override {
    interesting_usage_detected_ = usage_pattern;
  }

  void PrepareToExchangeCandidates(RawWrapperPtr other) {
    candidate_target_ = other;
  }

  bool HaveDataChannel() { return last_datachannel_; }

  absl::optional<int> interesting_usage_detected() {
    return interesting_usage_detected_;
  }

  void ClearInterestingUsageDetector() {
    interesting_usage_detected_ = absl::optional<int>();
  }

  bool candidate_gathered() const { return candidate_gathered_; }

 private:
  absl::optional<int> interesting_usage_detected_;
  bool candidate_gathered_ = false;
  RawWrapperPtr candidate_target_;  // Note: Not thread-safe against deletions.
};

class PeerConnectionWrapperForUsageHistogramTest
    : public PeerConnectionWrapper {
 public:
  using PeerConnectionWrapper::PeerConnectionWrapper;

  PeerConnection* GetInternalPeerConnection() {
    auto* pci =
        static_cast<PeerConnectionProxyWithInternal<PeerConnectionInterface>*>(
            pc());
    return static_cast<PeerConnection*>(pci->internal());
  }

  // Override with different return type
  ObserverForUsageHistogramTest* observer() {
    return static_cast<ObserverForUsageHistogramTest*>(
        PeerConnectionWrapper::observer());
  }

  void PrepareToExchangeCandidates(
      PeerConnectionWrapperForUsageHistogramTest* other) {
    observer()->PrepareToExchangeCandidates(other);
    other->observer()->PrepareToExchangeCandidates(this);
  }

  bool IsConnected() {
    return pc()->ice_connection_state() ==
               PeerConnectionInterface::kIceConnectionConnected ||
           pc()->ice_connection_state() ==
               PeerConnectionInterface::kIceConnectionCompleted;
  }

  bool HaveDataChannel() {
    return static_cast<ObserverForUsageHistogramTest*>(observer())
        ->HaveDataChannel();
  }
  void BufferIceCandidate(const webrtc::IceCandidateInterface* candidate) {
    std::string sdp;
    EXPECT_TRUE(candidate->ToString(&sdp));
    std::unique_ptr<webrtc::IceCandidateInterface> candidate_copy(
        CreateIceCandidate(candidate->sdp_mid(), candidate->sdp_mline_index(),
                           sdp, nullptr));
    buffered_candidates_.push_back(std::move(candidate_copy));
  }

  void AddBufferedIceCandidates() {
    for (const auto& candidate : buffered_candidates_) {
      EXPECT_TRUE(pc()->AddIceCandidate(candidate.get()));
    }
    buffered_candidates_.clear();
  }

  // This method performs the following actions in sequence:
  // 1. Exchange Offer and Answer.
  // 2. Exchange ICE candidates after both caller and callee complete
  // gathering.
  // 3. Wait for ICE to connect.
  //
  // This guarantees a deterministic sequence of events and also rules out the
  // occurrence of prflx candidates if the offer/answer signaling and the
  // candidate trickling race in order. In case prflx candidates need to be
  // simulated, see the approach used by tests below for that.
  bool ConnectTo(PeerConnectionWrapperForUsageHistogramTest* callee) {
    PrepareToExchangeCandidates(callee);
    if (!ExchangeOfferAnswerWith(callee)) {
      return false;
    }
    // Wait until the gathering completes before we signal the candidate.
    WAIT(observer()->ice_gathering_complete_, kDefaultTimeout);
    WAIT(callee->observer()->ice_gathering_complete_, kDefaultTimeout);
    AddBufferedIceCandidates();
    callee->AddBufferedIceCandidates();
    WAIT(IsConnected(), kDefaultTimeout);
    WAIT(callee->IsConnected(), kDefaultTimeout);
    return IsConnected() && callee->IsConnected();
  }

  bool GenerateOfferAndCollectCandidates() {
    auto offer = CreateOffer(RTCOfferAnswerOptions());
    if (!offer) {
      return false;
    }
    bool set_local_offer =
        SetLocalDescription(CloneSessionDescription(offer.get()));
    EXPECT_TRUE(set_local_offer);
    if (!set_local_offer) {
      return false;
    }
    EXPECT_TRUE_WAIT(observer()->ice_gathering_complete_, kDefaultTimeout);
    return true;
  }

  webrtc::PeerConnectionInterface::IceGatheringState ice_gathering_state() {
    return pc()->ice_gathering_state();
  }

 private:
  // Candidates that have been sent but not yet configured
  std::vector<std::unique_ptr<webrtc::IceCandidateInterface>>
      buffered_candidates_;
};

// Buffers candidates until we add them via AddBufferedIceCandidates.
void ObserverForUsageHistogramTest::OnIceCandidate(
    const webrtc::IceCandidateInterface* candidate) {
  // If target is not set, ignore. This happens in one-ended unit tests.
  if (candidate_target_) {
    this->candidate_target_->BufferIceCandidate(candidate);
  }
  candidate_gathered_ = true;
}

class PeerConnectionUsageHistogramTest : public ::testing::Test {
 protected:
  typedef std::unique_ptr<PeerConnectionWrapperForUsageHistogramTest>
      WrapperPtr;

  PeerConnectionUsageHistogramTest()
      : vss_(new rtc::VirtualSocketServer()), main_(vss_.get()) {
    webrtc::metrics::Reset();
  }

  WrapperPtr CreatePeerConnection() {
    return CreatePeerConnection(
        RTCConfiguration(), PeerConnectionFactoryInterface::Options(), nullptr);
  }

  WrapperPtr CreatePeerConnection(const RTCConfiguration& config) {
    return CreatePeerConnection(
        config, PeerConnectionFactoryInterface::Options(), nullptr);
  }

  WrapperPtr CreatePeerConnectionWithMdns(const RTCConfiguration& config) {
    auto resolver_factory =
        std::make_unique<NiceMock<webrtc::MockAsyncResolverFactory>>();

    webrtc::PeerConnectionDependencies deps(nullptr /* observer_in */);

    auto fake_network = NewFakeNetwork();
    fake_network->set_mdns_responder(
        std::make_unique<webrtc::FakeMdnsResponder>(rtc::Thread::Current()));
    fake_network->AddInterface(NextLocalAddress());

    std::unique_ptr<cricket::BasicPortAllocator> port_allocator(
        new cricket::BasicPortAllocator(fake_network));

    deps.async_resolver_factory = std::move(resolver_factory);
    deps.allocator = std::move(port_allocator);

    return CreatePeerConnection(
        config, PeerConnectionFactoryInterface::Options(), std::move(deps));
  }

  WrapperPtr CreatePeerConnectionWithImmediateReport() {
    RTCConfiguration configuration;
    configuration.report_usage_pattern_delay_ms = 0;
    return CreatePeerConnection(
        configuration, PeerConnectionFactoryInterface::Options(), nullptr);
  }

  WrapperPtr CreatePeerConnectionWithPrivateLocalAddresses() {
    auto* fake_network = NewFakeNetwork();
    fake_network->AddInterface(NextLocalAddress());
    fake_network->AddInterface(kPrivateLocalAddress);

    auto port_allocator =
        std::make_unique<cricket::BasicPortAllocator>(fake_network);
    return CreatePeerConnection(RTCConfiguration(),
                                PeerConnectionFactoryInterface::Options(),
                                std::move(port_allocator));
  }

  WrapperPtr CreatePeerConnectionWithPrivateIpv6LocalAddresses() {
    auto* fake_network = NewFakeNetwork();
    fake_network->AddInterface(NextLocalAddress());
    fake_network->AddInterface(kPrivateIpv6LocalAddress);

    auto port_allocator =
        std::make_unique<cricket::BasicPortAllocator>(fake_network);

    return CreatePeerConnection(RTCConfiguration(),
                                PeerConnectionFactoryInterface::Options(),
                                std::move(port_allocator));
  }

  WrapperPtr CreatePeerConnection(
      const RTCConfiguration& config,
      const PeerConnectionFactoryInterface::Options factory_options,
      std::unique_ptr<cricket::PortAllocator> allocator) {
    PeerConnectionDependencies deps(nullptr);
    deps.allocator = std::move(allocator);

    return CreatePeerConnection(config, factory_options, std::move(deps));
  }

  WrapperPtr CreatePeerConnection(
      const RTCConfiguration& config,
      const PeerConnectionFactoryInterface::Options factory_options,
      PeerConnectionDependencies deps) {
    rtc::scoped_refptr<PeerConnectionFactoryForUsageHistogramTest> pc_factory(
        new PeerConnectionFactoryForUsageHistogramTest());
    pc_factory->SetOptions(factory_options);

    // If no allocator is provided, one will be created using a network manager
    // that uses the host network. This doesn't work on all trybots.
    if (!deps.allocator) {
      auto fake_network = NewFakeNetwork();
      fake_network->AddInterface(NextLocalAddress());
      deps.allocator =
          std::make_unique<cricket::BasicPortAllocator>(fake_network);
    }

    auto observer = std::make_unique<ObserverForUsageHistogramTest>();
    deps.observer = observer.get();

    auto pc = pc_factory->CreatePeerConnection(config, std::move(deps));
    if (!pc) {
      return nullptr;
    }

    observer->SetPeerConnectionInterface(pc.get());
    auto wrapper = std::make_unique<PeerConnectionWrapperForUsageHistogramTest>(
        pc_factory, pc, std::move(observer));
    return wrapper;
  }

  int ObservedFingerprint() {
    // This works correctly only if there is only one sample value
    // that has been counted.
    // Returns -1 for "not found".
    return webrtc::metrics::MinSample(kUsagePatternMetric);
  }

  // The PeerConnection's port allocator is tied to the PeerConnection's
  // lifetime and expects the underlying NetworkManager to outlive it.  That
  // prevents us from having the PeerConnectionWrapper own the fake network.
  // Therefore, the test fixture will own all the fake networks even though
  // tests should access the fake network through the PeerConnectionWrapper.
  rtc::FakeNetworkManager* NewFakeNetwork() {
    fake_networks_.emplace_back(std::make_unique<rtc::FakeNetworkManager>());
    return fake_networks_.back().get();
  }

  rtc::SocketAddress NextLocalAddress() {
    RTC_DCHECK(next_local_address_ < (int)arraysize(kLocalAddrs));
    return kLocalAddrs[next_local_address_++];
  }

  std::vector<std::unique_ptr<rtc::FakeNetworkManager>> fake_networks_;
  int next_local_address_ = 0;
  std::unique_ptr<rtc::VirtualSocketServer> vss_;
  rtc::AutoSocketServerThread main_;
};

TEST_F(PeerConnectionUsageHistogramTest, UsageFingerprintHistogramFromTimeout) {
  auto pc = CreatePeerConnectionWithImmediateReport();

  int expected_fingerprint = MakeUsageFingerprint({});
  EXPECT_METRIC_EQ_WAIT(1, webrtc::metrics::NumSamples(kUsagePatternMetric),
                        kDefaultTimeout);
  EXPECT_METRIC_EQ(
      1, webrtc::metrics::NumEvents(kUsagePatternMetric, expected_fingerprint));
}

#ifndef WEBRTC_ANDROID
// These tests do not work on Android. Why is unclear.
// https://bugs.webrtc.org/9461

// Test getting the usage fingerprint for an audio/video connection.
TEST_F(PeerConnectionUsageHistogramTest, FingerprintAudioVideo) {
  auto caller = CreatePeerConnection();
  auto callee = CreatePeerConnection();
  caller->AddAudioTrack("audio");
  caller->AddVideoTrack("video");
  ASSERT_TRUE(caller->ConnectTo(callee.get()));
  caller->pc()->Close();
  callee->pc()->Close();
  int expected_fingerprint = MakeUsageFingerprint(
      {UsageEvent::AUDIO_ADDED, UsageEvent::VIDEO_ADDED,
       UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::ADD_ICE_CANDIDATE_SUCCEEDED,
       UsageEvent::ICE_STATE_CONNECTED, UsageEvent::REMOTE_CANDIDATE_ADDED,
       UsageEvent::DIRECT_CONNECTION_SELECTED, UsageEvent::CLOSE_CALLED});
  // In this case, we may or may not have PRIVATE_CANDIDATE_COLLECTED,
  // depending on the machine configuration.
  EXPECT_METRIC_EQ(2, webrtc::metrics::NumSamples(kUsagePatternMetric));
  EXPECT_METRIC_TRUE(
      webrtc::metrics::NumEvents(kUsagePatternMetric, expected_fingerprint) ==
          2 ||
      webrtc::metrics::NumEvents(
          kUsagePatternMetric,
          expected_fingerprint |
              static_cast<int>(UsageEvent::PRIVATE_CANDIDATE_COLLECTED)) == 2);
}

// Test getting the usage fingerprint when the caller collects an mDNS
// candidate.
TEST_F(PeerConnectionUsageHistogramTest, FingerprintWithMdnsCaller) {
  RTCConfiguration config;

  // Enable hostname candidates with mDNS names.
  auto caller = CreatePeerConnectionWithMdns(config);
  auto callee = CreatePeerConnection(config);

  caller->AddAudioTrack("audio");
  caller->AddVideoTrack("video");
  ASSERT_TRUE(caller->ConnectTo(callee.get()));
  caller->pc()->Close();
  callee->pc()->Close();

  int expected_fingerprint_caller = MakeUsageFingerprint(
      {UsageEvent::AUDIO_ADDED, UsageEvent::VIDEO_ADDED,
       UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::MDNS_CANDIDATE_COLLECTED,
       UsageEvent::ADD_ICE_CANDIDATE_SUCCEEDED, UsageEvent::ICE_STATE_CONNECTED,
       UsageEvent::REMOTE_CANDIDATE_ADDED,
       UsageEvent::DIRECT_CONNECTION_SELECTED, UsageEvent::CLOSE_CALLED});

  // Without a resolver, the callee cannot resolve the received mDNS candidate
  // but can still connect with the caller via a prflx candidate. As a result,
  // the bit for the direct connection should not be logged.
  int expected_fingerprint_callee = MakeUsageFingerprint(
      {UsageEvent::AUDIO_ADDED, UsageEvent::VIDEO_ADDED,
       UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::ADD_ICE_CANDIDATE_SUCCEEDED,
       UsageEvent::REMOTE_MDNS_CANDIDATE_ADDED, UsageEvent::ICE_STATE_CONNECTED,
       UsageEvent::REMOTE_CANDIDATE_ADDED, UsageEvent::CLOSE_CALLED});
  EXPECT_METRIC_EQ(2, webrtc::metrics::NumSamples(kUsagePatternMetric));
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumEvents(kUsagePatternMetric,
                                                 expected_fingerprint_caller));
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumEvents(kUsagePatternMetric,
                                                 expected_fingerprint_callee));
}

// Test getting the usage fingerprint when the callee collects an mDNS
// candidate.
TEST_F(PeerConnectionUsageHistogramTest, FingerprintWithMdnsCallee) {
  RTCConfiguration config;

  // Enable hostname candidates with mDNS names.
  auto caller = CreatePeerConnection(config);
  auto callee = CreatePeerConnectionWithMdns(config);

  caller->AddAudioTrack("audio");
  caller->AddVideoTrack("video");
  ASSERT_TRUE(caller->ConnectTo(callee.get()));
  caller->pc()->Close();
  callee->pc()->Close();

  // Similar to the test above, the caller connects with the callee via a prflx
  // candidate.
  int expected_fingerprint_caller = MakeUsageFingerprint(
      {UsageEvent::AUDIO_ADDED, UsageEvent::VIDEO_ADDED,
       UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::ADD_ICE_CANDIDATE_SUCCEEDED,
       UsageEvent::REMOTE_MDNS_CANDIDATE_ADDED, UsageEvent::ICE_STATE_CONNECTED,
       UsageEvent::REMOTE_CANDIDATE_ADDED, UsageEvent::CLOSE_CALLED});

  int expected_fingerprint_callee = MakeUsageFingerprint(
      {UsageEvent::AUDIO_ADDED, UsageEvent::VIDEO_ADDED,
       UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::MDNS_CANDIDATE_COLLECTED,
       UsageEvent::ADD_ICE_CANDIDATE_SUCCEEDED, UsageEvent::ICE_STATE_CONNECTED,
       UsageEvent::REMOTE_CANDIDATE_ADDED,
       UsageEvent::DIRECT_CONNECTION_SELECTED, UsageEvent::CLOSE_CALLED});
  EXPECT_METRIC_EQ(2, webrtc::metrics::NumSamples(kUsagePatternMetric));
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumEvents(kUsagePatternMetric,
                                                 expected_fingerprint_caller));
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumEvents(kUsagePatternMetric,
                                                 expected_fingerprint_callee));
}

#ifdef WEBRTC_HAVE_SCTP
TEST_F(PeerConnectionUsageHistogramTest, FingerprintDataOnly) {
  auto caller = CreatePeerConnection();
  auto callee = CreatePeerConnection();
  caller->CreateDataChannel("foodata");
  ASSERT_TRUE(caller->ConnectTo(callee.get()));
  ASSERT_TRUE_WAIT(callee->HaveDataChannel(), kDefaultTimeout);
  caller->pc()->Close();
  callee->pc()->Close();
  int expected_fingerprint = MakeUsageFingerprint(
      {UsageEvent::DATA_ADDED, UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::ADD_ICE_CANDIDATE_SUCCEEDED,
       UsageEvent::ICE_STATE_CONNECTED, UsageEvent::REMOTE_CANDIDATE_ADDED,
       UsageEvent::DIRECT_CONNECTION_SELECTED, UsageEvent::CLOSE_CALLED});
  EXPECT_METRIC_EQ(2, webrtc::metrics::NumSamples(kUsagePatternMetric));
  EXPECT_METRIC_TRUE(
      webrtc::metrics::NumEvents(kUsagePatternMetric, expected_fingerprint) ==
          2 ||
      webrtc::metrics::NumEvents(
          kUsagePatternMetric,
          expected_fingerprint |
              static_cast<int>(UsageEvent::PRIVATE_CANDIDATE_COLLECTED)) == 2);
}
#endif  // WEBRTC_HAVE_SCTP
#endif  // WEBRTC_ANDROID

TEST_F(PeerConnectionUsageHistogramTest, FingerprintStunTurn) {
  RTCConfiguration configuration;
  PeerConnection::IceServer server;
  server.urls = {"stun:dummy.stun.server"};
  configuration.servers.push_back(server);
  server.urls = {"turn:dummy.turn.server"};
  server.username = "username";
  server.password = "password";
  configuration.servers.push_back(server);
  auto caller = CreatePeerConnection(configuration);
  ASSERT_TRUE(caller);
  caller->pc()->Close();
  int expected_fingerprint = MakeUsageFingerprint(
      {UsageEvent::STUN_SERVER_ADDED, UsageEvent::TURN_SERVER_ADDED,
       UsageEvent::CLOSE_CALLED});
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumSamples(kUsagePatternMetric));
  EXPECT_METRIC_EQ(
      1, webrtc::metrics::NumEvents(kUsagePatternMetric, expected_fingerprint));
}

TEST_F(PeerConnectionUsageHistogramTest, FingerprintStunTurnInReconfiguration) {
  RTCConfiguration configuration;
  PeerConnection::IceServer server;
  server.urls = {"stun:dummy.stun.server"};
  configuration.servers.push_back(server);
  server.urls = {"turn:dummy.turn.server"};
  server.username = "username";
  server.password = "password";
  configuration.servers.push_back(server);
  auto caller = CreatePeerConnection();
  ASSERT_TRUE(caller);
  ASSERT_TRUE(caller->pc()->SetConfiguration(configuration).ok());
  caller->pc()->Close();
  int expected_fingerprint = MakeUsageFingerprint(
      {UsageEvent::STUN_SERVER_ADDED, UsageEvent::TURN_SERVER_ADDED,
       UsageEvent::CLOSE_CALLED});
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumSamples(kUsagePatternMetric));
  EXPECT_METRIC_EQ(
      1, webrtc::metrics::NumEvents(kUsagePatternMetric, expected_fingerprint));
}

TEST_F(PeerConnectionUsageHistogramTest, FingerprintWithPrivateIPCaller) {
  auto caller = CreatePeerConnectionWithPrivateLocalAddresses();
  auto callee = CreatePeerConnection();
  caller->AddAudioTrack("audio");
  ASSERT_TRUE(caller->ConnectTo(callee.get()));
  caller->pc()->Close();
  callee->pc()->Close();

  int expected_fingerprint_caller = MakeUsageFingerprint(
      {UsageEvent::AUDIO_ADDED, UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::PRIVATE_CANDIDATE_COLLECTED,
       UsageEvent::ADD_ICE_CANDIDATE_SUCCEEDED, UsageEvent::ICE_STATE_CONNECTED,
       UsageEvent::REMOTE_CANDIDATE_ADDED,
       UsageEvent::DIRECT_CONNECTION_SELECTED, UsageEvent::CLOSE_CALLED});

  int expected_fingerprint_callee = MakeUsageFingerprint(
      {UsageEvent::AUDIO_ADDED, UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::ADD_ICE_CANDIDATE_SUCCEEDED,
       UsageEvent::REMOTE_PRIVATE_CANDIDATE_ADDED,
       UsageEvent::ICE_STATE_CONNECTED, UsageEvent::REMOTE_CANDIDATE_ADDED,
       UsageEvent::DIRECT_CONNECTION_SELECTED, UsageEvent::CLOSE_CALLED});
  EXPECT_METRIC_EQ(2, webrtc::metrics::NumSamples(kUsagePatternMetric));
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumEvents(kUsagePatternMetric,
                                                 expected_fingerprint_caller));
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumEvents(kUsagePatternMetric,
                                                 expected_fingerprint_callee));
}

TEST_F(PeerConnectionUsageHistogramTest, FingerprintWithPrivateIpv6Callee) {
  auto caller = CreatePeerConnection();
  auto callee = CreatePeerConnectionWithPrivateIpv6LocalAddresses();
  caller->AddAudioTrack("audio");
  ASSERT_TRUE(caller->ConnectTo(callee.get()));
  caller->pc()->Close();
  callee->pc()->Close();

  int expected_fingerprint_caller = MakeUsageFingerprint(
      {UsageEvent::AUDIO_ADDED, UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::ADD_ICE_CANDIDATE_SUCCEEDED,
       UsageEvent::REMOTE_PRIVATE_CANDIDATE_ADDED,
       UsageEvent::ICE_STATE_CONNECTED, UsageEvent::REMOTE_CANDIDATE_ADDED,
       UsageEvent::REMOTE_IPV6_CANDIDATE_ADDED,
       UsageEvent::DIRECT_CONNECTION_SELECTED, UsageEvent::CLOSE_CALLED});

  int expected_fingerprint_callee = MakeUsageFingerprint(
      {UsageEvent::AUDIO_ADDED, UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::PRIVATE_CANDIDATE_COLLECTED,
       UsageEvent::IPV6_CANDIDATE_COLLECTED,
       UsageEvent::ADD_ICE_CANDIDATE_SUCCEEDED,
       UsageEvent::REMOTE_CANDIDATE_ADDED, UsageEvent::ICE_STATE_CONNECTED,
       UsageEvent::DIRECT_CONNECTION_SELECTED, UsageEvent::CLOSE_CALLED});
  EXPECT_METRIC_EQ(2, webrtc::metrics::NumSamples(kUsagePatternMetric));
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumEvents(kUsagePatternMetric,
                                                 expected_fingerprint_caller));
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumEvents(kUsagePatternMetric,
                                                 expected_fingerprint_callee));
}

#ifndef WEBRTC_ANDROID
#ifdef WEBRTC_HAVE_SCTP
// Test that the usage pattern bits for adding remote (private IPv6) candidates
// are set when the remote candidates are retrieved from the Offer SDP instead
// of trickled ICE messages.
TEST_F(PeerConnectionUsageHistogramTest,
       AddRemoteCandidatesFromRemoteDescription) {
  // We construct the following data-channel-only scenario. The caller collects
  // IPv6 private local candidates and appends them in the Offer as in
  // non-trickled sessions. The callee collects mDNS candidates that are not
  // contained in the Answer as in Trickle ICE. Only the Offer and Answer are
  // signaled and we expect a connection with prflx remote candidates at the
  // caller side.
  auto caller = CreatePeerConnectionWithPrivateIpv6LocalAddresses();
  auto callee = CreatePeerConnectionWithMdns(RTCConfiguration());
  caller->CreateDataChannel("test_channel");
  ASSERT_TRUE(caller->SetLocalDescription(caller->CreateOffer()));
  // Wait until the gathering completes so that the session description would
  // have contained ICE candidates.
  EXPECT_EQ_WAIT(webrtc::PeerConnectionInterface::kIceGatheringComplete,
                 caller->ice_gathering_state(), kDefaultTimeout);
  EXPECT_TRUE(caller->observer()->candidate_gathered());
  // Get the current offer that contains candidates and pass it to the callee.
  //
  // Note that we cannot use CloneSessionDescription on |cur_offer| to obtain an
  // SDP with candidates. The method above does not strictly copy everything, in
  // particular, not copying the ICE candidates.
  // TODO(qingsi): Technically, this is a bug. Fix it.
  auto cur_offer = caller->pc()->local_description();
  ASSERT_TRUE(cur_offer);
  std::string sdp_with_candidates_str;
  cur_offer->ToString(&sdp_with_candidates_str);
  auto offer = std::make_unique<JsepSessionDescription>(SdpType::kOffer);
  ASSERT_TRUE(SdpDeserialize(sdp_with_candidates_str, offer.get(),
                             nullptr /* error */));
  ASSERT_TRUE(callee->SetRemoteDescription(std::move(offer)));

  // By default, the Answer created does not contain ICE candidates.
  auto answer = callee->CreateAnswer();
  callee->SetLocalDescription(CloneSessionDescription(answer.get()));
  caller->SetRemoteDescription(std::move(answer));
  EXPECT_TRUE_WAIT(caller->IsConnected(), kDefaultTimeout);
  EXPECT_TRUE_WAIT(callee->IsConnected(), kDefaultTimeout);
  // The callee needs to process the open message to have the data channel open.
  EXPECT_TRUE_WAIT(callee->observer()->last_datachannel_ != nullptr,
                   kDefaultTimeout);
  caller->pc()->Close();
  callee->pc()->Close();

  // The caller should not have added any remote candidate either via
  // AddIceCandidate or from the remote description. Also, the caller connects
  // with the callee via a prflx candidate and hence no direct connection bit
  // should be set.
  int expected_fingerprint_caller = MakeUsageFingerprint(
      {UsageEvent::DATA_ADDED, UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::PRIVATE_CANDIDATE_COLLECTED,
       UsageEvent::IPV6_CANDIDATE_COLLECTED, UsageEvent::ICE_STATE_CONNECTED,
       UsageEvent::CLOSE_CALLED});

  int expected_fingerprint_callee = MakeUsageFingerprint(
      {UsageEvent::DATA_ADDED, UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::SET_REMOTE_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::MDNS_CANDIDATE_COLLECTED,
       UsageEvent::REMOTE_CANDIDATE_ADDED,
       UsageEvent::REMOTE_PRIVATE_CANDIDATE_ADDED,
       UsageEvent::REMOTE_IPV6_CANDIDATE_ADDED, UsageEvent::ICE_STATE_CONNECTED,
       UsageEvent::DIRECT_CONNECTION_SELECTED, UsageEvent::CLOSE_CALLED});
  EXPECT_METRIC_EQ(2, webrtc::metrics::NumSamples(kUsagePatternMetric));
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumEvents(kUsagePatternMetric,
                                                 expected_fingerprint_caller));
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumEvents(kUsagePatternMetric,
                                                 expected_fingerprint_callee));
}

TEST_F(PeerConnectionUsageHistogramTest, NotableUsageNoted) {
  auto caller = CreatePeerConnection();
  caller->CreateDataChannel("foo");
  caller->GenerateOfferAndCollectCandidates();
  caller->pc()->Close();
  int expected_fingerprint = MakeUsageFingerprint(
      {UsageEvent::DATA_ADDED, UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::CLOSE_CALLED});
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumSamples(kUsagePatternMetric));
  EXPECT_METRIC_TRUE(
      expected_fingerprint == ObservedFingerprint() ||
      (expected_fingerprint |
       static_cast<int>(UsageEvent::PRIVATE_CANDIDATE_COLLECTED)) ==
          ObservedFingerprint());
  EXPECT_METRIC_EQ(absl::make_optional(ObservedFingerprint()),
                   caller->observer()->interesting_usage_detected());
}

TEST_F(PeerConnectionUsageHistogramTest, NotableUsageOnEventFiring) {
  auto caller = CreatePeerConnection();
  caller->CreateDataChannel("foo");
  caller->GenerateOfferAndCollectCandidates();
  int expected_fingerprint = MakeUsageFingerprint(
      {UsageEvent::DATA_ADDED, UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED});
  EXPECT_METRIC_EQ(0, webrtc::metrics::NumSamples(kUsagePatternMetric));
  caller->GetInternalPeerConnection()->RequestUsagePatternReportForTesting();
  EXPECT_METRIC_EQ_WAIT(1, webrtc::metrics::NumSamples(kUsagePatternMetric),
                        kDefaultTimeout);
  EXPECT_METRIC_TRUE(
      expected_fingerprint == ObservedFingerprint() ||
      (expected_fingerprint |
       static_cast<int>(UsageEvent::PRIVATE_CANDIDATE_COLLECTED)) ==
          ObservedFingerprint());
  EXPECT_METRIC_EQ(absl::make_optional(ObservedFingerprint()),
                   caller->observer()->interesting_usage_detected());
}

TEST_F(PeerConnectionUsageHistogramTest,
       NoNotableUsageOnEventFiringAfterClose) {
  auto caller = CreatePeerConnection();
  caller->CreateDataChannel("foo");
  caller->GenerateOfferAndCollectCandidates();
  int expected_fingerprint = MakeUsageFingerprint(
      {UsageEvent::DATA_ADDED, UsageEvent::SET_LOCAL_DESCRIPTION_SUCCEEDED,
       UsageEvent::CANDIDATE_COLLECTED, UsageEvent::CLOSE_CALLED});
  EXPECT_METRIC_EQ(0, webrtc::metrics::NumSamples(kUsagePatternMetric));
  caller->pc()->Close();
  EXPECT_METRIC_EQ(1, webrtc::metrics::NumSamples(kUsagePatternMetric));
  caller->GetInternalPeerConnection()->RequestUsagePatternReportForTesting();
  caller->observer()->ClearInterestingUsageDetector();
  EXPECT_METRIC_EQ_WAIT(2, webrtc::metrics::NumSamples(kUsagePatternMetric),
                        kDefaultTimeout);
  EXPECT_METRIC_TRUE(
      expected_fingerprint == ObservedFingerprint() ||
      (expected_fingerprint |
       static_cast<int>(UsageEvent::PRIVATE_CANDIDATE_COLLECTED)) ==
          ObservedFingerprint());
  // After close, the usage-detection callback should NOT have been called.
  EXPECT_METRIC_FALSE(caller->observer()->interesting_usage_detected());
}
#endif
#endif

}  // namespace webrtc
