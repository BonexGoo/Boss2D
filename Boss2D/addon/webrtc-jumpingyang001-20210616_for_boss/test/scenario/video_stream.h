/*
 *  Copyright 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef TEST_SCENARIO_VIDEO_STREAM_H_
#define TEST_SCENARIO_VIDEO_STREAM_H_
#include <memory>
#include <string>
#include <vector>

#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_test__fake_encoder_h //original-code:"test/fake_encoder.h"
#include BOSS_WEBRTC_U_test__fake_videorenderer_h //original-code:"test/fake_videorenderer.h"
#include BOSS_WEBRTC_U_test__frame_generator_capturer_h //original-code:"test/frame_generator_capturer.h"
#include "test/logging/log_writer.h"
#include "test/scenario/call_client.h"
#include "test/scenario/column_printer.h"
#include "test/scenario/network_node.h"
#include "test/scenario/scenario_config.h"
#include "test/scenario/video_frame_matcher.h"
#include BOSS_WEBRTC_U_test__test_video_capturer_h //original-code:"test/test_video_capturer.h"

namespace webrtc {
namespace test {
// SendVideoStream provides an interface for changing parameters and retrieving
// states at run time.
class SendVideoStream {
 public:
  RTC_DISALLOW_COPY_AND_ASSIGN(SendVideoStream);
  ~SendVideoStream();
  void SetCaptureFramerate(int framerate);
  VideoSendStream::Stats GetStats() const;
  ColumnPrinter StatsPrinter();
  void Start();
  void Stop();
  void UpdateConfig(std::function<void(VideoStreamConfig*)> modifier);
  void UpdateActiveLayers(std::vector<bool> active_layers);
  bool UsingSsrc(uint32_t ssrc) const;
  bool UsingRtxSsrc(uint32_t ssrc) const;

 private:
  friend class Scenario;
  friend class VideoStreamPair;
  friend class ReceiveVideoStream;
  // Handles RTCP feedback for this stream.
  SendVideoStream(CallClient* sender,
                  VideoStreamConfig config,
                  Transport* send_transport,
                  VideoFrameMatcher* matcher);

  Mutex mutex_;
  std::vector<uint32_t> ssrcs_;
  std::vector<uint32_t> rtx_ssrcs_;
  VideoSendStream* send_stream_ = nullptr;
  CallClient* const sender_;
  VideoStreamConfig config_ RTC_GUARDED_BY(mutex_);
  std::unique_ptr<VideoEncoderFactory> encoder_factory_;
  std::vector<test::FakeEncoder*> fake_encoders_ RTC_GUARDED_BY(mutex_);
  std::unique_ptr<VideoBitrateAllocatorFactory> bitrate_allocator_factory_;
  std::unique_ptr<FrameGeneratorCapturer> video_capturer_;
  std::unique_ptr<ForwardingCapturedFrameTap> frame_tap_;
  int next_local_network_id_ = 0;
  int next_remote_network_id_ = 0;
};

// ReceiveVideoStream represents a video receiver. It can't be used directly.
class ReceiveVideoStream {
 public:
  RTC_DISALLOW_COPY_AND_ASSIGN(ReceiveVideoStream);
  ~ReceiveVideoStream();
  void Start();
  void Stop();
  VideoReceiveStream::Stats GetStats() const;

 private:
  friend class Scenario;
  friend class VideoStreamPair;
  ReceiveVideoStream(CallClient* receiver,
                     VideoStreamConfig config,
                     SendVideoStream* send_stream,
                     size_t chosen_stream,
                     Transport* feedback_transport,
                     VideoFrameMatcher* matcher);

  std::vector<VideoReceiveStream*> receive_streams_;
  FlexfecReceiveStream* flecfec_stream_ = nullptr;
  FakeVideoRenderer fake_renderer_;
  std::vector<std::unique_ptr<rtc::VideoSinkInterface<VideoFrame>>>
      render_taps_;
  CallClient* const receiver_;
  const VideoStreamConfig config_;
  std::unique_ptr<VideoDecoderFactory> decoder_factory_;
};

// VideoStreamPair represents a video streaming session. It can be used to
// access underlying send and receive classes. It can also be used in calls to
// the Scenario class.
class VideoStreamPair {
 public:
  RTC_DISALLOW_COPY_AND_ASSIGN(VideoStreamPair);
  ~VideoStreamPair();
  SendVideoStream* send() { return &send_stream_; }
  ReceiveVideoStream* receive() { return &receive_stream_; }
  VideoFrameMatcher* matcher() { return &matcher_; }

 private:
  friend class Scenario;
  VideoStreamPair(CallClient* sender,
                  CallClient* receiver,
                  VideoStreamConfig config);

  const VideoStreamConfig config_;

  VideoFrameMatcher matcher_;
  SendVideoStream send_stream_;
  ReceiveVideoStream receive_stream_;
};
}  // namespace test
}  // namespace webrtc

#endif  // TEST_SCENARIO_VIDEO_STREAM_H_
