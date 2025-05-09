/*
 *  Copyright (c) 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_net__dcsctp__socket__dcsctp_socket_h //original-code:"net/dcsctp/socket/dcsctp_socket.h"

#include <algorithm>
#include <cstdint>
#include <limits>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__abort_chunk_h //original-code:"net/dcsctp/packet/chunk/abort_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__chunk_h //original-code:"net/dcsctp/packet/chunk/chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__cookie_ack_chunk_h //original-code:"net/dcsctp/packet/chunk/cookie_ack_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__cookie_echo_chunk_h //original-code:"net/dcsctp/packet/chunk/cookie_echo_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__data_chunk_h //original-code:"net/dcsctp/packet/chunk/data_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__data_common_h //original-code:"net/dcsctp/packet/chunk/data_common.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__error_chunk_h //original-code:"net/dcsctp/packet/chunk/error_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__forward_tsn_chunk_h //original-code:"net/dcsctp/packet/chunk/forward_tsn_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__forward_tsn_common_h //original-code:"net/dcsctp/packet/chunk/forward_tsn_common.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__heartbeat_ack_chunk_h //original-code:"net/dcsctp/packet/chunk/heartbeat_ack_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__heartbeat_request_chunk_h //original-code:"net/dcsctp/packet/chunk/heartbeat_request_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__idata_chunk_h //original-code:"net/dcsctp/packet/chunk/idata_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__iforward_tsn_chunk_h //original-code:"net/dcsctp/packet/chunk/iforward_tsn_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__init_ack_chunk_h //original-code:"net/dcsctp/packet/chunk/init_ack_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__init_chunk_h //original-code:"net/dcsctp/packet/chunk/init_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__reconfig_chunk_h //original-code:"net/dcsctp/packet/chunk/reconfig_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__sack_chunk_h //original-code:"net/dcsctp/packet/chunk/sack_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__shutdown_ack_chunk_h //original-code:"net/dcsctp/packet/chunk/shutdown_ack_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__shutdown_chunk_h //original-code:"net/dcsctp/packet/chunk/shutdown_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk__shutdown_complete_chunk_h //original-code:"net/dcsctp/packet/chunk/shutdown_complete_chunk.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__chunk_validators_h //original-code:"net/dcsctp/packet/chunk_validators.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__data_h //original-code:"net/dcsctp/packet/data.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__cookie_received_while_shutting_down_cause_h //original-code:"net/dcsctp/packet/error_cause/cookie_received_while_shutting_down_cause.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__error_cause_h //original-code:"net/dcsctp/packet/error_cause/error_cause.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__no_user_data_cause_h //original-code:"net/dcsctp/packet/error_cause/no_user_data_cause.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__out_of_resource_error_cause_h //original-code:"net/dcsctp/packet/error_cause/out_of_resource_error_cause.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__protocol_violation_cause_h //original-code:"net/dcsctp/packet/error_cause/protocol_violation_cause.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__unrecognized_chunk_type_cause_h //original-code:"net/dcsctp/packet/error_cause/unrecognized_chunk_type_cause.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__error_cause__user_initiated_abort_cause_h //original-code:"net/dcsctp/packet/error_cause/user_initiated_abort_cause.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__parameter__forward_tsn_supported_parameter_h //original-code:"net/dcsctp/packet/parameter/forward_tsn_supported_parameter.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__parameter__parameter_h //original-code:"net/dcsctp/packet/parameter/parameter.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__parameter__state_cookie_parameter_h //original-code:"net/dcsctp/packet/parameter/state_cookie_parameter.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__parameter__supported_extensions_parameter_h //original-code:"net/dcsctp/packet/parameter/supported_extensions_parameter.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__sctp_packet_h //original-code:"net/dcsctp/packet/sctp_packet.h"
#include BOSS_WEBRTC_U_net__dcsctp__packet__tlv_trait_h //original-code:"net/dcsctp/packet/tlv_trait.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__dcsctp_message_h //original-code:"net/dcsctp/public/dcsctp_message.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__dcsctp_options_h //original-code:"net/dcsctp/public/dcsctp_options.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__dcsctp_socket_h //original-code:"net/dcsctp/public/dcsctp_socket.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__packet_observer_h //original-code:"net/dcsctp/public/packet_observer.h"
#include BOSS_WEBRTC_U_net__dcsctp__rx__data_tracker_h //original-code:"net/dcsctp/rx/data_tracker.h"
#include BOSS_WEBRTC_U_net__dcsctp__rx__reassembly_queue_h //original-code:"net/dcsctp/rx/reassembly_queue.h"
#include BOSS_WEBRTC_U_net__dcsctp__socket__callback_deferrer_h //original-code:"net/dcsctp/socket/callback_deferrer.h"
#include BOSS_WEBRTC_U_net__dcsctp__socket__capabilities_h //original-code:"net/dcsctp/socket/capabilities.h"
#include BOSS_WEBRTC_U_net__dcsctp__socket__heartbeat_handler_h //original-code:"net/dcsctp/socket/heartbeat_handler.h"
#include BOSS_WEBRTC_U_net__dcsctp__socket__state_cookie_h //original-code:"net/dcsctp/socket/state_cookie.h"
#include BOSS_WEBRTC_U_net__dcsctp__socket__stream_reset_handler_h //original-code:"net/dcsctp/socket/stream_reset_handler.h"
#include BOSS_WEBRTC_U_net__dcsctp__socket__transmission_control_block_h //original-code:"net/dcsctp/socket/transmission_control_block.h"
#include BOSS_WEBRTC_U_net__dcsctp__timer__timer_h //original-code:"net/dcsctp/timer/timer.h"
#include BOSS_WEBRTC_U_net__dcsctp__tx__retransmission_queue_h //original-code:"net/dcsctp/tx/retransmission_queue.h"
#include BOSS_WEBRTC_U_net__dcsctp__tx__send_queue_h //original-code:"net/dcsctp/tx/send_queue.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_format_h //original-code:"rtc_base/strings/string_format.h"

namespace dcsctp {
namespace {

// https://tools.ietf.org/html/rfc4960#section-5.1
constexpr uint32_t kMinVerificationTag = 1;
constexpr uint32_t kMaxVerificationTag = std::numeric_limits<uint32_t>::max();

// https://tools.ietf.org/html/rfc4960#section-3.3.2
constexpr uint32_t kMinInitialTsn = 0;
constexpr uint32_t kMaxInitialTsn = std::numeric_limits<uint32_t>::max();

Capabilities GetCapabilities(const DcSctpOptions& options,
                             const Parameters& parameters) {
  Capabilities capabilities;
  absl::optional<SupportedExtensionsParameter> supported_extensions =
      parameters.get<SupportedExtensionsParameter>();

  if (options.enable_partial_reliability) {
    capabilities.partial_reliability =
        parameters.get<ForwardTsnSupportedParameter>().has_value();
    if (supported_extensions.has_value()) {
      capabilities.partial_reliability |=
          supported_extensions->supports(ForwardTsnChunk::kType);
    }
  }

  if (options.enable_message_interleaving && supported_extensions.has_value()) {
    capabilities.message_interleaving =
        supported_extensions->supports(IDataChunk::kType) &&
        supported_extensions->supports(IForwardTsnChunk::kType);
  }
  if (supported_extensions.has_value() &&
      supported_extensions->supports(ReConfigChunk::kType)) {
    capabilities.reconfig = true;
  }
  return capabilities;
}

void AddCapabilityParameters(const DcSctpOptions& options,
                             Parameters::Builder& builder) {
  std::vector<uint8_t> chunk_types = {ReConfigChunk::kType};

  if (options.enable_partial_reliability) {
    builder.Add(ForwardTsnSupportedParameter());
    chunk_types.push_back(ForwardTsnChunk::kType);
  }
  if (options.enable_message_interleaving) {
    chunk_types.push_back(IDataChunk::kType);
    chunk_types.push_back(IForwardTsnChunk::kType);
  }
  builder.Add(SupportedExtensionsParameter(std::move(chunk_types)));
}

TieTag MakeTieTag(DcSctpSocketCallbacks& cb) {
  uint32_t tie_tag_upper =
      cb.GetRandomInt(0, std::numeric_limits<uint32_t>::max());
  uint32_t tie_tag_lower =
      cb.GetRandomInt(1, std::numeric_limits<uint32_t>::max());
  return TieTag(static_cast<uint64_t>(tie_tag_upper) << 32 |
                static_cast<uint64_t>(tie_tag_lower));
}

}  // namespace

DcSctpSocket::DcSctpSocket(absl::string_view log_prefix,
                           DcSctpSocketCallbacks& callbacks,
                           std::unique_ptr<PacketObserver> packet_observer,
                           const DcSctpOptions& options)
    : log_prefix_(std::string(log_prefix) + ": "),
      packet_observer_(std::move(packet_observer)),
      options_(options),
      callbacks_(callbacks),
      timer_manager_([this]() { return callbacks_.CreateTimeout(); }),
      t1_init_(timer_manager_.CreateTimer(
          "t1-init",
          [this]() { return OnInitTimerExpiry(); },
          TimerOptions(options.t1_init_timeout,
                       TimerBackoffAlgorithm::kExponential,
                       options.max_init_retransmits))),
      t1_cookie_(timer_manager_.CreateTimer(
          "t1-cookie",
          [this]() { return OnCookieTimerExpiry(); },
          TimerOptions(options.t1_cookie_timeout,
                       TimerBackoffAlgorithm::kExponential,
                       options.max_init_retransmits))),
      t2_shutdown_(timer_manager_.CreateTimer(
          "t2-shutdown",
          [this]() { return OnShutdownTimerExpiry(); },
          TimerOptions(options.t2_shutdown_timeout,
                       TimerBackoffAlgorithm::kExponential,
                       options.max_retransmissions))),
      send_queue_(
          log_prefix_,
          options_.max_send_buffer_size,
          [this](StreamID stream_id) {
            callbacks_.OnBufferedAmountLow(stream_id);
          },
          options_.total_buffered_amount_low_threshold,
          [this]() { callbacks_.OnTotalBufferedAmountLow(); }) {}

std::string DcSctpSocket::log_prefix() const {
  return log_prefix_ + "[" + std::string(ToString(state_)) + "] ";
}

bool DcSctpSocket::IsConsistent() const {
  switch (state_) {
    case State::kClosed:
      return (tcb_ == nullptr && !t1_init_->is_running() &&
              !t1_cookie_->is_running() && !t2_shutdown_->is_running());
    case State::kCookieWait:
      return (tcb_ == nullptr && t1_init_->is_running() &&
              !t1_cookie_->is_running() && !t2_shutdown_->is_running());
    case State::kCookieEchoed:
      return (tcb_ != nullptr && !t1_init_->is_running() &&
              t1_cookie_->is_running() && !t2_shutdown_->is_running() &&
              cookie_echo_chunk_.has_value());
    case State::kEstablished:
      return (tcb_ != nullptr && !t1_init_->is_running() &&
              !t1_cookie_->is_running() && !t2_shutdown_->is_running());
    case State::kShutdownPending:
      return (tcb_ != nullptr && !t1_init_->is_running() &&
              !t1_cookie_->is_running() && !t2_shutdown_->is_running());
    case State::kShutdownSent:
      return (tcb_ != nullptr && !t1_init_->is_running() &&
              !t1_cookie_->is_running() && t2_shutdown_->is_running());
    case State::kShutdownReceived:
      return (tcb_ != nullptr && !t1_init_->is_running() &&
              !t1_cookie_->is_running() && !t2_shutdown_->is_running());
    case State::kShutdownAckSent:
      return (tcb_ != nullptr && !t1_init_->is_running() &&
              !t1_cookie_->is_running() && t2_shutdown_->is_running());
  }
}

constexpr absl::string_view DcSctpSocket::ToString(DcSctpSocket::State state) {
  switch (state) {
    case DcSctpSocket::State::kClosed:
      return "CLOSED";
    case DcSctpSocket::State::kCookieWait:
      return "COOKIE_WAIT";
    case DcSctpSocket::State::kCookieEchoed:
      return "COOKIE_ECHOED";
    case DcSctpSocket::State::kEstablished:
      return "ESTABLISHED";
    case DcSctpSocket::State::kShutdownPending:
      return "SHUTDOWN_PENDING";
    case DcSctpSocket::State::kShutdownSent:
      return "SHUTDOWN_SENT";
    case DcSctpSocket::State::kShutdownReceived:
      return "SHUTDOWN_RECEIVED";
    case DcSctpSocket::State::kShutdownAckSent:
      return "SHUTDOWN_ACK_SENT";
  }
}

void DcSctpSocket::SetState(State state, absl::string_view reason) {
  if (state_ != state) {
    RTC_DLOG(LS_VERBOSE) << log_prefix_ << "Socket state changed from "
                         << ToString(state_) << " to " << ToString(state)
                         << " due to " << reason;
    state_ = state;
  }
}

void DcSctpSocket::SendInit() {
  Parameters::Builder params_builder;
  AddCapabilityParameters(options_, params_builder);
  InitChunk init(/*initiate_tag=*/connect_params_.verification_tag,
                 /*a_rwnd=*/options_.max_receiver_window_buffer_size,
                 options_.announced_maximum_outgoing_streams,
                 options_.announced_maximum_incoming_streams,
                 connect_params_.initial_tsn, params_builder.Build());
  SctpPacket::Builder b(VerificationTag(0), options_);
  b.Add(init);
  SendPacket(b);
}

