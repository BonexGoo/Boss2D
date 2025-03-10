/*
 *  Copyright 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__video_track_source_proxy_h //original-code:"pc/video_track_source_proxy.h"

#include BOSS_WEBRTC_U_api__media_stream_interface_h //original-code:"api/media_stream_interface.h"
#include BOSS_WEBRTC_U_api__video_track_source_proxy_factory_h //original-code:"api/video_track_source_proxy_factory.h"

namespace webrtc {

rtc::scoped_refptr<VideoTrackSourceInterface> CreateVideoTrackSourceProxy(
    rtc::Thread* signaling_thread,
    rtc::Thread* worker_thread,
    VideoTrackSourceInterface* source) {
  return VideoTrackSourceProxy::Create(signaling_thread, worker_thread, source);
}

}  // namespace webrtc
