/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_p2p__base__basicasyncresolverfactory_h //original-code:"p2p/base/basicasyncresolverfactory.h"

#include BOSS_WEBRTC_U_rtc_base__nethelpers_h //original-code:"rtc_base/nethelpers.h"

namespace webrtc {

rtc::AsyncResolverInterface* BasicAsyncResolverFactory::Create() {
  return new rtc::AsyncResolver();
}

}  // namespace webrtc
