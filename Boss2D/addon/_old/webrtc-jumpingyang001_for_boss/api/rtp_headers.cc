/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__rtp_headers_h //original-code:"api/rtp_headers.h"

#include <string.h>
#include <algorithm>
#include <limits>
#include <type_traits>

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__stringutils_h //original-code:"rtc_base/stringutils.h"

namespace webrtc {

RTPHeaderExtension::RTPHeaderExtension()
    : hasTransmissionTimeOffset(false),
      transmissionTimeOffset(0),
      hasAbsoluteSendTime(false),
      absoluteSendTime(0),
      hasTransportSequenceNumber(false),
      transportSequenceNumber(0),
      hasAudioLevel(false),
      voiceActivity(false),
      audioLevel(0),
      hasVideoRotation(false),
      videoRotation(kVideoRotation_0),
      hasVideoContentType(false),
      videoContentType(VideoContentType::UNSPECIFIED),
      has_video_timing(false) {}

RTPHeaderExtension::RTPHeaderExtension(const RTPHeaderExtension& other) =
    default;

RTPHeaderExtension& RTPHeaderExtension::operator=(
    const RTPHeaderExtension& other) = default;

RTPHeader::RTPHeader()
    : markerBit(false),
      payloadType(0),
      sequenceNumber(0),
      timestamp(0),
      ssrc(0),
      numCSRCs(0),
      arrOfCSRCs(),
      paddingLength(0),
      headerLength(0),
      payload_type_frequency(0),
      extension() {}

RTPHeader::RTPHeader(const RTPHeader& other) = default;

RTPHeader& RTPHeader::operator=(const RTPHeader& other) = default;

}  // namespace webrtc
