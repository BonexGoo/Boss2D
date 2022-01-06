/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_NETWORK_EMULATED_NETWORK_MANAGER_H_
#define TEST_NETWORK_EMULATED_NETWORK_MANAGER_H_

#include <functional>
#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__test__network_emulation_manager_h //original-code:"api/test/network_emulation_manager.h"
#include BOSS_WEBRTC_U_api__test__time_controller_h //original-code:"api/test/time_controller.h"
#include BOSS_WEBRTC_U_rtc_base__ip_address_h //original-code:"rtc_base/ip_address.h"
#include BOSS_WEBRTC_U_rtc_base__network_h //original-code:"rtc_base/network.h"
#include BOSS_WEBRTC_U_rtc_base__socket_server_h //original-code:"rtc_base/socket_server.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include "test/network/network_emulation.h"

namespace webrtc {
namespace test {

// Framework assumes that rtc::NetworkManager is called from network thread.
class EmulatedNetworkManager : public rtc::NetworkManagerBase,
                               public sigslot::has_slots<>,
                               public EmulatedNetworkManagerInterface {
 public:
  EmulatedNetworkManager(TimeController* time_controller,
                         TaskQueueForTest* task_queue,
                         EndpointsContainer* endpoints_container);

  void EnableEndpoint(EmulatedEndpointImpl* endpoint);
  void DisableEndpoint(EmulatedEndpointImpl* endpoint);

  // NetworkManager interface. All these methods are supposed to be called from
  // the same thread.
  void StartUpdating() override;
  void StopUpdating() override;

  // We don't support any address interfaces in the network emulation framework.
  void GetAnyAddressNetworks(NetworkList* networks) override {}

  // EmulatedNetworkManagerInterface API
  rtc::Thread* network_thread() override { return network_thread_.get(); }
  rtc::NetworkManager* network_manager() override { return this; }
  std::vector<EmulatedEndpoint*> endpoints() const override {
    return endpoints_container_->GetEndpoints();
  }
  void GetStats(std::function<void(std::unique_ptr<EmulatedNetworkStats>)>
                    stats_callback) const override;

 private:
  void UpdateNetworksOnce();
  void MaybeSignalNetworksChanged();

  TaskQueueForTest* const task_queue_;
  const EndpointsContainer* const endpoints_container_;
  std::unique_ptr<rtc::Thread> network_thread_;

  bool sent_first_update_ RTC_GUARDED_BY(network_thread_);
  int start_count_ RTC_GUARDED_BY(network_thread_);
};

}  // namespace test
}  // namespace webrtc

#endif  // TEST_NETWORK_EMULATED_NETWORK_MANAGER_H_
