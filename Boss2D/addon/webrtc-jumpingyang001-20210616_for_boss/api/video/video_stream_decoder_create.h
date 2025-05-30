/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_VIDEO_VIDEO_STREAM_DECODER_CREATE_H_
#define API_VIDEO_VIDEO_STREAM_DECODER_CREATE_H_

#include <map>
#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_api__task_queue__task_queue_factory_h //original-code:"api/task_queue/task_queue_factory.h"
#include BOSS_WEBRTC_U_api__video__video_stream_decoder_h //original-code:"api/video/video_stream_decoder.h"
#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"

namespace webrtc {
// The |decoder_settings| parameter is a map between:
// <payload type> -->  <<video format>, <number of cores>>.
// The video format is used when instantiating a decoder, and
// the number of cores is used when initializing the decoder.
std::unique_ptr<VideoStreamDecoderInterface> CreateVideoStreamDecoder(
    VideoStreamDecoderInterface::Callbacks* callbacks,
    VideoDecoderFactory* decoder_factory,
    TaskQueueFactory* task_queue_factory,
    std::map<int, std::pair<SdpVideoFormat, int>> decoder_settings);

}  // namespace webrtc

#endif  // API_VIDEO_VIDEO_STREAM_DECODER_CREATE_H_
