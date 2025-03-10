/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_call__rtp_video_sender_h //original-code:"call/rtp_video_sender.h"

#include <algorithm>
#include <memory>
#include <string>
#include <utility>

#include BOSS_WEBRTC_U_call__rtp_transport_controller_send_interface_h //original-code:"call/rtp_transport_controller_send_interface.h"
#include BOSS_WEBRTC_U_modules__pacing__packet_router_h //original-code:"modules/pacing/packet_router.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_sender_h //original-code:"modules/rtp_rtcp/source/rtp_sender.h"
#include BOSS_WEBRTC_U_modules__utility__include__process_thread_h //original-code:"modules/utility/include/process_thread.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__location_h //original-code:"rtc_base/location.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"

namespace webrtc {

namespace {
static const int kMinSendSidePacketHistorySize = 600;

std::vector<std::unique_ptr<RtpRtcp>> CreateRtpRtcpModules(
    const std::vector<uint32_t>& ssrcs,
    const std::vector<uint32_t>& protected_media_ssrcs,
    const RtcpConfig& rtcp_config,
    Transport* send_transport,
    RtcpIntraFrameObserver* intra_frame_callback,
    RtcpBandwidthObserver* bandwidth_callback,
    RtpTransportControllerSendInterface* transport,
    RtcpRttStats* rtt_stats,
    FlexfecSender* flexfec_sender,
    BitrateStatisticsObserver* bitrate_observer,
    FrameCountObserver* frame_count_observer,
    RtcpPacketTypeCounterObserver* rtcp_type_observer,
    SendSideDelayObserver* send_delay_observer,
    SendPacketObserver* send_packet_observer,
    RtcEventLog* event_log,
    RateLimiter* retransmission_rate_limiter,
    OverheadObserver* overhead_observer,
    RtpKeepAliveConfig keepalive_config) {
  RTC_DCHECK_GT(ssrcs.size(), 0);
  RtpRtcp::Configuration configuration;
  configuration.audio = false;
  configuration.receiver_only = false;
  configuration.outgoing_transport = send_transport;
  configuration.intra_frame_callback = intra_frame_callback;
  configuration.bandwidth_callback = bandwidth_callback;
  configuration.transport_feedback_callback =
      transport->transport_feedback_observer();
  configuration.rtt_stats = rtt_stats;
  configuration.rtcp_packet_type_counter_observer = rtcp_type_observer;
  configuration.paced_sender = transport->packet_sender();
  configuration.transport_sequence_number_allocator =
      transport->packet_router();
  configuration.send_bitrate_observer = bitrate_observer;
  configuration.send_frame_count_observer = frame_count_observer;
  configuration.send_side_delay_observer = send_delay_observer;
  configuration.send_packet_observer = send_packet_observer;
  configuration.event_log = event_log;
  configuration.retransmission_rate_limiter = retransmission_rate_limiter;
  configuration.overhead_observer = overhead_observer;
  configuration.keepalive_config = keepalive_config;
  configuration.rtcp_interval_config.video_interval_ms =
      rtcp_config.video_report_interval_ms;
  configuration.rtcp_interval_config.audio_interval_ms =
      rtcp_config.audio_report_interval_ms;
  std::vector<std::unique_ptr<RtpRtcp>> modules;
  const std::vector<uint32_t>& flexfec_protected_ssrcs = protected_media_ssrcs;
  for (uint32_t ssrc : ssrcs) {
    bool enable_flexfec = flexfec_sender != nullptr &&
                          std::find(flexfec_protected_ssrcs.begin(),
                                    flexfec_protected_ssrcs.end(),
                                    ssrc) != flexfec_protected_ssrcs.end();
    configuration.flexfec_sender = enable_flexfec ? flexfec_sender : nullptr;
    std::unique_ptr<RtpRtcp> rtp_rtcp =
        std::unique_ptr<RtpRtcp>(RtpRtcp::CreateRtpRtcp(configuration));
    rtp_rtcp->SetSendingStatus(false);
    rtp_rtcp->SetSendingMediaStatus(false);
    rtp_rtcp->SetRTCPStatus(RtcpMode::kCompound);
    modules.push_back(std::move(rtp_rtcp));
  }
  return modules;
}

absl::optional<size_t> GetSimulcastIdx(const CodecSpecificInfo* info) {
  if (!info)
    return absl::nullopt;
  switch (info->codecType) {
    case kVideoCodecVP8:
      return absl::optional<size_t>(info->codecSpecific.VP8.simulcastIdx);
    case kVideoCodecH264:
      return absl::optional<size_t>(info->codecSpecific.H264.simulcast_idx);
    case kVideoCodecMultiplex:
    case kVideoCodecGeneric:
      return absl::optional<size_t>(info->codecSpecific.generic.simulcast_idx);
    default:
      return absl::nullopt;
  }
}
bool PayloadTypeSupportsSkippingFecPackets(const std::string& payload_name) {
  const VideoCodecType codecType = PayloadStringToCodecType(payload_name);
  if (codecType == kVideoCodecVP8 || codecType == kVideoCodecVP9) {
    return true;
  }
  return false;
}

// TODO(brandtr): Update this function when we support multistream protection.
std::unique_ptr<FlexfecSender> MaybeCreateFlexfecSender(
    const RtpConfig& rtp,
    const std::map<uint32_t, RtpState>& suspended_ssrcs) {
  if (rtp.flexfec.payload_type < 0) {
    return nullptr;
  }
  RTC_DCHECK_GE(rtp.flexfec.payload_type, 0);
  RTC_DCHECK_LE(rtp.flexfec.payload_type, 127);
  if (rtp.flexfec.ssrc == 0) {
    RTC_LOG(LS_WARNING) << "FlexFEC is enabled, but no FlexFEC SSRC given. "
                           "Therefore disabling FlexFEC.";
    return nullptr;
  }
  if (rtp.flexfec.protected_media_ssrcs.empty()) {
    RTC_LOG(LS_WARNING)
        << "FlexFEC is enabled, but no protected media SSRC given. "
           "Therefore disabling FlexFEC.";
    return nullptr;
  }

  if (rtp.flexfec.protected_media_ssrcs.size() > 1) {
    RTC_LOG(LS_WARNING)
        << "The supplied FlexfecConfig contained multiple protected "
           "media streams, but our implementation currently only "
           "supports protecting a single media stream. "
           "To avoid confusion, disabling FlexFEC completely.";
    return nullptr;
  }

  const RtpState* rtp_state = nullptr;
  auto it = suspended_ssrcs.find(rtp.flexfec.ssrc);
  if (it != suspended_ssrcs.end()) {
    rtp_state = &it->second;
  }

  RTC_DCHECK_EQ(1U, rtp.flexfec.protected_media_ssrcs.size());
  return absl::make_unique<FlexfecSender>(
      rtp.flexfec.payload_type, rtp.flexfec.ssrc,
      rtp.flexfec.protected_media_ssrcs[0], rtp.mid, rtp.extensions,
      RTPSender::FecExtensionSizes(), rtp_state, Clock::GetRealTimeClock());
}
}  // namespace

RtpVideoSender::RtpVideoSender(
    const std::vector<uint32_t>& ssrcs,
    std::map<uint32_t, RtpState> suspended_ssrcs,
    const std::map<uint32_t, RtpPayloadState>& states,
    const RtpConfig& rtp_config,
    const RtcpConfig& rtcp_config,
    Transport* send_transport,
    const RtpSenderObservers& observers,
    RtpTransportControllerSendInterface* transport,
    RtcEventLog* event_log,
    RateLimiter* retransmission_limiter)
    : active_(false),
      module_process_thread_(nullptr),
      suspended_ssrcs_(std::move(suspended_ssrcs)),
      flexfec_sender_(MaybeCreateFlexfecSender(rtp_config, suspended_ssrcs_)),
      rtp_modules_(
          CreateRtpRtcpModules(ssrcs,
                               rtp_config.flexfec.protected_media_ssrcs,
                               rtcp_config,
                               send_transport,
                               observers.intra_frame_callback,
                               transport->GetBandwidthObserver(),
                               transport,
                               observers.rtcp_rtt_stats,
                               flexfec_sender_.get(),
                               observers.bitrate_observer,
                               observers.frame_count_observer,
                               observers.rtcp_type_observer,
                               observers.send_delay_observer,
                               observers.send_packet_observer,
                               event_log,
                               retransmission_limiter,
                               observers.overhead_observer,
                               transport->keepalive_config())),
      rtp_config_(rtp_config),
      transport_(transport) {
  RTC_DCHECK_EQ(ssrcs.size(), rtp_modules_.size());
  module_process_thread_checker_.DetachFromThread();
  // SSRCs are assumed to be sorted in the same order as |rtp_modules|.
  for (uint32_t ssrc : ssrcs) {
    // Restore state if it previously existed.
    const RtpPayloadState* state = nullptr;
    auto it = states.find(ssrc);
    if (it != states.end()) {
      state = &it->second;
      shared_frame_id_ = std::max(shared_frame_id_, state->shared_frame_id);
    }
    params_.push_back(RtpPayloadParams(ssrc, state));
  }

  // RTP/RTCP initialization.

  // We add the highest spatial layer first to ensure it'll be prioritized
  // when sending padding, with the hope that the packet rate will be smaller,
  // and that it's more important to protect than the lower layers.

  // TODO(nisse): Consider moving registration with PacketRouter last, after the
  // modules are fully configured.
  for (auto& rtp_rtcp : rtp_modules_) {
    constexpr bool remb_candidate = true;
    transport->packet_router()->AddSendRtpModule(rtp_rtcp.get(),
                                                 remb_candidate);
  }

  for (size_t i = 0; i < rtp_config_.extensions.size(); ++i) {
    const std::string& extension = rtp_config_.extensions[i].uri;
    int id = rtp_config_.extensions[i].id;
    // One-byte-extension local identifiers are in the range 1-14 inclusive.
    RTC_DCHECK_GE(id, 1);
    RTC_DCHECK_LE(id, 14);
    RTC_DCHECK(RtpExtension::IsSupportedForVideo(extension));
    for (auto& rtp_rtcp : rtp_modules_) {
      RTC_CHECK_EQ(0, rtp_rtcp->RegisterSendRtpHeaderExtension(
                          StringToRtpExtensionType(extension), id));
    }
  }

  ConfigureProtection(rtp_config);
  ConfigureSsrcs(rtp_config);

  if (!rtp_config.mid.empty()) {
    for (auto& rtp_rtcp : rtp_modules_) {
      rtp_rtcp->SetMid(rtp_config.mid);
    }
  }

  // TODO(pbos): Should we set CNAME on all RTP modules?
  rtp_modules_.front()->SetCNAME(rtp_config.c_name.c_str());

  for (auto& rtp_rtcp : rtp_modules_) {
    rtp_rtcp->RegisterRtcpStatisticsCallback(observers.rtcp_stats);
    rtp_rtcp->RegisterSendChannelRtpStatisticsCallback(observers.rtp_stats);
    rtp_rtcp->SetMaxRtpPacketSize(rtp_config.max_packet_size);
    rtp_rtcp->RegisterVideoSendPayload(rtp_config.payload_type,
                                       rtp_config.payload_name.c_str());
  }
}

RtpVideoSender::~RtpVideoSender() {
  for (auto& rtp_rtcp : rtp_modules_) {
    transport_->packet_router()->RemoveSendRtpModule(rtp_rtcp.get());
  }
}

void RtpVideoSender::RegisterProcessThread(
    ProcessThread* module_process_thread) {
  RTC_DCHECK_RUN_ON(&module_process_thread_checker_);
  RTC_DCHECK(!module_process_thread_);
  module_process_thread_ = module_process_thread;

  for (auto& rtp_rtcp : rtp_modules_)
    module_process_thread_->RegisterModule(rtp_rtcp.get(), RTC_FROM_HERE);
}

void RtpVideoSender::DeRegisterProcessThread() {
  RTC_DCHECK_RUN_ON(&module_process_thread_checker_);
  for (auto& rtp_rtcp : rtp_modules_)
    module_process_thread_->DeRegisterModule(rtp_rtcp.get());
}

void RtpVideoSender::SetActive(bool active) {
  rtc::CritScope lock(&crit_);
  if (active_ == active)
    return;
  const std::vector<bool> active_modules(rtp_modules_.size(), active);
  SetActiveModules(active_modules);
}

void RtpVideoSender::SetActiveModules(const std::vector<bool> active_modules) {
  rtc::CritScope lock(&crit_);
  RTC_DCHECK_EQ(rtp_modules_.size(), active_modules.size());
  active_ = false;
  for (size_t i = 0; i < active_modules.size(); ++i) {
    if (active_modules[i]) {
      active_ = true;
    }
    // Sends a kRtcpByeCode when going from true to false.
    rtp_modules_[i]->SetSendingStatus(active_modules[i]);
    // If set to false this module won't send media.
    rtp_modules_[i]->SetSendingMediaStatus(active_modules[i]);
  }
}

bool RtpVideoSender::IsActive() {
  rtc::CritScope lock(&crit_);
  return active_ && !rtp_modules_.empty();
}

EncodedImageCallback::Result RtpVideoSender::OnEncodedImage(
    const EncodedImage& encoded_image,
    const CodecSpecificInfo* codec_specific_info,
    const RTPFragmentationHeader* fragmentation) {
  rtc::CritScope lock(&crit_);
  RTC_DCHECK(!rtp_modules_.empty());
  if (!active_)
    return Result(Result::ERROR_SEND_FAILED);

  size_t stream_index = GetSimulcastIdx(codec_specific_info).value_or(0);
  RTC_DCHECK_LT(stream_index, rtp_modules_.size());
  RTPVideoHeader rtp_video_header = params_[stream_index].GetRtpVideoHeader(
      encoded_image, codec_specific_info);

  uint32_t frame_id;
  if (!rtp_modules_[stream_index]->Sending()) {
    // The payload router could be active but this module isn't sending.
    return Result(Result::ERROR_SEND_FAILED);
  }
  bool send_result = rtp_modules_[stream_index]->SendOutgoingData(
      encoded_image._frameType, rtp_config_.payload_type,
      encoded_image._timeStamp, encoded_image.capture_time_ms_,
      encoded_image._buffer, encoded_image._length, fragmentation,
      &rtp_video_header, &frame_id);
  if (!send_result)
    return Result(Result::ERROR_SEND_FAILED);

  return Result(Result::OK, frame_id);
}

void RtpVideoSender::OnBitrateAllocationUpdated(
    const VideoBitrateAllocation& bitrate) {
  rtc::CritScope lock(&crit_);
  if (IsActive()) {
    if (rtp_modules_.size() == 1) {
      // If spatial scalability is enabled, it is covered by a single stream.
      rtp_modules_[0]->SetVideoBitrateAllocation(bitrate);
    } else {
      std::vector<absl::optional<VideoBitrateAllocation>> layer_bitrates =
          bitrate.GetSimulcastAllocations();
      // Simulcast is in use, split the VideoBitrateAllocation into one struct
      // per rtp stream, moving over the temporal layer allocation.
      for (size_t i = 0; i < rtp_modules_.size(); ++i) {
        // The next spatial layer could be used if the current one is
        // inactive.
        if (layer_bitrates[i]) {
          rtp_modules_[i]->SetVideoBitrateAllocation(*layer_bitrates[i]);
        }
      }
    }
  }
}

void RtpVideoSender::ConfigureProtection(const RtpConfig& rtp_config) {
  // Consistency of FlexFEC parameters is checked in MaybeCreateFlexfecSender.
  const bool flexfec_enabled = (flexfec_sender_ != nullptr);

  // Consistency of NACK and RED+ULPFEC parameters is checked in this function.
  const bool nack_enabled = rtp_config.nack.rtp_history_ms > 0;
  int red_payload_type = rtp_config.ulpfec.red_payload_type;
  int ulpfec_payload_type = rtp_config.ulpfec.ulpfec_payload_type;

  // Shorthands.
  auto IsRedEnabled = [&]() { return red_payload_type >= 0; };
  auto IsUlpfecEnabled = [&]() { return ulpfec_payload_type >= 0; };
  auto DisableRedAndUlpfec = [&]() {
    red_payload_type = -1;
    ulpfec_payload_type = -1;
  };

  if (webrtc::field_trial::IsEnabled("WebRTC-DisableUlpFecExperiment")) {
    RTC_LOG(LS_INFO) << "Experiment to disable sending ULPFEC is enabled.";
    DisableRedAndUlpfec();
  }

  // If enabled, FlexFEC takes priority over RED+ULPFEC.
  if (flexfec_enabled) {
    if (IsUlpfecEnabled()) {
      RTC_LOG(LS_INFO)
          << "Both FlexFEC and ULPFEC are configured. Disabling ULPFEC.";
    }
    DisableRedAndUlpfec();
  }

  // Payload types without picture ID cannot determine that a stream is complete
  // without retransmitting FEC, so using ULPFEC + NACK for H.264 (for instance)
  // is a waste of bandwidth since FEC packets still have to be transmitted.
  // Note that this is not the case with FlexFEC.
  if (nack_enabled && IsUlpfecEnabled() &&
      !PayloadTypeSupportsSkippingFecPackets(rtp_config.payload_name)) {
    RTC_LOG(LS_WARNING)
        << "Transmitting payload type without picture ID using "
           "NACK+ULPFEC is a waste of bandwidth since ULPFEC packets "
           "also have to be retransmitted. Disabling ULPFEC.";
    DisableRedAndUlpfec();
  }

  // Verify payload types.
  if (IsUlpfecEnabled() ^ IsRedEnabled()) {
    RTC_LOG(LS_WARNING)
        << "Only RED or only ULPFEC enabled, but not both. Disabling both.";
    DisableRedAndUlpfec();
  }

  for (auto& rtp_rtcp : rtp_modules_) {
    // Set NACK.
    rtp_rtcp->SetStorePacketsStatus(true, kMinSendSidePacketHistorySize);
    // Set RED/ULPFEC information.
    rtp_rtcp->SetUlpfecConfig(red_payload_type, ulpfec_payload_type);
  }
}

bool RtpVideoSender::FecEnabled() const {
  const bool flexfec_enabled = (flexfec_sender_ != nullptr);
  int ulpfec_payload_type = rtp_config_.ulpfec.ulpfec_payload_type;
  return flexfec_enabled || ulpfec_payload_type >= 0;
}

bool RtpVideoSender::NackEnabled() const {
  const bool nack_enabled = rtp_config_.nack.rtp_history_ms > 0;
  return nack_enabled;
}

void RtpVideoSender::DeliverRtcp(const uint8_t* packet, size_t length) {
  // Runs on a network thread.
  for (auto& rtp_rtcp : rtp_modules_)
    rtp_rtcp->IncomingRtcpPacket(packet, length);
}

void RtpVideoSender::ProtectionRequest(const FecProtectionParams* delta_params,
                                       const FecProtectionParams* key_params,
                                       uint32_t* sent_video_rate_bps,
                                       uint32_t* sent_nack_rate_bps,
                                       uint32_t* sent_fec_rate_bps) {
  *sent_video_rate_bps = 0;
  *sent_nack_rate_bps = 0;
  *sent_fec_rate_bps = 0;
  for (auto& rtp_rtcp : rtp_modules_) {
    uint32_t not_used = 0;
    uint32_t module_video_rate = 0;
    uint32_t module_fec_rate = 0;
    uint32_t module_nack_rate = 0;
    rtp_rtcp->SetFecParameters(*delta_params, *key_params);
    rtp_rtcp->BitrateSent(&not_used, &module_video_rate, &module_fec_rate,
                          &module_nack_rate);
    *sent_video_rate_bps += module_video_rate;
    *sent_nack_rate_bps += module_nack_rate;
    *sent_fec_rate_bps += module_fec_rate;
  }
}

void RtpVideoSender::SetMaxRtpPacketSize(size_t max_rtp_packet_size) {
  for (auto& rtp_rtcp : rtp_modules_) {
    rtp_rtcp->SetMaxRtpPacketSize(max_rtp_packet_size);
  }
}

void RtpVideoSender::ConfigureSsrcs(const RtpConfig& rtp_config) {
  // Configure regular SSRCs.
  for (size_t i = 0; i < rtp_config.ssrcs.size(); ++i) {
    uint32_t ssrc = rtp_config.ssrcs[i];
    RtpRtcp* const rtp_rtcp = rtp_modules_[i].get();
    rtp_rtcp->SetSSRC(ssrc);

    // Restore RTP state if previous existed.
    auto it = suspended_ssrcs_.find(ssrc);
    if (it != suspended_ssrcs_.end())
      rtp_rtcp->SetRtpState(it->second);
  }

  // Set up RTX if available.
  if (rtp_config.rtx.ssrcs.empty())
    return;

  // Configure RTX SSRCs.
  RTC_DCHECK_EQ(rtp_config.rtx.ssrcs.size(), rtp_config.ssrcs.size());
  for (size_t i = 0; i < rtp_config.rtx.ssrcs.size(); ++i) {
    uint32_t ssrc = rtp_config.rtx.ssrcs[i];
    RtpRtcp* const rtp_rtcp = rtp_modules_[i].get();
    rtp_rtcp->SetRtxSsrc(ssrc);
    auto it = suspended_ssrcs_.find(ssrc);
    if (it != suspended_ssrcs_.end())
      rtp_rtcp->SetRtxState(it->second);
  }

  // Configure RTX payload types.
  RTC_DCHECK_GE(rtp_config.rtx.payload_type, 0);
  for (auto& rtp_rtcp : rtp_modules_) {
    rtp_rtcp->SetRtxSendPayloadType(rtp_config.rtx.payload_type,
                                    rtp_config.payload_type);
    rtp_rtcp->SetRtxSendStatus(kRtxRetransmitted | kRtxRedundantPayloads);
  }
  if (rtp_config.ulpfec.red_payload_type != -1 &&
      rtp_config.ulpfec.red_rtx_payload_type != -1) {
    for (auto& rtp_rtcp : rtp_modules_) {
      rtp_rtcp->SetRtxSendPayloadType(rtp_config.ulpfec.red_rtx_payload_type,
                                      rtp_config.ulpfec.red_payload_type);
    }
  }
}

void RtpVideoSender::OnNetworkAvailability(bool network_available) {
  for (auto& rtp_rtcp : rtp_modules_) {
    rtp_rtcp->SetRTCPStatus(network_available ? rtp_config_.rtcp_mode
                                              : RtcpMode::kOff);
  }
}

std::map<uint32_t, RtpState> RtpVideoSender::GetRtpStates() const {
  std::map<uint32_t, RtpState> rtp_states;

  for (size_t i = 0; i < rtp_config_.ssrcs.size(); ++i) {
    uint32_t ssrc = rtp_config_.ssrcs[i];
    RTC_DCHECK_EQ(ssrc, rtp_modules_[i]->SSRC());
    rtp_states[ssrc] = rtp_modules_[i]->GetRtpState();
  }

  for (size_t i = 0; i < rtp_config_.rtx.ssrcs.size(); ++i) {
    uint32_t ssrc = rtp_config_.rtx.ssrcs[i];
    rtp_states[ssrc] = rtp_modules_[i]->GetRtxState();
  }

  if (flexfec_sender_) {
    uint32_t ssrc = rtp_config_.flexfec.ssrc;
    rtp_states[ssrc] = flexfec_sender_->GetRtpState();
  }

  return rtp_states;
}

std::map<uint32_t, RtpPayloadState> RtpVideoSender::GetRtpPayloadStates()
    const {
  rtc::CritScope lock(&crit_);
  std::map<uint32_t, RtpPayloadState> payload_states;
  for (const auto& param : params_) {
    payload_states[param.ssrc()] = param.state();
    payload_states[param.ssrc()].shared_frame_id = shared_frame_id_;
  }
  return payload_states;
}
}  // namespace webrtc
