/*
 *  Copyright 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import "RTCRtpReceiver.h"

#include BOSS_WEBRTC_U_api__rtp_receiver_interface_h //original-code:"api/rtp_receiver_interface.h"

NS_ASSUME_NONNULL_BEGIN

@class RTC_OBJC_TYPE(RTCPeerConnectionFactory);

namespace webrtc {

class RtpReceiverDelegateAdapter : public RtpReceiverObserverInterface {
 public:
  RtpReceiverDelegateAdapter(RTC_OBJC_TYPE(RTCRtpReceiver) * receiver);

  void OnFirstPacketReceived(cricket::MediaType media_type) override;

 private:
  __weak RTC_OBJC_TYPE(RTCRtpReceiver) * receiver_;
};

}  // namespace webrtc

@interface RTC_OBJC_TYPE (RTCRtpReceiver)
()

    @property(nonatomic,
              readonly) rtc::scoped_refptr<webrtc::RtpReceiverInterface> nativeRtpReceiver;

/** Initialize an RTCRtpReceiver with a native RtpReceiverInterface. */
- (instancetype)initWithFactory:(RTC_OBJC_TYPE(RTCPeerConnectionFactory) *)factory
              nativeRtpReceiver:(rtc::scoped_refptr<webrtc::RtpReceiverInterface>)nativeRtpReceiver
    NS_DESIGNATED_INITIALIZER;

+ (RTCRtpMediaType)mediaTypeForNativeMediaType:(cricket::MediaType)nativeMediaType;

+ (cricket::MediaType)nativeMediaTypeForMediaType:(RTCRtpMediaType)mediaType;

+ (NSString*)stringForMediaType:(RTCRtpMediaType)mediaType;

@end

NS_ASSUME_NONNULL_END
