/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__aec3__block_framer_h //original-code:"modules/audio_processing/aec3/block_framer.h"

#include <algorithm>

#include BOSS_WEBRTC_U_modules__audio_processing__aec3__aec3_common_h //original-code:"modules/audio_processing/aec3/aec3_common.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {

BlockFramer::BlockFramer(size_t num_bands, size_t num_channels)
    : num_bands_(num_bands),
      num_channels_(num_channels),
      buffer_(num_bands_,
              std::vector<std::vector<float>>(
                  num_channels,
                  std::vector<float>(kBlockSize, 0.f))) {
  RTC_DCHECK_LT(0, num_bands);
  RTC_DCHECK_LT(0, num_channels);
}

BlockFramer::~BlockFramer() = default;

// All the constants are chosen so that the buffer is either empty or has enough
// samples for InsertBlockAndExtractSubFrame to produce a frame. In order to
// achieve this, the InsertBlockAndExtractSubFrame and InsertBlock methods need
// to be called in the correct order.
void BlockFramer::InsertBlock(
    const std::vector<std::vector<std::vector<float>>>& block) {
  RTC_DCHECK_EQ(num_bands_, block.size());
  for (size_t band = 0; band < num_bands_; ++band) {
    RTC_DCHECK_EQ(num_channels_, block[band].size());
    for (size_t channel = 0; channel < num_channels_; ++channel) {
      RTC_DCHECK_EQ(kBlockSize, block[band][channel].size());
      RTC_DCHECK_EQ(0, buffer_[band][channel].size());

      buffer_[band][channel].insert(buffer_[band][channel].begin(),
                                    block[band][channel].begin(),
                                    block[band][channel].end());
    }
  }
}

void BlockFramer::InsertBlockAndExtractSubFrame(
    const std::vector<std::vector<std::vector<float>>>& block,
    std::vector<std::vector<rtc::ArrayView<float>>>* sub_frame) {
  RTC_DCHECK(sub_frame);
  RTC_DCHECK_EQ(num_bands_, block.size());
  RTC_DCHECK_EQ(num_bands_, sub_frame->size());
  for (size_t band = 0; band < num_bands_; ++band) {
    RTC_DCHECK_EQ(num_channels_, block[band].size());
    RTC_DCHECK_EQ(num_channels_, (*sub_frame)[0].size());
    for (size_t channel = 0; channel < num_channels_; ++channel) {
      RTC_DCHECK_LE(kSubFrameLength,
                    buffer_[band][channel].size() + kBlockSize);
      RTC_DCHECK_EQ(kBlockSize, block[band][channel].size());
      RTC_DCHECK_GE(kBlockSize, buffer_[band][channel].size());
      RTC_DCHECK_EQ(kSubFrameLength, (*sub_frame)[band][channel].size());

      const int samples_to_frame =
          kSubFrameLength - buffer_[band][channel].size();
      std::copy(buffer_[band][channel].begin(), buffer_[band][channel].end(),
                (*sub_frame)[band][channel].begin());
      std::copy(
          block[band][channel].begin(),
          block[band][channel].begin() + samples_to_frame,
          (*sub_frame)[band][channel].begin() + buffer_[band][channel].size());
      buffer_[band][channel].clear();
      buffer_[band][channel].insert(
          buffer_[band][channel].begin(),
          block[band][channel].begin() + samples_to_frame,
          block[band][channel].end());
    }
  }
}

}  // namespace webrtc
