/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stddef.h>
#include BOSS_FAKEWIN_V_stdint_h //original-code:<stdint.h>

#include BOSS_WEBRTC_U_api__jsepsessiondescription_h //original-code:"api/jsepsessiondescription.h"

namespace webrtc {
void FuzzOneInput(const uint8_t* data, size_t size) {
  std::string message(reinterpret_cast<const char*>(data), size);
  webrtc::SdpParseError error;

  std::unique_ptr<webrtc::SessionDescriptionInterface> sdp(
      CreateSessionDescription("offer", message, &error));
}

}  // namespace webrtc
