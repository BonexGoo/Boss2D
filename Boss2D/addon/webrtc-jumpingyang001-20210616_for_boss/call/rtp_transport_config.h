/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef CALL_RTP_TRANSPORT_CONFIG_H_
#define CALL_RTP_TRANSPORT_CONFIG_H_

#include <memory>

#include BOSS_WEBRTC_U_api__network_state_predictor_h //original-code:"api/network_state_predictor.h"
#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_h //original-code:"api/rtc_event_log/rtc_event_log.h"
#include BOSS_WEBRTC_U_api__transport__bitrate_settings_h //original-code:"api/transport/bitrate_settings.h"
#include BOSS_WEBRTC_U_api__transport__network_control_h //original-code:"api/transport/network_control.h"
#include BOSS_WEBRTC_U_api__transport__webrtc_key_value_config_h //original-code:"api/transport/webrtc_key_value_config.h"
#include BOSS_WEBRTC_U_modules__utility__include__process_thread_h //original-code:"modules/utility/include/process_thread.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"

namespace webrtc {

struct RtpTransportConfig {
  // Bitrate config used until valid bitrate estimates are calculated. Also
  // used to cap total bitrate used. This comes from the remote connection.
  BitrateConstraints bitrate_config;

  // RtcEventLog to use for this call. Required.
  // Use webrtc::RtcEventLog::CreateNull() for a null implementation.
  RtcEventLog* event_log = nullptr;

  // Task Queue Factory to be used in this call. Required.
  TaskQueueFactory* task_queue_factory = nullptr;

  // NetworkStatePredictor to use for this call.
  NetworkStatePredictorFactoryInterface* network_state_predictor_factory =
      nullptr;

  // Network controller factory to use for this call.
  NetworkControllerFactoryInterface* network_controller_factory = nullptr;

  // Key-value mapping of internal configurations to apply,
  // e.g. field trials.
  const WebRtcKeyValueConfig* trials = nullptr;
};
}  // namespace webrtc

#endif  // CALL_RTP_TRANSPORT_CONFIG_H_
