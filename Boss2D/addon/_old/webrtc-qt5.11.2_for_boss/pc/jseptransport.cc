/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__jseptransport_h //original-code:"pc/jseptransport.h"

#include <memory>
#include <utility>  // for std::pair

#include BOSS_WEBRTC_U_api__candidate_h //original-code:"api/candidate.h"
#include BOSS_WEBRTC_U_p2p__base__p2pconstants_h //original-code:"p2p/base/p2pconstants.h"
#include BOSS_WEBRTC_U_p2p__base__p2ptransportchannel_h //original-code:"p2p/base/p2ptransportchannel.h"
#include BOSS_WEBRTC_U_p2p__base__port_h //original-code:"p2p/base/port.h"
#include BOSS_WEBRTC_U_rtc_base__bind_h //original-code:"rtc_base/bind.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

using webrtc::SdpType;

namespace cricket {

TransportChannelStats::TransportChannelStats() = default;

TransportChannelStats::TransportChannelStats(const TransportChannelStats&) =
    default;

TransportChannelStats::~TransportChannelStats() = default;

TransportStats::TransportStats() = default;

TransportStats::~TransportStats() = default;

bool BadTransportDescription(const std::string& desc, std::string* err_desc) {
  if (err_desc) {
    *err_desc = desc;
  }
  RTC_LOG(LS_ERROR) << desc;
  return false;
}

static bool VerifyIceParams(const TransportDescription& desc) {
  // For legacy protocols.
  if (desc.ice_ufrag.empty() && desc.ice_pwd.empty())
    return true;

  if (desc.ice_ufrag.length() < ICE_UFRAG_MIN_LENGTH ||
      desc.ice_ufrag.length() > ICE_UFRAG_MAX_LENGTH) {
    return false;
  }
  if (desc.ice_pwd.length() < ICE_PWD_MIN_LENGTH ||
      desc.ice_pwd.length() > ICE_PWD_MAX_LENGTH) {
    return false;
  }
  return true;
}

JsepTransport::JsepTransport(
    const std::string& mid,
    const rtc::scoped_refptr<rtc::RTCCertificate>& certificate)
    : mid_(mid), certificate_(certificate) {}

JsepTransport::~JsepTransport() = default;

bool JsepTransport::AddChannel(DtlsTransportInternal* dtls, int component) {
  if (channels_.find(component) != channels_.end()) {
    RTC_LOG(LS_ERROR) << "Adding channel for component " << component
                      << " twice.";
    return false;
  }
  channels_[component] = dtls;
  // Something's wrong if a channel is being added after a description is set.
  // This may currently occur if rtcp-mux is negotiated, then a new m= section
  // is added in a later offer/answer. But this is suboptimal and should be
  // changed; we shouldn't support going from muxed to non-muxed.
  // TODO(deadbeef): Once this is fixed, make the warning an error, and remove
  // the calls to "ApplyXTransportDescription" below.
  if (local_description_set_ || remote_description_set_) {
    RTC_LOG(LS_WARNING) << "Adding new transport channel after "
                           "transport description already applied.";
  }
  bool ret = true;
  std::string err;
  if (local_description_set_) {
    ret &= ApplyLocalTransportDescription(channels_[component], &err);
  }
  if (remote_description_set_) {
    ret &= ApplyRemoteTransportDescription(channels_[component], &err);
  }
  if (local_description_set_ && remote_description_set_) {
    ret &= ApplyNegotiatedTransportDescription(channels_[component], &err);
  }
  return ret;
}

bool JsepTransport::RemoveChannel(int component) {
  auto it = channels_.find(component);
  if (it == channels_.end()) {
    RTC_LOG(LS_ERROR) << "Trying to remove channel for component " << component
                      << ", which doesn't exist.";
    return false;
  }
  channels_.erase(component);
  return true;
}

bool JsepTransport::HasChannels() const {
  return !channels_.empty();
}

void JsepTransport::SetLocalCertificate(
    const rtc::scoped_refptr<rtc::RTCCertificate>& certificate) {
  certificate_ = certificate;
}

bool JsepTransport::GetLocalCertificate(
    rtc::scoped_refptr<rtc::RTCCertificate>* certificate) const {
  if (!certificate_) {
    return false;
  }

  *certificate = certificate_;
  return true;
}

bool JsepTransport::SetLocalTransportDescription(
    const TransportDescription& description,
    SdpType type,
    std::string* error_desc) {
  bool ret = true;

  if (!VerifyIceParams(description)) {
    return BadTransportDescription("Invalid ice-ufrag or ice-pwd length",
                                   error_desc);
  }

  bool ice_restarting =
      local_description_set_ &&
      IceCredentialsChanged(local_description_->ice_ufrag,
                            local_description_->ice_pwd, description.ice_ufrag,
                            description.ice_pwd);
  local_description_.reset(new TransportDescription(description));

  rtc::SSLFingerprint* local_fp =
      local_description_->identity_fingerprint.get();

  if (!local_fp) {
    certificate_ = nullptr;
  } else if (!VerifyCertificateFingerprint(certificate_.get(), local_fp,
                                           error_desc)) {
    return false;
  }

  for (const auto& kv : channels_) {
    ret &= ApplyLocalTransportDescription(kv.second, error_desc);
  }
  if (!ret) {
    return false;
  }

  // If PRANSWER/ANSWER is set, we should decide transport protocol type.
  if (type == SdpType::kPrAnswer || type == SdpType::kAnswer) {
    ret &= NegotiateTransportDescription(type, error_desc);
  }
  if (!ret) {
    return false;
  }

  if (needs_ice_restart_ && ice_restarting) {
    needs_ice_restart_ = false;
    RTC_LOG(LS_VERBOSE) << "needs-ice-restart flag cleared for transport "
                        << mid();
  }

  local_description_set_ = true;
  return true;
}

bool JsepTransport::SetRemoteTransportDescription(
    const TransportDescription& description,
    SdpType type,
    std::string* error_desc) {
  bool ret = true;

  if (!VerifyIceParams(description)) {
    return BadTransportDescription("Invalid ice-ufrag or ice-pwd length",
                                   error_desc);
  }

  remote_description_.reset(new TransportDescription(description));
  for (const auto& kv : channels_) {
    ret &= ApplyRemoteTransportDescription(kv.second, error_desc);
  }

  // If PRANSWER/ANSWER is set, we should decide transport protocol type.
  if (type == SdpType::kPrAnswer || type == SdpType::kAnswer) {
    ret = NegotiateTransportDescription(SdpType::kOffer, error_desc);
  }
  if (ret) {
    remote_description_set_ = true;
  }

  return ret;
}

void JsepTransport::SetNeedsIceRestartFlag() {
  if (!needs_ice_restart_) {
    needs_ice_restart_ = true;
    RTC_LOG(LS_VERBOSE) << "needs-ice-restart flag set for transport " << mid();
  }
}

bool JsepTransport::NeedsIceRestart() const {
  return needs_ice_restart_;
}

rtc::Optional<rtc::SSLRole> JsepTransport::GetSslRole() const {
  return ssl_role_;
}

bool JsepTransport::GetStats(TransportStats* stats) {
  stats->transport_name = mid();
  stats->channel_stats.clear();
  for (auto& kv : channels_) {
    DtlsTransportInternal* dtls_transport = kv.second;
    TransportChannelStats substats;
    substats.component = kv.first;
    dtls_transport->GetSrtpCryptoSuite(&substats.srtp_crypto_suite);
    dtls_transport->GetSslCipherSuite(&substats.ssl_cipher_suite);
    substats.dtls_state = dtls_transport->dtls_state();
    if (!dtls_transport->ice_transport()->GetStats(
            &substats.connection_infos)) {
      return false;
    }
    stats->channel_stats.push_back(substats);
  }
  return true;
}

bool JsepTransport::VerifyCertificateFingerprint(
    const rtc::RTCCertificate* certificate,
    const rtc::SSLFingerprint* fingerprint,
    std::string* error_desc) const {
  if (!fingerprint) {
    return BadTransportDescription("No fingerprint.", error_desc);
  }
  if (!certificate) {
    return BadTransportDescription(
        "Fingerprint provided but no identity available.", error_desc);
  }
  std::unique_ptr<rtc::SSLFingerprint> fp_tmp(rtc::SSLFingerprint::Create(
      fingerprint->algorithm, certificate->identity()));
  RTC_DCHECK(fp_tmp.get() != NULL);
  if (*fp_tmp == *fingerprint) {
    return true;
  }
  std::ostringstream desc;
  desc << "Local fingerprint does not match identity. Expected: ";
  desc << fp_tmp->ToString();
  desc << " Got: " << fingerprint->ToString();
  return BadTransportDescription(desc.str(), error_desc);
}

bool JsepTransport::ApplyLocalTransportDescription(
    DtlsTransportInternal* dtls_transport,
    std::string* error_desc) {
  dtls_transport->ice_transport()->SetIceParameters(
      local_description_->GetIceParameters());
  return true;
}

bool JsepTransport::ApplyRemoteTransportDescription(
    DtlsTransportInternal* dtls_transport,
    std::string* error_desc) {
  dtls_transport->ice_transport()->SetRemoteIceParameters(
      remote_description_->GetIceParameters());
  dtls_transport->ice_transport()->SetRemoteIceMode(
      remote_description_->ice_mode);
  return true;
}

bool JsepTransport::ApplyNegotiatedTransportDescription(
    DtlsTransportInternal* dtls_transport,
    std::string* error_desc) {
  // Set SSL role. Role must be set before fingerprint is applied, which
  // initiates DTLS setup.
  if (ssl_role_ && !dtls_transport->SetSslRole(*ssl_role_)) {
    return BadTransportDescription("Failed to set SSL role for the channel.",
                                   error_desc);
  }
  // Apply remote fingerprint.
  if (!dtls_transport->SetRemoteFingerprint(
          remote_fingerprint_->algorithm,
          reinterpret_cast<const uint8_t*>(remote_fingerprint_->digest.data()),
          remote_fingerprint_->digest.size())) {
    return BadTransportDescription("Failed to apply remote fingerprint.",
                                   error_desc);
  }
  return true;
}

bool JsepTransport::NegotiateTransportDescription(
    SdpType local_description_type,
    std::string* error_desc) {
  if (!local_description_ || !remote_description_) {
    const std::string msg =
        "Applying an answer transport description "
        "without applying any offer.";
    return BadTransportDescription(msg, error_desc);
  }
  rtc::SSLFingerprint* local_fp =
      local_description_->identity_fingerprint.get();
  rtc::SSLFingerprint* remote_fp =
      remote_description_->identity_fingerprint.get();
  if (remote_fp && local_fp) {
    remote_fingerprint_.reset(new rtc::SSLFingerprint(*remote_fp));
    if (!NegotiateRole(local_description_type, error_desc)) {
      return false;
    }
  } else if (local_fp && (local_description_type == SdpType::kAnswer)) {
    return BadTransportDescription(
        "Local fingerprint supplied when caller didn't offer DTLS.",
        error_desc);
  } else {
    // We are not doing DTLS
    remote_fingerprint_.reset(new rtc::SSLFingerprint("", nullptr, 0));
  }
  // Now that we have negotiated everything, push it downward.
  // Note that we cache the result so that if we have race conditions
  // between future SetRemote/SetLocal invocations and new channel
  // creation, we have the negotiation state saved until a new
  // negotiation happens.
  for (const auto& kv : channels_) {
    if (!ApplyNegotiatedTransportDescription(kv.second, error_desc)) {
      return false;
    }
  }
  return true;
}

bool JsepTransport::NegotiateRole(SdpType local_description_type,
                                  std::string* error_desc) {
  if (!local_description_ || !remote_description_) {
    const std::string msg =
        "Local and Remote description must be set before "
        "transport descriptions are negotiated";
    return BadTransportDescription(msg, error_desc);
  }

  // From RFC 4145, section-4.1, The following are the values that the
  // 'setup' attribute can take in an offer/answer exchange:
  //       Offer      Answer
  //      ________________
  //      active     passive / holdconn
  //      passive    active / holdconn
  //      actpass    active / passive / holdconn
  //      holdconn   holdconn
  //
  // Set the role that is most conformant with RFC 5763, Section 5, bullet 1
  // The endpoint MUST use the setup attribute defined in [RFC4145].
  // The endpoint that is the offerer MUST use the setup attribute
  // value of setup:actpass and be prepared to receive a client_hello
  // before it receives the answer.  The answerer MUST use either a
  // setup attribute value of setup:active or setup:passive.  Note that
  // if the answerer uses setup:passive, then the DTLS handshake will
  // not begin until the answerer is received, which adds additional
  // latency. setup:active allows the answer and the DTLS handshake to
  // occur in parallel.  Thus, setup:active is RECOMMENDED.  Whichever
  // party is active MUST initiate a DTLS handshake by sending a
  // ClientHello over each flow (host/port quartet).
  // IOW - actpass and passive modes should be treated as server and
  // active as client.
  ConnectionRole local_connection_role = local_description_->connection_role;
  ConnectionRole remote_connection_role = remote_description_->connection_role;

  bool is_remote_server = false;
  if (local_description_type == SdpType::kOffer) {
    if (local_connection_role != CONNECTIONROLE_ACTPASS) {
      return BadTransportDescription(
          "Offerer must use actpass value for setup attribute.", error_desc);
    }

    if (remote_connection_role == CONNECTIONROLE_ACTIVE ||
        remote_connection_role == CONNECTIONROLE_PASSIVE ||
        remote_connection_role == CONNECTIONROLE_NONE) {
      is_remote_server = (remote_connection_role == CONNECTIONROLE_PASSIVE);
    } else {
      const std::string msg =
          "Answerer must use either active or passive value "
          "for setup attribute.";
      return BadTransportDescription(msg, error_desc);
    }
    // If remote is NONE or ACTIVE it will act as client.
  } else {
    if (remote_connection_role != CONNECTIONROLE_ACTPASS &&
        remote_connection_role != CONNECTIONROLE_NONE) {
      // Accept a remote role attribute that's not "actpass", but matches the
      // current negotiated role. This is allowed by dtls-sdp, though our
      // implementation will never generate such an offer as it's not
      // recommended.
      //
      // See https://datatracker.ietf.org/doc/html/draft-ietf-mmusic-dtls-sdp,
      // section 5.5.
      if (!ssl_role_ ||
          (*ssl_role_ == rtc::SSL_CLIENT &&
           remote_connection_role == CONNECTIONROLE_ACTIVE) ||
          (*ssl_role_ == rtc::SSL_SERVER &&
           remote_connection_role == CONNECTIONROLE_PASSIVE)) {
        return BadTransportDescription(
            "Offerer must use actpass value or current negotiated role for "
            "setup attribute.",
            error_desc);
      }
    }

    if (local_connection_role == CONNECTIONROLE_ACTIVE ||
        local_connection_role == CONNECTIONROLE_PASSIVE) {
      is_remote_server = (local_connection_role == CONNECTIONROLE_ACTIVE);
    } else {
      const std::string msg =
          "Answerer must use either active or passive value "
          "for setup attribute.";
      return BadTransportDescription(msg, error_desc);
    }

    // If local is passive, local will act as server.
  }

  ssl_role_.emplace(is_remote_server ? rtc::SSL_CLIENT : rtc::SSL_SERVER);
  return true;
}

}  // namespace cricket
