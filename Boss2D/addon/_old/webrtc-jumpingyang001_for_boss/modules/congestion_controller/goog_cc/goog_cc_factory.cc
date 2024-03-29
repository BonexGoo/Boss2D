/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__include__goog_cc_factory_h //original-code:"modules/congestion_controller/goog_cc/include/goog_cc_factory.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__goog_cc__goog_cc_network_control_h //original-code:"modules/congestion_controller/goog_cc/goog_cc_network_control.h"
namespace webrtc {
GoogCcNetworkControllerFactory::GoogCcNetworkControllerFactory(
    RtcEventLog* event_log)
    : event_log_(event_log) {}

std::unique_ptr<NetworkControllerInterface>
GoogCcNetworkControllerFactory::Create(NetworkControllerConfig config) {
  return absl::make_unique<GoogCcNetworkController>(event_log_, config, false);
}

TimeDelta GoogCcNetworkControllerFactory::GetProcessInterval() const {
  const int64_t kUpdateIntervalMs = 25;
  return TimeDelta::ms(kUpdateIntervalMs);
}

GoogCcFeedbackNetworkControllerFactory::GoogCcFeedbackNetworkControllerFactory(
    RtcEventLog* event_log)
    : event_log_(event_log) {}

std::unique_ptr<NetworkControllerInterface>
GoogCcFeedbackNetworkControllerFactory::Create(NetworkControllerConfig config) {
  return absl::make_unique<GoogCcNetworkController>(event_log_, config, true);
}

TimeDelta GoogCcFeedbackNetworkControllerFactory::GetProcessInterval() const {
  const int64_t kUpdateIntervalMs = 25;
  return TimeDelta::ms(kUpdateIntervalMs);
}
}  // namespace webrtc
