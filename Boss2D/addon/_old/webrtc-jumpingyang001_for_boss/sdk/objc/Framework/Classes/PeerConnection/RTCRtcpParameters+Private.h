/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import "WebRTC/RTCRtcpParameters.h"

#include BOSS_WEBRTC_U_api__rtpparameters_h //original-code:"api/rtpparameters.h"

NS_ASSUME_NONNULL_BEGIN

@interface RTCRtcpParameters ()

/** Returns the equivalent native RtcpParameters structure. */
@property(nonatomic, readonly) webrtc::RtcpParameters nativeParameters;

/** Initialize the object with a native RtcpParameters structure. */
- (instancetype)initWithNativeParameters:(const webrtc::RtcpParameters &)nativeParameters;

@end

NS_ASSUME_NONNULL_END
