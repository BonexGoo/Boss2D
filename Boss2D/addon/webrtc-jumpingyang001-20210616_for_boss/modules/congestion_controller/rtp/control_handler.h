/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_CONGESTION_CONTROLLER_RTP_CONTROL_HANDLER_H_
#define MODULES_CONGESTION_CONTROLLER_RTP_CONTROL_HANDLER_H_

#include <stdint.h>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__transport__network_types_h //original-code:"api/transport/network_types.h"
#include BOSS_WEBRTC_U_api__units__data_size_h //original-code:"api/units/data_size.h"
#include BOSS_WEBRTC_U_api__units__time_delta_h //original-code:"api/units/time_delta.h"
#include BOSS_WEBRTC_U_modules__pacing__paced_sender_h //original-code:"modules/pacing/paced_sender.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__system__no_unique_address_h //original-code:"rtc_base/system/no_unique_address.h"

namespace webrtc {
// This is used to observe the network controller state and route calls to
// the proper handler. It also keeps cached values for safe asynchronous use.
// This makes sure that things running on the worker queue can't access state
// in RtpTransportControllerSend, which would risk causing data race on
// destruction unless members are properly ordered.
class CongestionControlHandler {
 public:
  CongestionControlHandler();
  ~CongestionControlHandler();

  void SetTargetRate(TargetTransferRate new_target_rate);
  void SetNetworkAvailability(bool network_available);
  void SetPacerQueue(TimeDelta expected_queue_time);
  absl::optional<TargetTransferRate> GetUpdate();

 private:
  absl::optional<TargetTransferRate> last_incoming_;
  absl::optional<TargetTransferRate> last_reported_;
  bool network_available_ = true;
  bool encoder_paused_in_last_report_ = false;

  const bool disable_pacer_emergency_stop_;
  int64_t pacer_expected_queue_ms_ = 0;

  RTC_NO_UNIQUE_ADDRESS SequenceChecker sequenced_checker_;
  RTC_DISALLOW_COPY_AND_ASSIGN(CongestionControlHandler);
};
}  // namespace webrtc
#endif  // MODULES_CONGESTION_CONTROLLER_RTP_CONTROL_HANDLER_H_
