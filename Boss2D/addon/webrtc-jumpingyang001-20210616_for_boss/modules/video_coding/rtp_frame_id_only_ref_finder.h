/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_RTP_FRAME_ID_ONLY_REF_FINDER_H_
#define MODULES_VIDEO_CODING_RTP_FRAME_ID_ONLY_REF_FINDER_H_

#include <memory>

#include BOSS_ABSEILCPP_U_absl__container__inlined_vector_h //original-code:"absl/container/inlined_vector.h"
#include BOSS_WEBRTC_U_modules__video_coding__frame_object_h //original-code:"modules/video_coding/frame_object.h"
#include BOSS_WEBRTC_U_modules__video_coding__rtp_frame_reference_finder_h //original-code:"modules/video_coding/rtp_frame_reference_finder.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__sequence_number_util_h //original-code:"rtc_base/numerics/sequence_number_util.h"

namespace webrtc {

class RtpFrameIdOnlyRefFinder {
 public:
  RtpFrameIdOnlyRefFinder() = default;

  RtpFrameReferenceFinder::ReturnVector ManageFrame(
      std::unique_ptr<RtpFrameObject> frame,
      int frame_id);

 private:
  static constexpr int kFrameIdLength = 1 << 15;
  SeqNumUnwrapper<uint16_t, kFrameIdLength> unwrapper_;
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_RTP_FRAME_ID_ONLY_REF_FINDER_H_
