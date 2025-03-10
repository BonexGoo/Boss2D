/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__video_coding__utility__moving_average_h //original-code:"modules/video_coding/utility/moving_average.h"

#include <algorithm>

namespace webrtc {

MovingAverage::MovingAverage(size_t s) : sum_history_(s + 1, 0) {}
MovingAverage::~MovingAverage() = default;

void MovingAverage::AddSample(int sample) {
  count_++;
  sum_ += sample;
  sum_history_[count_ % sum_history_.size()] = sum_;
}

absl::optional<int> MovingAverage::GetAverage() const {
  return GetAverage(size());
}

absl::optional<int> MovingAverage::GetAverage(size_t num_samples) const {
  if (num_samples > size() || num_samples == 0)
    return absl::nullopt;
  int sum = sum_ - sum_history_[(count_ - num_samples) % sum_history_.size()];
  return sum / static_cast<int>(num_samples);
}

void MovingAverage::Reset() {
  count_ = 0;
  sum_ = 0;
  std::fill(sum_history_.begin(), sum_history_.end(), 0);
}

size_t MovingAverage::size() const {
  return std::min(count_, sum_history_.size() - 1);
}

}  // namespace webrtc
