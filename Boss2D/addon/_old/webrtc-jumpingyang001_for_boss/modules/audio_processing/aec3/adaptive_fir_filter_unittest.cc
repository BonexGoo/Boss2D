/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/audio_processing/aec3/adaptive_fir_filter.h"

// Defines WEBRTC_ARCH_X86_FAMILY, used below.
#include BOSS_WEBRTC_U_rtc_base__system__arch_h //original-code:"rtc_base/system/arch.h"

#include <math.h>
#include <algorithm>
#include <numeric>
#include <string>
#if defined(WEBRTC_ARCH_X86_FAMILY)
#include <emmintrin.h>
#endif
#include "modules/audio_processing/aec3/aec3_fft.h"
#include "modules/audio_processing/aec3/aec_state.h"
#include "modules/audio_processing/aec3/cascaded_biquad_filter.h"
#include "modules/audio_processing/aec3/render_delay_buffer.h"
#include "modules/audio_processing/aec3/render_signal_analyzer.h"
#include "modules/audio_processing/aec3/shadow_filter_update_gain.h"
#include BOSS_WEBRTC_U_modules__audio_processing__logging__apm_data_dumper_h //original-code:"modules/audio_processing/logging/apm_data_dumper.h"
#include "modules/audio_processing/test/echo_canceller_test_tools.h"
#include BOSS_WEBRTC_U_rtc_base__arraysize_h //original-code:"rtc_base/arraysize.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_minmax_h //original-code:"rtc_base/numerics/safe_minmax.h"
#include BOSS_WEBRTC_U_rtc_base__random_h //original-code:"rtc_base/random.h"
#include BOSS_WEBRTC_U_system_wrappers__include__cpu_features_wrapper_h //original-code:"system_wrappers/include/cpu_features_wrapper.h"
#include "test/gtest.h"

