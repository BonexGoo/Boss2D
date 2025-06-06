/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_GENERIC_ACK_RECEIVED_H_
#define LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_GENERIC_ACK_RECEIVED_H_

#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_h //original-code:"api/rtc_event_log/rtc_event.h"

namespace webrtc {

struct AckedPacket {
  // The packet number that was acked.
  int64_t packet_number;

  // The time where the packet was received. Not every ACK will
  // include the receive timestamp.
  absl::optional<int64_t> receive_acked_packet_time_ms;
};

class RtcEventGenericAckReceived final : public RtcEvent {
 public:
  static constexpr Type kType = Type::GenericAckReceived;

  // For a collection of acked packets, it creates a vector of logs to log with
  // the same timestamp.
  static std::vector<std::unique_ptr<RtcEventGenericAckReceived>> CreateLogs(
      int64_t packet_number,
      const std::vector<AckedPacket>& acked_packets);

  ~RtcEventGenericAckReceived() override;

  std::unique_ptr<RtcEventGenericAckReceived> Copy() const;

  Type GetType() const override { return kType; }
  bool IsConfigEvent() const override { return false; }

  // An identifier of the packet which contained an ack.
  int64_t packet_number() const { return packet_number_; }

  // An identifier of the acked packet.
  int64_t acked_packet_number() const { return acked_packet_number_; }

  // Timestamp when the |acked_packet_number| was received by the remote side.
  absl::optional<int64_t> receive_acked_packet_time_ms() const {
    return receive_acked_packet_time_ms_;
  }

 private:
  RtcEventGenericAckReceived(const RtcEventGenericAckReceived& packet);

  // When the ack is received, |packet_number| identifies the packet which
  // contained an ack for |acked_packet_number|, and contains the
  // |receive_acked_packet_time_ms| on which the |acked_packet_number| was
  // received on the remote side. The |receive_acked_packet_time_ms| may be
  // null.
  RtcEventGenericAckReceived(
      int64_t timestamp_us,
      int64_t packet_number,
      int64_t acked_packet_number,
      absl::optional<int64_t> receive_acked_packet_time_ms);

  const int64_t packet_number_;
  const int64_t acked_packet_number_;
  const absl::optional<int64_t> receive_acked_packet_time_ms_;
};

struct LoggedGenericAckReceived {
  LoggedGenericAckReceived() = default;
  LoggedGenericAckReceived(int64_t timestamp_us,
                           int64_t packet_number,
                           int64_t acked_packet_number,
                           absl::optional<int64_t> receive_acked_packet_time_ms)
      : timestamp_us(timestamp_us),
        packet_number(packet_number),
        acked_packet_number(acked_packet_number),
        receive_acked_packet_time_ms(receive_acked_packet_time_ms) {}

  int64_t log_time_us() const { return timestamp_us; }
  int64_t log_time_ms() const { return timestamp_us / 1000; }

  int64_t timestamp_us;
  int64_t packet_number;
  int64_t acked_packet_number;
  absl::optional<int64_t> receive_acked_packet_time_ms;
};

}  // namespace webrtc

#endif  // LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_GENERIC_ACK_RECEIVED_H_
