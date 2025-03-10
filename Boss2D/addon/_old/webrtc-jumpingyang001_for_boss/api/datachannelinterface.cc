/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__datachannelinterface_h //original-code:"api/datachannelinterface.h"

namespace webrtc {

bool DataChannelInterface::ordered() const {
  return false;
}

uint16_t DataChannelInterface::maxRetransmitTime() const {
  return 0;
}

uint16_t DataChannelInterface::maxRetransmits() const {
  return 0;
}

std::string DataChannelInterface::protocol() const {
  return std::string();
}

bool DataChannelInterface::negotiated() const {
  return false;
}

}  // namespace webrtc
