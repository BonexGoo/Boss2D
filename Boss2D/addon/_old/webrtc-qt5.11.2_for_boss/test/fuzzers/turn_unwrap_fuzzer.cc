/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stddef.h>
#include BOSS_FAKEWIN_V_stdint_h //original-code:<stdint.h>

#include BOSS_WEBRTC_U_media__base__turnutils_h //original-code:"media/base/turnutils.h"

namespace webrtc {
void FuzzOneInput(const uint8_t* data, size_t size) {
  size_t content_position;
  size_t content_size;
  cricket::UnwrapTurnPacket(data, size, &content_position, &content_size);
}
}  // namespace webrtc
