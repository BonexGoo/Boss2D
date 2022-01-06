/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__neteq__default_neteq_controller_factory_h //original-code:"api/neteq/default_neteq_controller_factory.h"
#include BOSS_WEBRTC_U_modules__audio_coding__neteq__decision_logic_h //original-code:"modules/audio_coding/neteq/decision_logic.h"

namespace webrtc {

DefaultNetEqControllerFactory::DefaultNetEqControllerFactory() = default;
DefaultNetEqControllerFactory::~DefaultNetEqControllerFactory() = default;

std::unique_ptr<NetEqController>
DefaultNetEqControllerFactory::CreateNetEqController(
    const NetEqController::Config& config) const {
  return std::make_unique<DecisionLogic>(config);
}

}  // namespace webrtc
