/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef P2P_CLIENT_BASIC_PORT_ALLOCATOR_H_
#define P2P_CLIENT_BASIC_PORT_ALLOCATOR_H_

#include <memory>
#include <string>
#include <vector>

#include BOSS_WEBRTC_U_api__turn_customizer_h //original-code:"api/turn_customizer.h"
#include BOSS_WEBRTC_U_p2p__base__port_allocator_h //original-code:"p2p/base/port_allocator.h"
#include BOSS_WEBRTC_U_p2p__client__relay_port_factory_interface_h //original-code:"p2p/client/relay_port_factory_interface.h"
#include BOSS_WEBRTC_U_p2p__client__turn_port_factory_h //original-code:"p2p/client/turn_port_factory.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__network_h //original-code:"rtc_base/network.h"
#include BOSS_WEBRTC_U_rtc_base__system__rtc_export_h //original-code:"rtc_base/system/rtc_export.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__pending_task_safety_flag_h //original-code:"rtc_base/task_utils/pending_task_safety_flag.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace cricket {

class RTC_EXPORT BasicPortAllocator : public PortAllocator {
 public:
  // note: The (optional) relay_port_factory is owned by caller
  // and must have a life time that exceeds that of BasicPortAllocator.
  BasicPortAllocator(rtc::NetworkManager* network_manager,
                     rtc::PacketSocketFactory* socket_factory,
                     webrtc::TurnCustomizer* customizer = nullptr,
                     RelayPortFactoryInterface* relay_port_factory = nullptr);
  explicit BasicPortAllocator(rtc::NetworkManager* network_manager);
  BasicPortAllocator(rtc::NetworkManager* network_manager,
                     const ServerAddresses& stun_servers);
  BasicPortAllocator(rtc::NetworkManager* network_manager,
                     rtc::PacketSocketFactory* socket_factory,
                     const ServerAddresses& stun_servers);
  ~BasicPortAllocator() override;

  // Set to kDefaultNetworkIgnoreMask by default.
  void SetNetworkIgnoreMask(int network_ignore_mask) override;
  int network_ignore_mask() const {
    CheckRunOnValidThreadIfInitialized();
    return network_ignore_mask_;
  }

  rtc::NetworkManager* network_manager() const {
    CheckRunOnValidThreadIfInitialized();
    return network_manager_;
  }

  // If socket_factory() is set to NULL each PortAllocatorSession
  // creates its own socket factory.
  rtc::PacketSocketFactory* socket_factory() {
    CheckRunOnValidThreadIfInitialized();
    return socket_factory_;
  }

  PortAllocatorSession* CreateSessionInternal(
      const std::string& content_name,
      int component,
      const std::string& ice_ufrag,
      const std::string& ice_pwd) override;

  // Convenience method that adds a TURN server to the configuration.
  void AddTurnServer(const RelayServerConfig& turn_server);

  RelayPortFactoryInterface* relay_port_factory() {
    CheckRunOnValidThreadIfInitialized();
    return relay_port_factory_;
  }

 private:
  void OnIceRegathering(PortAllocatorSession* session,
                        IceRegatheringReason reason);

  // This function makes sure that relay_port_factory_ is set properly.
  void InitRelayPortFactory(RelayPortFactoryInterface* relay_port_factory);

  bool MdnsObfuscationEnabled() const override;

  rtc::NetworkManager* network_manager_;
  rtc::PacketSocketFactory* socket_factory_;
  int network_ignore_mask_ = rtc::kDefaultNetworkIgnoreMask;

  // This is the factory being used.
  RelayPortFactoryInterface* relay_port_factory_;

  // This instance is created if caller does pass a factory.
  std::unique_ptr<RelayPortFactoryInterface> default_relay_port_factory_;
};

struct PortConfiguration;
class AllocationSequence;

enum class SessionState {
  GATHERING,  // Actively allocating ports and gathering candidates.
  CLEARED,    // Current allocation process has been stopped but may start
              // new ones.
  STOPPED     // This session has completely stopped, no new allocation
              // process will be started.
};

