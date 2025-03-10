/*
 *  Copyright 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_media__sctp__dcsctp_transport_h //original-code:"media/sctp/dcsctp_transport.h"

#include <cstdint>
#include <limits>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_media__base__media_channel_h //original-code:"media/base/media_channel.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__packet_observer_h //original-code:"net/dcsctp/public/packet_observer.h"
#include BOSS_WEBRTC_U_net__dcsctp__public__types_h //original-code:"net/dcsctp/public/types.h"
#include BOSS_WEBRTC_U_net__dcsctp__socket__dcsctp_socket_h //original-code:"net/dcsctp/socket/dcsctp_socket.h"
#include BOSS_WEBRTC_U_p2p__base__packet_transport_internal_h //original-code:"p2p/base/packet_transport_internal.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__strings__string_builder_h //original-code:"rtc_base/strings/string_builder.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__trace_event_h //original-code:"rtc_base/trace_event.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {

namespace {

enum class WebrtcPPID : dcsctp::PPID::UnderlyingType {
  // https://www.rfc-editor.org/rfc/rfc8832.html#section-8.1
  kDCEP = 50,
  // https://www.rfc-editor.org/rfc/rfc8831.html#section-8
  kString = 51,
  kBinaryPartial = 52,  // Deprecated
  kBinary = 53,
  kStringPartial = 54,  // Deprecated
  kStringEmpty = 56,
  kBinaryEmpty = 57,
};

WebrtcPPID ToPPID(DataMessageType message_type, size_t size) {
  switch (message_type) {
    case webrtc::DataMessageType::kControl:
      return WebrtcPPID::kDCEP;
    case webrtc::DataMessageType::kText:
      return size > 0 ? WebrtcPPID::kString : WebrtcPPID::kStringEmpty;
    case webrtc::DataMessageType::kBinary:
      return size > 0 ? WebrtcPPID::kBinary : WebrtcPPID::kBinaryEmpty;
  }
}

absl::optional<DataMessageType> ToDataMessageType(dcsctp::PPID ppid) {
  switch (static_cast<WebrtcPPID>(ppid.value())) {
    case WebrtcPPID::kDCEP:
      return webrtc::DataMessageType::kControl;
    case WebrtcPPID::kString:
    case WebrtcPPID::kStringPartial:
    case WebrtcPPID::kStringEmpty:
      return webrtc::DataMessageType::kText;
    case WebrtcPPID::kBinary:
    case WebrtcPPID::kBinaryPartial:
    case WebrtcPPID::kBinaryEmpty:
      return webrtc::DataMessageType::kBinary;
  }
  return absl::nullopt;
}

bool IsEmptyPPID(dcsctp::PPID ppid) {
  WebrtcPPID webrtc_ppid = static_cast<WebrtcPPID>(ppid.value());
  return webrtc_ppid == WebrtcPPID::kStringEmpty ||
         webrtc_ppid == WebrtcPPID::kBinaryEmpty;
}

// Print outs all sent and received packets to the logs, at LS_VERBOSE severity.
class TextPcapPacketObserver : public dcsctp::PacketObserver {
 public:
  explicit TextPcapPacketObserver(absl::string_view name) : name_(name) {}

  void OnSentPacket(dcsctp::TimeMs now, rtc::ArrayView<const uint8_t> payload) {
    PrintPacket("O ", now, payload);
  }

  void OnReceivedPacket(dcsctp::TimeMs now,
                        rtc::ArrayView<const uint8_t> payload) {
    PrintPacket("I ", now, payload);
  }

 private:
  void PrintPacket(absl::string_view prefix,
                   dcsctp::TimeMs now,
                   rtc::ArrayView<const uint8_t> payload) {
    rtc::StringBuilder s;
    s << "\n" << prefix;
    int64_t remaining = *now % (24 * 60 * 60 * 1000);
    int hours = remaining / (60 * 60 * 1000);
    remaining = remaining % (60 * 60 * 1000);
    int minutes = remaining / (60 * 1000);
    remaining = remaining % (60 * 1000);
    int seconds = remaining / 1000;
    int ms = remaining % 1000;
    s.AppendFormat("%02d:%02d:%02d.%03d", hours, minutes, seconds, ms);
    s << " 0000";
    for (uint8_t byte : payload) {
      s.AppendFormat(" %02x", byte);
    }
    s << " # SCTP_PACKET " << name_;
    RTC_LOG(LS_VERBOSE) << s.str();
  }

  const std::string name_;
};

}  // namespace

DcSctpTransport::DcSctpTransport(rtc::Thread* network_thread,
                                 rtc::PacketTransportInternal* transport,
                                 Clock* clock)
    : network_thread_(network_thread),
      transport_(transport),
      clock_(clock),
      random_(clock_->TimeInMicroseconds()),
      task_queue_timeout_factory_(
          *network_thread,
          [this]() { return TimeMillis(); },
          [this](dcsctp::TimeoutID timeout_id) {
            socket_->HandleTimeout(timeout_id);
          }) {
  RTC_DCHECK_RUN_ON(network_thread_);
  static int instance_count = 0;
  rtc::StringBuilder sb;
  sb << debug_name_ << instance_count++;
  debug_name_ = sb.Release();
  ConnectTransportSignals();
}

DcSctpTransport::~DcSctpTransport() {
  if (socket_) {
    socket_->Close();
  }
}

void DcSctpTransport::SetDtlsTransport(
    rtc::PacketTransportInternal* transport) {
  RTC_DCHECK_RUN_ON(network_thread_);
  DisconnectTransportSignals();
  transport_ = transport;
  ConnectTransportSignals();
  MaybeConnectSocket();
}

bool DcSctpTransport::Start(int local_sctp_port,
                            int remote_sctp_port,
                            int max_message_size) {
  RTC_DCHECK_RUN_ON(network_thread_);
  RTC_DCHECK(max_message_size > 0);

  RTC_LOG(LS_INFO) << debug_name_ << "->Start(local=" << local_sctp_port
                   << ", remote=" << remote_sctp_port
                   << ", max_message_size=" << max_message_size << ")";

  if (!socket_) {
    dcsctp::DcSctpOptions options;
    options.local_port = local_sctp_port;
    options.remote_port = remote_sctp_port;
    options.max_message_size = max_message_size;

    std::unique_ptr<dcsctp::PacketObserver> packet_observer;
    if (RTC_LOG_CHECK_LEVEL(LS_VERBOSE)) {
      packet_observer = std::make_unique<TextPcapPacketObserver>(debug_name_);
    }

    socket_ = std::make_unique<dcsctp::DcSctpSocket>(
        debug_name_, *this, std::move(packet_observer), options);
  } else {
    if (local_sctp_port != socket_->options().local_port ||
        remote_sctp_port != socket_->options().remote_port) {
      RTC_LOG(LS_ERROR)
          << debug_name_ << "->Start(local=" << local_sctp_port
          << ", remote=" << remote_sctp_port
          << "): Can't change ports on already started transport.";
      return false;
    }
    socket_->SetMaxMessageSize(max_message_size);
  }

  MaybeConnectSocket();

  return true;
}

bool DcSctpTransport::OpenStream(int sid) {
  RTC_LOG(LS_INFO) << debug_name_ << "->OpenStream(" << sid << ").";
  if (!socket_) {
    RTC_LOG(LS_ERROR) << debug_name_ << "->OpenStream(sid=" << sid
                      << "): Transport is not started.";
    return false;
  }
  return true;
}

bool DcSctpTransport::ResetStream(int sid) {
  RTC_LOG(LS_INFO) << debug_name_ << "->ResetStream(" << sid << ").";
  if (!socket_) {
    RTC_LOG(LS_ERROR) << debug_name_ << "->OpenStream(sid=" << sid
                      << "): Transport is not started.";
    return false;
  }
  dcsctp::StreamID streams[1] = {dcsctp::StreamID(static_cast<uint16_t>(sid))};
  socket_->ResetStreams(streams);
  return true;
}

bool DcSctpTransport::SendData(int sid,
                               const SendDataParams& params,
                               const rtc::CopyOnWriteBuffer& payload,
                               cricket::SendDataResult* result) {
  RTC_DCHECK_RUN_ON(network_thread_);

  RTC_LOG(LS_VERBOSE) << debug_name_ << "->SendData(sid=" << sid
                      << ", type=" << static_cast<int>(params.type)
                      << ", length=" << payload.size() << ").";

  if (!socket_) {
    RTC_LOG(LS_ERROR) << debug_name_
                      << "->SendData(...): Transport is not started.";
    *result = cricket::SDR_ERROR;
    return false;
  }

  auto max_message_size = socket_->options().max_message_size;
  if (max_message_size > 0 && payload.size() > max_message_size) {
    RTC_LOG(LS_WARNING) << debug_name_
                        << "->SendData(...): "
                           "Trying to send packet bigger "
                           "than the max message size: "
                        << payload.size() << " vs max of " << max_message_size;
    *result = cricket::SDR_ERROR;
    return false;
  }

  std::vector<uint8_t> message_payload(payload.cdata(),
                                       payload.cdata() + payload.size());
  if (message_payload.empty()) {
    // https://www.rfc-editor.org/rfc/rfc8831.html#section-6.6
    // SCTP does not support the sending of empty user messages. Therefore, if
    // an empty message has to be sent, the appropriate PPID (WebRTC String
    // Empty or WebRTC Binary Empty) is used, and the SCTP user message of one
    // zero byte is sent.
    message_payload.push_back('\0');
  }

  dcsctp::DcSctpMessage message(
      dcsctp::StreamID(static_cast<uint16_t>(sid)),
      dcsctp::PPID(static_cast<uint16_t>(ToPPID(params.type, payload.size()))),
      std::move(message_payload));

  dcsctp::SendOptions send_options;
  send_options.unordered = dcsctp::IsUnordered(!params.ordered);
  if (params.max_rtx_ms.has_value()) {
    RTC_DCHECK(*params.max_rtx_ms >= 0 &&
               *params.max_rtx_ms <= std::numeric_limits<uint16_t>::max());
    send_options.lifetime = dcsctp::DurationMs(*params.max_rtx_ms);
  }
  if (params.max_rtx_count.has_value()) {
    RTC_DCHECK(*params.max_rtx_count >= 0 &&
               *params.max_rtx_count <= std::numeric_limits<uint16_t>::max());
    send_options.max_retransmissions = *params.max_rtx_count;
  }

  auto error = socket_->Send(std::move(message), send_options);
  switch (error) {
    case dcsctp::SendStatus::kSuccess:
      *result = cricket::SDR_SUCCESS;
      break;
    case dcsctp::SendStatus::kErrorResourceExhaustion:
      *result = cricket::SDR_BLOCK;
      ready_to_send_data_ = false;
      break;
    default:
      RTC_LOG(LS_ERROR) << debug_name_
                        << "->SendData(...): send() failed with error "
                        << dcsctp::ToString(error) << ".";
      *result = cricket::SDR_ERROR;
  }

  return *result == cricket::SDR_SUCCESS;
}

bool DcSctpTransport::ReadyToSendData() {
  return ready_to_send_data_;
}

int DcSctpTransport::max_message_size() const {
  if (!socket_) {
    RTC_LOG(LS_ERROR) << debug_name_
                      << "->max_message_size(...): Transport is not started.";
    return 0;
  }
  return socket_->options().max_message_size;
}

absl::optional<int> DcSctpTransport::max_outbound_streams() const {
  if (!socket_)
    return absl::nullopt;
  return socket_->options().announced_maximum_outgoing_streams;
}

absl::optional<int> DcSctpTransport::max_inbound_streams() const {
  if (!socket_)
    return absl::nullopt;
  return socket_->options().announced_maximum_incoming_streams;
}

void DcSctpTransport::set_debug_name_for_testing(const char* debug_name) {
  debug_name_ = debug_name;
}

void DcSctpTransport::SendPacket(rtc::ArrayView<const uint8_t> data) {
  RTC_DCHECK_RUN_ON(network_thread_);
  RTC_DCHECK(socket_);

  if (data.size() > (socket_->options().mtu)) {
    RTC_LOG(LS_ERROR) << debug_name_
                      << "->SendPacket(...): "
                         "SCTP seems to have made a packet that is bigger "
                         "than its official MTU: "
                      << data.size() << " vs max of " << socket_->options().mtu;
    return;
  }
  TRACE_EVENT0("webrtc", "DcSctpTransport::SendPacket");

  if (!transport_ || !transport_->writable())
    return;

  RTC_LOG(LS_VERBOSE) << debug_name_ << "->SendPacket(length=" << data.size()
                      << ")";

  auto result =
      transport_->SendPacket(reinterpret_cast<const char*>(data.data()),
                             data.size(), rtc::PacketOptions(), 0);

  if (result < 0) {
    RTC_LOG(LS_WARNING) << debug_name_ << "->SendPacket(length=" << data.size()
                        << ") failed with error: " << transport_->GetError()
                        << ".";
  }
}

std::unique_ptr<dcsctp::Timeout> DcSctpTransport::CreateTimeout() {
  return task_queue_timeout_factory_.CreateTimeout();
}

dcsctp::TimeMs DcSctpTransport::TimeMillis() {
  return dcsctp::TimeMs(clock_->TimeInMilliseconds());
}

uint32_t DcSctpTransport::GetRandomInt(uint32_t low, uint32_t high) {
  return random_.Rand(low, high);
}

void DcSctpTransport::OnTotalBufferedAmountLow() {
  if (!ready_to_send_data_) {
    ready_to_send_data_ = true;
    SignalReadyToSendData();
  }
}

void DcSctpTransport::OnMessageReceived(dcsctp::DcSctpMessage message) {
  RTC_DCHECK_RUN_ON(network_thread_);
  RTC_LOG(LS_VERBOSE) << debug_name_ << "->OnMessageReceived(sid="
                      << message.stream_id().value()
                      << ", ppid=" << message.ppid().value()
                      << ", length=" << message.payload().size() << ").";
  cricket::ReceiveDataParams receive_data_params;
  receive_data_params.sid = message.stream_id().value();
  auto type = ToDataMessageType(message.ppid());
  if (!type.has_value()) {
    RTC_LOG(LS_VERBOSE) << debug_name_
                        << "->OnMessageReceived(): Received an unknown PPID "
                        << message.ppid().value()
                        << " on an SCTP packet. Dropping.";
  }
  receive_data_params.type = *type;
  // No seq_num available from dcSCTP
  receive_data_params.seq_num = 0;
  receive_buffer_.Clear();
  if (!IsEmptyPPID(message.ppid()))
    receive_buffer_.AppendData(message.payload().data(),
                               message.payload().size());

  SignalDataReceived(receive_data_params, receive_buffer_);
}

void DcSctpTransport::OnError(dcsctp::ErrorKind error,
                              absl::string_view message) {
  RTC_LOG(LS_ERROR) << debug_name_
                    << "->OnError(error=" << dcsctp::ToString(error)
                    << ", message=" << message << ").";
}

void DcSctpTransport::OnAborted(dcsctp::ErrorKind error,
                                absl::string_view message) {
  RTC_LOG(LS_ERROR) << debug_name_
                    << "->OnAborted(error=" << dcsctp::ToString(error)
                    << ", message=" << message << ").";
  ready_to_send_data_ = false;
}

void DcSctpTransport::OnConnected() {
  RTC_LOG(LS_INFO) << debug_name_ << "->OnConnected().";
  ready_to_send_data_ = true;
  SignalReadyToSendData();
  SignalAssociationChangeCommunicationUp();
}

void DcSctpTransport::OnClosed() {
  RTC_LOG(LS_INFO) << debug_name_ << "->OnClosed().";
  ready_to_send_data_ = false;
}

void DcSctpTransport::OnConnectionRestarted() {
  RTC_LOG(LS_INFO) << debug_name_ << "->OnConnectionRestarted().";
}

void DcSctpTransport::OnStreamsResetFailed(
    rtc::ArrayView<const dcsctp::StreamID> outgoing_streams,
    absl::string_view reason) {
  // TODO(orphis): Need a test to check for correct behavior
  for (auto& stream_id : outgoing_streams) {
    RTC_LOG(LS_WARNING)
        << debug_name_
        << "->OnStreamsResetFailed(...): Outgoing stream reset failed"
        << ", sid=" << stream_id.value() << ", reason: " << reason << ".";
  }
}

void DcSctpTransport::OnStreamsResetPerformed(
    rtc::ArrayView<const dcsctp::StreamID> outgoing_streams) {
  for (auto& stream_id : outgoing_streams) {
    RTC_LOG(LS_INFO) << debug_name_
                     << "->OnStreamsResetPerformed(...): Outgoing stream reset"
                     << ", sid=" << stream_id.value();
    SignalClosingProcedureComplete(stream_id.value());
  }
}

void DcSctpTransport::OnIncomingStreamsReset(
    rtc::ArrayView<const dcsctp::StreamID> incoming_streams) {
  for (auto& stream_id : incoming_streams) {
    RTC_LOG(LS_INFO) << debug_name_
                     << "->OnIncomingStreamsReset(...): Incoming stream reset"
                     << ", sid=" << stream_id.value();
    SignalClosingProcedureStartedRemotely(stream_id.value());
    SignalClosingProcedureComplete(stream_id.value());
  }
}

void DcSctpTransport::ConnectTransportSignals() {
  RTC_DCHECK_RUN_ON(network_thread_);
  if (!transport_) {
    return;
  }
  transport_->SignalWritableState.connect(
      this, &DcSctpTransport::OnTransportWritableState);
  transport_->SignalReadPacket.connect(this,
                                       &DcSctpTransport::OnTransportReadPacket);
  transport_->SignalClosed.connect(this, &DcSctpTransport::OnTransportClosed);
}

void DcSctpTransport::DisconnectTransportSignals() {
  RTC_DCHECK_RUN_ON(network_thread_);
  if (!transport_) {
    return;
  }
  transport_->SignalWritableState.disconnect(this);
  transport_->SignalReadPacket.disconnect(this);
  transport_->SignalClosed.disconnect(this);
}

void DcSctpTransport::OnTransportWritableState(
    rtc::PacketTransportInternal* transport) {
  RTC_DCHECK_RUN_ON(network_thread_);
  RTC_DCHECK_EQ(transport_, transport);

  RTC_LOG(LS_VERBOSE) << debug_name_
                      << "->OnTransportWritableState(), writable="
                      << transport->writable();

  MaybeConnectSocket();
}

void DcSctpTransport::OnTransportReadPacket(
    rtc::PacketTransportInternal* transport,
    const char* data,
    size_t length,
    const int64_t& /* packet_time_us */,
    int flags) {
  if (flags) {
    // We are only interested in SCTP packets.
    return;
  }

  RTC_LOG(LS_VERBOSE) << debug_name_
                      << "->OnTransportReadPacket(), length=" << length;
  if (socket_) {
    socket_->ReceivePacket(rtc::ArrayView<const uint8_t>(
        reinterpret_cast<const uint8_t*>(data), length));
  }
}

void DcSctpTransport::OnTransportClosed(
    rtc::PacketTransportInternal* transport) {
  RTC_LOG(LS_VERBOSE) << debug_name_ << "->OnTransportClosed().";
  SignalClosedAbruptly();
}

void DcSctpTransport::MaybeConnectSocket() {
  if (transport_ && transport_->writable() && socket_ &&
      socket_->state() == dcsctp::SocketState::kClosed) {
    socket_->Connect();
  }
}
}  // namespace webrtc
