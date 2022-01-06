// author BOSS

/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__video__video_stream_decoder_create_h //original-code:"api/video/video_stream_decoder_create.h"

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_video__video_stream_decoder_impl_h //original-code:"video/video_stream_decoder_impl.h"

namespace webrtc {
std::unique_ptr<VideoStreamDecoder_api_BOSS> CreateVideoStreamDecoder( // modified by BOSS, original-code: VideoStreamDecoder, replace-code: VideoStreamDecoder_api_BOSS
    VideoStreamDecoder_api_BOSS::Callbacks* callbacks, // modified by BOSS, original-code: VideoStreamDecoder, replace-code: VideoStreamDecoder_api_BOSS
    VideoDecoderFactory* decoder_factory,
    std::map<int, std::pair<SdpVideoFormat, int>> decoder_settings) {
  return absl::make_unique<VideoStreamDecoderImpl>(callbacks, decoder_factory,
                                                   std::move(decoder_settings));
}
}  // namespace webrtc
