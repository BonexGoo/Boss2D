/*
 *  Copyright 2016 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_STATS_RTC_STATS_COLLECTOR_CALLBACK_H_
#define API_STATS_RTC_STATS_COLLECTOR_CALLBACK_H_

#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__stats__rtc_stats_report_h //original-code:"api/stats/rtc_stats_report.h"
#include BOSS_WEBRTC_U_rtc_base__ref_count_h //original-code:"rtc_base/ref_count.h"

namespace webrtc {

class RTCStatsCollectorCallback : public rtc::RefCountInterface {
 public:
  ~RTCStatsCollectorCallback() override = default;

  virtual void OnStatsDelivered(
      const rtc::scoped_refptr<const RTCStatsReport>& report) = 0;
};

}  // namespace webrtc

#endif  // API_STATS_RTC_STATS_COLLECTOR_CALLBACK_H_
