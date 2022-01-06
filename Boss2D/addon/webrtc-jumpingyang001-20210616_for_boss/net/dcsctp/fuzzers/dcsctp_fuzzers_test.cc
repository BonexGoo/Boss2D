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

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__sctp_packet_h //original-code:"net/dcsctp/packet/sctp_packet.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__dcsctp_socket_h //original-code:"net/dcsctp/public/dcsctp_socket.h"
#include BOSS_WEBRTC_U_net__dcsctp__socket__dcsctp_socket_h //original-code:"net/dcsctp/socket/dcsctp_socket.h"
#include "net/dcsctp/testing/testing_macros.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace dcsctp {
namespace dcsctp_fuzzers {
namespace {

// This is a testbed where fuzzed data that cause issues can be evaluated and
// crashes reproduced. Use `xxd -i ./crash-abc` to generate `data` below.
TEST(DcsctpFuzzersTest, PassesTestbed) {
  uint8_t data[] = {0x07, 0x09, 0x00, 0x01, 0x11, 0xff, 0xff};

  FuzzerCallbacks cb;
  DcSctpOptions options;
  options.disable_checksum_verification = true;
  DcSctpSocket socket("A", cb, nullptr, options);

  FuzzSocket(socket, cb, data);
}

}  // namespace
}  // namespace dcsctp_fuzzers
}  // namespace dcsctp
