/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_PACING_RTP_PACKET_PACER_H_
#define MODULES_PACING_RTP_PACKET_PACER_H_

#include <stdint.h>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__units__data_rate_h //original-code:"api/units/data_rate.h"
#include BOSS_WEBRTC_U_api__units__data_size_h //original-code:"api/units/data_size.h"
#include BOSS_WEBRTC_U_api__units__time_delta_h //original-code:"api/units/time_delta.h"
#include BOSS_WEBRTC_U_api__units__timestamp_h //original-code:"api/units/timestamp.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_packet_sender_h //original-code:"modules/rtp_rtcp/include/rtp_packet_sender.h"

namespace webrtc {

class RtpPacketPacer {
 public:
  virtual ~RtpPacketPacer() = default;

  virtual void CreateProbeCluster(DataRate bitrate, int cluster_id) = 0;

  // Temporarily pause all sending.
  virtual void Pause() = 0;

  // Resume sending packets.
  virtual void Resume() = 0;

  virtual void SetCongestionWindow(DataSize congestion_window_size) = 0;
  virtual void UpdateOutstandingData(DataSize outstanding_data) = 0;

  // Sets the pacing rates. Must be called once before packets can be sent.
  virtual void SetPacingRates(DataRate pacing_rate, DataRate padding_rate) = 0;

  // Time since the oldest packet currently in the queue was added.
  virtual TimeDelta OldestPacketWaitTime() const = 0;

  // Sum of payload + padding bytes of all packets currently in the pacer queue.
  virtual DataSize QueueSizeData() const = 0;

  // Returns the time when the first packet was sent.
  virtual absl::optional<Timestamp> FirstSentPacketTime() const = 0;

  // Returns the expected number of milliseconds it will take to send the
  // current packets in the queue, given the current size and bitrate, ignoring
  // priority.
  virtual TimeDelta ExpectedQueueTime() const = 0;

  // Set the average upper bound on pacer queuing delay. The pacer may send at
  // a higher rate than what was configured via SetPacingRates() in order to
  // keep ExpectedQueueTimeMs() below |limit_ms| on average.
  virtual void SetQueueTimeLimit(TimeDelta limit) = 0;

  // Currently audio traffic is not accounted by pacer and passed through.
  // With the introduction of audio BWE audio traffic will be accounted for
  // the pacer budget calculation. The audio traffic still will be injected
  // at high priority.
  virtual void SetAccountForAudioPackets(bool account_for_audio) = 0;
  virtual void SetIncludeOverhead() = 0;
  virtual void SetTransportOverhead(DataSize overhead_per_packet) = 0;
};

}  // namespace webrtc
#endif  // MODULES_PACING_RTP_PACKET_PACER_H_
