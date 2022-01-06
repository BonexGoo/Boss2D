/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef CALL_FLEXFEC_RECEIVE_STREAM_IMPL_H_
#define CALL_FLEXFEC_RECEIVE_STREAM_IMPL_H_

#include <memory>

#include BOSS_WEBRTC_U_call__flexfec_receive_stream_h //original-code:"call/flexfec_receive_stream.h"
#include BOSS_WEBRTC_U_call__rtp_packet_sink_interface_h //original-code:"call/rtp_packet_sink_interface.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_rtcp_impl2_h //original-code:"modules/rtp_rtcp/source/rtp_rtcp_impl2.h"
#include BOSS_WEBRTC_U_rtc_base__system__no_unique_address_h //original-code:"rtc_base/system/no_unique_address.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {

class FlexfecReceiver;
class ProcessThread;
class ReceiveStatistics;
class RecoveredPacketReceiver;
class RtcpRttStats;
class RtpPacketReceived;
class RtpRtcp;
class RtpStreamReceiverControllerInterface;
class RtpStreamReceiverInterface;

class FlexfecReceiveStreamImpl : public FlexfecReceiveStream {
 public:
  FlexfecReceiveStreamImpl(
      Clock* clock,
      const Config& config,
      RecoveredPacketReceiver* recovered_packet_receiver,
      RtcpRttStats* rtt_stats,
      ProcessThread* process_thread);
  // Destruction happens on the worker thread. Prior to destruction the caller
  // must ensure that a registration with the transport has been cleared. See
  // `RegisterWithTransport` for details.
  // TODO(tommi): As a further improvement to this, performing the full
  // destruction on the network thread could be made the default.
  ~FlexfecReceiveStreamImpl() override;

  // Called on the network thread to register/unregister with the network
  // transport.
  void RegisterWithTransport(
      RtpStreamReceiverControllerInterface* receiver_controller);
  // If registration has previously been done (via `RegisterWithTransport`) then
  // `UnregisterFromTransport` must be called prior to destruction, on the
  // network thread.
  void UnregisterFromTransport();

  // RtpPacketSinkInterface.
  void OnRtpPacket(const RtpPacketReceived& packet) override;

  Stats GetStats() const override;

  // ReceiveStream impl.
  const RtpConfig& rtp_config() const override { return config_.rtp; }

 private:
  RTC_NO_UNIQUE_ADDRESS SequenceChecker packet_sequence_checker_;

  // Config.
  const Config config_;

  // Erasure code interfacing.
  const std::unique_ptr<FlexfecReceiver> receiver_;

  // RTCP reporting.
  const std::unique_ptr<ReceiveStatistics> rtp_receive_statistics_;
  const std::unique_ptr<ModuleRtpRtcpImpl2> rtp_rtcp_;
  ProcessThread* const process_thread_;

  std::unique_ptr<RtpStreamReceiverInterface> rtp_stream_receiver_
      RTC_GUARDED_BY(packet_sequence_checker_);
};

}  // namespace webrtc

#endif  // CALL_FLEXFEC_RECEIVE_STREAM_IMPL_H_
