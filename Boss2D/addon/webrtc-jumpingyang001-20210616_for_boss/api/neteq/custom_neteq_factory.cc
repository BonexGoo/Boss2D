/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__neteq__custom_neteq_factory_h //original-code:"api/neteq/custom_neteq_factory.h"

#include <utility>

#include BOSS_WEBRTC_U_modules__audio_coding__neteq__neteq_impl_h //original-code:"modules/audio_coding/neteq/neteq_impl.h"

namespace webrtc {

CustomNetEqFactory::CustomNetEqFactory(
    std::unique_ptr<NetEqControllerFactory> controller_factory)
    : controller_factory_(std::move(controller_factory)) {}

CustomNetEqFactory::~CustomNetEqFactory() = default;

std::unique_ptr<NetEq> CustomNetEqFactory::CreateNetEq(
    const NetEq::Config& config,
    const rtc::scoped_refptr<AudioDecoderFactory>& decoder_factory,
    Clock* clock) const {
  return std::make_unique<NetEqImpl>(
      config, NetEqImpl::Dependencies(config, clock, decoder_factory,
                                      *controller_factory_));
}

}  // namespace webrtc
