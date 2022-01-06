/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stddef.h>
#include <stdint.h>

#include <string>

#include BOSS_WEBRTC_U_api__data_channel_interface_h //original-code:"api/data_channel_interface.h"
#include BOSS_WEBRTC_U_pc__sctp_utils_h //original-code:"pc/sctp_utils.h"
#include BOSS_WEBRTC_U_rtc_base__copy_on_write_buffer_h //original-code:"rtc_base/copy_on_write_buffer.h"

namespace webrtc {

void FuzzOneInput(const uint8_t* data, size_t size) {
  rtc::CopyOnWriteBuffer payload(data, size);
  std::string label;
  DataChannelInit config;
  IsOpenMessage(payload);
  ParseDataChannelOpenMessage(payload, &label, &config);
  ParseDataChannelOpenAckMessage(payload);
}

}  // namespace webrtc