void DcSctpSocket::MakeConnectionParameters() {
  VerificationTag new_verification_tag(
      callbacks_.GetRandomInt(kMinVerificationTag, kMaxVerificationTag));
  TSN initial_tsn(callbacks_.GetRandomInt(kMinInitialTsn, kMaxInitialTsn));
  connect_params_.initial_tsn = initial_tsn;
  connect_params_.verification_tag = new_verification_tag;
}

void DcSctpSocket::Connect() {
  if (state_ == State::kClosed) {
    MakeConnectionParameters();
    RTC_DLOG(LS_INFO)
        << log_prefix()
        << rtc::StringFormat(
               "Connecting. my_verification_tag=%08x, my_initial_tsn=%u",
               *connect_params_.verification_tag, *connect_params_.initial_tsn);
    SendInit();
    t1_init_->Start();
    SetState(State::kCookieWait, "Connect called");
  } else {
    RTC_DLOG(LS_WARNING) << log_prefix()
                         << "Called Connect on a socket that is not closed";
  }
  RTC_DCHECK(IsConsistent());
  callbacks_.TriggerDeferred();
}

void DcSctpSocket::Shutdown() {
  if (tcb_ != nullptr) {
    // https://tools.ietf.org/html/rfc4960#section-9.2
    // "Upon receipt of the SHUTDOWN primitive from its upper layer, the
    // endpoint enters the SHUTDOWN-PENDING state and remains there until all
    // outstanding data has been acknowledged by its peer."

    // TODO(webrtc:12739): Remove this check, as it just hides the problem that
    // the socket can transition from ShutdownSent to ShutdownPending, or
    // ShutdownAckSent to ShutdownPending which is illegal.
    if (state_ != State::kShutdownSent && state_ != State::kShutdownAckSent) {
      SetState(State::kShutdownPending, "Shutdown called");
      t1_init_->Stop();
      t1_cookie_->Stop();
      MaybeSendShutdownOrAck();
    }
  } else {
    // Connection closed before even starting to connect, or during the initial
    // connection phase. There is no outstanding data, so the socket can just
    // be closed (stopping any connection timers, if any), as this is the
    // client's intention, by calling Shutdown.
    InternalClose(ErrorKind::kNoError, "");
  }
  RTC_DCHECK(IsConsistent());
  callbacks_.TriggerDeferred();
}

void DcSctpSocket::Close() {
  if (state_ != State::kClosed) {
    if (tcb_ != nullptr) {
      SctpPacket::Builder b = tcb_->PacketBuilder();
      b.Add(AbortChunk(/*filled_in_verification_tag=*/true,
                       Parameters::Builder()
                           .Add(UserInitiatedAbortCause("Close called"))
                           .Build()));
      SendPacket(b);
    }
    InternalClose(ErrorKind::kNoError, "");
  } else {
    RTC_DLOG(LS_INFO) << log_prefix() << "Called Close on a closed socket";
  }
  RTC_DCHECK(IsConsistent());
  callbacks_.TriggerDeferred();
}

void DcSctpSocket::CloseConnectionBecauseOfTooManyTransmissionErrors() {
  SendPacket(tcb_->PacketBuilder().Add(AbortChunk(
      true, Parameters::Builder()
                .Add(UserInitiatedAbortCause("Too many retransmissions"))
                .Build())));
  InternalClose(ErrorKind::kTooManyRetries, "Too many retransmissions");
}

