/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"

namespace webrtc {

SdpVideoFormat::SdpVideoFormat(const std::string& name) : name(name) {}

SdpVideoFormat::SdpVideoFormat(const std::string& name,
                               const Parameters& parameters)
    : name(name), parameters(parameters) {}

SdpVideoFormat::SdpVideoFormat(const SdpVideoFormat&) = default;
SdpVideoFormat::SdpVideoFormat(SdpVideoFormat&&) = default;
SdpVideoFormat& SdpVideoFormat::operator=(const SdpVideoFormat&) = default;
SdpVideoFormat& SdpVideoFormat::operator=(SdpVideoFormat&&) = default;

SdpVideoFormat::~SdpVideoFormat() = default;

bool operator==(const SdpVideoFormat& a, const SdpVideoFormat& b) {
  return a.name == b.name && a.parameters == b.parameters;
}

}  // namespace webrtc
