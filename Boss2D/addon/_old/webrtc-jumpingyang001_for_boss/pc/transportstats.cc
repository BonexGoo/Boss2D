/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include BOSS_WEBRTC_U_pc__transportstats_h //original-code:"pc/transportstats.h"

namespace cricket {

TransportChannelStats::TransportChannelStats() = default;

TransportChannelStats::TransportChannelStats(const TransportChannelStats&) =
    default;

TransportChannelStats::~TransportChannelStats() = default;

TransportStats::TransportStats() = default;

TransportStats::~TransportStats() = default;

}  // namespace cricket
