/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__congestion_controller__bbr__bbr_factory_h //original-code:"modules/congestion_controller/bbr/bbr_factory.h"
#include <memory>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_modules__congestion_controller__bbr__bbr_network_controller_h //original-code:"modules/congestion_controller/bbr/bbr_network_controller.h"

namespace webrtc {

BbrNetworkControllerFactory::BbrNetworkControllerFactory() {}

std::unique_ptr<NetworkControllerInterface> BbrNetworkControllerFactory::Create(
    NetworkControllerConfig config) {
  return absl::make_unique<bbr::BbrNetworkController>(config);
}

TimeDelta BbrNetworkControllerFactory::GetProcessInterval() const {
  return TimeDelta::PlusInfinity();
}

}  // namespace webrtc
