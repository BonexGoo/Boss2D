/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_ICE_CANDIDATE_PAIR_H_
#define LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_ICE_CANDIDATE_PAIR_H_

#include <stdint.h>

#include <memory>

#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_h //original-code:"api/rtc_event_log/rtc_event.h"

namespace webrtc {

enum class IceCandidatePairEventType {
  kCheckSent,
  kCheckReceived,
  kCheckResponseSent,
  kCheckResponseReceived,
  kNumValues,
};

class RtcEventIceCandidatePair final : public RtcEvent {
 public:
  static constexpr Type kType = Type::IceCandidatePairEvent;

  RtcEventIceCandidatePair(IceCandidatePairEventType type,
                           uint32_t candidate_pair_id,
                           uint32_t transaction_id);

  ~RtcEventIceCandidatePair() override;

  Type GetType() const override { return kType; }
  bool IsConfigEvent() const override { return false; }

  std::unique_ptr<RtcEventIceCandidatePair> Copy() const;

  IceCandidatePairEventType type() const { return type_; }
  uint32_t candidate_pair_id() const { return candidate_pair_id_; }
  uint32_t transaction_id() const { return transaction_id_; }

 private:
  RtcEventIceCandidatePair(const RtcEventIceCandidatePair& other);

  const IceCandidatePairEventType type_;
  const uint32_t candidate_pair_id_;
  const uint32_t transaction_id_;
};

struct LoggedIceCandidatePairEvent {
  LoggedIceCandidatePairEvent() = default;
  LoggedIceCandidatePairEvent(int64_t timestamp_us,
                              IceCandidatePairEventType type,
                              uint32_t candidate_pair_id,
                              uint32_t transaction_id)
      : timestamp_us(timestamp_us),
        type(type),
        candidate_pair_id(candidate_pair_id),
        transaction_id(transaction_id) {}

  int64_t log_time_us() const { return timestamp_us; }
  int64_t log_time_ms() const { return timestamp_us / 1000; }

  int64_t timestamp_us;
  IceCandidatePairEventType type;
  uint32_t candidate_pair_id;
  uint32_t transaction_id;
};

}  // namespace webrtc

#endif  // LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_ICE_CANDIDATE_PAIR_H_
