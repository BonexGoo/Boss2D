/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__srtptransport_h //original-code:"pc/srtptransport.h"

#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_media__base__rtputils_h //original-code:"media/base/rtputils.h"
#include BOSS_WEBRTC_U_pc__rtptransport_h //original-code:"pc/rtptransport.h"
#include BOSS_WEBRTC_U_pc__srtpsession_h //original-code:"pc/srtpsession.h"
#include BOSS_WEBRTC_U_rtc_base__asyncpacketsocket_h //original-code:"rtc_base/asyncpacketsocket.h"
#include BOSS_WEBRTC_U_rtc_base__copyonwritebuffer_h //original-code:"rtc_base/copyonwritebuffer.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"
#include BOSS_WEBRTC_U_rtc_base__third_party__base64__base64_h //original-code:"rtc_base/third_party/base64/base64.h"
#include BOSS_WEBRTC_U_rtc_base__trace_event_h //original-code:"rtc_base/trace_event.h"
#include BOSS_WEBRTC_U_rtc_base__zero_memory_h //original-code:"rtc_base/zero_memory.h"

namespace webrtc {

SrtpTransport::SrtpTransport(bool rtcp_mux_enabled)
    : RtpTransport(rtcp_mux_enabled) {}

RTCError SrtpTransport::SetSrtpSendKey(const cricket::CryptoParams& params) {
  if (send_params_) {
    LOG_AND_RETURN_ERROR(
        webrtc::RTCErrorType::UNSUPPORTED_OPERATION,
        "Setting the SRTP send key twice is currently unsupported.");
  }
  if (recv_params_ && recv_params_->cipher_suite != params.cipher_suite) {
    LOG_AND_RETURN_ERROR(
        webrtc::RTCErrorType::UNSUPPORTED_OPERATION,
        "The send key and receive key must have the same cipher suite.");
  }

  send_cipher_suite_ = rtc::SrtpCryptoSuiteFromName(params.cipher_suite);
  if (*send_cipher_suite_ == rtc::SRTP_INVALID_CRYPTO_SUITE) {
    return RTCError(RTCErrorType::INVALID_PARAMETER,
                    "Invalid SRTP crypto suite");
  }

  int send_key_len, send_salt_len;
  if (!rtc::GetSrtpKeyAndSaltLengths(*send_cipher_suite_, &send_key_len,
                                     &send_salt_len)) {
    return RTCError(RTCErrorType::INVALID_PARAMETER,
                    "Could not get lengths for crypto suite(s):"
                    " send cipher_suite ");
  }

  send_key_ = rtc::ZeroOnFreeBuffer<uint8_t>(send_key_len + send_salt_len);
  if (!ParseKeyParams(params.key_params, send_key_.data(), send_key_.size())) {
    return RTCError(RTCErrorType::INVALID_PARAMETER,
                    "Failed to parse the crypto key params");
  }

  if (!MaybeSetKeyParams()) {
    return RTCError(RTCErrorType::INVALID_PARAMETER,
                    "Failed to set the crypto key params");
  }
  send_params_ = params;
  return RTCError::OK();
}

RTCError SrtpTransport::SetSrtpReceiveKey(const cricket::CryptoParams& params) {
  if (recv_params_) {
    LOG_AND_RETURN_ERROR(
        webrtc::RTCErrorType::UNSUPPORTED_OPERATION,
        "Setting the SRTP send key twice is currently unsupported.");
  }
  if (send_params_ && send_params_->cipher_suite != params.cipher_suite) {
    LOG_AND_RETURN_ERROR(
        webrtc::RTCErrorType::UNSUPPORTED_OPERATION,
        "The send key and receive key must have the same cipher suite.");
  }

  recv_cipher_suite_ = rtc::SrtpCryptoSuiteFromName(params.cipher_suite);
  if (*recv_cipher_suite_ == rtc::SRTP_INVALID_CRYPTO_SUITE) {
    return RTCError(RTCErrorType::INVALID_PARAMETER,
                    "Invalid SRTP crypto suite");
  }

  int recv_key_len, recv_salt_len;
  if (!rtc::GetSrtpKeyAndSaltLengths(*recv_cipher_suite_, &recv_key_len,
                                     &recv_salt_len)) {
    return RTCError(RTCErrorType::INVALID_PARAMETER,
                    "Could not get lengths for crypto suite(s):"
                    " recv cipher_suite ");
  }

  recv_key_ = rtc::ZeroOnFreeBuffer<uint8_t>(recv_key_len + recv_salt_len);
  if (!ParseKeyParams(params.key_params, recv_key_.data(), recv_key_.size())) {
    return RTCError(RTCErrorType::INVALID_PARAMETER,
                    "Failed to parse the crypto key params");
  }

  if (!MaybeSetKeyParams()) {
    return RTCError(RTCErrorType::INVALID_PARAMETER,
                    "Failed to set the crypto key params");
  }
  recv_params_ = params;
  return RTCError::OK();
}

bool SrtpTransport::SendRtpPacket(rtc::CopyOnWriteBuffer* packet,
                                  const rtc::PacketOptions& options,
                                  int flags) {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_ERROR)
        << "Failed to send the packet because SRTP transport is inactive.";
    return false;
  }
  rtc::PacketOptions updated_options = options;
  TRACE_EVENT0("webrtc", "SRTP Encode");
  bool res;
  uint8_t* data = packet->data();
  int len = rtc::checked_cast<int>(packet->size());