void DcSctpSocket::InternalClose(ErrorKind error, absl::string_view message) {
  if (state_ != State::kClosed) {
    t1_init_->Stop();
    t1_cookie_->Stop();
    t2_shutdown_->Stop();
    tcb_ = nullptr;
    cookie_echo_chunk_ = absl::nullopt;

    if (error == ErrorKind::kNoError) {
      callbacks_.OnClosed();
    } else {
      callbacks_.OnAborted(error, message);
    }
    SetState(State::kClosed, message);
  }
  // This method's purpose is to abort/close and make it consistent by ensuring
  // that e.g. all timers really are stopped.
  RTC_DCHECK(IsConsistent());
}

SendStatus DcSctpSocket::Send(DcSctpMessage message,
                              const SendOptions& send_options) {
  if (message.payload().empty()) {
    callbacks_.OnError(ErrorKind::kProtocolViolation,
                       "Unable to send empty message");
    return SendStatus::kErrorMessageEmpty;
  }
  if (message.payload().size() > options_.max_message_size) {
    callbacks_.OnError(ErrorKind::kProtocolViolation,
                       "Unable to send too large message");
    return SendStatus::kErrorMessageTooLarge;
  }
  if (state_ == State::kShutdownPending || state_ == State::kShutdownSent ||
      state_ == State::kShutdownReceived || state_ == State::kShutdownAckSent) {
    // https://tools.ietf.org/html/rfc4960#section-9.2
    // "An endpoint should reject any new data request from its upper layer
    // if it is in the SHUTDOWN-PENDING, SHUTDOWN-SENT, SHUTDOWN-RECEIVED, or
    // SHUTDOWN-ACK-SENT state."
    callbacks_.OnError(ErrorKind::kWrongSequence,
                       "Unable to send message as the socket is shutting down");
    return SendStatus::kErrorShuttingDown;
  }
  if (send_queue_.IsFull()) {
    callbacks_.OnError(ErrorKind::kResourceExhaustion,
                       "Unable to send message as the send queue is full");
    return SendStatus::kErrorResourceExhaustion;
  }

  TimeMs now = callbacks_.TimeMillis();
  send_queue_.Add(now, std::move(message), send_options);
  if (tcb_ != nullptr) {
    tcb_->SendBufferedPackets(now);
  }

  RTC_DCHECK(IsConsistent());
  callbacks_.TriggerDeferred();
  return SendStatus::kSuccess;
}

ResetStreamsStatus DcSctpSocket::ResetStreams(
    rtc::ArrayView<const StreamID> outgoing_streams) {
  if (tcb_ == nullptr) {
    callbacks_.OnError(ErrorKind::kWrongSequence,
                       "Can't reset streams as the socket is not connected");
    return ResetStreamsStatus::kNotConnected;
  }
  if (!tcb_->capabilities().reconfig) {
    callbacks_.OnError(ErrorKind::kUnsupportedOperation,
                       "Can't reset streams as the peer doesn't support it");
    return ResetStreamsStatus::kNotSupported;
  }

  tcb_->stream_reset_handler().ResetStreams(outgoing_streams);
  absl::optional<ReConfigChunk> reconfig =
      tcb_->stream_reset_handler().MakeStreamResetRequest();
  if (reconfig.has_value()) {
    SctpPacket::Builder builder = tcb_->PacketBuilder();
    builder.Add(*reconfig);
    SendPacket(builder);
  }

  RTC_DCHECK(IsConsistent());
  callbacks_.TriggerDeferred();
  return ResetStreamsStatus::kPerformed;
}

SocketState DcSctpSocket::state() const {
  switch (state_) {
    case State::kClosed:
      return SocketState::kClosed;
    case State::kCookieWait:
      ABSL_FALLTHROUGH_INTENDED;
    case State::kCookieEchoed:
      return SocketState::kConnecting;
    case State::kEstablished:
      return SocketState::kConnected;
    case State::kShutdownPending:
      ABSL_FALLTHROUGH_INTENDED;
    case State::kShutdownSent:
      ABSL_FALLTHROUGH_INTENDED;
    case State::kShutdownReceived:
      ABSL_FALLTHROUGH_INTENDED;
    case State::kShutdownAckSent:
      return SocketState::kShuttingDown;
  }
}

void DcSctpSocket::SetMaxMessageSize(size_t max_message_size) {
  options_.max_message_size = max_message_size;
}

size_t DcSctpSocket::buffered_amount(StreamID stream_id) const {
  return send_queue_.buffered_amount(stream_id);
}

size_t DcSctpSocket::buffered_amount_low_threshold(StreamID stream_id) const {
  return send_queue_.buffered_amount_low_threshold(stream_id);
}

void DcSctpSocket::SetBufferedAmountLowThreshold(StreamID stream_id,
                                                 size_t bytes) {
  send_queue_.SetBufferedAmountLowThreshold(stream_id, bytes);
}

void DcSctpSocket::MaybeSendShutdownOnPacketReceived(const SctpPacket& packet) {
  if (state_ == State::kShutdownSent) {
    bool has_data_chunk =
        std::find_if(packet.descriptors().begin(), packet.descriptors().end(),
                     [](const SctpPacket::ChunkDescriptor& descriptor) {
                       return descriptor.type == DataChunk::kType;
                     }) != packet.descriptors().end();
    if (has_data_chunk) {
      // https://tools.ietf.org/html/rfc4960#section-9.2
      // "While in the SHUTDOWN-SENT state, the SHUTDOWN sender MUST immediately
      // respond to each received packet containing one or more DATA chunks with
      // a SHUTDOWN chunk and restart the T2-shutdown timer.""
      SendShutdown();
      t2_shutdown_->set_duration(tcb_->current_rto());
      t2_shutdown_->Start();
    }
  }
}

bool DcSctpSocket::ValidatePacket(const SctpPacket& packet) {
  const CommonHeader& header = packet.common_header();
  VerificationTag my_verification_tag =
      tcb_ != nullptr ? tcb_->my_verification_tag() : VerificationTag(0);

  if (header.verification_tag == VerificationTag(0)) {
    if (packet.descriptors().size() == 1 &&
        packet.descriptors()[0].type == InitChunk::kType) {
      // https://tools.ietf.org/html/rfc4960#section-8.5.1
      // "When an endpoint receives an SCTP packet with the Verification Tag
      // set to 0, it should verify that the packet contains only an INIT chunk.
      // Otherwise, the receiver MUST silently discard the packet.""
      return true;
    }
    callbacks_.OnError(
        ErrorKind::kParseFailed,
        "Only a single INIT chunk can be present in packets sent on "
        "verification_tag = 0");
    return false;
  }

  if (packet.descriptors().size() == 1 &&
      packet.descriptors()[0].type == AbortChunk::kType) {
    // https://tools.ietf.org/html/rfc4960#section-8.5.1
    // "The receiver of an ABORT MUST accept the packet if the Verification
    // Tag field of the packet matches its own tag and the T bit is not set OR
    // if it is set to its peer's tag and the T bit is set in the Chunk Flags.
    // Otherwise, the receiver MUST silently discard the packet and take no
    // further action."
    bool t_bit = (packet.descriptors()[0].flags & 0x01) != 0;
    if (t_bit && tcb_ == nullptr) {
      // Can't verify the tag - assume it's okey.
      return true;
    }
    if ((!t_bit && header.verification_tag == my_verification_tag) ||
        (t_bit && header.verification_tag == tcb_->peer_verification_tag())) {
      return true;
    }
    callbacks_.OnError(ErrorKind::kParseFailed,
                       "ABORT chunk verification tag was wrong");
    return false;
  }

  if (packet.descriptors()[0].type == InitAckChunk::kType) {
    if (header.verification_tag == connect_params_.verification_tag) {
      return true;
    }
    callbacks_.OnError(
        ErrorKind::kParseFailed,
        rtc::StringFormat(
            "Packet has invalid verification tag: %08x, expected %08x",
            *header.verification_tag, *connect_params_.verification_tag));
    return false;
  }

  if (packet.descriptors()[0].type == CookieEchoChunk::kType) {
    // Handled in chunk handler (due to RFC 4960, section 5.2.4).
    return true;
  }

  if (packet.descriptors().size() == 1 &&
      packet.descriptors()[0].type == ShutdownCompleteChunk::kType) {
    // https://tools.ietf.org/html/rfc4960#section-8.5.1
    // "The receiver of a SHUTDOWN COMPLETE shall accept the packet if the
    // Verification Tag field of the packet matches its own tag and the T bit is
    // not set OR if it is set to its peer's tag and the T bit is set in the
    // Chunk Flags.  Otherwise, the receiver MUST silently discard the packet
    // and take no further action."
    bool t_bit = (packet.descriptors()[0].flags & 0x01) != 0;
    if (t_bit && tcb_ == nullptr) {
      // Can't verify the tag - assume it's okey.
      return true;
    }
    if ((!t_bit && header.verification_tag == my_verification_tag) ||
        (t_bit && header.verification_tag == tcb_->peer_verification_tag())) {
      return true;
    }
    callbacks_.OnError(ErrorKind::kParseFailed,
                       "SHUTDOWN_COMPLETE chunk verification tag was wrong");
    return false;
  }

  // https://tools.ietf.org/html/rfc4960#section-8.5
  // "When receiving an SCTP packet, the endpoint MUST ensure that the value
  // in the Verification Tag field of the received SCTP packet matches its own
  // tag.  If the received Verification Tag value does not match the receiver's
  // own tag value, the receiver shall silently discard the packet and shall not
  // process it any further..."
  if (header.verification_tag == my_verification_tag) {
    return true;
  }

  callbacks_.OnError(
      ErrorKind::kParseFailed,
      rtc::StringFormat(
          "Packet has invalid verification tag: %08x, expected %08x",
          *header.verification_tag, *my_verification_tag));
  return false;
}

