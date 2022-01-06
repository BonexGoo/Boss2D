/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_REMOTE_BITRATE_ESTIMATOR_INCLUDE_BWE_DEFINES_H_
#define MODULES_REMOTE_BITRATE_ESTIMATOR_INCLUDE_BWE_DEFINES_H_

#include <stdint.h>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__network_state_predictor_h //original-code:"api/network_state_predictor.h"
#include BOSS_WEBRTC_U_api__units__data_rate_h //original-code:"api/units/data_rate.h"

namespace webrtc {

namespace congestion_controller {
int GetMinBitrateBps();
DataRate GetMinBitrate();
}  // namespace congestion_controller

static const int64_t kBitrateWindowMs = 1000;

extern const char kBweTypeHistogram[];

enum BweNames {
  kReceiverNoExtension = 0,
  kReceiverTOffset = 1,
  kReceiverAbsSendTime = 2,
  kSendSideTransportSeqNum = 3,
  kBweNamesMax = 4
};

struct RateControlInput {
  RateControlInput(BandwidthUsage bw_state,
                   const absl::optional<DataRate>& estimated_throughput);
  ~RateControlInput();

  BandwidthUsage bw_state;
  absl::optional<DataRate> estimated_throughput;
};
}  // namespace webrtc

#endif  // MODULES_REMOTE_BITRATE_ESTIMATOR_INCLUDE_BWE_DEFINES_H_
