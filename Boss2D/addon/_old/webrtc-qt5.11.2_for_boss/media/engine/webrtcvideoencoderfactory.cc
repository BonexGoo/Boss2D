/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_media__engine__webrtcvideoencoderfactory_h //original-code:"media/engine/webrtcvideoencoderfactory.h"

namespace cricket {

bool WebRtcVideoEncoderFactory::EncoderTypeHasInternalSource(
    webrtc::VideoCodecType type) const {
  return false;
}

}  // namespace cricket
