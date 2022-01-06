/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef CALL_CALL_CONFIG_H_
#define CALL_CALL_CONFIG_H_

#include BOSS_WEBRTC_U_api__fec_controller_h //original-code:"api/fec_controller.h"
#include BOSS_WEBRTC_U_api__neteq__neteq_factory_h //original-code:"api/neteq/neteq_factory.h"
#include BOSS_WEBRTC_U_api__network_state_predictor_h //original-code:"api/network_state_predictor.h"
#include BOSS_WEBRTC_U_api__rtc_error_h //original-code:"api/rtc_error.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_factory_h //original-code:"api/task_queue/task_queue_factory.h"
#include BOSS_WEBRTC_U_api__transport__bitrate_settings_h //original-code:"api/transport/bitrate_settings.h"
#include BOSS_WEBRTC_U_api__transport__network_control_h //original-code:"api/transport/network_control.h"
#include BOSS_WEBRTC_U_api__transport__webrtc_key_value_config_h //original-code:"api/transport/webrtc_key_value_config.h"
#include BOSS_WEBRTC_U_call__audio_state_h //original-code:"call/audio_state.h"
#include BOSS_WEBRTC_U_call__rtp_transport_config_h //original-code:"call/rtp_transport_config.h"
#include BOSS_WEBRTC_U_call__rtp_transport_controller_send_factory_interface_h //original-code:"call/rtp_transport_controller_send_factory_interface.h"

namespace webrtc {

class AudioProcessing;
class RtcEventLog;

struct CallConfig {
  // If |network_task_queue| is set to nullptr, Call will assume that network
  // related callbacks will be made on the same TQ as the Call instance was
  // constructed on.
  explicit CallConfig(RtcEventLog* event_log,
                      TaskQueueBase* network_task_queue = nullptr);
  CallConfig(const CallConfig&);
  RtpTransportConfig ExtractTransportConfig() const;
  ~CallConfig();

  // Bitrate config used until valid bitrate estimates are calculated. Also
  // used to cap total bitrate used. This comes from the remote connection.
  BitrateConstraints bitrate_config;

  // AudioState which is possibly shared between multiple calls.
  rtc::scoped_refptr<AudioState> audio_state;

  // Audio Processing Module to be used in this call.
  AudioProcessing* audio_processing = nullptr;

  // RtcEventLog to use for this call. Required.
  // Use webrtc::RtcEventLog::CreateNull() for a null implementation.
  RtcEventLog* const event_log = nullptr;

  // FecController to use for this call.
  FecControllerFactoryInterface* fec_controller_factory = nullptr;

  // Task Queue Factory to be used in this call. Required.
  TaskQueueFactory* task_queue_factory = nullptr;

  // NetworkStatePredictor to use for this call.
  NetworkStatePredictorFactoryInterface* network_state_predictor_factory =
      nullptr;

  // Network controller factory to use for this call.
  NetworkControllerFactoryInterface* network_controller_factory = nullptr;

  // NetEq factory to use for this call.
  NetEqFactory* neteq_factory = nullptr;

  // Key-value mapping of internal configurations to apply,
  // e.g. field trials.
  const WebRtcKeyValueConfig* trials = nullptr;

  TaskQueueBase* const network_task_queue_ = nullptr;
  // RtpTransportControllerSend to use for this call.
  RtpTransportControllerSendFactoryInterface*
      rtp_transport_controller_send_factory = nullptr;
};

}  // namespace webrtc

#endif  // CALL_CALL_CONFIG_H_
