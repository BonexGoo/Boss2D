/*
 *  Copyright 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import "RTCRtpSender.h"

#include BOSS_WEBRTC_U_api__rtp_sender_interface_h //original-code:"api/rtp_sender_interface.h"

NS_ASSUME_NONNULL_BEGIN

@class RTC_OBJC_TYPE(RTCPeerConnectionFactory);

@interface RTC_OBJC_TYPE (RTCRtpSender)
()

    @property(nonatomic, readonly) rtc::scoped_refptr<webrtc::RtpSenderInterface> nativeRtpSender;

/** Initialize an RTCRtpSender with a native RtpSenderInterface. */
- (instancetype)initWithFactory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory
                nativeRtpSender:(rtc::scoped_refptr<webrtc::RtpSenderInterface>)nativeRtpSender
    NS_DESIGNATED_INITIALIZER;

@end

NS_ASSUME_NONNULL_END