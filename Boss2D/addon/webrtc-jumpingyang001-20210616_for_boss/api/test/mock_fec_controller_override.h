/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_TEST_MOCK_FEC_CONTROLLER_OVERRIDE_H_
#define API_TEST_MOCK_FEC_CONTROLLER_OVERRIDE_H_

#include BOSS_WEBRTC_U_api__fec_controller_override_h //original-code:"api/fec_controller_override.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

class MockFecControllerOverride : public FecControllerOverride {
 public:
  MOCK_METHOD(void, SetFecAllowed, (bool fec_allowed), (override));
};

}  // namespace webrtc

#endif  // API_TEST_MOCK_FEC_CONTROLLER_OVERRIDE_H_