void DcSctpSocket::HandleTimeout(TimeoutID timeout_id) {
  timer_manager_.HandleTimeout(timeout_id);

  if (tcb_ != nullptr && tcb_->HasTooManyTxErrors()) {
    // Tearing down the TCB has to be done outside the handlers.
    CloseConnectionBecauseOfTooManyTransmissionErrors();
  }

  RTC_DCHECK(IsConsistent());
  callbacks_.TriggerDeferred();
}

void DcSctpSocket::ReceivePacket(rtc::ArrayView<const uint8_t> data) {
  if (packet_observer_ != nullptr) {
    packet_observer_->OnReceivedPacket(callbacks_.TimeMillis(), data);
  }

  absl::optional<SctpPacket> packet =
      SctpPacket::Parse(data, options_.disable_checksum_verification);
  if (!packet.has_value()) {
    // https://tools.ietf.org/html/rfc4960#section-6.8
    // "The default procedure for handling invalid SCTP packets is to
    // silently discard them."
    callbacks_.OnError(ErrorKind::kParseFailed,
                       "Failed to parse received SCTP packet");
    RTC_DCHECK(IsConsistent());
    callbacks_.TriggerDeferred();
    return;
  }

  if (RTC_DLOG_IS_ON) {
    for (const auto& descriptor : packet->descriptors()) {
      RTC_DLOG(LS_VERBOSE) << log_prefix() << "Received "
                           << DebugConvertChunkToString(descriptor.data);
    }
  }

  if (!ValidatePacket(*packet)) {
    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Packet failed verification tag check - dropping";
    RTC_DCHECK(IsConsistent());
    callbacks_.TriggerDeferred();
    return;
  }

  MaybeSendShutdownOnPacketReceived(*packet);

  for (const auto& descriptor : packet->descriptors()) {
    if (!Dispatch(packet->common_header(), descriptor)) {
      break;
    }
  }

  if (tcb_ != nullptr) {
    tcb_->data_tracker().ObservePacketEnd();
    tcb_->MaybeSendSack();
  }

  RTC_DCHECK(IsConsistent());
  callbacks_.TriggerDeferred();
}

void DcSctpSocket::DebugPrintOutgoing(rtc::ArrayView<const uint8_t> payload) {
  auto packet = SctpPacket::Parse(payload);
  RTC_DCHECK(packet.has_value());

  for (const auto& desc : packet->descriptors()) {
    RTC_DLOG(LS_VERBOSE) << log_prefix() << "Sent "
                         << DebugConvertChunkToString(desc.data);
  }
}

bool DcSctpSocket::Dispatch(const CommonHeader& header,
                            const SctpPacket::ChunkDescriptor& descriptor) {
  switch (descriptor.type) {
    case DataChunk::kType:
      HandleData(header, descriptor);
      break;
    case InitChunk::kType:
      HandleInit(header, descriptor);
      break;
    case InitAckChunk::kType:
      HandleInitAck(header, descriptor);
      break;
    case SackChunk::kType:
      HandleSack(header, descriptor);
      break;
    case HeartbeatRequestChunk::kType:
      HandleHeartbeatRequest(header, descriptor);
      break;
    case HeartbeatAckChunk::kType:
      HandleHeartbeatAck(header, descriptor);
      break;
    case AbortChunk::kType:
      HandleAbort(header, descriptor);
      break;
    case ErrorChunk::kType:
      HandleError(header, descriptor);
      break;
    case CookieEchoChunk::kType:
      HandleCookieEcho(header, descriptor);
      break;
    case CookieAckChunk::kType:
      HandleCookieAck(header, descriptor);
      break;
    case ShutdownChunk::kType:
      HandleShutdown(header, descriptor);
      break;
    case ShutdownAckChunk::kType:
      HandleShutdownAck(header, descriptor);
      break;
    case ShutdownCompleteChunk::kType:
      HandleShutdownComplete(header, descriptor);
      break;
    case ReConfigChunk::kType:
      HandleReconfig(header, descriptor);
      break;
    case ForwardTsnChunk::kType:
      HandleForwardTsn(header, descriptor);
      break;
    case IDataChunk::kType:
      HandleIData(header, descriptor);
      break;
    case IForwardTsnChunk::kType:
      HandleForwardTsn(header, descriptor);
      break;
    default:
      return HandleUnrecognizedChunk(descriptor);
  }
  return true;
}

bool DcSctpSocket::HandleUnrecognizedChunk(
    const SctpPacket::ChunkDescriptor& descriptor) {
  bool report_as_error = (descriptor.type & 0x40) != 0;
  bool continue_processing = (descriptor.type & 0x80) != 0;
  RTC_DLOG(LS_VERBOSE) << log_prefix() << "Received unknown chunk: "
                       << static_cast<int>(descriptor.type);
  if (report_as_error) {
    rtc::StringBuilder sb;
    sb << "Received unknown chunk of type: "
       << static_cast<int>(descriptor.type) << " with report-error bit set";
    callbacks_.OnError(ErrorKind::kParseFailed, sb.str());
    RTC_DLOG(LS_VERBOSE)
        << log_prefix()
        << "Unknown chunk, with type indicating it should be reported.";

    // https://tools.ietf.org/html/rfc4960#section-3.2
    // "... report in an ERROR chunk using the 'Unrecognized Chunk Type'
    // cause."
    if (tcb_ != nullptr) {
      // Need TCB - this chunk must be sent with a correct verification tag.
      SendPacket(tcb_->PacketBuilder().Add(
          ErrorChunk(Parameters::Builder()
                         .Add(UnrecognizedChunkTypeCause(std::vector<uint8_t>(
                             descriptor.data.begin(), descriptor.data.end())))
                         .Build())));
    }
  }
  if (!continue_processing) {
    // https://tools.ietf.org/html/rfc4960#section-3.2
    // "Stop processing this SCTP packet and discard it, do not process any
    // further chunks within it."
    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Unknown chunk, with type indicating not to "
                            "process any further chunks";
  }

  return continue_processing;
}

absl::optional<DurationMs> DcSctpSocket::OnInitTimerExpiry() {
  RTC_DLOG(LS_VERBOSE) << log_prefix() << "Timer " << t1_init_->name()
                       << " has expired: " << t1_init_->expiration_count()
                       << "/" << t1_init_->options().max_restarts;
  RTC_DCHECK(state_ == State::kCookieWait);

  if (t1_init_->is_running()) {
    SendInit();
  } else {
    InternalClose(ErrorKind::kTooManyRetries, "No INIT_ACK received");
  }
  RTC_DCHECK(IsConsistent());
  return absl::nullopt;
}

