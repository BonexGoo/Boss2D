/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef COMMON_AUDIO_FIR_FILTER_C_H_
#define COMMON_AUDIO_FIR_FILTER_C_H_

#include <string.h>

#include <memory>

#include BOSS_WEBRTC_U_common_audio__fir_filter_h //original-code:"common_audio/fir_filter.h"

namespace webrtc {

class FIRFilterC : public FIRFilter {
 public:
  FIRFilterC(const float* coefficients, size_t coefficients_length);
  ~FIRFilterC() override;

  void Filter(const float* in, size_t length, float* out) override;

 private:
  size_t coefficients_length_;
  size_t state_length_;
  std::unique_ptr<float[]> coefficients_;
  std::unique_ptr<float[]> state_;
};

}  // namespace webrtc

#endif  // COMMON_AUDIO_FIR_FILTER_C_H_
