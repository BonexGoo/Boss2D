/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_INCLUDE_RTP_RTCP_H_
#define MODULES_RTP_RTCP_INCLUDE_RTP_RTCP_H_

#include <memory>

#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include BOSS_WEBRTC_U_modules__include__module_h //original-code:"modules/include/module.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_rtcp_interface_h //original-code:"modules/rtp_rtcp/source/rtp_rtcp_interface.h"

namespace webrtc {

// DEPRECATED. Do not use.
class RtpRtcp : public Module, public RtpRtcpInterface {
 public:
  // Instantiates a deprecated version of the RtpRtcp module.
  static std::unique_ptr<RtpRtcp> ABSL_DEPRECATED("")
      Create(const Configuration& configuration) {
    return DEPRECATED_Create(configuration);
  }

  static std::unique_ptr<RtpRtcp> DEPRECATED_Create(
      const Configuration& configuration);

  // Requests new key frame.
  // using PLI, https://tools.ietf.org/html/rfc4585#section-6.3.1.1
  void SendPictureLossIndication() { SendRTCP(kRtcpPli); }
  // using FIR, https://tools.ietf.org/html/rfc5104#section-4.3.1.2
  void SendFullIntraRequest() { SendRTCP(kRtcpFir); }
};

}  // namespace webrtc

#endif  // MODULES_RTP_RTCP_INCLUDE_RTP_RTCP_H_