absl::optional<DurationMs> DcSctpSocket::OnCookieTimerExpiry() {
  // https://tools.ietf.org/html/rfc4960#section-4
  // "If the T1-cookie timer expires, the endpoint MUST retransmit COOKIE
  // ECHO and restart the T1-cookie timer without changing state.  This MUST
  // be repeated up to 'Max.Init.Retransmits' times. After that, the endpoint
  // MUST abort the initialization process and report the error to the SCTP
  // user."
  RTC_DLOG(LS_VERBOSE) << log_prefix() << "Timer " << t1_cookie_->name()
                       << " has expired: " << t1_cookie_->expiration_count()
                       << "/" << t1_cookie_->options().max_restarts;

  RTC_DCHECK(state_ == State::kCookieEchoed);

  if (t1_cookie_->is_running()) {
    SendCookieEcho();
  } else {
    InternalClose(ErrorKind::kTooManyRetries, "No COOKIE_ACK received");
  }

  RTC_DCHECK(IsConsistent());
  return absl::nullopt;
}

absl::optional<DurationMs> DcSctpSocket::OnShutdownTimerExpiry() {
  RTC_DLOG(LS_VERBOSE) << log_prefix() << "Timer " << t2_shutdown_->name()
                       << " has expired: " << t2_shutdown_->expiration_count()
                       << "/" << t2_shutdown_->options().max_restarts;

  if (!t2_shutdown_->is_running()) {
    // https://tools.ietf.org/html/rfc4960#section-9.2
    // "An endpoint should limit the number of retransmissions of the SHUTDOWN
    // chunk to the protocol parameter 'Association.Max.Retrans'. If this
    // threshold is exceeded, the endpoint should destroy the TCB..."

    SendPacket(tcb_->PacketBuilder().Add(
        AbortChunk(true, Parameters::Builder()
                             .Add(UserInitiatedAbortCause(
                                 "Too many retransmissions of SHUTDOWN"))
                             .Build())));

    InternalClose(ErrorKind::kTooManyRetries, "No SHUTDOWN_ACK received");
    RTC_DCHECK(IsConsistent());
    return absl::nullopt;
  }

  // https://tools.ietf.org/html/rfc4960#section-9.2
  // "If the timer expires, the endpoint must resend the SHUTDOWN with the
  // updated last sequential TSN received from its peer."
  SendShutdown();
  RTC_DCHECK(IsConsistent());
  return tcb_->current_rto();
}

void DcSctpSocket::SendPacket(SctpPacket::Builder& builder) {
  if (builder.empty()) {
    return;
  }

  std::vector<uint8_t> payload = builder.Build();

  if (RTC_DLOG_IS_ON) {
    DebugPrintOutgoing(payload);
  }

  // The heartbeat interval timer is restarted for every sent packet, to
  // fire when the outgoing channel is inactive.
  if (tcb_ != nullptr) {
    tcb_->heartbeat_handler().RestartTimer();
  }

  if (packet_observer_ != nullptr) {
    packet_observer_->OnSentPacket(callbacks_.TimeMillis(), payload);
  }
  callbacks_.SendPacket(payload);
}

bool DcSctpSocket::ValidateHasTCB() {
  if (tcb_ != nullptr) {
    return true;
  }

  callbacks_.OnError(
      ErrorKind::kNotConnected,
      "Received unexpected commands on socket that is not connected");
  return false;
}

void DcSctpSocket::ReportFailedToParseChunk(int chunk_type) {
  rtc::StringBuilder sb;
  sb << "Failed to parse chunk of type: " << chunk_type;
  callbacks_.OnError(ErrorKind::kParseFailed, sb.str());
}

void DcSctpSocket::HandleData(const CommonHeader& header,
                              const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<DataChunk> chunk = DataChunk::Parse(descriptor.data);
  if (ValidateParseSuccess(chunk) && ValidateHasTCB()) {
    HandleDataCommon(*chunk);
  }
}

void DcSctpSocket::HandleIData(const CommonHeader& header,
                               const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<IDataChunk> chunk = IDataChunk::Parse(descriptor.data);
  if (ValidateParseSuccess(chunk) && ValidateHasTCB()) {
    HandleDataCommon(*chunk);
  }
}

void DcSctpSocket::HandleDataCommon(AnyDataChunk& chunk) {
  TSN tsn = chunk.tsn();
  AnyDataChunk::ImmediateAckFlag immediate_ack = chunk.options().immediate_ack;
  Data data = std::move(chunk).extract();

  if (data.payload.empty()) {
    // Empty DATA chunks are illegal.
    SendPacket(tcb_->PacketBuilder().Add(
        ErrorChunk(Parameters::Builder().Add(NoUserDataCause(tsn)).Build())));
    callbacks_.OnError(ErrorKind::kProtocolViolation,
                       "Received DATA chunk with no user data");
    return;
  }

  RTC_DLOG(LS_VERBOSE) << log_prefix() << "Handle DATA, queue_size="
                       << tcb_->reassembly_queue().queued_bytes()
                       << ", water_mark="
                       << tcb_->reassembly_queue().watermark_bytes()
                       << ", full=" << tcb_->reassembly_queue().is_full()
                       << ", above="
                       << tcb_->reassembly_queue().is_above_watermark();

  if (tcb_->reassembly_queue().is_full()) {
    // If the reassembly queue is full, there is nothing that can be done. The
    // specification only allows dropping gap-ack-blocks, and that's not
    // likely to help as the socket has been trying to fill gaps since the
    // watermark was reached.
    SendPacket(tcb_->PacketBuilder().Add(AbortChunk(
        true, Parameters::Builder().Add(OutOfResourceErrorCause()).Build())));
    InternalClose(ErrorKind::kResourceExhaustion,
                  "Reassembly Queue is exhausted");
    return;
  }

  if (tcb_->reassembly_queue().is_above_watermark()) {
    RTC_DLOG(LS_VERBOSE) << log_prefix() << "Is above high watermark";
    // If the reassembly queue is above its high watermark, only accept data
    // chunks that increase its cumulative ack tsn in an attempt to fill gaps
    // to deliver messages.
    if (!tcb_->data_tracker().will_increase_cum_ack_tsn(tsn)) {
      RTC_DLOG(LS_VERBOSE) << log_prefix()
                           << "Rejected data because of exceeding watermark";
      tcb_->data_tracker().ForceImmediateSack();
      return;
    }
  }

  if (!tcb_->data_tracker().IsTSNValid(tsn)) {
    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Rejected data because of failing TSN validity";
    return;
  }

  tcb_->data_tracker().Observe(tsn, immediate_ack);
  tcb_->reassembly_queue().MaybeResetStreamsDeferred(
      tcb_->data_tracker().last_cumulative_acked_tsn());
  tcb_->reassembly_queue().Add(tsn, std::move(data));
  DeliverReassembledMessages();
}

