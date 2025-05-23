/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_PROBE_CLUSTER_CREATED_H_
#define LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_PROBE_CLUSTER_CREATED_H_

#include <stdint.h>

#include <memory>

#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_h //original-code:"api/rtc_event_log/rtc_event.h"

namespace webrtc {

class RtcEventProbeClusterCreated final : public RtcEvent {
 public:
  static constexpr Type kType = Type::ProbeClusterCreated;

  RtcEventProbeClusterCreated(int32_t id,
                              int32_t bitrate_bps,
                              uint32_t min_probes,
                              uint32_t min_bytes);
  ~RtcEventProbeClusterCreated() override = default;

  Type GetType() const override { return kType; }
  bool IsConfigEvent() const override { return false; }

  std::unique_ptr<RtcEventProbeClusterCreated> Copy() const;

  int32_t id() const { return id_; }
  int32_t bitrate_bps() const { return bitrate_bps_; }
  uint32_t min_probes() const { return min_probes_; }
  uint32_t min_bytes() const { return min_bytes_; }

 private:
  RtcEventProbeClusterCreated(const RtcEventProbeClusterCreated& other);

  const int32_t id_;
  const int32_t bitrate_bps_;
  const uint32_t min_probes_;
  const uint32_t min_bytes_;
};

struct LoggedBweProbeClusterCreatedEvent {
  LoggedBweProbeClusterCreatedEvent() = default;
  LoggedBweProbeClusterCreatedEvent(int64_t timestamp_us,
                                    int32_t id,
                                    int32_t bitrate_bps,
                                    uint32_t min_packets,
                                    uint32_t min_bytes)
      : timestamp_us(timestamp_us),
        id(id),
        bitrate_bps(bitrate_bps),
        min_packets(min_packets),
        min_bytes(min_bytes) {}

  int64_t log_time_us() const { return timestamp_us; }
  int64_t log_time_ms() const { return timestamp_us / 1000; }

  int64_t timestamp_us;
  int32_t id;
  int32_t bitrate_bps;
  uint32_t min_packets;
  uint32_t min_bytes;
};

}  // namespace webrtc

#endif  // LOGGING_RTC_EVENT_LOG_EVENTS_RTC_EVENT_PROBE_CLUSTER_CREATED_H_