// This class is thread-compatible and assumes it's created, operated upon and
// destroyed on the network thread.
class RTC_EXPORT BasicPortAllocatorSession : public PortAllocatorSession {
 public:
  BasicPortAllocatorSession(BasicPortAllocator* allocator,
                            const std::string& content_name,
                            int component,
                            const std::string& ice_ufrag,
                            const std::string& ice_pwd);
  ~BasicPortAllocatorSession() override;

  virtual BasicPortAllocator* allocator();
  rtc::Thread* network_thread() { return network_thread_; }
  rtc::PacketSocketFactory* socket_factory() { return socket_factory_; }

  // If the new filter allows new types of candidates compared to the previous
  // filter, gathered candidates that were discarded because of not matching the
  // previous filter will be signaled if they match the new one.
  //
  // We do not perform any regathering since the port allocator flags decide
  // the type of candidates to gather and the candidate filter only controls the
  // signaling of candidates. As a result, with the candidate filter changed
  // alone, all newly allowed candidates for signaling should already be
  // gathered by the respective cricket::Port.
  void SetCandidateFilter(uint32_t filter) override;
  void StartGettingPorts() override;
  void StopGettingPorts() override;
  void ClearGettingPorts() override;
  bool IsGettingPorts() override;
  bool IsCleared() const override;
  bool IsStopped() const override;
  // These will all be cricket::Ports.
  std::vector<PortInterface*> ReadyPorts() const override;
  std::vector<Candidate> ReadyCandidates() const override;
  bool CandidatesAllocationDone() const override;
  void RegatherOnFailedNetworks() override;
  void GetCandidateStatsFromReadyPorts(
      CandidateStatsList* candidate_stats_list) const override;
  void SetStunKeepaliveIntervalForReadyPorts(
      const absl::optional<int>& stun_keepalive_interval) override;
  void PruneAllPorts() override;

 protected:
  void UpdateIceParametersInternal() override;

  // Starts the process of getting the port configurations.
  virtual void GetPortConfigurations();

  // Adds a port configuration that is now ready.  Once we have one for each
  // network (or a timeout occurs), we will start allocating ports.
  void ConfigReady(std::unique_ptr<PortConfiguration> config);
  // TODO(bugs.webrtc.org/12840) Remove once unused in downstream projects.
  ABSL_DEPRECATED(
      "Use ConfigReady(std::unique_ptr<PortConfiguration>) instead!")
  void ConfigReady(PortConfiguration* config);

 private:
  class PortData {
   public:
    enum State {
      STATE_INPROGRESS,  // Still gathering candidates.
      STATE_COMPLETE,    // All candidates allocated and ready for process.
      STATE_ERROR,       // Error in gathering candidates.
      STATE_PRUNED       // Pruned by higher priority ports on the same network
                         // interface. Only TURN ports may be pruned.
    };

    PortData() {}
    PortData(Port* port, AllocationSequence* seq)
        : port_(port), sequence_(seq) {}

    Port* port() const { return port_; }
    AllocationSequence* sequence() const { return sequence_; }
    bool has_pairable_candidate() const { return has_pairable_candidate_; }
    State state() const { return state_; }
    bool complete() const { return state_ == STATE_COMPLETE; }
    bool error() const { return state_ == STATE_ERROR; }
    bool pruned() const { return state_ == STATE_PRUNED; }
    bool inprogress() const { return state_ == STATE_INPROGRESS; }
    // Returns true if this port is ready to be used.
    bool ready() const {
      return has_pairable_candidate_ && state_ != STATE_ERROR &&
             state_ != STATE_PRUNED;
    }
    // Sets the state to "PRUNED" and prunes the Port.
    void Prune() {
      state_ = STATE_PRUNED;
      if (port()) {
        port()->Prune();
      }
    }
    void set_has_pairable_candidate(bool has_pairable_candidate) {
      if (has_pairable_candidate) {
        RTC_DCHECK(state_ == STATE_INPROGRESS);
      }
      has_pairable_candidate_ = has_pairable_candidate;
    }
    void set_state(State state) {
      RTC_DCHECK(state != STATE_ERROR || state_ == STATE_INPROGRESS);
      state_ = state;
    }

