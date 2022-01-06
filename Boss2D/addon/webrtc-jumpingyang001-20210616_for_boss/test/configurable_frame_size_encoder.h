/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_CONFIGURABLE_FRAME_SIZE_ENCODER_H_
#define TEST_CONFIGURABLE_FRAME_SIZE_ENCODER_H_

#include <stddef.h>
#include <stdint.h>

#include <functional>
#include <memory>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__video__video_bitrate_allocation_h //original-code:"api/video/video_bitrate_allocation.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"

namespace webrtc {
namespace test {

class ConfigurableFrameSizeEncoder : public VideoEncoder {
 public:
  explicit ConfigurableFrameSizeEncoder(size_t max_frame_size);
  ~ConfigurableFrameSizeEncoder() override;

  void SetFecControllerOverride(
      FecControllerOverride* fec_controller_override) override;

  int32_t InitEncode(const VideoCodec* codec_settings,
                     const Settings& settings) override;

  int32_t Encode(const VideoFrame& input_image,
                 const std::vector<VideoFrameType>* frame_types) override;

  int32_t RegisterEncodeCompleteCallback(
      EncodedImageCallback* callback) override;

  int32_t Release() override;

  void SetRates(const RateControlParameters& parameters) override;

  int32_t SetFrameSize(size_t size);

  void SetCodecType(VideoCodecType codec_type_);

  void RegisterPostEncodeCallback(
      std::function<void(void)> post_encode_callback);

 private:
  EncodedImageCallback* callback_;
  absl::optional<std::function<void(void)>> post_encode_callback_;

  size_t current_frame_size_;
  VideoCodecType codec_type_;
};

}  // namespace test
}  // namespace webrtc

#endif  // TEST_CONFIGURABLE_FRAME_SIZE_ENCODER_H_