void DcSctpSocket::HandleInit(const CommonHeader& header,
                              const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<InitChunk> chunk = InitChunk::Parse(descriptor.data);
  if (!ValidateParseSuccess(chunk)) {
    return;
  }

  if (chunk->initiate_tag() == VerificationTag(0) ||
      chunk->nbr_outbound_streams() == 0 || chunk->nbr_inbound_streams() == 0) {
    // https://tools.ietf.org/html/rfc4960#section-3.3.2
    // "If the value of the Initiate Tag in a received INIT chunk is found
    // to be 0, the receiver MUST treat it as an error and close the
    // association by transmitting an ABORT."

    // "A receiver of an INIT with the OS value set to 0 SHOULD abort the
    // association."

    // "A receiver of an INIT with the MIS value of 0 SHOULD abort the
    // association."

    SendPacket(SctpPacket::Builder(VerificationTag(0), options_)
                   .Add(AbortChunk(
                       /*filled_in_verification_tag=*/false,
                       Parameters::Builder()
                           .Add(ProtocolViolationCause("INIT malformed"))
                           .Build())));
    InternalClose(ErrorKind::kProtocolViolation, "Received invalid INIT");
    return;
  }

  if (state_ == State::kShutdownAckSent) {
    // https://tools.ietf.org/html/rfc4960#section-9.2
    // "If an endpoint is in the SHUTDOWN-ACK-SENT state and receives an
    // INIT chunk (e.g., if the SHUTDOWN COMPLETE was lost) with source and
    // destination transport addresses (either in the IP addresses or in the
    // INIT chunk) that belong to this association, it should discard the INIT
    // chunk and retransmit the SHUTDOWN ACK chunk."
    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Received Init indicating lost ShutdownComplete";
    SendShutdownAck();
    return;
  }

  TieTag tie_tag(0);
  if (state_ == State::kClosed) {
    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Received Init in closed state (normal)";

    MakeConnectionParameters();
  } else if (state_ == State::kCookieWait || state_ == State::kCookieEchoed) {
    // https://tools.ietf.org/html/rfc4960#section-5.2.1
    // "This usually indicates an initialization collision, i.e., each
    // endpoint is attempting, at about the same time, to establish an
    // association with the other endpoint. Upon receipt of an INIT in the
    // COOKIE-WAIT state, an endpoint MUST respond with an INIT ACK using the
    // same parameters it sent in its original INIT chunk (including its
    // Initiate Tag, unchanged).  When responding, the endpoint MUST send the
    // INIT ACK back to the same address that the original INIT (sent by this
    // endpoint) was sent."
    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Received Init indicating simultaneous connections";
  } else {
    RTC_DCHECK(tcb_ != nullptr);
    // https://tools.ietf.org/html/rfc4960#section-5.2.2
    // "The outbound SCTP packet containing this INIT ACK MUST carry a
    // Verification Tag value equal to the Initiate Tag found in the
    // unexpected INIT.  And the INIT ACK MUST contain a new Initiate Tag
    // (randomly generated; see Section 5.3.1).  Other parameters for the
    // endpoint SHOULD be copied from the existing parameters of the
    // association (e.g., number of outbound streams) into the INIT ACK and
    // cookie."
    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Received Init indicating restarted connection";
    // Create a new verification tag - different from the previous one.
    for (int tries = 0; tries < 10; ++tries) {
      connect_params_.verification_tag = VerificationTag(
          callbacks_.GetRandomInt(kMinVerificationTag, kMaxVerificationTag));
      if (connect_params_.verification_tag != tcb_->my_verification_tag()) {
        break;
      }
    }

    // Make the initial TSN make a large jump, so that there is no overlap
    // with the old and new association.
    connect_params_.initial_tsn =
        TSN(*tcb_->retransmission_queue().next_tsn() + 1000000);
    tie_tag = tcb_->tie_tag();
  }

  RTC_DLOG(LS_VERBOSE)
      << log_prefix()
      << rtc::StringFormat(
             "Proceeding with connection. my_verification_tag=%08x, "
             "my_initial_tsn=%u, peer_verification_tag=%08x, "
             "peer_initial_tsn=%u",
             *connect_params_.verification_tag, *connect_params_.initial_tsn,
             *chunk->initiate_tag(), *chunk->initial_tsn());

  Capabilities capabilities = GetCapabilities(options_, chunk->parameters());

  SctpPacket::Builder b(chunk->initiate_tag(), options_);
  Parameters::Builder params_builder =
      Parameters::Builder().Add(StateCookieParameter(
          StateCookie(chunk->initiate_tag(), chunk->initial_tsn(),
                      chunk->a_rwnd(), tie_tag, capabilities)
              .Serialize()));
  AddCapabilityParameters(options_, params_builder);

  InitAckChunk init_ack(/*initiate_tag=*/connect_params_.verification_tag,
                        options_.max_receiver_window_buffer_size,
                        options_.announced_maximum_outgoing_streams,
                        options_.announced_maximum_incoming_streams,
                        connect_params_.initial_tsn, params_builder.Build());
  b.Add(init_ack);
  SendPacket(b);
}

void DcSctpSocket::SendCookieEcho() {
  RTC_DCHECK(tcb_ != nullptr);
  TimeMs now = callbacks_.TimeMillis();
  SctpPacket::Builder b = tcb_->PacketBuilder();
  b.Add(*cookie_echo_chunk_);

  // https://tools.ietf.org/html/rfc4960#section-5.1
  // "The COOKIE ECHO chunk can be bundled with any pending outbound DATA
  // chunks, but it MUST be the first chunk in the packet and until the COOKIE
  // ACK is returned the sender MUST NOT send any other packets to the peer."
  tcb_->SendBufferedPackets(b, now, /*only_one_packet=*/true);
}

void DcSctpSocket::HandleInitAck(
    const CommonHeader& header,
    const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<InitAckChunk> chunk = InitAckChunk::Parse(descriptor.data);
  if (!ValidateParseSuccess(chunk)) {
    return;
  }

  if (state_ != State::kCookieWait) {
    // https://tools.ietf.org/html/rfc4960#section-5.2.3
    // "If an INIT ACK is received by an endpoint in any state other than
    // the COOKIE-WAIT state, the endpoint should discard the INIT ACK chunk."
    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Received INIT_ACK in unexpected state";
    return;
  }

  auto cookie = chunk->parameters().get<StateCookieParameter>();
  if (!cookie.has_value()) {
    SendPacket(SctpPacket::Builder(connect_params_.verification_tag, options_)
                   .Add(AbortChunk(
                       /*filled_in_verification_tag=*/false,
                       Parameters::Builder()
                           .Add(ProtocolViolationCause("INIT-ACK malformed"))
                           .Build())));
    InternalClose(ErrorKind::kProtocolViolation,
                  "InitAck chunk doesn't contain a cookie");
    return;
  }
  Capabilities capabilities = GetCapabilities(options_, chunk->parameters());
  t1_init_->Stop();

  tcb_ = std::make_unique<TransmissionControlBlock>(
      timer_manager_, log_prefix_, options_, capabilities, callbacks_,
      send_queue_, connect_params_.verification_tag,
      connect_params_.initial_tsn, chunk->initiate_tag(), chunk->initial_tsn(),
      chunk->a_rwnd(), MakeTieTag(callbacks_),
      [this]() { return state_ == State::kEstablished; },
      [this](SctpPacket::Builder& builder) { return SendPacket(builder); });
  RTC_DLOG(LS_VERBOSE) << log_prefix()
                       << "Created peer TCB: " << tcb_->ToString();

  SetState(State::kCookieEchoed, "INIT_ACK received");

  // The connection isn't fully established just yet.
  cookie_echo_chunk_ = CookieEchoChunk(cookie->data());
  SendCookieEcho();
  t1_cookie_->Start();
}

void DcSctpSocket::HandleCookieEcho(
    const CommonHeader& header,
    const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<CookieEchoChunk> chunk =
      CookieEchoChunk::Parse(descriptor.data);
  if (!ValidateParseSuccess(chunk)) {
    return;
  }

  absl::optional<StateCookie> cookie =
      StateCookie::Deserialize(chunk->cookie());
  if (!cookie.has_value()) {
    callbacks_.OnError(ErrorKind::kParseFailed, "Failed to parse state cookie");
    return;
  }

  if (tcb_ != nullptr) {
    if (!HandleCookieEchoWithTCB(header, *cookie)) {
      return;
    }
  } else {
    if (header.verification_tag != connect_params_.verification_tag) {
      callbacks_.OnError(
          ErrorKind::kParseFailed,
          rtc::StringFormat(
              "Received CookieEcho with invalid verification tag: %08x, "
              "expected %08x",
              *header.verification_tag, *connect_params_.verification_tag));
      return;
    }
  }

  // The init timer can be running on simultaneous connections.
  t1_init_->Stop();
  t1_cookie_->Stop();
  if (state_ != State::kEstablished) {
    cookie_echo_chunk_ = absl::nullopt;
    SetState(State::kEstablished, "COOKIE_ECHO received");
    callbacks_.OnConnected();
  }

  if (tcb_ == nullptr) {
    tcb_ = std::make_unique<TransmissionControlBlock>(
        timer_manager_, log_prefix_, options_, cookie->capabilities(),
        callbacks_, send_queue_, connect_params_.verification_tag,
        connect_params_.initial_tsn, cookie->initiate_tag(),
        cookie->initial_tsn(), cookie->a_rwnd(), MakeTieTag(callbacks_),
        [this]() { return state_ == State::kEstablished; },
        [this](SctpPacket::Builder& builder) { return SendPacket(builder); });
    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Created peer TCB: " << tcb_->ToString();
  }

  SctpPacket::Builder b = tcb_->PacketBuilder();
  b.Add(CookieAckChunk());

  // https://tools.ietf.org/html/rfc4960#section-5.1
  // "A COOKIE ACK chunk may be bundled with any pending DATA chunks (and/or
  // SACK chunks), but the COOKIE ACK chunk MUST be the first chunk in the
  // packet."
  tcb_->SendBufferedPackets(b, callbacks_.TimeMillis());
}

