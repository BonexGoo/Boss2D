/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_TEST_STATS_OBSERVER_INTERFACE_H_
#define API_TEST_STATS_OBSERVER_INTERFACE_H_

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__stats__rtc_stats_report_h //original-code:"api/stats/rtc_stats_report.h"

namespace webrtc {
namespace webrtc_pc_e2e {

// API is in development and can be changed without notice.
class StatsObserverInterface {
 public:
  virtual ~StatsObserverInterface() = default;

  // Method called when stats reports are available for the PeerConnection
  // identified by |pc_label|.
  virtual void OnStatsReports(
      absl::string_view pc_label,
      const rtc::scoped_refptr<const RTCStatsReport>& report) = 0;
};

}  // namespace webrtc_pc_e2e
}  // namespace webrtc

#endif  // API_TEST_STATS_OBSERVER_INTERFACE_H_
