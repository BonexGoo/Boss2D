/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <vector>

#include BOSS_WEBRTC_U_modules__audio_processing__residual_echo_detector_h //original-code:"modules/audio_processing/residual_echo_detector.h"
#include BOSS_WEBRTC_U_rtc_base__refcountedobject_h //original-code:"rtc_base/refcountedobject.h"
#include "test/gtest.h"

namespace webrtc {

TEST(ResidualEchoDetectorTests, Echo) {
  rtc::scoped_refptr<ResidualEchoDetector> echo_detector =
      new rtc::RefCountedObject<ResidualEchoDetector>();
  echo_detector->SetReliabilityForTest(1.0f);
  std::vector<float> ones(160, 1.f);
  std::vector<float> zeros(160, 0.f);

  // In this test the capture signal has a delay of 10 frames w.r.t. the render
  // signal, but is otherwise identical. Both signals are periodic with a 20
  // frame interval.
  for (int i = 0; i < 1000; i++) {
    if (i % 20 == 0) {
      echo_detector->AnalyzeRenderAudio(ones);
      echo_detector->AnalyzeCaptureAudio(zeros);
    } else if (i % 20 == 10) {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(ones);
    } else {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(zeros);
    }
  }
  // We expect to detect echo with near certain likelihood.
  auto ed_metrics = echo_detector->GetMetrics();
  EXPECT_NEAR(1.f, ed_metrics.echo_likelihood, 0.01f);
}

TEST(ResidualEchoDetectorTests, NoEcho) {
  rtc::scoped_refptr<ResidualEchoDetector> echo_detector =
      new rtc::RefCountedObject<ResidualEchoDetector>();
  echo_detector->SetReliabilityForTest(1.0f);
  std::vector<float> ones(160, 1.f);
  std::vector<float> zeros(160, 0.f);

  // In this test the capture signal is always zero, so no echo should be
  // detected.
  for (int i = 0; i < 1000; i++) {
    if (i % 20 == 0) {
      echo_detector->AnalyzeRenderAudio(ones);
    } else {
      echo_detector->AnalyzeRenderAudio(zeros);
    }
    echo_detector->AnalyzeCaptureAudio(zeros);
  }
  // We expect to not detect any echo.
  auto ed_metrics = echo_detector->GetMetrics();
  EXPECT_NEAR(0.f, ed_metrics.echo_likelihood, 0.01f);
}

TEST(ResidualEchoDetectorTests, EchoWithRenderClockDrift) {
  rtc::scoped_refptr<ResidualEchoDetector> echo_detector =
      new rtc::RefCountedObject<ResidualEchoDetector>();
  echo_detector->SetReliabilityForTest(1.0f);
  std::vector<float> ones(160, 1.f);
  std::vector<float> zeros(160, 0.f);

  // In this test the capture signal has a delay of 10 frames w.r.t. the render
  // signal, but is otherwise identical. Both signals are periodic with a 20
  // frame interval. There is a simulated clock drift of 1% in this test, with
  // the render side producing data slightly faster.
  for (int i = 0; i < 1000; i++) {
    if (i % 20 == 0) {
      echo_detector->AnalyzeRenderAudio(ones);
      echo_detector->AnalyzeCaptureAudio(zeros);
    } else if (i % 20 == 10) {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(ones);
    } else {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(zeros);
    }
    if (i % 100 == 0) {
      // This is causing the simulated clock drift.
      echo_detector->AnalyzeRenderAudio(zeros);
    }
  }
  // We expect to detect echo with high likelihood. Clock drift is harder to
  // correct on the render side than on the capture side. This is due to the
  // render buffer, clock drift can only be discovered after a certain delay.
  // A growing buffer can be caused by jitter or clock drift and it's not
  // possible to make this decision right away. For this reason we only expect
  // an echo likelihood of 75% in this test.
  auto ed_metrics = echo_detector->GetMetrics();
  EXPECT_GT(ed_metrics.echo_likelihood, 0.75f);
}

TEST(ResidualEchoDetectorTests, EchoWithCaptureClockDrift) {
  rtc::scoped_refptr<ResidualEchoDetector> echo_detector =
      new rtc::RefCountedObject<ResidualEchoDetector>();
  echo_detector->SetReliabilityForTest(1.0f);
  std::vector<float> ones(160, 1.f);
  std::vector<float> zeros(160, 0.f);

  // In this test the capture signal has a delay of 10 frames w.r.t. the render
  // signal, but is otherwise identical. Both signals are periodic with a 20
  // frame interval. There is a simulated clock drift of 1% in this test, with
  // the capture side producing data slightly faster.
  for (int i = 0; i < 1000; i++) {
    if (i % 20 == 0) {
      echo_detector->AnalyzeRenderAudio(ones);
      echo_detector->AnalyzeCaptureAudio(zeros);
    } else if (i % 20 == 10) {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(ones);
    } else {
      echo_detector->AnalyzeRenderAudio(zeros);
      echo_detector->AnalyzeCaptureAudio(zeros);
    }
    if (i % 100 == 0) {
      // This is causing the simulated clock drift.
      echo_detector->AnalyzeCaptureAudio(zeros);
    }
  }
  // We expect to detect echo with near certain likelihood.
  auto ed_metrics = echo_detector->GetMetrics();
  EXPECT_NEAR(1.f, ed_metrics.echo_likelihood, 0.01f);
}

}  // namespace webrtc
