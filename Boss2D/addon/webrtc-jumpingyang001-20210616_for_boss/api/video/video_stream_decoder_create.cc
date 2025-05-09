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

#include <memory>

#include BOSS_WEBRTC_U_video__video_stream_decoder_impl_h //original-code:"video/video_stream_decoder_impl.h"

namespace webrtc {

std::unique_ptr<VideoStreamDecoderInterface> CreateVideoStreamDecoder(
    VideoStreamDecoderInterface::Callbacks* callbacks,
    VideoDecoderFactory* decoder_factory,
    TaskQueueFactory* task_queue_factory,
    std::map<int, std::pair<SdpVideoFormat, int>> decoder_settings) {
  return std::make_unique<VideoStreamDecoderImpl>(callbacks, decoder_factory,
                                                  task_queue_factory,
                                                  std::move(decoder_settings));
}

}  // namespace webrtc
