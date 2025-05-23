/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_MAC_VIDEO_RENDERER_MAC_H_
#define TEST_MAC_VIDEO_RENDERER_MAC_H_

#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include "test/gl/gl_renderer.h"

@class CocoaWindow;

namespace webrtc {
namespace test {

class MacRenderer : public GlRenderer {
 public:
  MacRenderer();
  virtual ~MacRenderer();

  bool Init(const char* window_title, int width, int height);

  // Implements GlRenderer.
  void OnFrame(const VideoFrame& frame) override;

 private:
  CocoaWindow* window_;

  RTC_DISALLOW_COPY_AND_ASSIGN(MacRenderer);
};
}  // namespace test
}  // namespace webrtc

#endif  // TEST_MAC_VIDEO_RENDERER_MAC_H_
