/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef CALL_RTP_TRANSPORT_CONTROLLER_SEND_FACTORY_INTERFACE_H_
#define CALL_RTP_TRANSPORT_CONTROLLER_SEND_FACTORY_INTERFACE_H_

#include <memory>

#include BOSS_WEBRTC_U_call__rtp_transport_config_h //original-code:"call/rtp_transport_config.h"
#include BOSS_WEBRTC_U_call__rtp_transport_controller_send_interface_h //original-code:"call/rtp_transport_controller_send_interface.h"
#include BOSS_WEBRTC_U_modules__utility__include__process_thread_h //original-code:"modules/utility/include/process_thread.h"

namespace webrtc {
// A factory used for dependency injection on the send side of the transport
// controller.
class RtpTransportControllerSendFactoryInterface {
 public:
  virtual std::unique_ptr<RtpTransportControllerSendInterface> Create(
      const RtpTransportConfig& config,
      Clock* clock,
      std::unique_ptr<ProcessThread> process_thread) = 0;

  virtual ~RtpTransportControllerSendFactoryInterface() {}
};
}  // namespace webrtc
#endif  // CALL_RTP_TRANSPORT_CONTROLLER_SEND_FACTORY_INTERFACE_H_
