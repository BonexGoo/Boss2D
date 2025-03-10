/*
 *  Copyright 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__jsepsessiondescription_h //original-code:"api/jsepsessiondescription.h"

#include <memory>

#include BOSS_WEBRTC_U_p2p__base__port_h //original-code:"p2p/base/port.h"
#include BOSS_WEBRTC_U_pc__mediasession_h //original-code:"pc/mediasession.h"
#include BOSS_WEBRTC_U_pc__webrtcsdp_h //original-code:"pc/webrtcsdp.h"
#include BOSS_WEBRTC_U_rtc_base__arraysize_h //original-code:"rtc_base/arraysize.h"
#include BOSS_WEBRTC_U_rtc_base__ptr_util_h //original-code:"rtc_base/ptr_util.h"
#include BOSS_WEBRTC_U_rtc_base__stringencode_h //original-code:"rtc_base/stringencode.h"

using cricket::SessionDescription;

namespace webrtc {
namespace {

// RFC 5245
// It is RECOMMENDED that default candidates be chosen based on the
// likelihood of those candidates to work with the peer that is being
// contacted.  It is RECOMMENDED that relayed > reflexive > host.
constexpr int kPreferenceUnknown = 0;
constexpr int kPreferenceHost = 1;
constexpr int kPreferenceReflexive = 2;
constexpr int kPreferenceRelayed = 3;

constexpr char kDummyAddress[] = "0.0.0.0";
constexpr int kDummyPort = 9;

int GetCandidatePreferenceFromType(const std::string& type) {
  int preference = kPreferenceUnknown;
  if (type == cricket::LOCAL_PORT_TYPE) {
    preference = kPreferenceHost;
  } else if (type == cricket::STUN_PORT_TYPE) {
    preference = kPreferenceReflexive;
  } else if (type == cricket::RELAY_PORT_TYPE) {
    preference = kPreferenceRelayed;
  } else {
    preference = kPreferenceUnknown;
  }
  return preference;
}

// Update the connection address for the MediaContentDescription based on the
// candidates.
void UpdateConnectionAddress(
    const JsepCandidateCollection& candidate_collection,
    cricket::MediaContentDescription* media_desc) {
  int port = kDummyPort;
  std::string ip = kDummyAddress;
  int current_preference = kPreferenceUnknown;
  int current_family = AF_UNSPEC;
  for (size_t i = 0; i < candidate_collection.count(); ++i) {
    const IceCandidateInterface* jsep_candidate = candidate_collection.at(i);
    if (jsep_candidate->candidate().component() !=
        cricket::ICE_CANDIDATE_COMPONENT_RTP) {
      continue;
    }
    // Default destination should be UDP only.
    if (jsep_candidate->candidate().protocol() != cricket::UDP_PROTOCOL_NAME) {
      continue;
    }
    const int preference =
        GetCandidatePreferenceFromType(jsep_candidate->candidate().type());
    const int family = jsep_candidate->candidate().address().ipaddr().family();
    // See if this candidate is more preferable then the current one if it's the
    // same family. Or if the current family is IPv4 already so we could safely
    // ignore all IPv6 ones. WebRTC bug 4269.
    // http://code.google.com/p/webrtc/issues/detail?id=4269
    if ((preference <= current_preference && current_family == family) ||
        (current_family == AF_INET && family == AF_INET6)) {
      continue;
    }
    current_preference = preference;
    current_family = family;
    port = jsep_candidate->candidate().address().port();
    ip = jsep_candidate->candidate().address().ipaddr().ToString();
  }
  rtc::SocketAddress connection_addr;
  connection_addr.SetIP(ip);
  connection_addr.SetPort(port);
  media_desc->set_connection_address(connection_addr);
}

}  // namespace

const char SessionDescriptionInterface::kOffer[] = "offer";
const char SessionDescriptionInterface::kPrAnswer[] = "pranswer";
const char SessionDescriptionInterface::kAnswer[] = "answer";

const int JsepSessionDescription::kDefaultVideoCodecId = 100;
const char JsepSessionDescription::kDefaultVideoCodecName[] = "VP8";

const char* SdpTypeToString(SdpType type) {
  switch (type) {
    case SdpType::kOffer:
      return SessionDescriptionInterface::kOffer;
    case SdpType::kPrAnswer:
      return SessionDescriptionInterface::kPrAnswer;
    case SdpType::kAnswer:
      return SessionDescriptionInterface::kAnswer;
  }
  return "";
}

rtc::Optional<SdpType> SdpTypeFromString(const std::string& type_str) {
  if (type_str == SessionDescriptionInterface::kOffer) {
    return SdpType::kOffer;
  } else if (type_str == SessionDescriptionInterface::kPrAnswer) {
    return SdpType::kPrAnswer;
  } else if (type_str == SessionDescriptionInterface::kAnswer) {
    return SdpType::kAnswer;
  } else {
    return rtc::nullopt;
  }
}

// TODO(steveanton): Remove this default implementation once Chromium has been
// updated.
SdpType SessionDescriptionInterface::GetType() const {
  rtc::Optional<SdpType> maybe_type = SdpTypeFromString(type());
  if (maybe_type) {
    return *maybe_type;
  } else {
    RTC_LOG(LS_WARNING) << "Default implementation of "
                           "SessionDescriptionInterface::GetType does not "
                           "recognize the result from type(), returning "
                           "kOffer.";
    return SdpType::kOffer;
  }
}

SessionDescriptionInterface* CreateSessionDescription(const std::string& type,
                                                      const std::string& sdp,
                                                      SdpParseError* error) {
  rtc::Optional<SdpType> maybe_type = SdpTypeFromString(type);
  if (!maybe_type) {
    return nullptr;
  }

  return CreateSessionDescription(*maybe_type, sdp, error).release();
}

std::unique_ptr<SessionDescriptionInterface> CreateSessionDescription(
    SdpType type,
    const std::string& sdp) {
  return CreateSessionDescription(type, sdp, nullptr);
}

std::unique_ptr<SessionDescriptionInterface> CreateSessionDescription(
    SdpType type,
    const std::string& sdp,
    SdpParseError* error_out) {
  auto jsep_desc = rtc::MakeUnique<JsepSessionDescription>(type);
  if (!SdpDeserialize(sdp, jsep_desc.get(), error_out)) {
    return nullptr;
  }
  return std::move(jsep_desc);
}

JsepSessionDescription::JsepSessionDescription(SdpType type) : type_(type) {}

JsepSessionDescription::JsepSessionDescription(const std::string& type) {
  rtc::Optional<SdpType> maybe_type = SdpTypeFromString(type);
  if (maybe_type) {
    type_ = *maybe_type;
  } else {
    RTC_LOG(LS_WARNING)
        << "JsepSessionDescription constructed with invalid type string: "
        << type << ". Assuming it is an offer.";
    type_ = SdpType::kOffer;
  }
}

JsepSessionDescription::~JsepSessionDescription() {}

bool JsepSessionDescription::Initialize(
    cricket::SessionDescription* description,
    const std::string& session_id,
    const std::string& session_version) {
  if (!description)
    return false;

  session_id_ = session_id;
  session_version_ = session_version;
  description_.reset(description);
  candidate_collection_.resize(number_of_mediasections());
  return true;
}

bool JsepSessionDescription::AddCandidate(
    const IceCandidateInterface* candidate) {
  if (!candidate || candidate->sdp_mline_index() < 0)
    return false;
  size_t mediasection_index = 0;
  if (!GetMediasectionIndex(candidate, &mediasection_index)) {
    return false;
  }
  if (mediasection_index >= number_of_mediasections())
    return false;
  const std::string& content_name =
      description_->contents()[mediasection_index].name;
  const cricket::TransportInfo* transport_info =
      description_->GetTransportInfoByName(content_name);
  if (!transport_info) {
    return false;
  }

  cricket::Candidate updated_candidate = candidate->candidate();
  if (updated_candidate.username().empty()) {
    updated_candidate.set_username(transport_info->description.ice_ufrag);
  }
  if (updated_candidate.password().empty()) {
    updated_candidate.set_password(transport_info->description.ice_pwd);
  }

  std::unique_ptr<JsepIceCandidate> updated_candidate_wrapper(
      new JsepIceCandidate(candidate->sdp_mid(),
                           static_cast<int>(mediasection_index),
                           updated_candidate));
  if (!candidate_collection_[mediasection_index].HasCandidate(
          updated_candidate_wrapper.get())) {
    candidate_collection_[mediasection_index].add(
        updated_candidate_wrapper.release());
    UpdateConnectionAddress(
        candidate_collection_[mediasection_index],
        description_->contents()[mediasection_index].media_description());
  }

  return true;
}

size_t JsepSessionDescription::RemoveCandidates(
    const std::vector<cricket::Candidate>& candidates) {
  size_t num_removed = 0;
  for (auto& candidate : candidates) {
    int mediasection_index = GetMediasectionIndex(candidate);
    if (mediasection_index < 0) {
      // Not found.
      continue;
    }
    num_removed += candidate_collection_[mediasection_index].remove(candidate);
    UpdateConnectionAddress(
        candidate_collection_[mediasection_index],
        description_->contents()[mediasection_index].media_description());
  }
  return num_removed;
}

size_t JsepSessionDescription::number_of_mediasections() const {
  if (!description_)
    return 0;
  return description_->contents().size();
}

const IceCandidateCollection* JsepSessionDescription::candidates(
    size_t mediasection_index) const {
  if (mediasection_index >= candidate_collection_.size())
    return NULL;
  return &candidate_collection_[mediasection_index];
}

bool JsepSessionDescription::ToString(std::string* out) const {
  if (!description_ || !out) {
    return false;
  }
  *out = SdpSerialize(*this, false);
  return !out->empty();
}

bool JsepSessionDescription::GetMediasectionIndex(
    const IceCandidateInterface* candidate,
    size_t* index) {
  if (!candidate || !index) {
    return false;
  }
  *index = static_cast<size_t>(candidate->sdp_mline_index());
  if (description_ && !candidate->sdp_mid().empty()) {
    bool found = false;
    // Try to match the sdp_mid with content name.
    for (size_t i = 0; i < description_->contents().size(); ++i) {
      if (candidate->sdp_mid() == description_->contents().at(i).name) {
        *index = i;
        found = true;
        break;
      }
    }
    if (!found) {
      // If the sdp_mid is presented but we can't find a match, we consider
      // this as an error.
      return false;
    }
  }
  return true;
}

int JsepSessionDescription::GetMediasectionIndex(
    const cricket::Candidate& candidate) {
  // Find the description with a matching transport name of the candidate.
  const std::string& transport_name = candidate.transport_name();
  for (size_t i = 0; i < description_->contents().size(); ++i) {
    if (transport_name == description_->contents().at(i).name) {
      return static_cast<int>(i);
    }
  }
  return -1;
}

}  // namespace webrtc