// If ENABLE_EXTERNAL_AUTH flag is on then packet authentication is not done
// inside libsrtp for a RTP packet. A external HMAC module will be writing
// a fake HMAC value. This is ONLY done for a RTP packet.
// Socket layer will update rtp sendtime extension header if present in
// packet with current time before updating the HMAC.
#if !defined(ENABLE_EXTERNAL_AUTH)
  res = ProtectRtp(data, len, static_cast<int>(packet->capacity()), &len);
#else
  if (!IsExternalAuthActive()) {
    res = ProtectRtp(data, len, static_cast<int>(packet->capacity()), &len);
  } else {
    updated_options.packet_time_params.rtp_sendtime_extension_id =
        rtp_abs_sendtime_extn_id_;
    res = ProtectRtp(data, len, static_cast<int>(packet->capacity()), &len,
                     &updated_options.packet_time_params.srtp_packet_index);
    // If protection succeeds, let's get auth params from srtp.
    if (res) {
      uint8_t* auth_key = nullptr;
      int key_len = 0;
      res = GetRtpAuthParams(
          &auth_key, &key_len,
          &updated_options.packet_time_params.srtp_auth_tag_len);
      if (res) {
        updated_options.packet_time_params.srtp_auth_key.resize(key_len);
        updated_options.packet_time_params.srtp_auth_key.assign(
            auth_key, auth_key + key_len);
      }
    }
  }
#endif
  if (!res) {
    int seq_num = -1;
    uint32_t ssrc = 0;
    cricket::GetRtpSeqNum(data, len, &seq_num);
    cricket::GetRtpSsrc(data, len, &ssrc);
    RTC_LOG(LS_ERROR) << "Failed to protect RTP packet: size=" << len
                      << ", seqnum=" << seq_num << ", SSRC=" << ssrc;
    return false;
  }

  // Update the length of the packet now that we've added the auth tag.
  packet->SetSize(len);
  return SendPacket(/*rtcp=*/false, packet, updated_options, flags);
}

