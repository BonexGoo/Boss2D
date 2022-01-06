/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <cstdint>
#include <memory>

#include BOSS_WEBRTC_U_api__call__call_factory_interface_h //original-code:"api/call/call_factory_interface.h"
#include BOSS_WEBRTC_U_api__peer_connection_interface_h //original-code:"api/peer_connection_interface.h"
#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_factory_h //original-code:"api/rtc_event_log/rtc_event_log_factory.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__task_queue__default_task_queue_factory_h //original-code:"api/task_queue/default_task_queue_factory.h"
#include BOSS_WEBRTC_U_api__transport__field_trial_based_config_h //original-code:"api/transport/field_trial_based_config.h"
#include BOSS_WEBRTC_U_call__simulated_network_h //original-code:"call/simulated_network.h"
#include BOSS_WEBRTC_U_media__engine__webrtc_media_engine_h //original-code:"media/engine/webrtc_media_engine.h"
#include BOSS_WEBRTC_U_media__engine__webrtc_media_engine_defaults_h //original-code:"media/engine/webrtc_media_engine_defaults.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__test_audio_device_h //original-code:"modules/audio_device/include/test_audio_device.h"
#include BOSS_WEBRTC_U_p2p__client__basic_port_allocator_h //original-code:"p2p/client/basic_port_allocator.h"
#include BOSS_WEBRTC_U_pc__peer_connection_wrapper_h //original-code:"pc/peer_connection_wrapper.h"
#include "pc/test/mock_peer_connection_observers.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include "test/network/network_emulation.h"
#include "test/network/network_emulation_manager.h"

