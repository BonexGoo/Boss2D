/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__units__data_rate_h //original-code:"api/units/data_rate.h"

#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"

namespace webrtc {

std::string ToString(const DataRate& value) {
  char buf[64];
  rtc::SimpleStringBuilder sb(buf);
  if (value.IsInfinite()) {
    sb << "inf bps";
  } else {
    sb << value.bps() << " bps";
  }
  return sb.str();
}
}  // namespace webrtc
