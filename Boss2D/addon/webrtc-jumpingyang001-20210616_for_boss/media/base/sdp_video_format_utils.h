/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MEDIA_BASE_SDP_VIDEO_FORMAT_UTILS_H_
#define MEDIA_BASE_SDP_VIDEO_FORMAT_UTILS_H_

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"

namespace webrtc {
// Generate codec parameters that will be used as answer in an SDP negotiation
// based on local supported parameters and remote offered parameters. Both
// |local_supported_params|, |remote_offered_params|, and |answer_params|
// represent sendrecv media descriptions, i.e they are a mix of both encode and
// decode capabilities. In theory, when the profile in |local_supported_params|
// represent a strict superset of the profile in |remote_offered_params|, we
// could limit the profile in |answer_params| to the profile in
// |remote_offered_params|. However, to simplify the code, each supported H264
// profile should be listed explicitly in the list of local supported codecs,
// even if they are redundant. Then each local codec in the list should be
// tested one at a time against the remote codec, and only when the profiles are
// equal should this function be called. Therefore, this function does not need
// to handle profile intersection, and the profile of |local_supported_params|
// and |remote_offered_params| must be equal before calling this function. The
// parameters that are used when negotiating are the level part of
// profile-level-id and level-asymmetry-allowed.
void H264GenerateProfileLevelIdForAnswer(
    const SdpVideoFormat::Parameters& local_supported_params,
    const SdpVideoFormat::Parameters& remote_offered_params,
    SdpVideoFormat::Parameters* answer_params);

// Parse max frame rate from SDP FMTP line. absl::nullopt is returned if the
// field is missing or not a number.
absl::optional<int> ParseSdpForVPxMaxFrameRate(
    const SdpVideoFormat::Parameters& params);

// Parse max frame size from SDP FMTP line. absl::nullopt is returned if the
// field is missing or not a number. Please note that the value is stored in sub
// blocks but the returned value is in total number of pixels.
absl::optional<int> ParseSdpForVPxMaxFrameSize(
    const SdpVideoFormat::Parameters& params);

}  // namespace webrtc

#endif  // MEDIA_BASE_SDP_VIDEO_FORMAT_UTILS_H_
