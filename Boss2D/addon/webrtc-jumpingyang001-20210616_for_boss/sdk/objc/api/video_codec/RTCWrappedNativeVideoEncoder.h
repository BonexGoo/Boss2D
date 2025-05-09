/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#import <Foundation/Foundation.h>

#import "base/RTCMacros.h"
#import "base/RTCVideoEncoder.h"

#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_media__base__codec_h //original-code:"media/base/codec.h"

@interface RTC_OBJC_TYPE (RTCWrappedNativeVideoEncoder) : NSObject <RTC_OBJC_TYPE (RTCVideoEncoder)>

- (instancetype)initWithNativeEncoder:(std::unique_ptr<webrtc::VideoEncoder>)encoder;

/* This moves the ownership of the wrapped encoder to the caller. */
- (std::unique_ptr<webrtc::VideoEncoder>)releaseWrappedEncoder;

@end
