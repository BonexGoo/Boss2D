/* Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_CODECS_VP9_SVC_CONFIG_H_
#define MODULES_VIDEO_CODING_CODECS_VP9_SVC_CONFIG_H_

#include <stddef.h>

#include <vector>

#include BOSS_WEBRTC_U_api__video_codecs__spatial_layer_h //original-code:"api/video_codecs/spatial_layer.h"

namespace webrtc {

std::vector<SpatialLayer> GetSvcConfig(size_t input_width,
                                       size_t input_height,
                                       float max_framerate_fps,
                                       size_t first_active_layer,
                                       size_t num_spatial_layers,
                                       size_t num_temporal_layers,
                                       bool is_screen_sharing);

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_VP9_SVC_CONFIG_H_
