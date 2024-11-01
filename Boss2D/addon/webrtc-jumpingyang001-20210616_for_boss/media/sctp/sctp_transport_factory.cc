/*
 *  Copyright 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_media__sctp__sctp_transport_factory_h //original-code:"media/sctp/sctp_transport_factory.h"

#include BOSS_WEBRTC_U_rtc_base__system__unused_h //original-code:"rtc_base/system/unused.h"

#ifdef WEBRTC_HAVE_DCSCTP
#include BOSS_WEBRTC_U_media__sctp__dcsctp_transport_h //original-code:"media/sctp/dcsctp_transport.h"          // nogncheck
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"        // nogncheck
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"  // nogncheck
#endif

#ifdef WEBRTC_HAVE_USRSCTP
#include BOSS_WEBRTC_U_media__sctp__usrsctp_transport_h //original-code:"media/sctp/usrsctp_transport.h"  // nogncheck
#endif

namespace cricket {

SctpTransportFactory::SctpTransportFactory(rtc::Thread* network_thread)
    : network_thread_(network_thread), use_dcsctp_("Enabled", false) {
  RTC_UNUSED(network_thread_);
#ifdef WEBRTC_HAVE_DCSCTP
  webrtc::ParseFieldTrial({&use_dcsctp_}, webrtc::field_trial::FindFullName(
                                              "WebRTC-DataChannel-Dcsctp"));
#endif
}

std::unique_ptr<SctpTransportInternal>
SctpTransportFactory::CreateSctpTransport(
    rtc::PacketTransportInternal* transport) {
  std::unique_ptr<SctpTransportInternal> result;
#ifdef WEBRTC_HAVE_DCSCTP
  if (use_dcsctp_.Get()) {
    result = std::unique_ptr<SctpTransportInternal>(new webrtc::DcSctpTransport(
        network_thread_, transport, webrtc::Clock::GetRealTimeClock()));
  }
#endif
#ifdef WEBRTC_HAVE_USRSCTP
  if (!result) {
    result = std::unique_ptr<SctpTransportInternal>(
        new UsrsctpTransport(network_thread_, transport));
  }
#endif
  return result;
}

}  // namespace cricket
