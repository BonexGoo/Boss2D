/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_PC_E2E_CROSS_MEDIA_METRICS_REPORTER_H_
#define TEST_PC_E2E_CROSS_MEDIA_METRICS_REPORTER_H_

#include <map>
#include <string>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__numerics__samples_stats_counter_h //original-code:"api/numerics/samples_stats_counter.h"
#include BOSS_WEBRTC_U_api__test__peerconnection_quality_test_fixture_h //original-code:"api/test/peerconnection_quality_test_fixture.h"
#include BOSS_WEBRTC_U_api__test__track_id_stream_info_map_h //original-code:"api/test/track_id_stream_info_map.h"
#include BOSS_WEBRTC_U_api__units__timestamp_h //original-code:"api/units/timestamp.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include "test/testsupport/perf_test.h"

namespace webrtc {
namespace webrtc_pc_e2e {

class CrossMediaMetricsReporter
    : public PeerConnectionE2EQualityTestFixture::QualityMetricsReporter {
 public:
  CrossMediaMetricsReporter() = default;
  ~CrossMediaMetricsReporter() override = default;

  void Start(absl::string_view test_case_name,
             const TrackIdStreamInfoMap* reporter_helper) override;
  void OnStatsReports(
      absl::string_view pc_label,
      const rtc::scoped_refptr<const RTCStatsReport>& report) override;
  void StopAndReportResults() override;

 private:
  struct StatsInfo {
    SamplesStatsCounter audio_ahead_ms;
    SamplesStatsCounter video_ahead_ms;

    std::string audio_stream_label;
    std::string video_stream_label;
  };

  static void ReportResult(const std::string& metric_name,
                           const std::string& test_case_name,
                           const SamplesStatsCounter& counter,
                           const std::string& unit,
                           webrtc::test::ImproveDirection improve_direction =
                               webrtc::test::ImproveDirection::kNone);
  std::string GetTestCaseName(const std::string& stream_label,
                              const std::string& sync_group) const;

  std::string test_case_name_;
  const TrackIdStreamInfoMap* reporter_helper_;

  Mutex mutex_;
  std::map<std::string, StatsInfo> stats_info_ RTC_GUARDED_BY(mutex_);
};

}  // namespace webrtc_pc_e2e
}  // namespace webrtc

#endif  // TEST_PC_E2E_CROSS_MEDIA_METRICS_REPORTER_H_
