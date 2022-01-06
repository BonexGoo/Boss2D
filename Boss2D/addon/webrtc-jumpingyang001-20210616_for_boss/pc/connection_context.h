/*
 *  Copyright 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_CONNECTION_CONTEXT_H_
#define PC_CONNECTION_CONTEXT_H_

#include <memory>
#include <string>

#include BOSS_WEBRTC_U_api__call__call_factory_interface_h //original-code:"api/call/call_factory_interface.h"
#include BOSS_WEBRTC_U_api__media_stream_interface_h //original-code:"api/media_stream_interface.h"
#include BOSS_WEBRTC_U_api__peer_connection_interface_h //original-code:"api/peer_connection_interface.h"
#include BOSS_WEBRTC_U_api__ref_counted_base_h //original-code:"api/ref_counted_base.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__transport__sctp_transport_factory_interface_h //original-code:"api/transport/sctp_transport_factory_interface.h"
#include BOSS_WEBRTC_U_api__transport__webrtc_key_value_config_h //original-code:"api/transport/webrtc_key_value_config.h"
#include BOSS_WEBRTC_U_media__base__media_engine_h //original-code:"media/base/media_engine.h"
#include BOSS_WEBRTC_U_p2p__base__basic_packet_socket_factory_h //original-code:"p2p/base/basic_packet_socket_factory.h"
#include BOSS_WEBRTC_U_pc__channel_manager_h //original-code:"pc/channel_manager.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__network_h //original-code:"rtc_base/network.h"
#include BOSS_WEBRTC_U_rtc_base__network_monitor_factory_h //original-code:"rtc_base/network_monitor_factory.h"
#include BOSS_WEBRTC_U_rtc_base__rtc_certificate_generator_h //original-code:"rtc_base/rtc_certificate_generator.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace rtc {
class BasicNetworkManager;
class BasicPacketSocketFactory;
}  // namespace rtc

namespace webrtc {

class RtcEventLog;

// This class contains resources needed by PeerConnection and associated
// objects. A reference to this object is passed to each PeerConnection. The
// methods on this object are assumed not to change the state in any way that
// interferes with the operation of other PeerConnections.
//
// This class must be created and destroyed on the signaling thread.
class ConnectionContext final
    : public rtc::RefCountedNonVirtual<ConnectionContext> {
 public:
  // Creates a ConnectionContext. May return null if initialization fails.
  // The Dependencies class allows simple management of all new dependencies
  // being added to the ConnectionContext.
  static rtc::scoped_refptr<ConnectionContext> Create(
      PeerConnectionFactoryDependencies* dependencies);

  // This class is not copyable or movable.
  ConnectionContext(const ConnectionContext&) = delete;
  ConnectionContext& operator=(const ConnectionContext&) = delete;

  // Functions called from PeerConnection and friends
  SctpTransportFactoryInterface* sctp_transport_factory() const {
    return sctp_factory_.get();
  }

  cricket::ChannelManager* channel_manager() const;

  rtc::Thread* signaling_thread() { return signaling_thread_; }
  const rtc::Thread* signaling_thread() const { return signaling_thread_; }
  rtc::Thread* worker_thread() { return worker_thread_; }
  const rtc::Thread* worker_thread() const { return worker_thread_; }
  rtc::Thread* network_thread() { return network_thread_; }
  const rtc::Thread* network_thread() const { return network_thread_; }

  const WebRtcKeyValueConfig& trials() const { return *trials_.get(); }

  // Accessors only used from the PeerConnectionFactory class
  rtc::BasicNetworkManager* default_network_manager() {
    RTC_DCHECK_RUN_ON(signaling_thread_);
    return default_network_manager_.get();
  }
  rtc::BasicPacketSocketFactory* default_socket_factory() {
    RTC_DCHECK_RUN_ON(signaling_thread_);
    return default_socket_factory_.get();
  }
  CallFactoryInterface* call_factory() {
    RTC_DCHECK_RUN_ON(worker_thread_);
    return call_factory_.get();
  }

 protected:
  explicit ConnectionContext(PeerConnectionFactoryDependencies* dependencies);

  friend class rtc::RefCountedNonVirtual<ConnectionContext>;
  ~ConnectionContext();

 private:
  // The following three variables are used to communicate between the
  // constructor and the destructor, and are never exposed externally.
  bool wraps_current_thread_;
  // Note: Since owned_network_thread_ and owned_worker_thread_ are used
  // in the initialization of network_thread_ and worker_thread_, they
  // must be declared before them, so that they are initialized first.
  std::unique_ptr<rtc::Thread> owned_network_thread_
      RTC_GUARDED_BY(signaling_thread_);
  std::unique_ptr<rtc::Thread> owned_worker_thread_
      RTC_GUARDED_BY(signaling_thread_);
  rtc::Thread* const network_thread_;
  rtc::Thread* const worker_thread_;
  rtc::Thread* const signaling_thread_;
  // channel_manager is accessed both on signaling thread and worker thread.
  std::unique_ptr<cricket::ChannelManager> channel_manager_;
  std::unique_ptr<rtc::NetworkMonitorFactory> const network_monitor_factory_
      RTC_GUARDED_BY(signaling_thread_);
  std::unique_ptr<rtc::BasicNetworkManager> default_network_manager_
      RTC_GUARDED_BY(signaling_thread_);
  std::unique_ptr<webrtc::CallFactoryInterface> const call_factory_
      RTC_GUARDED_BY(worker_thread_);

  std::unique_ptr<rtc::BasicPacketSocketFactory> default_socket_factory_
      RTC_GUARDED_BY(signaling_thread_);
  std::unique_ptr<SctpTransportFactoryInterface> const sctp_factory_;
  // Accessed both on signaling thread and worker thread.
  std::unique_ptr<WebRtcKeyValueConfig> const trials_;
};

}  // namespace webrtc

#endif  // PC_CONNECTION_CONTEXT_H_
