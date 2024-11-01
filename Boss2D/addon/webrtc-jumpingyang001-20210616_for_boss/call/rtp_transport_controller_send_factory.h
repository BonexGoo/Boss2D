/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef CALL_RTP_TRANSPORT_CONTROLLER_SEND_FACTORY_H_
#define CALL_RTP_TRANSPORT_CONTROLLER_SEND_FACTORY_H_

#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_call__rtp_transport_controller_send_h //original-code:"call/rtp_transport_controller_send.h"
#include BOSS_WEBRTC_U_call__rtp_transport_controller_send_factory_interface_h //original-code:"call/rtp_transport_controller_send_factory_interface.h"

namespace webrtc {
class RtpTransportControllerSendFactory
    : public RtpTransportControllerSendFactoryInterface {
 public:
  std::unique_ptr<RtpTransportControllerSendInterface> Create(
      const RtpTransportConfig& config,
      Clock* clock,
      std::unique_ptr<ProcessThread> process_thread) override {
    return std::make_unique<RtpTransportControllerSend>(
        clock, config.event_log, config.network_state_predictor_factory,
        config.network_controller_factory, config.bitrate_config,
        std::move(process_thread), config.task_queue_factory, config.trials);
  }

  virtual ~RtpTransportControllerSendFactory() {}
};
}  // namespace webrtc
#endif  // CALL_RTP_TRANSPORT_CONTROLLER_SEND_FACTORY_H_
