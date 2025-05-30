/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_call__flexfec_receive_stream_impl_h //original-code:"call/flexfec_receive_stream_impl.h"

#include <stddef.h>

#include <cstdint>
#include <string>
#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__call__transport_h //original-code:"api/call/transport.h"
#include BOSS_WEBRTC_U_api__rtp_parameters_h //original-code:"api/rtp_parameters.h"
#include BOSS_WEBRTC_U_call__rtp_stream_receiver_controller_interface_h //original-code:"call/rtp_stream_receiver_controller_interface.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__flexfec_receiver_h //original-code:"modules/rtp_rtcp/include/flexfec_receiver.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__receive_statistics_h //original-code:"modules/rtp_rtcp/include/receive_statistics.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_received_h //original-code:"modules/rtp_rtcp/source/rtp_packet_received.h"
#include BOSS_WEBRTC_U_modules__utility__include__process_thread_h //original-code:"modules/utility/include/process_thread.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__location_h //original-code:"rtc_base/location.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {

std::string FlexfecReceiveStream::Stats::ToString(int64_t time_ms) const {
  char buf[1024];
  rtc::SimpleStringBuilder ss(buf);
  ss << "FlexfecReceiveStream stats: " << time_ms
     << ", {flexfec_bitrate_bps: " << flexfec_bitrate_bps << "}";
  return ss.str();
}

std::string FlexfecReceiveStream::Config::ToString() const {
  char buf[1024];
  rtc::SimpleStringBuilder ss(buf);
  ss << "{payload_type: " << payload_type;
  ss << ", remote_ssrc: " << rtp.remote_ssrc;
  ss << ", local_ssrc: " << rtp.local_ssrc;
  ss << ", protected_media_ssrcs: [";
  size_t i = 0;
  for (; i + 1 < protected_media_ssrcs.size(); ++i)
    ss << protected_media_ssrcs[i] << ", ";
  if (!protected_media_ssrcs.empty())
    ss << protected_media_ssrcs[i];
  ss << "], transport_cc: " << (rtp.transport_cc ? "on" : "off");
  ss << ", rtp.extensions: [";
  i = 0;
  for (; i + 1 < rtp.extensions.size(); ++i)
    ss << rtp.extensions[i].ToString() << ", ";
  if (!rtp.extensions.empty())
    ss << rtp.extensions[i].ToString();
  ss << "]}";
  return ss.str();
}

bool FlexfecReceiveStream::Config::IsCompleteAndEnabled() const {
  // Check if FlexFEC is enabled.
  if (payload_type < 0)
    return false;
  // Do we have the necessary SSRC information?
  if (rtp.remote_ssrc == 0)
    return false;
  // TODO(brandtr): Update this check when we support multistream protection.
  if (protected_media_ssrcs.size() != 1u)
    return false;
  return true;
}

namespace {

// TODO(brandtr): Update this function when we support multistream protection.
std::unique_ptr<FlexfecReceiver> MaybeCreateFlexfecReceiver(
    Clock* clock,
    const FlexfecReceiveStream::Config& config,
    RecoveredPacketReceiver* recovered_packet_receiver) {
  if (config.payload_type < 0) {
    RTC_LOG(LS_WARNING)
        << "Invalid FlexFEC payload type given. "
           "This FlexfecReceiveStream will therefore be useless.";
    return nullptr;
  }
  RTC_DCHECK_GE(config.payload_type, 0);
  RTC_DCHECK_LE(config.payload_type, 127);
  if (config.rtp.remote_ssrc == 0) {
    RTC_LOG(LS_WARNING)
        << "Invalid FlexFEC SSRC given. "
           "This FlexfecReceiveStream will therefore be useless.";
    return nullptr;
  }
  if (config.protected_media_ssrcs.empty()) {
    RTC_LOG(LS_WARNING)
        << "No protected media SSRC supplied. "
           "This FlexfecReceiveStream will therefore be useless.";
    return nullptr;
  }

  if (config.protected_media_ssrcs.size() > 1) {
    RTC_LOG(LS_WARNING)
        << "The supplied FlexfecConfig contained multiple protected "
           "media streams, but our implementation currently only "
           "supports protecting a single media stream. "
           "To avoid confusion, disabling FlexFEC completely.";
    return nullptr;
  }
  RTC_DCHECK_EQ(1U, config.protected_media_ssrcs.size());
  return std::unique_ptr<FlexfecReceiver>(new FlexfecReceiver(
      clock, config.rtp.remote_ssrc, config.protected_media_ssrcs[0],
      recovered_packet_receiver));
}

std::unique_ptr<ModuleRtpRtcpImpl2> CreateRtpRtcpModule(
    Clock* clock,
    ReceiveStatistics* receive_statistics,
    const FlexfecReceiveStreamImpl::Config& config,
    RtcpRttStats* rtt_stats) {
  RtpRtcpInterface::Configuration configuration;
  configuration.audio = false;
  configuration.receiver_only = true;
  configuration.clock = clock;
  configuration.receive_statistics = receive_statistics;
  configuration.outgoing_transport = config.rtcp_send_transport;
  configuration.rtt_stats = rtt_stats;
  configuration.local_media_ssrc = config.rtp.local_ssrc;
  return ModuleRtpRtcpImpl2::Create(configuration);
}

}  // namespace

