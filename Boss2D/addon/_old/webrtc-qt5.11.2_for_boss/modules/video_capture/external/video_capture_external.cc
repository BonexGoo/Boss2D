/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__video_capture__video_capture_impl_h //original-code:"modules/video_capture/video_capture_impl.h"
#include BOSS_WEBRTC_U_rtc_base__refcount_h //original-code:"rtc_base/refcount.h"
#include BOSS_WEBRTC_U_rtc_base__refcountedobject_h //original-code:"rtc_base/refcountedobject.h"

namespace webrtc {

namespace videocapturemodule {

rtc::scoped_refptr<VideoCaptureModule> VideoCaptureImpl::Create(
    const char* deviceUniqueIdUTF8) {
  return new rtc::RefCountedObject<VideoCaptureImpl>();
}

}  // namespace videocapturemodule

}  // namespace webrtc
