/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef MODULES_VIDEO_CODING_SVC_SCALABILITY_STRUCTURE_TEST_HELPERS_H_
#define MODULES_VIDEO_CODING_SVC_SCALABILITY_STRUCTURE_TEST_HELPERS_H_

#include <stdint.h>

#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__transport__rtp__dependency_descriptor_h //original-code:"api/transport/rtp/dependency_descriptor.h"
#include BOSS_WEBRTC_U_api__video__video_bitrate_allocation_h //original-code:"api/video/video_bitrate_allocation.h"
#include BOSS_WEBRTC_U_common_video__generic_frame_descriptor__generic_frame_info_h //original-code:"common_video/generic_frame_descriptor/generic_frame_info.h"
#include BOSS_WEBRTC_U_modules__video_coding__chain_diff_calculator_h //original-code:"modules/video_coding/chain_diff_calculator.h"
#include BOSS_WEBRTC_U_modules__video_coding__frame_dependencies_calculator_h //original-code:"modules/video_coding/frame_dependencies_calculator.h"
#include BOSS_WEBRTC_U_modules__video_coding__svc__scalable_video_controller_h //original-code:"modules/video_coding/svc/scalable_video_controller.h"

namespace webrtc {

// Creates bitrate allocation with non-zero bitrate for given number of temporal
// layers for each spatial layer.
VideoBitrateAllocation EnableTemporalLayers(int s0, int s1 = 0, int s2 = 0);

class ScalabilityStructureWrapper {
 public:
  explicit ScalabilityStructureWrapper(ScalableVideoController& structure)
      : structure_controller_(structure) {}

  std::vector<GenericFrameInfo> GenerateFrames(int num_temporal_units) {
    std::vector<GenericFrameInfo> frames;
    GenerateFrames(num_temporal_units, frames);
    return frames;
  }
  void GenerateFrames(int num_temporal_units,
                      std::vector<GenericFrameInfo>& frames);

  // Returns false and ADD_FAILUREs for frames with invalid references.
  // In particular validates no frame frame reference to frame before frames[0].
  // In error messages frames are indexed starting with 0.
  bool FrameReferencesAreValid(
      rtc::ArrayView<const GenericFrameInfo> frames) const;

 private:
  ScalableVideoController& structure_controller_;
  FrameDependenciesCalculator frame_deps_calculator_;
  ChainDiffCalculator chain_diff_calculator_;
  int64_t frame_id_ = 0;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_SVC_SCALABILITY_STRUCTURE_TEST_HELPERS_H_
