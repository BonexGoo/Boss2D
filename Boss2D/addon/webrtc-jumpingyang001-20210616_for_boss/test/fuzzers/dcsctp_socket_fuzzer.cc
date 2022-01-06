/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__fuzzers__dcsctp_fuzzers_h //original-code:"net/dcsctp/fuzzers/dcsctp_fuzzers.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__dcsctp_message_h //original-code:"net/dcsctp/public/dcsctp_message.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__dcsctp_options_h //original-code:"net/dcsctp/public/dcsctp_options.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__dcsctp_socket_h //original-code:"net/dcsctp/public/dcsctp_socket.h"
#include BOSS_WEBRTC_U_net__dcsctp__socket__dcsctp_socket_h //original-code:"net/dcsctp/socket/dcsctp_socket.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace webrtc {

void FuzzOneInput(const uint8_t* data, size_t size) {
  dcsctp::dcsctp_fuzzers::FuzzerCallbacks cb;
  dcsctp::DcSctpOptions options;
  options.disable_checksum_verification = true;
  dcsctp::DcSctpSocket socket("A", cb, nullptr, options);

  dcsctp::dcsctp_fuzzers::FuzzSocket(socket, cb,
                                     rtc::ArrayView<const uint8_t>(data, size));
}
}  // namespace webrtc
