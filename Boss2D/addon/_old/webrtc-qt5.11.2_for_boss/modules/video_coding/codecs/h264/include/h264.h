/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 *
 */

#ifndef MODULES_VIDEO_CODING_CODECS_H264_INCLUDE_H264_H_
#define MODULES_VIDEO_CODING_CODECS_H264_INCLUDE_H264_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_media__base__codec_h //original-code:"media/base/codec.h"
#include "modules/video_coding/include/video_codec_interface.h"

namespace webrtc {

struct SdpVideoFormat;

// Set to disable the H.264 encoder/decoder implementations that are provided if
// |rtc_use_h264| build flag is true (if false, this function does nothing).
// This function should only be called before or during WebRTC initialization
// and is not thread-safe.
void DisableRtcUseH264();

// Returns a vector with all supported internal H264 profiles that we can
// negotiate in SDP, in order of preference.
std::vector<SdpVideoFormat> SupportedH264Codecs();

class H264Encoder : public VideoEncoder {
 public:
  static std::unique_ptr<H264Encoder> Create(const cricket::VideoCodec& codec);
  // If H.264 is supported (any implementation).
  static bool IsSupported();

  ~H264Encoder() override {}
};

class H264Decoder : public VideoDecoder {
 public:
  static std::unique_ptr<H264Decoder> Create();
  static bool IsSupported();

  ~H264Decoder() override {}
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_H264_INCLUDE_H264_H_
