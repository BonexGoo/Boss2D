/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__aec3__frame_blocker_h //original-code:"modules/audio_processing/aec3/frame_blocker.h"

#include BOSS_WEBRTC_U_modules__audio_processing__aec3__aec3_common_h //original-code:"modules/audio_processing/aec3/aec3_common.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {

FrameBlocker::FrameBlocker(size_t num_bands, size_t num_channels)
    : num_bands_(num_bands),
      num_channels_(num_channels),
      buffer_(num_bands_, std::vector<std::vector<float>>(num_channels)) {
  RTC_DCHECK_LT(0, num_bands);
  RTC_DCHECK_LT(0, num_channels);
  for (auto& band : buffer_) {
    for (auto& channel : band) {
      channel.reserve(kBlockSize);
      RTC_DCHECK(channel.empty());
    }
  }
}

FrameBlocker::~FrameBlocker() = default;

void FrameBlocker::InsertSubFrameAndExtractBlock(
    const std::vector<std::vector<rtc::ArrayView<float>>>& sub_frame,
    std::vector<std::vector<std::vector<float>>>* block) {
  RTC_DCHECK(block);
  RTC_DCHECK_EQ(num_bands_, block->size());
  RTC_DCHECK_EQ(num_bands_, sub_frame.size());
  for (size_t band = 0; band < num_bands_; ++band) {
    RTC_DCHECK_EQ(num_channels_, (*block)[band].size());
    RTC_DCHECK_EQ(num_channels_, sub_frame[band].size());
    for (size_t channel = 0; channel < num_channels_; ++channel) {
      RTC_DCHECK_GE(kBlockSize - 16, buffer_[band][channel].size());
      RTC_DCHECK_EQ(kBlockSize, (*block)[band][channel].size());
      RTC_DCHECK_EQ(kSubFrameLength, sub_frame[band][channel].size());
      const int samples_to_block = kBlockSize - buffer_[band][channel].size();
      (*block)[band][channel].clear();
      (*block)[band][channel].insert((*block)[band][channel].begin(),
                                     buffer_[band][channel].begin(),
                                     buffer_[band][channel].end());
      (*block)[band][channel].insert(
          (*block)[band][channel].begin() + buffer_[band][channel].size(),
          sub_frame[band][channel].begin(),
          sub_frame[band][channel].begin() + samples_to_block);
      buffer_[band][channel].clear();
      buffer_[band][channel].insert(
          buffer_[band][channel].begin(),
          sub_frame[band][channel].begin() + samples_to_block,
          sub_frame[band][channel].end());
    }
  }
}

bool FrameBlocker::IsBlockAvailable() const {
  return kBlockSize == buffer_[0][0].size();
}

void FrameBlocker::ExtractBlock(
    std::vector<std::vector<std::vector<float>>>* block) {
  RTC_DCHECK(block);
  RTC_DCHECK_EQ(num_bands_, block->size());
  RTC_DCHECK(IsBlockAvailable());
  for (size_t band = 0; band < num_bands_; ++band) {
    RTC_DCHECK_EQ(num_channels_, (*block)[band].size());
    for (size_t channel = 0; channel < num_channels_; ++channel) {
      RTC_DCHECK_EQ(kBlockSize, buffer_[band][channel].size());
      RTC_DCHECK_EQ(kBlockSize, (*block)[band][channel].size());
      (*block)[band][channel].clear();
      (*block)[band][channel].insert((*block)[band][channel].begin(),
                                     buffer_[band][channel].begin(),
                                     buffer_[band][channel].end());
      buffer_[band][channel].clear();
    }
  }
}

}  // namespace webrtc
