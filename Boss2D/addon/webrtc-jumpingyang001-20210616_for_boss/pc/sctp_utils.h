/*
 *  Copyright 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_SCTP_UTILS_H_
#define PC_SCTP_UTILS_H_

#include <string>

#include BOSS_WEBRTC_U_api__data_channel_interface_h //original-code:"api/data_channel_interface.h"
#include BOSS_WEBRTC_U_api__transport__data_channel_transport_interface_h //original-code:"api/transport/data_channel_transport_interface.h"
#include BOSS_WEBRTC_U_media__base__media_channel_h //original-code:"media/base/media_channel.h"
#include BOSS_WEBRTC_U_rtc_base__copy_on_write_buffer_h //original-code:"rtc_base/copy_on_write_buffer.h"

namespace rtc {
class CopyOnWriteBuffer;
}  // namespace rtc

namespace webrtc {
struct DataChannelInit;

// Read the message type and return true if it's an OPEN message.
bool IsOpenMessage(const rtc::CopyOnWriteBuffer& payload);

bool ParseDataChannelOpenMessage(const rtc::CopyOnWriteBuffer& payload,
                                 std::string* label,
                                 DataChannelInit* config);

bool ParseDataChannelOpenAckMessage(const rtc::CopyOnWriteBuffer& payload);

bool WriteDataChannelOpenMessage(const std::string& label,
                                 const DataChannelInit& config,
                                 rtc::CopyOnWriteBuffer* payload);

void WriteDataChannelOpenAckMessage(rtc::CopyOnWriteBuffer* payload);

}  // namespace webrtc

#endif  // PC_SCTP_UTILS_H_