namespace webrtc {
namespace aec3 {
namespace {

std::string ProduceDebugText(size_t delay) {
  std::ostringstream ss;
  ss << ", Delay: " << delay;
  return ss.str();
}

}  // namespace

#if defined(WEBRTC_HAS_NEON)
// Verifies that the optimized methods for filter adaptation are similar to
// their reference counterparts.
TEST(AdaptiveFirFilter, FilterAdaptationNeonOptimizations) {
  std::unique_ptr<RenderDelayBuffer> render_delay_buffer(
      RenderDelayBuffer::Create(EchoCanceller3Config(), 3));
  Random random_generator(42U);
  std::vector<std::vector<float>> x(3, std::vector<float>(kBlockSize, 0.f));
  FftData S_C;
  FftData S_NEON;
  FftData G;
  Aec3Fft fft;
  std::vector<FftData> H_C(10);
  std::vector<FftData> H_NEON(10);
  for (auto& H_j : H_C) {
    H_j.Clear();
  }
  for (auto& H_j : H_NEON) {
    H_j.Clear();
  }

  for (size_t k = 0; k < 30; ++k) {
    RandomizeSampleVector(&random_generator, x[0]);
    render_delay_buffer->Insert(x);
    if (k == 0) {
      render_delay_buffer->Reset();
    }
    render_delay_buffer->PrepareCaptureProcessing();
  }
  auto* const render_buffer = render_delay_buffer->GetRenderBuffer();

  for (size_t j = 0; j < G.re.size(); ++j) {
    G.re[j] = j / 10001.f;
  }
  for (size_t j = 1; j < G.im.size() - 1; ++j) {
    G.im[j] = j / 20001.f;
  }
  G.im[0] = 0.f;
  G.im[G.im.size() - 1] = 0.f;

  AdaptPartitions_NEON(*render_buffer, G, H_NEON);
  AdaptPartitions(*render_buffer, G, H_C);
  AdaptPartitions_NEON(*render_buffer, G, H_NEON);
  AdaptPartitions(*render_buffer, G, H_C);

  for (size_t l = 0; l < H_C.size(); ++l) {
    for (size_t j = 0; j < H_C[l].im.size(); ++j) {
      EXPECT_NEAR(H_C[l].re[j], H_NEON[l].re[j], fabs(H_C[l].re[j] * 0.00001f));
      EXPECT_NEAR(H_C[l].im[j], H_NEON[l].im[j], fabs(H_C[l].im[j] * 0.00001f));
    }
  }

  ApplyFilter_NEON(*render_buffer, H_NEON, &S_NEON);
  ApplyFilter(*render_buffer, H_C, &S_C);
  for (size_t j = 0; j < S_C.re.size(); ++j) {
    EXPECT_NEAR(S_C.re[j], S_NEON.re[j], fabs(S_C.re[j] * 0.00001f));
    EXPECT_NEAR(S_C.im[j], S_NEON.im[j], fabs(S_C.re[j] * 0.00001f));
  }
}

// Verifies that the optimized method for frequency response computation is
// bitexact to the reference counterpart.
TEST(AdaptiveFirFilter, UpdateFrequencyResponseNeonOptimization) {
  const size_t kNumPartitions = 12;
  std::vector<FftData> H(kNumPartitions);
  std::vector<std::array<float, kFftLengthBy2Plus1>> H2(kNumPartitions);
  std::vector<std::array<float, kFftLengthBy2Plus1>> H2_NEON(kNumPartitions);

  for (size_t j = 0; j < H.size(); ++j) {
    for (size_t k = 0; k < H[j].re.size(); ++k) {
      H[j].re[k] = k + j / 3.f;
      H[j].im[k] = j + k / 7.f;
    }
  }

  UpdateFrequencyResponse(H, &H2);
  UpdateFrequencyResponse_NEON(H, &H2_NEON);

  for (size_t j = 0; j < H2.size(); ++j) {
    for (size_t k = 0; k < H[j].re.size(); ++k) {
      EXPECT_FLOAT_EQ(H2[j][k], H2_NEON[j][k]);
    }
  }
}

// Verifies that the optimized method for echo return loss computation is
// bitexact to the reference counterpart.
TEST(AdaptiveFirFilter, UpdateErlNeonOptimization) {
  const size_t kNumPartitions = 12;
  std::vector<std::array<float, kFftLengthBy2Plus1>> H2(kNumPartitions);
  std::array<float, kFftLengthBy2Plus1> erl;
  std::array<float, kFftLengthBy2Plus1> erl_NEON;

  for (size_t j = 0; j < H2.size(); ++j) {
    for (size_t k = 0; k < H2[j].size(); ++k) {
      H2[j][k] = k + j / 3.f;
    }
  }

  UpdateErlEstimator(H2, &erl);
  UpdateErlEstimator_NEON(H2, &erl_NEON);

  for (size_t j = 0; j < erl.size(); ++j) {
    EXPECT_FLOAT_EQ(erl[j], erl_NEON[j]);
  }
}

#endif

#if defined(WEBRTC_ARCH_X86_FAMILY)
// Verifies that the optimized methods for filter adaptation are bitexact to
// their reference counterparts.
TEST(AdaptiveFirFilter, FilterAdaptationSse2Optimizations) {
  bool use_sse2 = (WebRtc_GetCPUInfo(kSSE2) != 0);
  if (use_sse2) {
    std::unique_ptr<RenderDelayBuffer> render_delay_buffer(
        RenderDelayBuffer::Create(EchoCanceller3Config(), 3));
    Random random_generator(42U);
    std::vector<std::vector<float>> x(3, std::vector<float>(kBlockSize, 0.f));
    FftData S_C;
    FftData S_SSE2;
    FftData G;
    Aec3Fft fft;
    std::vector<FftData> H_C(10);
    std::vector<FftData> H_SSE2(10);
    for (auto& H_j : H_C) {
      H_j.Clear();
    }
    for (auto& H_j : H_SSE2) {
      H_j.Clear();
    }

    for (size_t k = 0; k < 500; ++k) {
      RandomizeSampleVector(&random_generator, x[0]);
      render_delay_buffer->Insert(x);
      if (k == 0) {
        render_delay_buffer->Reset();
      }
      render_delay_buffer->PrepareCaptureProcessing();
      auto* const render_buffer = render_delay_buffer->GetRenderBuffer();

      ApplyFilter_SSE2(*render_buffer, H_SSE2, &S_SSE2);
      ApplyFilter(*render_buffer, H_C, &S_C);
      for (size_t j = 0; j < S_C.re.size(); ++j) {
        EXPECT_FLOAT_EQ(S_C.re[j], S_SSE2.re[j]);
        EXPECT_FLOAT_EQ(S_C.im[j], S_SSE2.im[j]);
      }

      std::for_each(G.re.begin(), G.re.end(),
                    [&](float& a) { a = random_generator.Rand<float>(); });
      std::for_each(G.im.begin(), G.im.end(),
                    [&](float& a) { a = random_generator.Rand<float>(); });

      AdaptPartitions_SSE2(*render_buffer, G, H_SSE2);
      AdaptPartitions(*render_buffer, G, H_C);

      for (size_t k = 0; k < H_C.size(); ++k) {
        for (size_t j = 0; j < H_C[k].re.size(); ++j) {
          EXPECT_FLOAT_EQ(H_C[k].re[j], H_SSE2[k].re[j]);
          EXPECT_FLOAT_EQ(H_C[k].im[j], H_SSE2[k].im[j]);
        }
      }
    }
  }
}

// Verifies that the optimized method for frequency response computation is
// bitexact to the reference counterpart.
TEST(AdaptiveFirFilter, UpdateFrequencyResponseSse2Optimization) {
  bool use_sse2 = (WebRtc_GetCPUInfo(kSSE2) != 0);
  if (use_sse2) {
    const size_t kNumPartitions = 12;
    std::vector<FftData> H(kNumPartitions);
    std::vector<std::array<float, kFftLengthBy2Plus1>> H2(kNumPartitions);
    std::vector<std::array<float, kFftLengthBy2Plus1>> H2_SSE2(kNumPartitions);

    for (size_t j = 0; j < H.size(); ++j) {
      for (size_t k = 0; k < H[j].re.size(); ++k) {
        H[j].re[k] = k + j / 3.f;
        H[j].im[k] = j + k / 7.f;
      }
    }

    UpdateFrequencyResponse(H, &H2);
    UpdateFrequencyResponse_SSE2(H, &H2_SSE2);

    for (size_t j = 0; j < H2.size(); ++j) {
      for (size_t k = 0; k < H[j].re.size(); ++k) {
        EXPECT_FLOAT_EQ(H2[j][k], H2_SSE2[j][k]);
      }
    }
  }
}

// Verifies that the optimized method for echo return loss computation is
// bitexact to the reference counterpart.
TEST(AdaptiveFirFilter, UpdateErlSse2Optimization) {
  bool use_sse2 = (WebRtc_GetCPUInfo(kSSE2) != 0);
  if (use_sse2) {
    const size_t kNumPartitions = 12;
    std::vector<std::array<float, kFftLengthBy2Plus1>> H2(kNumPartitions);
    std::array<float, kFftLengthBy2Plus1> erl;
    std::array<float, kFftLengthBy2Plus1> erl_SSE2;

    for (size_t j = 0; j < H2.size(); ++j) {
      for (size_t k = 0; k < H2[j].size(); ++k) {
        H2[j][k] = k + j / 3.f;
      }
    }

    UpdateErlEstimator(H2, &erl);
    UpdateErlEstimator_SSE2(H2, &erl_SSE2);

    for (size_t j = 0; j < erl.size(); ++j) {
      EXPECT_FLOAT_EQ(erl[j], erl_SSE2[j]);
    }
  }
}

#endif

#if RTC_DCHECK_IS_ON && GTEST_HAS_DEATH_TEST && !defined(WEBRTC_ANDROID)
// Verifies that the check for non-null data dumper works.
TEST(AdaptiveFirFilter, NullDataDumper) {
  EXPECT_DEATH(AdaptiveFirFilter(9, 9, 250, DetectOptimization(), nullptr), "");
}

// Verifies that the check for non-null filter output works.
TEST(AdaptiveFirFilter, NullFilterOutput) {
  ApmDataDumper data_dumper(42);
  AdaptiveFirFilter filter(9, 9, 250, DetectOptimization(), &data_dumper);
  std::unique_ptr<RenderDelayBuffer> render_delay_buffer(
      RenderDelayBuffer::Create(EchoCanceller3Config(), 3));
  EXPECT_DEATH(filter.Filter(*render_delay_buffer->GetRenderBuffer(), nullptr),
               "");
}

#endif

// Verifies that the filter statistics can be accessed when filter statistics
// are turned on.
TEST(AdaptiveFirFilter, FilterStatisticsAccess) {
  ApmDataDumper data_dumper(42);
  AdaptiveFirFilter filter(9, 9, 250, DetectOptimization(), &data_dumper);
  filter.Erl();
  filter.FilterFrequencyResponse();
}

// Verifies that the filter size if correctly repported.
TEST(AdaptiveFirFilter, FilterSize) {
  ApmDataDumper data_dumper(42);
  for (size_t filter_size = 1; filter_size < 5; ++filter_size) {
    AdaptiveFirFilter filter(filter_size, filter_size, 250,
                             DetectOptimization(), &data_dumper);
    EXPECT_EQ(filter_size, filter.SizePartitions());
  }
}

// Verifies that the filter is being able to properly filter a signal and to
// adapt its coefficients.
TEST(AdaptiveFirFilter, FilterAndAdapt) {
  constexpr size_t kNumBlocksToProcess = 1000;
  ApmDataDumper data_dumper(42);
  EchoCanceller3Config config;
  AdaptiveFirFilter filter(config.filter.main.length_blocks,
                           config.filter.main.length_blocks,
                           config.filter.config_change_duration_blocks,
                           DetectOptimization(), &data_dumper);
  Aec3Fft fft;
  config.delay.min_echo_path_delay_blocks = 0;
  config.delay.default_delay = 1;
  std::unique_ptr<RenderDelayBuffer> render_delay_buffer(
      RenderDelayBuffer::Create(config, 3));
  ShadowFilterUpdateGain gain(config.filter.shadow,
                              config.filter.config_change_duration_blocks);
  Random random_generator(42U);
  std::vector<std::vector<float>> x(3, std::vector<float>(kBlockSize, 0.f));
  std::vector<float> n(kBlockSize, 0.f);
  std::vector<float> y(kBlockSize, 0.f);
  AecState aec_state(EchoCanceller3Config{});
  RenderSignalAnalyzer render_signal_analyzer(config);
  absl::optional<DelayEstimate> delay_estimate;
  std::vector<float> e(kBlockSize, 0.f);
  std::array<float, kFftLength> s_scratch;
  SubtractorOutput output;
  FftData S;
  FftData G;
  FftData E;
  std::array<float, kFftLengthBy2Plus1> Y2;
  std::array<float, kFftLengthBy2Plus1> E2_main;
  std::array<float, kFftLengthBy2Plus1> E2_shadow;
  // [B,A] = butter(2,100/8000,'high')
  constexpr CascadedBiQuadFilter::BiQuadCoefficients
      kHighPassFilterCoefficients = {{0.97261f, -1.94523f, 0.97261f},
                                     {-1.94448f, 0.94598f}};
  Y2.fill(0.f);
  E2_main.fill(0.f);
  E2_shadow.fill(0.f);
  output.Reset();

  constexpr float kScale = 1.0f / kFftLengthBy2;

  for (size_t delay_samples : {0, 64, 150, 200, 301}) {
    DelayBuffer<float> delay_buffer(delay_samples);
    CascadedBiQuadFilter x_hp_filter(kHighPassFilterCoefficients, 1);
    CascadedBiQuadFilter y_hp_filter(kHighPassFilterCoefficients, 1);

    SCOPED_TRACE(ProduceDebugText(delay_samples));
    for (size_t j = 0; j < kNumBlocksToProcess; ++j) {
      RandomizeSampleVector(&random_generator, x[0]);
      delay_buffer.Delay(x[0], y);

      RandomizeSampleVector(&random_generator, n);
      static constexpr float kNoiseScaling = 1.f / 100.f;
      std::transform(y.begin(), y.end(), n.begin(), y.begin(),
                     [](float a, float b) { return a + b * kNoiseScaling; });

      x_hp_filter.Process(x[0]);
      y_hp_filter.Process(y);

      render_delay_buffer->Insert(x);
      if (j == 0) {
        render_delay_buffer->Reset();
      }
      render_delay_buffer->PrepareCaptureProcessing();
      auto* const render_buffer = render_delay_buffer->GetRenderBuffer();

      render_signal_analyzer.Update(*render_buffer,
                                    aec_state.FilterDelayBlocks());

      filter.Filter(*render_buffer, &S);
      fft.Ifft(S, &s_scratch);
      std::transform(y.begin(), y.end(), s_scratch.begin() + kFftLengthBy2,
                     e.begin(),
                     [&](float a, float b) { return a - b * kScale; });
      std::for_each(e.begin(), e.end(),
                    [](float& a) { a = rtc::SafeClamp(a, -32768.f, 32767.f); });
      fft.ZeroPaddedFft(e, Aec3Fft::Window::kRectangular, &E);
      for (size_t k = 0; k < kBlockSize; ++k) {
        output.s_main[k] = kScale * s_scratch[k + kFftLengthBy2];
      }

      std::array<float, kFftLengthBy2Plus1> render_power;
      render_buffer->SpectralSum(filter.SizePartitions(), &render_power);
      gain.Compute(render_power, render_signal_analyzer, E,
                   filter.SizePartitions(), false, &G);
      filter.Adapt(*render_buffer, G);
      aec_state.HandleEchoPathChange(EchoPathVariability(
          false, EchoPathVariability::DelayAdjustment::kNone, false));

      aec_state.Update(delay_estimate, filter.FilterFrequencyResponse(),
                       filter.FilterImpulseResponse(), *render_buffer, E2_main,
                       Y2, output, y);
    }
    // Verify that the filter is able to perform well.
    EXPECT_LT(1000 * std::inner_product(e.begin(), e.end(), e.begin(), 0.f),
              std::inner_product(y.begin(), y.end(), y.begin(), 0.f));
  }
}
}  // namespace aec3
}  // namespace webrtc
