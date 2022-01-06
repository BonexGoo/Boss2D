/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video__video_sink_interface_h //original-code:"api/video/video_sink_interface.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_config_h //original-code:"api/video_codecs/video_encoder_config.h"
#include BOSS_WEBRTC_U_call__rtp_config_h //original-code:"call/rtp_config.h"
#include BOSS_WEBRTC_U_call__video_receive_stream_h //original-code:"call/video_receive_stream.h"
#include BOSS_WEBRTC_U_call__video_send_stream_h //original-code:"call/video_send_stream.h"
#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"
#include BOSS_WEBRTC_U_test__frame_generator_capturer_h //original-code:"test/frame_generator_capturer.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"
#include BOSS_WEBRTC_U_video__end_to_end_tests__multi_stream_tester_h //original-code:"video/end_to_end_tests/multi_stream_tester.h"

namespace webrtc {
// Each renderer verifies that it receives the expected resolution, and as soon
// as every renderer has received a frame, the test finishes.
TEST(MultiStreamEndToEndTest, SendsAndReceivesMultipleStreams) {
  class VideoOutputObserver : public rtc::VideoSinkInterface<VideoFrame> {
   public:
    VideoOutputObserver(const MultiStreamTester::CodecSettings& settings,
                        uint32_t ssrc,
                        test::FrameGeneratorCapturer** frame_generator)
        : settings_(settings), ssrc_(ssrc), frame_generator_(frame_generator) {}

    void OnFrame(const VideoFrame& video_frame) override {
      EXPECT_EQ(settings_.width, video_frame.width());
      EXPECT_EQ(settings_.height, video_frame.height());
      (*frame_generator_)->Stop();
      done_.Set();
    }

    uint32_t Ssrc() { return ssrc_; }

    bool Wait() { return done_.Wait(30 * 1000); }

   private:
    const MultiStreamTester::CodecSettings& settings_;
    const uint32_t ssrc_;
    test::FrameGeneratorCapturer** const frame_generator_;
    rtc::Event done_;
  };

  class Tester : public MultiStreamTester {
   public:
    Tester() = default;
    ~Tester() override = default;

   protected:
    void Wait() override {
      for (const auto& observer : observers_) {
        EXPECT_TRUE(observer->Wait())
            << "Time out waiting for from on ssrc " << observer->Ssrc();
      }
    }

    void UpdateSendConfig(
        size_t stream_index,
        VideoSendStream::Config* send_config,
        VideoEncoderConfig* encoder_config,
        test::FrameGeneratorCapturer** frame_generator) override {
      observers_[stream_index] = std::make_unique<VideoOutputObserver>(
          codec_settings[stream_index], send_config->rtp.ssrcs.front(),
          frame_generator);
    }

    void UpdateReceiveConfig(
        size_t stream_index,
        VideoReceiveStream::Config* receive_config) override {
      receive_config->renderer = observers_[stream_index].get();
    }

   private:
    std::unique_ptr<VideoOutputObserver> observers_[kNumStreams];
  } tester;

  tester.RunTest();
}
}  // namespace webrtc
