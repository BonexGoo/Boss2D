/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef NET_DCSCTP_TESTING_DATA_GENERATOR_H_
#define NET_DCSCTP_TESTING_DATA_GENERATOR_H_

#include <cstdint>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__common__internal_types_h //original-code:"net/dcsctp/common/internal_types.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__data_h //original-code:"net/dcsctp/packet/data.h"

namespace dcsctp {

struct DataGeneratorOptions {
  StreamID stream_id = StreamID(1);
  absl::optional<MID> message_id = absl::nullopt;
  PPID ppid = PPID(53);
};

// Generates Data with correct sequence numbers, and used only in unit tests.
class DataGenerator {
 public:
  explicit DataGenerator(MID start_message_id = MID(0))
      : message_id_(start_message_id) {}

  // Generates ordered "data" with the provided `payload` and flags, which can
  // contain "B" for setting the "is_beginning" flag, and/or "E" for setting the
  // "is_end" flag.
  Data Ordered(std::vector<uint8_t> payload,
               absl::string_view flags = "",
               const DataGeneratorOptions opts = {});

  // Generates unordered "data" with the provided `payload` and flags, which can
  // contain "B" for setting the "is_beginning" flag, and/or "E" for setting the
  // "is_end" flag.
  Data Unordered(std::vector<uint8_t> payload,
                 absl::string_view flags = "",
                 const DataGeneratorOptions opts = {});

  // Resets the Message ID identifier - simulating a "stream reset".
  void ResetStream() { message_id_ = MID(0); }

 private:
  MID message_id_;
  FSN fsn_ = FSN(0);
};
}  // namespace dcsctp

#endif  // NET_DCSCTP_TESTING_DATA_GENERATOR_H_