namespace webrtc {
namespace test {
namespace {

constexpr int kDefaultTimeoutMs = 1000;
constexpr int kMaxAptitude = 32000;
constexpr int kSamplingFrequency = 48000;
constexpr char kSignalThreadName[] = "signaling_thread";

bool AddIceCandidates(PeerConnectionWrapper* peer,
                      std::vector<const IceCandidateInterface*> candidates) {
  bool success = true;
  for (const auto candidate : candidates) {
    if (!peer->pc()->AddIceCandidate(candidate)) {
      success = false;
    }
  }
  return success;
}

rtc::scoped_refptr<PeerConnectionFactoryInterface> CreatePeerConnectionFactory(
    rtc::Thread* signaling_thread,
    rtc::Thread* network_thread) {
  PeerConnectionFactoryDependencies pcf_deps;
  pcf_deps.task_queue_factory = CreateDefaultTaskQueueFactory();
  pcf_deps.call_factory = CreateCallFactory();
  pcf_deps.event_log_factory =
      std::make_unique<RtcEventLogFactory>(pcf_deps.task_queue_factory.get());
  pcf_deps.network_thread = network_thread;
  pcf_deps.signaling_thread = signaling_thread;
  pcf_deps.trials = std::make_unique<FieldTrialBasedConfig>();
  cricket::MediaEngineDependencies media_deps;
  media_deps.task_queue_factory = pcf_deps.task_queue_factory.get();
  media_deps.adm = TestAudioDeviceModule::Create(
      media_deps.task_queue_factory,
      TestAudioDeviceModule::CreatePulsedNoiseCapturer(kMaxAptitude,
                                                       kSamplingFrequency),
      TestAudioDeviceModule::CreateDiscardRenderer(kSamplingFrequency),
      /*speed=*/1.f);
  media_deps.trials = pcf_deps.trials.get();
  SetMediaEngineDefaults(&media_deps);
  pcf_deps.media_engine = cricket::CreateMediaEngine(std::move(media_deps));
  return CreateModularPeerConnectionFactory(std::move(pcf_deps));
}

rtc::scoped_refptr<PeerConnectionInterface> CreatePeerConnection(
    const rtc::scoped_refptr<PeerConnectionFactoryInterface>& pcf,
    PeerConnectionObserver* observer,
    rtc::NetworkManager* network_manager,
    EmulatedTURNServerInterface* turn_server = nullptr) {
  PeerConnectionDependencies pc_deps(observer);
  auto port_allocator =
      std::make_unique<cricket::BasicPortAllocator>(network_manager);

  // This test does not support TCP
  int flags = cricket::PORTALLOCATOR_DISABLE_TCP;
  port_allocator->set_flags(port_allocator->flags() | flags);

  pc_deps.allocator = std::move(port_allocator);
  PeerConnectionInterface::RTCConfiguration rtc_configuration;
  rtc_configuration.sdp_semantics = SdpSemantics::kUnifiedPlan;
  if (turn_server != nullptr) {
    webrtc::PeerConnectionInterface::IceServer server;
    server.username = turn_server->GetIceServerConfig().username;
    server.password = turn_server->GetIceServerConfig().username;
    server.urls.push_back(turn_server->GetIceServerConfig().url);
    rtc_configuration.servers.push_back(server);
  }

  auto result =
      pcf->CreatePeerConnectionOrError(rtc_configuration, std::move(pc_deps));
  if (!result.ok()) {
    return nullptr;
  }
  return result.MoveValue();
}

}  // namespace

TEST(NetworkEmulationManagerPCTest, Run) {
  std::unique_ptr<rtc::Thread> signaling_thread = rtc::Thread::Create();
  signaling_thread->SetName(kSignalThreadName, nullptr);
  signaling_thread->Start();

  // Setup emulated network
  NetworkEmulationManagerImpl emulation(TimeMode::kRealTime);

  EmulatedNetworkNode* alice_node = emulation.CreateEmulatedNode(
      std::make_unique<SimulatedNetwork>(BuiltInNetworkBehaviorConfig()));
  EmulatedNetworkNode* bob_node = emulation.CreateEmulatedNode(
      std::make_unique<SimulatedNetwork>(BuiltInNetworkBehaviorConfig()));
  EmulatedEndpoint* alice_endpoint =
      emulation.CreateEndpoint(EmulatedEndpointConfig());
  EmulatedEndpoint* bob_endpoint =
      emulation.CreateEndpoint(EmulatedEndpointConfig());
  emulation.CreateRoute(alice_endpoint, {alice_node}, bob_endpoint);
  emulation.CreateRoute(bob_endpoint, {bob_node}, alice_endpoint);

  EmulatedNetworkManagerInterface* alice_network =
      emulation.CreateEmulatedNetworkManagerInterface({alice_endpoint});
  EmulatedNetworkManagerInterface* bob_network =
      emulation.CreateEmulatedNetworkManagerInterface({bob_endpoint});

  // Setup peer connections.
  rtc::scoped_refptr<PeerConnectionFactoryInterface> alice_pcf;
  rtc::scoped_refptr<PeerConnectionInterface> alice_pc;
  std::unique_ptr<MockPeerConnectionObserver> alice_observer =
      std::make_unique<MockPeerConnectionObserver>();

  rtc::scoped_refptr<PeerConnectionFactoryInterface> bob_pcf;
  rtc::scoped_refptr<PeerConnectionInterface> bob_pc;
  std::unique_ptr<MockPeerConnectionObserver> bob_observer =
      std::make_unique<MockPeerConnectionObserver>();

  signaling_thread->Invoke<void>(RTC_FROM_HERE, [&]() {
    alice_pcf = CreatePeerConnectionFactory(signaling_thread.get(),
                                            alice_network->network_thread());
    alice_pc = CreatePeerConnection(alice_pcf, alice_observer.get(),
                                    alice_network->network_manager());

    bob_pcf = CreatePeerConnectionFactory(signaling_thread.get(),
                                          bob_network->network_thread());
    bob_pc = CreatePeerConnection(bob_pcf, bob_observer.get(),
                                  bob_network->network_manager());
  });

  std::unique_ptr<PeerConnectionWrapper> alice =
      std::make_unique<PeerConnectionWrapper>(alice_pcf, alice_pc,
                                              std::move(alice_observer));
  std::unique_ptr<PeerConnectionWrapper> bob =
      std::make_unique<PeerConnectionWrapper>(bob_pcf, bob_pc,
                                              std::move(bob_observer));

  signaling_thread->Invoke<void>(RTC_FROM_HERE, [&]() {
    rtc::scoped_refptr<webrtc::AudioSourceInterface> source =
        alice_pcf->CreateAudioSource(cricket::AudioOptions());
    rtc::scoped_refptr<AudioTrackInterface> track =
        alice_pcf->CreateAudioTrack("audio", source);
    alice->AddTransceiver(track);

    // Connect peers.
    ASSERT_TRUE(alice->ExchangeOfferAnswerWith(bob.get()));
    // Do the SDP negotiation, and also exchange ice candidates.
    ASSERT_TRUE_WAIT(
        alice->signaling_state() == PeerConnectionInterface::kStable,
        kDefaultTimeoutMs);
    ASSERT_TRUE_WAIT(alice->IsIceGatheringDone(), kDefaultTimeoutMs);
    ASSERT_TRUE_WAIT(bob->IsIceGatheringDone(), kDefaultTimeoutMs);

    // Connect an ICE candidate pairs.
    ASSERT_TRUE(
        AddIceCandidates(bob.get(), alice->observer()->GetAllCandidates()));
    ASSERT_TRUE(
        AddIceCandidates(alice.get(), bob->observer()->GetAllCandidates()));
    // This means that ICE and DTLS are connected.
    ASSERT_TRUE_WAIT(bob->IsIceConnected(), kDefaultTimeoutMs);
    ASSERT_TRUE_WAIT(alice->IsIceConnected(), kDefaultTimeoutMs);

    // Close peer connections
    alice->pc()->Close();
    bob->pc()->Close();

    // Delete peers.
    alice.reset();
    bob.reset();
  });
}

TEST(NetworkEmulationManagerPCTest, RunTURN) {
  std::unique_ptr<rtc::Thread> signaling_thread = rtc::Thread::Create();
  signaling_thread->SetName(kSignalThreadName, nullptr);
  signaling_thread->Start();

  // Setup emulated network
  NetworkEmulationManagerImpl emulation(TimeMode::kRealTime);

  EmulatedNetworkNode* alice_node = emulation.CreateEmulatedNode(
      std::make_unique<SimulatedNetwork>(BuiltInNetworkBehaviorConfig()));
  EmulatedNetworkNode* bob_node = emulation.CreateEmulatedNode(
      std::make_unique<SimulatedNetwork>(BuiltInNetworkBehaviorConfig()));
  EmulatedNetworkNode* turn_node = emulation.CreateEmulatedNode(
      std::make_unique<SimulatedNetwork>(BuiltInNetworkBehaviorConfig()));
  EmulatedEndpoint* alice_endpoint =
      emulation.CreateEndpoint(EmulatedEndpointConfig());
  EmulatedEndpoint* bob_endpoint =
      emulation.CreateEndpoint(EmulatedEndpointConfig());
  EmulatedTURNServerInterface* alice_turn =
      emulation.CreateTURNServer(EmulatedTURNServerConfig());
  EmulatedTURNServerInterface* bob_turn =
      emulation.CreateTURNServer(EmulatedTURNServerConfig());

  emulation.CreateRoute(alice_endpoint, {alice_node},
                        alice_turn->GetClientEndpoint());
  emulation.CreateRoute(alice_turn->GetClientEndpoint(), {alice_node},
                        alice_endpoint);

  emulation.CreateRoute(bob_endpoint, {bob_node},
                        bob_turn->GetClientEndpoint());
  emulation.CreateRoute(bob_turn->GetClientEndpoint(), {bob_node},
                        bob_endpoint);

  emulation.CreateRoute(alice_turn->GetPeerEndpoint(), {turn_node},
                        bob_turn->GetPeerEndpoint());
  emulation.CreateRoute(bob_turn->GetPeerEndpoint(), {turn_node},
                        alice_turn->GetPeerEndpoint());

  EmulatedNetworkManagerInterface* alice_network =
      emulation.CreateEmulatedNetworkManagerInterface({alice_endpoint});
  EmulatedNetworkManagerInterface* bob_network =
      emulation.CreateEmulatedNetworkManagerInterface({bob_endpoint});

  // Setup peer connections.
  rtc::scoped_refptr<PeerConnectionFactoryInterface> alice_pcf;
  rtc::scoped_refptr<PeerConnectionInterface> alice_pc;
  std::unique_ptr<MockPeerConnectionObserver> alice_observer =
      std::make_unique<MockPeerConnectionObserver>();

  rtc::scoped_refptr<PeerConnectionFactoryInterface> bob_pcf;
  rtc::scoped_refptr<PeerConnectionInterface> bob_pc;
  std::unique_ptr<MockPeerConnectionObserver> bob_observer =
      std::make_unique<MockPeerConnectionObserver>();

  signaling_thread->Invoke<void>(RTC_FROM_HERE, [&]() {
    alice_pcf = CreatePeerConnectionFactory(signaling_thread.get(),
                                            alice_network->network_thread());
    alice_pc =
        CreatePeerConnection(alice_pcf, alice_observer.get(),
                             alice_network->network_manager(), alice_turn);

    bob_pcf = CreatePeerConnectionFactory(signaling_thread.get(),
                                          bob_network->network_thread());
    bob_pc = CreatePeerConnection(bob_pcf, bob_observer.get(),
                                  bob_network->network_manager(), bob_turn);
  });

  std::unique_ptr<PeerConnectionWrapper> alice =
      std::make_unique<PeerConnectionWrapper>(alice_pcf, alice_pc,
                                              std::move(alice_observer));
  std::unique_ptr<PeerConnectionWrapper> bob =
      std::make_unique<PeerConnectionWrapper>(bob_pcf, bob_pc,
                                              std::move(bob_observer));

  signaling_thread->Invoke<void>(RTC_FROM_HERE, [&]() {
    rtc::scoped_refptr<webrtc::AudioSourceInterface> source =
        alice_pcf->CreateAudioSource(cricket::AudioOptions());
    rtc::scoped_refptr<AudioTrackInterface> track =
        alice_pcf->CreateAudioTrack("audio", source);
    alice->AddTransceiver(track);

    // Connect peers.
    ASSERT_TRUE(alice->ExchangeOfferAnswerWith(bob.get()));
    // Do the SDP negotiation, and also exchange ice candidates.
    ASSERT_TRUE_WAIT(
        alice->signaling_state() == PeerConnectionInterface::kStable,
        kDefaultTimeoutMs);
    ASSERT_TRUE_WAIT(alice->IsIceGatheringDone(), kDefaultTimeoutMs);
    ASSERT_TRUE_WAIT(bob->IsIceGatheringDone(), kDefaultTimeoutMs);

    // Connect an ICE candidate pairs.
    ASSERT_TRUE(
        AddIceCandidates(bob.get(), alice->observer()->GetAllCandidates()));
    ASSERT_TRUE(
        AddIceCandidates(alice.get(), bob->observer()->GetAllCandidates()));
    // This means that ICE and DTLS are connected.
    ASSERT_TRUE_WAIT(bob->IsIceConnected(), kDefaultTimeoutMs);
    ASSERT_TRUE_WAIT(alice->IsIceConnected(), kDefaultTimeoutMs);

    // Close peer connections
    alice->pc()->Close();
    bob->pc()->Close();

    // Delete peers.
    alice.reset();
    bob.reset();
  });
}

}  // namespace test
}  // namespace webrtc