bool DcSctpSocket::HandleCookieEchoWithTCB(const CommonHeader& header,
                                           const StateCookie& cookie) {
  RTC_DLOG(LS_VERBOSE) << log_prefix()
                       << "Handling CookieEchoChunk with TCB. local_tag="
                       << *tcb_->my_verification_tag()
                       << ", peer_tag=" << *header.verification_tag
                       << ", tcb_tag=" << *tcb_->peer_verification_tag()
                       << ", cookie_tag=" << *cookie.initiate_tag()
                       << ", local_tie_tag=" << *tcb_->tie_tag()
                       << ", peer_tie_tag=" << *cookie.tie_tag();
  // https://tools.ietf.org/html/rfc4960#section-5.2.4
  // "Handle a COOKIE ECHO when a TCB Exists"
  if (header.verification_tag != tcb_->my_verification_tag() &&
      tcb_->peer_verification_tag() != cookie.initiate_tag() &&
      cookie.tie_tag() == tcb_->tie_tag()) {
    // "A) In this case, the peer may have restarted."
    if (state_ == State::kShutdownAckSent) {
      // "If the endpoint is in the SHUTDOWN-ACK-SENT state and recognizes
      // that the peer has restarted ...  it MUST NOT set up a new association
      // but instead resend the SHUTDOWN ACK and send an ERROR chunk with a
      // "Cookie Received While Shutting Down" error cause to its peer."
      SctpPacket::Builder b(cookie.initiate_tag(), options_);
      b.Add(ShutdownAckChunk());
      b.Add(ErrorChunk(Parameters::Builder()
                           .Add(CookieReceivedWhileShuttingDownCause())
                           .Build()));
      SendPacket(b);
      callbacks_.OnError(ErrorKind::kWrongSequence,
                         "Received COOKIE-ECHO while shutting down");
      return false;
    }

    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Received COOKIE-ECHO indicating a restarted peer";

    // If a message was partly sent, and the peer restarted, resend it in
    // full by resetting the send queue.
    send_queue_.Reset();
    tcb_ = nullptr;
    callbacks_.OnConnectionRestarted();
  } else if (header.verification_tag == tcb_->my_verification_tag() &&
             tcb_->peer_verification_tag() != cookie.initiate_tag()) {
    // TODO(boivie): Handle the peer_tag == 0?
    // "B) In this case, both sides may be attempting to start an
    // association at about the same time, but the peer endpoint started its
    // INIT after responding to the local endpoint's INIT."
    RTC_DLOG(LS_VERBOSE)
        << log_prefix()
        << "Received COOKIE-ECHO indicating simultaneous connections";
    tcb_ = nullptr;
  } else if (header.verification_tag != tcb_->my_verification_tag() &&
             tcb_->peer_verification_tag() == cookie.initiate_tag() &&
             cookie.tie_tag() == TieTag(0)) {
    // "C) In this case, the local endpoint's cookie has arrived late.
    // Before it arrived, the local endpoint sent an INIT and received an
    // INIT ACK and finally sent a COOKIE ECHO with the peer's same tag but
    // a new tag of its own. The cookie should be silently discarded. The
    // endpoint SHOULD NOT change states and should leave any timers
    // running."
    RTC_DLOG(LS_VERBOSE)
        << log_prefix()
        << "Received COOKIE-ECHO indicating a late COOKIE-ECHO. Discarding";
    return false;
  } else if (header.verification_tag == tcb_->my_verification_tag() &&
             tcb_->peer_verification_tag() == cookie.initiate_tag()) {
    // "D) When both local and remote tags match, the endpoint should enter
    // the ESTABLISHED state, if it is in the COOKIE-ECHOED state.  It
    // should stop any cookie timer that may be running and send a COOKIE
    // ACK."
    RTC_DLOG(LS_VERBOSE)
        << log_prefix()
        << "Received duplicate COOKIE-ECHO, probably because of peer not "
           "receiving COOKIE-ACK and retransmitting COOKIE-ECHO. Continuing.";
  }
  return true;
}

void DcSctpSocket::HandleCookieAck(
    const CommonHeader& header,
    const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<CookieAckChunk> chunk = CookieAckChunk::Parse(descriptor.data);
  if (!ValidateParseSuccess(chunk)) {
    return;
  }

  if (state_ != State::kCookieEchoed) {
    // https://tools.ietf.org/html/rfc4960#section-5.2.5
    // "At any state other than COOKIE-ECHOED, an endpoint should silently
    // discard a received COOKIE ACK chunk."
    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Received COOKIE_ACK not in COOKIE_ECHOED state";
    return;
  }

  // RFC 4960, Errata ID: 4400
  t1_cookie_->Stop();
  cookie_echo_chunk_ = absl::nullopt;
  SetState(State::kEstablished, "COOKIE_ACK received");
  tcb_->SendBufferedPackets(callbacks_.TimeMillis());
  callbacks_.OnConnected();
}

void DcSctpSocket::DeliverReassembledMessages() {
  if (tcb_->reassembly_queue().HasMessages()) {
    for (auto& message : tcb_->reassembly_queue().FlushMessages()) {
      callbacks_.OnMessageReceived(std::move(message));
    }
  }
}

void DcSctpSocket::HandleSack(const CommonHeader& header,
                              const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<SackChunk> chunk = SackChunk::Parse(descriptor.data);

  if (ValidateParseSuccess(chunk) && ValidateHasTCB()) {
    TimeMs now = callbacks_.TimeMillis();
    SackChunk sack = ChunkValidators::Clean(*std::move(chunk));

    if (tcb_->retransmission_queue().HandleSack(now, sack)) {
      MaybeSendShutdownOrAck();
      // Receiving an ACK will decrease outstanding bytes (maybe now below
      // cwnd?) or indicate packet loss that may result in sending FORWARD-TSN.
      tcb_->SendBufferedPackets(now);
    } else {
      RTC_DLOG(LS_VERBOSE) << log_prefix()
                           << "Dropping out-of-order SACK with TSN "
                           << *sack.cumulative_tsn_ack();
    }
  }
}

void DcSctpSocket::HandleHeartbeatRequest(
    const CommonHeader& header,
    const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<HeartbeatRequestChunk> chunk =
      HeartbeatRequestChunk::Parse(descriptor.data);

  if (ValidateParseSuccess(chunk) && ValidateHasTCB()) {
    tcb_->heartbeat_handler().HandleHeartbeatRequest(*std::move(chunk));
  }
}

void DcSctpSocket::HandleHeartbeatAck(
    const CommonHeader& header,
    const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<HeartbeatAckChunk> chunk =
      HeartbeatAckChunk::Parse(descriptor.data);

  if (ValidateParseSuccess(chunk) && ValidateHasTCB()) {
    tcb_->heartbeat_handler().HandleHeartbeatAck(*std::move(chunk));
  }
}

void DcSctpSocket::HandleAbort(const CommonHeader& header,
                               const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<AbortChunk> chunk = AbortChunk::Parse(descriptor.data);
  if (ValidateParseSuccess(chunk)) {
    std::string error_string = ErrorCausesToString(chunk->error_causes());
    if (tcb_ == nullptr) {
      // https://tools.ietf.org/html/rfc4960#section-3.3.7
      // "If an endpoint receives an ABORT with a format error or no TCB is
      // found, it MUST silently discard it."
      RTC_DLOG(LS_VERBOSE) << log_prefix() << "Received ABORT (" << error_string
                           << ") on a connection with no TCB. Ignoring";
      return;
    }

    RTC_DLOG(LS_WARNING) << log_prefix() << "Received ABORT (" << error_string
                         << ") - closing connection.";
    InternalClose(ErrorKind::kPeerReported, error_string);
  }
}

void DcSctpSocket::HandleError(const CommonHeader& header,
                               const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<ErrorChunk> chunk = ErrorChunk::Parse(descriptor.data);
  if (ValidateParseSuccess(chunk)) {
    std::string error_string = ErrorCausesToString(chunk->error_causes());
    if (tcb_ == nullptr) {
      RTC_DLOG(LS_VERBOSE) << log_prefix() << "Received ERROR (" << error_string
                           << ") on a connection with no TCB. Ignoring";
      return;
    }

    RTC_DLOG(LS_WARNING) << log_prefix() << "Received ERROR: " << error_string;
    callbacks_.OnError(ErrorKind::kPeerReported,
                       "Peer reported error: " + error_string);
  }
}

void DcSctpSocket::HandleReconfig(
    const CommonHeader& header,
    const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<ReConfigChunk> chunk = ReConfigChunk::Parse(descriptor.data);
  if (ValidateParseSuccess(chunk) && ValidateHasTCB()) {
    tcb_->stream_reset_handler().HandleReConfig(*std::move(chunk));
  }
}