bool SrtpTransport::SendRtcpPacket(rtc::CopyOnWriteBuffer* packet,
                                   const rtc::PacketOptions& options,
                                   int flags) {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_ERROR)
        << "Failed to send the packet because SRTP transport is inactive.";
    return false;
  }

  TRACE_EVENT0("webrtc", "SRTP Encode");
  uint8_t* data = packet->data();
  int len = rtc::checked_cast<int>(packet->size());
  if (!ProtectRtcp(data, len, static_cast<int>(packet->capacity()), &len)) {
    int type = -1;
    cricket::GetRtcpType(data, len, &type);
    RTC_LOG(LS_ERROR) << "Failed to protect RTCP packet: size=" << len
                      << ", type=" << type;
    return false;
  }
  // Update the length of the packet now that we've added the auth tag.
  packet->SetSize(len);

  return SendPacket(/*rtcp=*/true, packet, options, flags);
}

void SrtpTransport::OnRtpPacketReceived(rtc::CopyOnWriteBuffer* packet,
                                        const rtc::PacketTime& packet_time) {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_WARNING)
        << "Inactive SRTP transport received an RTP packet. Drop it.";
    return;
  }
  TRACE_EVENT0("webrtc", "SRTP Decode");
  char* data = packet->data<char>();
  int len = rtc::checked_cast<int>(packet->size());
  if (!UnprotectRtp(data, len, &len)) {
    int seq_num = -1;
    uint32_t ssrc = 0;
    cricket::GetRtpSeqNum(data, len, &seq_num);
    cricket::GetRtpSsrc(data, len, &ssrc);
    RTC_LOG(LS_ERROR) << "Failed to unprotect RTP packet: size=" << len
                      << ", seqnum=" << seq_num << ", SSRC=" << ssrc;
    return;
  }
  packet->SetSize(len);
  DemuxPacket(packet, packet_time);
}

void SrtpTransport::OnRtcpPacketReceived(rtc::CopyOnWriteBuffer* packet,
                                         const rtc::PacketTime& packet_time) {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_WARNING)
        << "Inactive SRTP transport received an RTCP packet. Drop it.";
    return;
  }
  TRACE_EVENT0("webrtc", "SRTP Decode");
  char* data = packet->data<char>();
  int len = rtc::checked_cast<int>(packet->size());
  if (!UnprotectRtcp(data, len, &len)) {
    int type = -1;
    cricket::GetRtcpType(data, len, &type);
    RTC_LOG(LS_ERROR) << "Failed to unprotect RTCP packet: size=" << len
                      << ", type=" << type;
    return;
  }
  packet->SetSize(len);
  SignalRtcpPacketReceived(packet, packet_time);
}

void SrtpTransport::OnNetworkRouteChanged(
    absl::optional<rtc::NetworkRoute> network_route) {
  // Only append the SRTP overhead when there is a selected network route.
  if (network_route) {
    int srtp_overhead = 0;
    if (IsSrtpActive()) {
      GetSrtpOverhead(&srtp_overhead);
    }
    network_route->packet_overhead += srtp_overhead;
  }
  SignalNetworkRouteChanged(network_route);
}

void SrtpTransport::OnWritableState(
    rtc::PacketTransportInternal* packet_transport) {
  SignalWritableState(IsWritable(/*rtcp=*/true) && IsWritable(/*rtcp=*/true));
}

bool SrtpTransport::SetRtpParams(int send_cs,
                                 const uint8_t* send_key,
                                 int send_key_len,
                                 const std::vector<int>& send_extension_ids,
                                 int recv_cs,
                                 const uint8_t* recv_key,
                                 int recv_key_len,
                                 const std::vector<int>& recv_extension_ids) {
  // If parameters are being set for the first time, we should create new SRTP
  // sessions and call "SetSend/SetRecv". Otherwise we should call
  // "UpdateSend"/"UpdateRecv" on the existing sessions, which will internally
  // call "srtp_update".
  bool new_sessions = false;
  if (!send_session_) {
    RTC_DCHECK(!recv_session_);
    CreateSrtpSessions();
    new_sessions = true;
  }
  bool ret = new_sessions
                 ? send_session_->SetSend(send_cs, send_key, send_key_len,
                                          send_extension_ids)
                 : send_session_->UpdateSend(send_cs, send_key, send_key_len,
                                             send_extension_ids);
  if (!ret) {
    ResetParams();
    return false;
  }

  ret = new_sessions ? recv_session_->SetRecv(recv_cs, recv_key, recv_key_len,
                                              recv_extension_ids)
                     : recv_session_->UpdateRecv(
                           recv_cs, recv_key, recv_key_len, recv_extension_ids);
  if (!ret) {
    ResetParams();
    return false;
  }

  RTC_LOG(LS_INFO) << "SRTP " << (new_sessions ? "activated" : "updated")
                   << " with negotiated parameters: send cipher_suite "
                   << send_cs << " recv cipher_suite " << recv_cs;
  MaybeUpdateWritableState();
  return true;
}

