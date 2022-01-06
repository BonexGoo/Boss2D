/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef RTC_BASE_EXPERIMENTS_MIN_VIDEO_BITRATE_EXPERIMENT_H_
#define RTC_BASE_EXPERIMENTS_MIN_VIDEO_BITRATE_EXPERIMENT_H_

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__units__data_rate_h //original-code:"api/units/data_rate.h"
#include BOSS_WEBRTC_U_api__video__video_codec_type_h //original-code:"api/video/video_codec_type.h"

namespace webrtc {

extern const int kDefaultMinVideoBitrateBps;

// Return the experiment-driven minimum video bitrate.
// If no experiment is effective, returns nullopt.
absl::optional<DataRate> GetExperimentalMinVideoBitrate(VideoCodecType type);

}  // namespace webrtc

#endif  // RTC_BASE_EXPERIMENTS_MIN_VIDEO_BITRATE_EXPERIMENT_H_
