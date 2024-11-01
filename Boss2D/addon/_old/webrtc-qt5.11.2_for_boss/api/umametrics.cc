/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__umametrics_h //original-code:"api/umametrics.h"

namespace webrtc {

void MetricsObserverInterface::IncrementSparseEnumCounter(
    PeerConnectionEnumCounterType type,
    int counter) {
  IncrementEnumCounter(type, counter, 0 /* Ignored */);
}

}  // namespace webrtc