   private:
    Port* port_ = nullptr;
    AllocationSequence* sequence_ = nullptr;
    bool has_pairable_candidate_ = false;
    State state_ = STATE_INPROGRESS;
  };

  void OnConfigReady(std::unique_ptr<PortConfiguration> config);
  void OnConfigStop();
  void AllocatePorts();
  void OnAllocate(int allocation_epoch);
  void DoAllocate(bool disable_equivalent_phases);
  void OnNetworksChanged();
  void OnAllocationSequenceObjectsCreated();
  void DisableEquivalentPhases(rtc::Network* network,
                               PortConfiguration* config,
                               uint32_t* flags);
  void AddAllocatedPort(Port* port,
                        AllocationSequence* seq,
                        bool prepare_address);
  void OnCandidateReady(Port* port, const Candidate& c);
  void OnCandidateError(Port* port, const IceCandidateErrorEvent& event);
  void OnPortComplete(Port* port);
  void OnPortError(Port* port);
  void OnProtocolEnabled(AllocationSequence* seq, ProtocolType proto);
  void OnPortDestroyed(PortInterface* port);
  void MaybeSignalCandidatesAllocationDone();
  void OnPortAllocationComplete();
  PortData* FindPort(Port* port);
  std::vector<rtc::Network*> GetNetworks();
  std::vector<rtc::Network*> GetFailedNetworks();
  void Regather(const std::vector<rtc::Network*>& networks,
                bool disable_equivalent_phases,
                IceRegatheringReason reason);

  bool CheckCandidateFilter(const Candidate& c) const;
  bool CandidatePairable(const Candidate& c, const Port* port) const;

  std::vector<PortData*> GetUnprunedPorts(
      const std::vector<rtc::Network*>& networks);
  // Prunes ports and signal the remote side to remove the candidates that
  // were previously signaled from these ports.
  void PrunePortsAndRemoveCandidates(
      const std::vector<PortData*>& port_data_list);
  // Gets filtered and sanitized candidates generated from a port and
  // append to |candidates|.
  void GetCandidatesFromPort(const PortData& data,
                             std::vector<Candidate>* candidates) const;
  Port* GetBestTurnPortForNetwork(const std::string& network_name) const;
  // Returns true if at least one TURN port is pruned.
  bool PruneTurnPorts(Port* newly_pairable_turn_port);
  bool PruneNewlyPairableTurnPort(PortData* newly_pairable_turn_port);

  BasicPortAllocator* allocator_;
  rtc::Thread* network_thread_;
  std::unique_ptr<rtc::PacketSocketFactory> owned_socket_factory_;
  rtc::PacketSocketFactory* socket_factory_;
  bool allocation_started_;
  bool network_manager_started_;
  bool allocation_sequences_created_;
  std::vector<std::unique_ptr<PortConfiguration>> configs_;
  std::vector<AllocationSequence*> sequences_;
  std::vector<PortData> ports_;
  std::vector<IceCandidateErrorEvent> candidate_error_events_;
  uint32_t candidate_filter_ = CF_ALL;
  // Policy on how to prune turn ports, taken from the port allocator.
  webrtc::PortPrunePolicy turn_port_prune_policy_;
  SessionState state_ = SessionState::CLEARED;
  int allocation_epoch_ RTC_GUARDED_BY(network_thread_) = 0;
  webrtc::ScopedTaskSafety network_safety_;

  friend class AllocationSequence;
};

// Records configuration information useful in creating ports.
// TODO(deadbeef): Rename "relay" to "turn_server" in this struct.
struct RTC_EXPORT PortConfiguration {
  // TODO(jiayl): remove |stun_address| when Chrome is updated.
  rtc::SocketAddress stun_address;
  ServerAddresses stun_servers;
  std::string username;
  std::string password;
  bool use_turn_server_as_stun_server_disabled = false;

  typedef std::vector<RelayServerConfig> RelayList;
  RelayList relays;

  // TODO(jiayl): remove this ctor when Chrome is updated.
  PortConfiguration(const rtc::SocketAddress& stun_address,
                    const std::string& username,
                    const std::string& password);

