/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_AGC2_RNN_VAD_RNN_FC_H_
#define MODULES_AUDIO_PROCESSING_AGC2_RNN_VAD_RNN_FC_H_

#include <array>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__function_view_h //original-code:"api/function_view.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__cpu_features_h //original-code:"modules/audio_processing/agc2/cpu_features.h"
#include BOSS_WEBRTC_U_modules__audio_processing__agc2__rnn_vad__vector_math_h //original-code:"modules/audio_processing/agc2/rnn_vad/vector_math.h"

namespace webrtc {
namespace rnn_vad {

// Activation function for a neural network cell.
enum class ActivationFunction { kTansigApproximated, kSigmoidApproximated };

// Maximum number of units for an FC layer.
constexpr int kFullyConnectedLayerMaxUnits = 24;

// Fully-connected layer with a custom activation function which owns the output
// buffer.
class FullyConnectedLayer {
 public:
  // Ctor. `output_size` cannot be greater than `kFullyConnectedLayerMaxUnits`.
  FullyConnectedLayer(int input_size,
                      int output_size,
                      rtc::ArrayView<const int8_t> bias,
                      rtc::ArrayView<const int8_t> weights,
                      ActivationFunction activation_function,
                      const AvailableCpuFeatures& cpu_features,
                      absl::string_view layer_name);
  FullyConnectedLayer(const FullyConnectedLayer&) = delete;
  FullyConnectedLayer& operator=(const FullyConnectedLayer&) = delete;
  ~FullyConnectedLayer();

  // Returns the size of the input vector.
  int input_size() const { return input_size_; }
  // Returns the pointer to the first element of the output buffer.
  const float* data() const { return output_.data(); }
  // Returns the size of the output buffer.
  int size() const { return output_size_; }

  // Computes the fully-connected layer output.
  void ComputeOutput(rtc::ArrayView<const float> input);

 private:
  const int input_size_;
  const int output_size_;
  const std::vector<float> bias_;
  const std::vector<float> weights_;
  const VectorMath vector_math_;
  rtc::FunctionView<float(float)> activation_function_;
  // Over-allocated array with size equal to `output_size_`.
  std::array<float, kFullyConnectedLayerMaxUnits> output_;
};

}  // namespace rnn_vad
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_AGC2_RNN_VAD_RNN_FC_H_
