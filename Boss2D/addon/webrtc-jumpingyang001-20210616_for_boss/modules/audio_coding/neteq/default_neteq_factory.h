/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_NETEQ_DEFAULT_NETEQ_FACTORY_H_
#define MODULES_AUDIO_CODING_NETEQ_DEFAULT_NETEQ_FACTORY_H_

#include <memory>

#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_factory_h //original-code:"api/audio_codecs/audio_decoder_factory.h"
#include BOSS_WEBRTC_U_api__neteq__default_neteq_controller_factory_h //original-code:"api/neteq/default_neteq_controller_factory.h"
#include BOSS_WEBRTC_U_api__neteq__neteq_factory_h //original-code:"api/neteq/neteq_factory.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {

class DefaultNetEqFactory : public NetEqFactory {
 public:
  DefaultNetEqFactory();
  ~DefaultNetEqFactory() override;
  DefaultNetEqFactory(const DefaultNetEqFactory&) = delete;
  DefaultNetEqFactory& operator=(const DefaultNetEqFactory&) = delete;

  std::unique_ptr<NetEq> CreateNetEq(
      const NetEq::Config& config,
      const rtc::scoped_refptr<AudioDecoderFactory>& decoder_factory,
      Clock* clock) const override;

 private:
  const DefaultNetEqControllerFactory controller_factory_;
};

}  // namespace webrtc
#endif  // MODULES_AUDIO_CODING_NETEQ_DEFAULT_NETEQ_FACTORY_H_
