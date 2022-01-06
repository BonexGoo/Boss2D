/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_raw_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer_raw.h"

#include <utility>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer.h"
#include BOSS_WEBRTC_U_rtc_base__copy_on_write_buffer_h //original-code:"rtc_base/copy_on_write_buffer.h"

namespace webrtc {

absl::optional<VideoRtpDepacketizer::ParsedRtpPayload>
VideoRtpDepacketizerRaw::Parse(rtc::CopyOnWriteBuffer rtp_payload) {
  absl::optional<ParsedRtpPayload> parsed(absl::in_place);
  parsed->video_payload = std::move(rtp_payload);
  return parsed;
}

}  // namespace webrtc
