/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_NETEQ_NETEQ_FACTORY_H_
#define API_NETEQ_NETEQ_FACTORY_H_

#include <memory>

#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_factory_h //original-code:"api/audio_codecs/audio_decoder_factory.h"
#include BOSS_WEBRTC_U_api__neteq__neteq_h //original-code:"api/neteq/neteq.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {

// Creates NetEq instances using the settings provided in the config struct.
class NetEqFactory {
 public:
  virtual ~NetEqFactory() = default;

  // Creates a new NetEq object, with parameters set in |config|. The |config|
  // object will only have to be valid for the duration of the call to this
  // method.
  virtual std::unique_ptr<NetEq> CreateNetEq(
      const NetEq::Config& config,
      const rtc::scoped_refptr<AudioDecoderFactory>& decoder_factory,
      Clock* clock) const = 0;
};

}  // namespace webrtc
#endif  // API_NETEQ_NETEQ_FACTORY_H_
