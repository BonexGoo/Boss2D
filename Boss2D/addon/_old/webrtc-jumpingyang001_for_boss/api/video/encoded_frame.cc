/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__video__encoded_frame_h //original-code:"api/video/encoded_frame.h"

namespace webrtc {
namespace video_coding {

bool EncodedFrame::delayed_by_retransmission() const {
  return 0;
}

uint32_t EncodedFrame::Timestamp() const {
  return timestamp;
}

void EncodedFrame::SetTimestamp(uint32_t rtp_timestamp) {
  timestamp = rtp_timestamp;
}

}  // namespace video_coding
}  // namespace webrtc
