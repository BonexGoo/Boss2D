/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_FUNCTION_VIDEO_DECODER_FACTORY_H_
#define TEST_FUNCTION_VIDEO_DECODER_FACTORY_H_

#include <functional>
#include <memory>
#include <utility>
#include <vector>

#include BOSS_WEBRTC_U_api__video_codecs__sdp_video_format_h //original-code:"api/video_codecs/sdp_video_format.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_factory_h //original-code:"api/video_codecs/video_decoder_factory.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {
namespace test {

// A decoder factory producing decoders by calling a supplied create function.
class FunctionVideoDecoderFactory final : public VideoDecoderFactory {
 public:
  explicit FunctionVideoDecoderFactory(
      std::function<std::unique_ptr<VideoDecoder>()> create)
      : create_(std::move(create)) {}

  // Unused by tests.
  std::vector<SdpVideoFormat> GetSupportedFormats() const override {
    RTC_NOTREACHED();
    return {};
  }

  std::unique_ptr<VideoDecoder> CreateVideoDecoder(
      const SdpVideoFormat& /* format */) override {
    return create_();
  }

 private:
  const std::function<std::unique_ptr<VideoDecoder>()> create_;
};

}  // namespace test
}  // namespace webrtc

#endif  // TEST_FUNCTION_VIDEO_DECODER_FACTORY_H_
