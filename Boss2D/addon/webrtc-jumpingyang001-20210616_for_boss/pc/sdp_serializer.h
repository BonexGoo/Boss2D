/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_SDP_SERIALIZER_H_
#define PC_SDP_SERIALIZER_H_

#include <string>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__rtc_error_h //original-code:"api/rtc_error.h"
#include BOSS_WEBRTC_U_media__base__rid_description_h //original-code:"media/base/rid_description.h"
#include BOSS_WEBRTC_U_pc__session_description_h //original-code:"pc/session_description.h"
#include BOSS_WEBRTC_U_pc__simulcast_description_h //original-code:"pc/simulcast_description.h"

namespace webrtc {

// This class should serialize components of the SDP (and not the SDP itself).
// Example:
//     SimulcastDescription can be serialized and deserialized by this class.
//     The serializer will know how to translate the data to spec-compliant
//     format without knowing about the SDP attribute details (a=simulcast:)
// Usage:
//     Consider the SDP attribute for simulcast a=simulcast:<configuration>.
//     The SDP serializtion code (webrtcsdp.h) should use |SdpSerializer| to
//     serialize and deserialize the <configuration> section.
// This class will allow testing the serialization of components without
// having to serialize the entire SDP while hiding implementation details
// from callers of sdp serialization (webrtcsdp.h).
class SdpSerializer {
 public:
  // Serialization for the Simulcast description according to
  // https://tools.ietf.org/html/draft-ietf-mmusic-sdp-simulcast-13#section-5.1
  std::string SerializeSimulcastDescription(
      const cricket::SimulcastDescription& simulcast) const;

  // Deserialization for the SimulcastDescription according to
  // https://tools.ietf.org/html/draft-ietf-mmusic-sdp-simulcast-13#section-5.1
  RTCErrorOr<cricket::SimulcastDescription> DeserializeSimulcastDescription(
      absl::string_view string) const;

  // Serialization for the RID description according to
  // https://tools.ietf.org/html/draft-ietf-mmusic-rid-15#section-10
  std::string SerializeRidDescription(
      const cricket::RidDescription& rid_description) const;

  // Deserialization for the RidDescription according to
  // https://tools.ietf.org/html/draft-ietf-mmusic-rid-15#section-10
  RTCErrorOr<cricket::RidDescription> DeserializeRidDescription(
      absl::string_view string) const;
};

}  // namespace webrtc

#endif  // PC_SDP_SERIALIZER_H_