bool SrtpTransport::SetRtcpParams(int send_cs,
                                  const uint8_t* send_key,
                                  int send_key_len,
                                  const std::vector<int>& send_extension_ids,
                                  int recv_cs,
                                  const uint8_t* recv_key,
                                  int recv_key_len,
                                  const std::vector<int>& recv_extension_ids) {
  // This can only be called once, but can be safely called after
  // SetRtpParams
  if (send_rtcp_session_ || recv_rtcp_session_) {
    RTC_LOG(LS_ERROR) << "Tried to set SRTCP Params when filter already active";
    return false;
  }

  send_rtcp_session_.reset(new cricket::SrtpSession());
  if (!send_rtcp_session_->SetSend(send_cs, send_key, send_key_len,
                                   send_extension_ids)) {
    return false;
  }

  recv_rtcp_session_.reset(new cricket::SrtpSession());
  if (!recv_rtcp_session_->SetRecv(recv_cs, recv_key, recv_key_len,
                                   recv_extension_ids)) {
    return false;
  }

  RTC_LOG(LS_INFO) << "SRTCP activated with negotiated parameters:"
                      " send cipher_suite "
                   << send_cs << " recv cipher_suite " << recv_cs;
  MaybeUpdateWritableState();
  return true;
}

bool SrtpTransport::IsSrtpActive() const {
  return send_session_ && recv_session_;
}

bool SrtpTransport::IsWritable(bool rtcp) const {
  return IsSrtpActive() && RtpTransport::IsWritable(rtcp);
}

void SrtpTransport::ResetParams() {
  send_session_ = nullptr;
  recv_session_ = nullptr;
  send_rtcp_session_ = nullptr;
  recv_rtcp_session_ = nullptr;
  MaybeUpdateWritableState();
  RTC_LOG(LS_INFO) << "The params in SRTP transport are reset.";
}

void SrtpTransport::CreateSrtpSessions() {
  send_session_.reset(new cricket::SrtpSession());
  recv_session_.reset(new cricket::SrtpSession());
  if (external_auth_enabled_) {
    send_session_->EnableExternalAuth();
  }
}

bool SrtpTransport::ProtectRtp(void* p, int in_len, int max_len, int* out_len) {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_WARNING) << "Failed to ProtectRtp: SRTP not active";
    return false;
  }
  RTC_CHECK(send_session_);
  return send_session_->ProtectRtp(p, in_len, max_len, out_len);
}

bool SrtpTransport::ProtectRtp(void* p,
                               int in_len,
                               int max_len,
                               int* out_len,
                               int64_t* index) {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_WARNING) << "Failed to ProtectRtp: SRTP not active";
    return false;
  }
  RTC_CHECK(send_session_);
  return send_session_->ProtectRtp(p, in_len, max_len, out_len, index);
}

bool SrtpTransport::ProtectRtcp(void* p,
                                int in_len,
                                int max_len,
                                int* out_len) {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_WARNING) << "Failed to ProtectRtcp: SRTP not active";
    return false;
  }
  if (send_rtcp_session_) {
    return send_rtcp_session_->ProtectRtcp(p, in_len, max_len, out_len);
  } else {
    RTC_CHECK(send_session_);
    return send_session_->ProtectRtcp(p, in_len, max_len, out_len);
  }
}

