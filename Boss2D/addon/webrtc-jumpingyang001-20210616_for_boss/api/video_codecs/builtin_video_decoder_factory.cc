/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__video_codecs__builtin_video_decoder_factory_h //original-code:"api/video_codecs/builtin_video_decoder_factory.h"

#include <memory>

#include BOSS_WEBRTC_U_media__engine__internal_decoder_factory_h //original-code:"media/engine/internal_decoder_factory.h"

namespace webrtc {

std::unique_ptr<VideoDecoderFactory> CreateBuiltinVideoDecoderFactory() {
  return std::make_unique<InternalDecoderFactory>();
}

}  // namespace webrtc
