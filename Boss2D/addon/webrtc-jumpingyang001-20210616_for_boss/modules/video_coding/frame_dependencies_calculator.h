/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_FRAME_DEPENDENCIES_CALCULATOR_H_
#define MODULES_VIDEO_CODING_FRAME_DEPENDENCIES_CALCULATOR_H_

#include <stdint.h>

#include <vector>

#include BOSS_ABSEILCPP_U_absl__container__inlined_vector_h //original-code:"absl/container/inlined_vector.h"
#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_common_video__generic_frame_descriptor__generic_frame_info_h //original-code:"common_video/generic_frame_descriptor/generic_frame_info.h"

namespace webrtc {

// This class is thread compatible.
class FrameDependenciesCalculator {
 public:
  FrameDependenciesCalculator() = default;
  FrameDependenciesCalculator(const FrameDependenciesCalculator&) = default;
  FrameDependenciesCalculator& operator=(const FrameDependenciesCalculator&) =
      default;

  // Calculates frame dependencies based on previous encoder buffer usage.
  absl::InlinedVector<int64_t, 5> FromBuffersUsage(
      int64_t frame_id,
      rtc::ArrayView<const CodecBufferUsage> buffers_usage);

 private:
  struct BufferUsage {
    absl::optional<int64_t> frame_id;
    absl::InlinedVector<int64_t, 4> dependencies;
  };

  absl::InlinedVector<BufferUsage, 4> buffers_;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_FRAME_DEPENDENCIES_CALCULATOR_H_
