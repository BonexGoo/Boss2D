/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_VIDEOSINKINTERFACE_H_
#define API_VIDEOSINKINTERFACE_H_

#include BOSS_WEBRTC_V_rtc_base__checks_h //original-code:<rtc_base/checks.h>

namespace rtc {

template <typename VideoFrameT>
class VideoSinkInterface {
 public:
  virtual ~VideoSinkInterface() {}

  virtual void OnFrame(const VideoFrameT& frame) = 0;

  // Should be called by the source when it discards the frame due to rate
  // limiting.
  virtual void OnDiscardedFrame() {}
};

}  // namespace rtc

#endif  // API_VIDEOSINKINTERFACE_H_