void DcSctpSocket::HandleShutdown(
    const CommonHeader& header,
    const SctpPacket::ChunkDescriptor& descriptor) {
  if (!ValidateParseSuccess(ShutdownChunk::Parse(descriptor.data))) {
    return;
  }

  if (state_ == State::kClosed) {
    return;
  } else if (state_ == State::kCookieWait || state_ == State::kCookieEchoed) {
    // https://tools.ietf.org/html/rfc4960#section-9.2
    // "If a SHUTDOWN is received in the COOKIE-WAIT or COOKIE ECHOED state,
    // the SHUTDOWN chunk SHOULD be silently discarded."
  } else if (state_ == State::kShutdownSent) {
    // https://tools.ietf.org/html/rfc4960#section-9.2
    // "If an endpoint is in the SHUTDOWN-SENT state and receives a
    // SHUTDOWN chunk from its peer, the endpoint shall respond immediately
    // with a SHUTDOWN ACK to its peer, and move into the SHUTDOWN-ACK-SENT
    // state restarting its T2-shutdown timer."
    SendShutdownAck();
    SetState(State::kShutdownAckSent, "SHUTDOWN received");
  } else if (state_ == State::kShutdownAckSent) {
    // TODO(webrtc:12739): This condition should be removed and handled by the
    // next (state_ != State::kShutdownReceived).
    return;
  } else if (state_ != State::kShutdownReceived) {
    RTC_DLOG(LS_VERBOSE) << log_prefix()
                         << "Received SHUTDOWN - shutting down the socket";
    // https://tools.ietf.org/html/rfc4960#section-9.2
    // "Upon reception of the SHUTDOWN, the peer endpoint shall enter the
    // SHUTDOWN-RECEIVED state, stop accepting new data from its SCTP user,
    // and verify, by checking the Cumulative TSN Ack field of the chunk, that
    // all its outstanding DATA chunks have been received by the SHUTDOWN
    // sender."
    SetState(State::kShutdownReceived, "SHUTDOWN received");
    MaybeSendShutdownOrAck();
  }
}

void DcSctpSocket::HandleShutdownAck(
    const CommonHeader& header,
    const SctpPacket::ChunkDescriptor& descriptor) {
  if (!ValidateParseSuccess(ShutdownAckChunk::Parse(descriptor.data))) {
    return;
  }

  if (state_ == State::kShutdownSent || state_ == State::kShutdownAckSent) {
    // https://tools.ietf.org/html/rfc4960#section-9.2
    // "Upon the receipt of the SHUTDOWN ACK, the SHUTDOWN sender shall stop
    // the T2-shutdown timer, send a SHUTDOWN COMPLETE chunk to its peer, and
    // remove all record of the association."

    // "If an endpoint is in the SHUTDOWN-ACK-SENT state and receives a
    // SHUTDOWN ACK, it shall stop the T2-shutdown timer, send a SHUTDOWN
    // COMPLETE chunk to its peer, and remove all record of the association."

    SctpPacket::Builder b = tcb_->PacketBuilder();
    b.Add(ShutdownCompleteChunk(/*tag_reflected=*/false));
    SendPacket(b);
    InternalClose(ErrorKind::kNoError, "");
  } else {
    // https://tools.ietf.org/html/rfc4960#section-8.5.1
    // "If the receiver is in COOKIE-ECHOED or COOKIE-WAIT state
    // the procedures in Section 8.4 SHOULD be followed; in other words, it
    // should be treated as an Out Of The Blue packet."

    // https://tools.ietf.org/html/rfc4960#section-8.4
    // "If the packet contains a SHUTDOWN ACK chunk, the receiver
    // should respond to the sender of the OOTB packet with a SHUTDOWN
    // COMPLETE. When sending the SHUTDOWN COMPLETE, the receiver of the OOTB
    // packet must fill in the Verification Tag field of the outbound packet
    // with the Verification Tag received in the SHUTDOWN ACK and set the T
    // bit in the Chunk Flags to indicate that the Verification Tag is
    // reflected."

    SctpPacket::Builder b(header.verification_tag, options_);
    b.Add(ShutdownCompleteChunk(/*tag_reflected=*/true));
    SendPacket(b);
  }
}

void DcSctpSocket::HandleShutdownComplete(
    const CommonHeader& header,
    const SctpPacket::ChunkDescriptor& descriptor) {
  if (!ValidateParseSuccess(ShutdownCompleteChunk::Parse(descriptor.data))) {
    return;
  }

  if (state_ == State::kShutdownAckSent) {
    // https://tools.ietf.org/html/rfc4960#section-9.2
    // "Upon reception of the SHUTDOWN COMPLETE chunk, the endpoint will
    // verify that it is in the SHUTDOWN-ACK-SENT state; if it is not, the
    // chunk should be discarded.  If the endpoint is in the SHUTDOWN-ACK-SENT
    // state, the endpoint should stop the T2-shutdown timer and remove all
    // knowledge of the association (and thus the association enters the
    // CLOSED state)."
    InternalClose(ErrorKind::kNoError, "");
  }
}

void DcSctpSocket::HandleForwardTsn(
    const CommonHeader& header,
    const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<ForwardTsnChunk> chunk =
      ForwardTsnChunk::Parse(descriptor.data);
  if (ValidateParseSuccess(chunk) && ValidateHasTCB()) {
    HandleForwardTsnCommon(*chunk);
  }
}

void DcSctpSocket::HandleIForwardTsn(
    const CommonHeader& header,
    const SctpPacket::ChunkDescriptor& descriptor) {
  absl::optional<IForwardTsnChunk> chunk =
      IForwardTsnChunk::Parse(descriptor.data);
  if (ValidateParseSuccess(chunk) && ValidateHasTCB()) {
    HandleForwardTsnCommon(*chunk);
  }
}

void DcSctpSocket::HandleForwardTsnCommon(const AnyForwardTsnChunk& chunk) {
  if (!tcb_->capabilities().partial_reliability) {
    SctpPacket::Builder b = tcb_->PacketBuilder();
    b.Add(AbortChunk(/*filled_in_verification_tag=*/true,
                     Parameters::Builder()
                         .Add(ProtocolViolationCause(
                             "I-FORWARD-TSN received, but not indicated "
                             "during connection establishment"))
                         .Build()));
    SendPacket(b);

    callbacks_.OnError(ErrorKind::kProtocolViolation,
                       "Received a FORWARD_TSN without announced peer support");
    return;
  }
  tcb_->data_tracker().HandleForwardTsn(chunk.new_cumulative_tsn());
  tcb_->reassembly_queue().Handle(chunk);
  // A forward TSN - for ordered streams - may allow messages to be
  // delivered.
  DeliverReassembledMessages();

  // Processing a FORWARD_TSN might result in sending a SACK.
  tcb_->MaybeSendSack();
}

void DcSctpSocket::MaybeSendShutdownOrAck() {
  if (tcb_->retransmission_queue().outstanding_bytes() != 0) {
    return;
  }

  if (state_ == State::kShutdownPending) {
    // https://tools.ietf.org/html/rfc4960#section-9.2
    // "Once all its outstanding data has been acknowledged, the endpoint
    // shall send a SHUTDOWN chunk to its peer including in the Cumulative TSN
    // Ack field the last sequential TSN it has received from the peer. It
    // shall then start the T2-shutdown timer and enter the SHUTDOWN-SENT
    // state.""

    SendShutdown();
    t2_shutdown_->set_duration(tcb_->current_rto());
    t2_shutdown_->Start();
    SetState(State::kShutdownSent, "No more outstanding data");
  } else if (state_ == State::kShutdownReceived) {
    // https://tools.ietf.org/html/rfc4960#section-9.2
    // "If the receiver of the SHUTDOWN has no more outstanding DATA
    // chunks, the SHUTDOWN receiver MUST send a SHUTDOWN ACK and start a
    // T2-shutdown timer of its own, entering the SHUTDOWN-ACK-SENT state.  If
    // the timer expires, the endpoint must resend the SHUTDOWN ACK."

    SendShutdownAck();
    SetState(State::kShutdownAckSent, "No more outstanding data");
  }
}

void DcSctpSocket::SendShutdown() {
  SctpPacket::Builder b = tcb_->PacketBuilder();
  b.Add(ShutdownChunk(tcb_->data_tracker().last_cumulative_acked_tsn()));
  SendPacket(b);
}

void DcSctpSocket::SendShutdownAck() {
  SendPacket(tcb_->PacketBuilder().Add(ShutdownAckChunk()));
  t2_shutdown_->set_duration(tcb_->current_rto());
  t2_shutdown_->Start();
}

}  // namespace dcsctp
