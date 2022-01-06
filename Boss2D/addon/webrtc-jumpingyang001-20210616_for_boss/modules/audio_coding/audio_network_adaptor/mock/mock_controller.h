/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_MOCK_MOCK_CONTROLLER_H_
#define MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_MOCK_MOCK_CONTROLLER_H_

#include BOSS_WEBRTC_U_modules__audio_coding__audio_network_adaptor__controller_h //original-code:"modules/audio_coding/audio_network_adaptor/controller.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

class MockController : public Controller {
 public:
  ~MockController() override { Die(); }
  MOCK_METHOD(void, Die, ());
  MOCK_METHOD(void,
              UpdateNetworkMetrics,
              (const NetworkMetrics& network_metrics),
              (override));
  MOCK_METHOD(void,
              MakeDecision,
              (AudioEncoderRuntimeConfig * config),
              (override));
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_MOCK_MOCK_CONTROLLER_H_