bool SrtpTransport::UnprotectRtp(void* p, int in_len, int* out_len) {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_WARNING) << "Failed to UnprotectRtp: SRTP not active";
    return false;
  }
  RTC_CHECK(recv_session_);
  return recv_session_->UnprotectRtp(p, in_len, out_len);
}

bool SrtpTransport::UnprotectRtcp(void* p, int in_len, int* out_len) {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_WARNING) << "Failed to UnprotectRtcp: SRTP not active";
    return false;
  }
  if (recv_rtcp_session_) {
    return recv_rtcp_session_->UnprotectRtcp(p, in_len, out_len);
  } else {
    RTC_CHECK(recv_session_);
    return recv_session_->UnprotectRtcp(p, in_len, out_len);
  }
}

bool SrtpTransport::GetRtpAuthParams(uint8_t** key,
                                     int* key_len,
                                     int* tag_len) {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_WARNING) << "Failed to GetRtpAuthParams: SRTP not active";
    return false;
  }

  RTC_CHECK(send_session_);
  return send_session_->GetRtpAuthParams(key, key_len, tag_len);
}

bool SrtpTransport::GetSrtpOverhead(int* srtp_overhead) const {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_WARNING) << "Failed to GetSrtpOverhead: SRTP not active";
    return false;
  }

  RTC_CHECK(send_session_);
  *srtp_overhead = send_session_->GetSrtpOverhead();
  return true;
}

void SrtpTransport::EnableExternalAuth() {
  RTC_DCHECK(!IsSrtpActive());
  external_auth_enabled_ = true;
}

bool SrtpTransport::IsExternalAuthEnabled() const {
  return external_auth_enabled_;
}

bool SrtpTransport::IsExternalAuthActive() const {
  if (!IsSrtpActive()) {
    RTC_LOG(LS_WARNING)
        << "Failed to check IsExternalAuthActive: SRTP not active";
    return false;
  }

  RTC_CHECK(send_session_);
  return send_session_->IsExternalAuthActive();
}

bool SrtpTransport::MaybeSetKeyParams() {
  if (!send_cipher_suite_ || !recv_cipher_suite_) {
    return true;
  }

  return SetRtpParams(*send_cipher_suite_, send_key_.data(),
                      static_cast<int>(send_key_.size()), std::vector<int>(),
                      *recv_cipher_suite_, recv_key_.data(),
                      static_cast<int>(recv_key_.size()), std::vector<int>());
}

bool SrtpTransport::ParseKeyParams(const std::string& key_params,
                                   uint8_t* key,
                                   size_t len) {
  // example key_params: "inline:YUJDZGVmZ2hpSktMbW9QUXJzVHVWd3l6MTIzNDU2"

  // Fail if key-method is wrong.
  if (key_params.find("inline:") != 0) {
    return false;
  }

  // Fail if base64 decode fails, or the key is the wrong size.
  std::string key_b64(key_params.substr(7)), key_str;
  if (!rtc::Base64::Decode(key_b64, rtc::Base64::DO_STRICT, &key_str,
                           nullptr) ||
      key_str.size() != len) {
    return false;
  }

  memcpy(key, key_str.c_str(), len);
  // TODO(bugs.webrtc.org/8905): Switch to ZeroOnFreeBuffer for storing
  // sensitive data.
  rtc::ExplicitZeroMemory(&key_str[0], key_str.size());
  return true;
}

void SrtpTransport::MaybeUpdateWritableState() {
  bool writable = IsWritable(/*rtcp=*/true) && IsWritable(/*rtcp=*/false);
  // Only fire the signal if the writable state changes.
  if (writable_ != writable) {
    writable_ = writable;
    SignalWritableState(writable_);
  }
}

}  // namespace webrtc
