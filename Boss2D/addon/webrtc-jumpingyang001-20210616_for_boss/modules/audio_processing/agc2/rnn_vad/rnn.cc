/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__rnn_h //original-code:"modules/audio_processing/agc2/rnn_vad/rnn.h"

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_RNNOISE_U_third_party__rnnoise__src__rnn_vad_weights_h //original-code:"third_party/rnnoise/src/rnn_vad_weights.h"

namespace webrtc {
namespace rnn_vad {
namespace {

using ::rnnoise::kInputLayerInputSize;
static_assert(kFeatureVectorSize == kInputLayerInputSize, "");
using ::rnnoise::kInputDenseBias;
using ::rnnoise::kInputDenseWeights;
using ::rnnoise::kInputLayerOutputSize;
static_assert(kInputLayerOutputSize <= kFullyConnectedLayerMaxUnits, "");

using ::rnnoise::kHiddenGruBias;
using ::rnnoise::kHiddenGruRecurrentWeights;
using ::rnnoise::kHiddenGruWeights;
using ::rnnoise::kHiddenLayerOutputSize;
static_assert(kHiddenLayerOutputSize <= kGruLayerMaxUnits, "");

using ::rnnoise::kOutputDenseBias;
using ::rnnoise::kOutputDenseWeights;
using ::rnnoise::kOutputLayerOutputSize;
static_assert(kOutputLayerOutputSize <= kFullyConnectedLayerMaxUnits, "");

}  // namespace

RnnVad::RnnVad(const AvailableCpuFeatures& cpu_features)
    : input_(kInputLayerInputSize,
             kInputLayerOutputSize,
             kInputDenseBias,
             kInputDenseWeights,
             ActivationFunction::kTansigApproximated,
             cpu_features,
             /*layer_name=*/"FC1"),
      hidden_(kInputLayerOutputSize,
              kHiddenLayerOutputSize,
              kHiddenGruBias,
              kHiddenGruWeights,
              kHiddenGruRecurrentWeights,
              cpu_features,
              /*layer_name=*/"GRU1"),
      output_(kHiddenLayerOutputSize,
              kOutputLayerOutputSize,
              kOutputDenseBias,
              kOutputDenseWeights,
              ActivationFunction::kSigmoidApproximated,
              // The output layer is just 24x1. The unoptimized code is faster.
              NoAvailableCpuFeatures(),
              /*layer_name=*/"FC2") {
  // Input-output chaining size checks.
  RTC_DCHECK_EQ(input_.size(), hidden_.input_size())
      << "The input and the hidden layers sizes do not match.";
  RTC_DCHECK_EQ(hidden_.size(), output_.input_size())
      << "The hidden and the output layers sizes do not match.";
}

RnnVad::~RnnVad() = default;

void RnnVad::Reset() {
  hidden_.Reset();
}

float RnnVad::ComputeVadProbability(
    rtc::ArrayView<const float, kFeatureVectorSize> feature_vector,
    bool is_silence) {
  if (is_silence) {
    Reset();
    return 0.f;
  }
  input_.ComputeOutput(feature_vector);
  hidden_.ComputeOutput(input_);
  output_.ComputeOutput(hidden_);
  RTC_DCHECK_EQ(output_.size(), 1);
  return output_.data()[0];
}

}  // namespace rnn_vad
}  // namespace webrtc