FlexfecReceiveStreamImpl::FlexfecReceiveStreamImpl(
    Clock* clock,
    const Config& config,
    RecoveredPacketReceiver* recovered_packet_receiver,
    RtcpRttStats* rtt_stats,
    ProcessThread* process_thread)
    : config_(config),
      receiver_(MaybeCreateFlexfecReceiver(clock,
                                           config_,
                                           recovered_packet_receiver)),
      rtp_receive_statistics_(ReceiveStatistics::Create(clock)),
      rtp_rtcp_(CreateRtpRtcpModule(clock,
                                    rtp_receive_statistics_.get(),
                                    config_,
                                    rtt_stats)),
      process_thread_(process_thread) {
  RTC_LOG(LS_INFO) << "FlexfecReceiveStreamImpl: " << config_.ToString();

  packet_sequence_checker_.Detach();

  // RTCP reporting.
  rtp_rtcp_->SetRTCPStatus(config_.rtcp_mode);
  process_thread_->RegisterModule(rtp_rtcp_.get(), RTC_FROM_HERE);
}

FlexfecReceiveStreamImpl::~FlexfecReceiveStreamImpl() {
  RTC_LOG(LS_INFO) << "~FlexfecReceiveStreamImpl: " << config_.ToString();
  process_thread_->DeRegisterModule(rtp_rtcp_.get());
}

void FlexfecReceiveStreamImpl::RegisterWithTransport(
    RtpStreamReceiverControllerInterface* receiver_controller) {
  RTC_DCHECK_RUN_ON(&packet_sequence_checker_);
  RTC_DCHECK(!rtp_stream_receiver_);

  if (!receiver_)
    return;

  // TODO(nisse): OnRtpPacket in this class delegates all real work to
  // `receiver_`. So maybe we don't need to implement RtpPacketSinkInterface
  // here at all, we'd then delete the OnRtpPacket method and instead register
  // `receiver_` as the RtpPacketSinkInterface for this stream.
  rtp_stream_receiver_ =
      receiver_controller->CreateReceiver(config_.rtp.remote_ssrc, this);
}

void FlexfecReceiveStreamImpl::UnregisterFromTransport() {
  RTC_DCHECK_RUN_ON(&packet_sequence_checker_);
  rtp_stream_receiver_.reset();
}

void FlexfecReceiveStreamImpl::OnRtpPacket(const RtpPacketReceived& packet) {
  if (!receiver_)
    return;

  receiver_->OnRtpPacket(packet);

  // Do not report media packets in the RTCP RRs generated by |rtp_rtcp_|.
  if (packet.Ssrc() == config_.rtp.remote_ssrc) {
    rtp_receive_statistics_->OnRtpPacket(packet);
  }
}

// TODO(brandtr): Implement this member function when we have designed the
// stats for FlexFEC.
FlexfecReceiveStreamImpl::Stats FlexfecReceiveStreamImpl::GetStats() const {
  return FlexfecReceiveStream::Stats();
}

}  // namespace webrtc