  PortConfiguration(const ServerAddresses& stun_servers,
                    const std::string& username,
                    const std::string& password);

  // Returns addresses of both the explicitly configured STUN servers,
  // and TURN servers that should be used as STUN servers.
  ServerAddresses StunServers();

  // Adds another relay server, with the given ports and modifier, to the list.
  void AddRelay(const RelayServerConfig& config);

  // Determines whether the given relay server supports the given protocol.
  bool SupportsProtocol(const RelayServerConfig& relay,
                        ProtocolType type) const;
  bool SupportsProtocol(ProtocolType type) const;
  // Helper method returns the server addresses for the matching RelayType and
  // Protocol type.
  ServerAddresses GetRelayServerAddresses(ProtocolType type) const;
};

class UDPPort;
class TurnPort;

// Performs the allocation of ports, in a sequenced (timed) manner, for a given
// network and IP address.
// This class is thread-compatible.
class AllocationSequence : public sigslot::has_slots<> {
 public:
  enum State {
    kInit,       // Initial state.
    kRunning,    // Started allocating ports.
    kStopped,    // Stopped from running.
    kCompleted,  // All ports are allocated.

    // kInit --> kRunning --> {kCompleted|kStopped}
  };
  // |port_allocation_complete_callback| is called when AllocationSequence is
  // done with allocating ports. This signal is useful when port allocation
  // fails which doesn't result in any candidates. Using this signal
  // BasicPortAllocatorSession can send its candidate discovery conclusion
  // signal. Without this signal, BasicPortAllocatorSession doesn't have any
  // event to trigger signal. This can also be achieved by starting a timer in
  // BPAS, but this is less deterministic.
  AllocationSequence(BasicPortAllocatorSession* session,
                     rtc::Network* network,
                     PortConfiguration* config,
                     uint32_t flags,
                     std::function<void()> port_allocation_complete_callback);
  void Init();
  void Clear();
  void OnNetworkFailed();

  State state() const { return state_; }
  rtc::Network* network() const { return network_; }

  bool network_failed() const { return network_failed_; }
  void set_network_failed() { network_failed_ = true; }

  // Disables the phases for a new sequence that this one already covers for an
  // equivalent network setup.
  void DisableEquivalentPhases(rtc::Network* network,
                               PortConfiguration* config,
                               uint32_t* flags);

  // Starts and stops the sequence.  When started, it will continue allocating
  // new ports on its own timed schedule.
  void Start();
  void Stop();

 protected:
  // For testing.
  void CreateTurnPort(const RelayServerConfig& config);

 private:
  typedef std::vector<ProtocolType> ProtocolList;

  void Process(int epoch);
  bool IsFlagSet(uint32_t flag) { return ((flags_ & flag) != 0); }
  void CreateUDPPorts();
  void CreateTCPPorts();
  void CreateStunPorts();
  void CreateRelayPorts();

  void OnReadPacket(rtc::AsyncPacketSocket* socket,
                    const char* data,
                    size_t size,
                    const rtc::SocketAddress& remote_addr,
                    const int64_t& packet_time_us);

  void OnPortDestroyed(PortInterface* port);

  BasicPortAllocatorSession* session_;
  bool network_failed_ = false;
  rtc::Network* network_;
  // Compared with the new best IP in DisableEquivalentPhases.
  rtc::IPAddress previous_best_ip_;
  PortConfiguration* config_;
  State state_;
  uint32_t flags_;
  ProtocolList protocols_;
  std::unique_ptr<rtc::AsyncPacketSocket> udp_socket_;
  // There will be only one udp port per AllocationSequence.
  UDPPort* udp_port_;
  std::vector<Port*> relay_ports_;
  int phase_;
  std::function<void()> port_allocation_complete_callback_;
  // This counter is sampled and passed together with tasks when tasks are
  // posted. If the sampled counter doesn't match |epoch_| on reception, the
  // posted task is ignored.
  int epoch_ = 0;
  webrtc::ScopedTaskSafety safety_;
};

}  // namespace cricket

#endif  // P2P_CLIENT_BASIC_PORT_ALLOCATOR_H_
