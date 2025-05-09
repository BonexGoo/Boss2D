/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import "RTCStatisticsReport.h"

#include BOSS_WEBRTC_U_api__stats__rtc_stats_report_h //original-code:"api/stats/rtc_stats_report.h"

@interface RTC_OBJC_TYPE (RTCStatisticsReport) (Private)

- (instancetype)initWithReport : (const webrtc::RTCStatsReport &)report;

@end
