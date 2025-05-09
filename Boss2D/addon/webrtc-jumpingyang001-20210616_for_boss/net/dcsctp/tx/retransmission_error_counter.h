/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef NET_DCSCTP_TX_RETRANSMISSION_ERROR_COUNTER_H_
#define NET_DCSCTP_TX_RETRANSMISSION_ERROR_COUNTER_H_

#include <functional>
#include <string>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__dcsctp_options_h //original-code:"net/dcsctp/public/dcsctp_options.h"

namespace dcsctp {

// The RetransmissionErrorCounter is a simple counter with a limit, and when
// the limit is exceeded, the counter is exhausted and the connection will
// be closed. It's incremented on retransmission errors, such as the T3-RTX
// timer expiring, but also missing heartbeats and stream reset requests.
class RetransmissionErrorCounter {
 public:
  RetransmissionErrorCounter(absl::string_view log_prefix,
                             const DcSctpOptions& options)
      : log_prefix_(std::string(log_prefix) + "rtx-errors: "),
        limit_(options.max_retransmissions) {}

  // Increments the retransmission timer. If the maximum error count has been
  // reached, `false` will be returned.
  bool Increment(absl::string_view reason);
  bool IsExhausted() const { return counter_ > limit_; }

  // Clears the retransmission errors.
  void Clear();

  // Returns its current value
  int value() const { return counter_; }

 private:
  const std::string log_prefix_;
  const int limit_;
  int counter_ = 0;
};
}  // namespace dcsctp

#endif  // NET_DCSCTP_TX_RETRANSMISSION_ERROR_COUNTER_H_
