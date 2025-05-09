/*
 *  Copyright 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_media__base__video_broadcaster_h //original-code:"media/base/video_broadcaster.h"

#include <limits>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__video__i420_buffer_h //original-code:"api/video/i420_buffer.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video__video_rotation_h //original-code:"api/video/video_rotation.h"
#include BOSS_WEBRTC_U_media__base__fake_video_renderer_h //original-code:"media/base/fake_video_renderer.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

using cricket::FakeVideoRenderer;
using rtc::VideoBroadcaster;
using rtc::VideoSinkWants;

TEST(VideoBroadcasterTest, frame_wanted) {
  VideoBroadcaster broadcaster;
  EXPECT_FALSE(broadcaster.frame_wanted());

  FakeVideoRenderer sink;
  broadcaster.AddOrUpdateSink(&sink, rtc::VideoSinkWants());
  EXPECT_TRUE(broadcaster.frame_wanted());

  broadcaster.RemoveSink(&sink);
  EXPECT_FALSE(broadcaster.frame_wanted());
}

TEST(VideoBroadcasterTest, OnFrame) {
  VideoBroadcaster broadcaster;

  FakeVideoRenderer sink1;
  FakeVideoRenderer sink2;
  broadcaster.AddOrUpdateSink(&sink1, rtc::VideoSinkWants());
  broadcaster.AddOrUpdateSink(&sink2, rtc::VideoSinkWants());
  static int kWidth = 100;
  static int kHeight = 50;

  rtc::scoped_refptr<webrtc::I420Buffer> buffer(
      webrtc::I420Buffer::Create(kWidth, kHeight));
  // Initialize, to avoid warnings on use of initialized values.
  webrtc::I420Buffer::SetBlack(buffer);

  webrtc::VideoFrame frame = webrtc::VideoFrame::Builder()
                                 .set_video_frame_buffer(buffer)
                                 .set_rotation(webrtc::kVideoRotation_0)
                                 .set_timestamp_us(0)
                                 .build();

  broadcaster.OnFrame(frame);
  EXPECT_EQ(1, sink1.num_rendered_frames());
  EXPECT_EQ(1, sink2.num_rendered_frames());

  broadcaster.RemoveSink(&sink1);
  broadcaster.OnFrame(frame);
  EXPECT_EQ(1, sink1.num_rendered_frames());
  EXPECT_EQ(2, sink2.num_rendered_frames());

  broadcaster.AddOrUpdateSink(&sink1, rtc::VideoSinkWants());
  broadcaster.OnFrame(frame);
  EXPECT_EQ(2, sink1.num_rendered_frames());
  EXPECT_EQ(3, sink2.num_rendered_frames());
}

TEST(VideoBroadcasterTest, AppliesRotationIfAnySinkWantsRotationApplied) {
  VideoBroadcaster broadcaster;
  EXPECT_FALSE(broadcaster.wants().rotation_applied);

  FakeVideoRenderer sink1;
  VideoSinkWants wants1;
  wants1.rotation_applied = false;

  broadcaster.AddOrUpdateSink(&sink1, wants1);
  EXPECT_FALSE(broadcaster.wants().rotation_applied);

  FakeVideoRenderer sink2;
  VideoSinkWants wants2;
  wants2.rotation_applied = true;

  broadcaster.AddOrUpdateSink(&sink2, wants2);
  EXPECT_TRUE(broadcaster.wants().rotation_applied);

  broadcaster.RemoveSink(&sink2);
  EXPECT_FALSE(broadcaster.wants().rotation_applied);
}

TEST(VideoBroadcasterTest, AppliesMinOfSinkWantsMaxPixelCount) {
  VideoBroadcaster broadcaster;
  EXPECT_EQ(std::numeric_limits<int>::max(),
            broadcaster.wants().max_pixel_count);

  FakeVideoRenderer sink1;
  VideoSinkWants wants1;
  wants1.max_pixel_count = 1280 * 720;

  broadcaster.AddOrUpdateSink(&sink1, wants1);
  EXPECT_EQ(1280 * 720, broadcaster.wants().max_pixel_count);

  FakeVideoRenderer sink2;
  VideoSinkWants wants2;
  wants2.max_pixel_count = 640 * 360;
  broadcaster.AddOrUpdateSink(&sink2, wants2);
  EXPECT_EQ(640 * 360, broadcaster.wants().max_pixel_count);

  broadcaster.RemoveSink(&sink2);
  EXPECT_EQ(1280 * 720, broadcaster.wants().max_pixel_count);
}

TEST(VideoBroadcasterTest, AppliesMinOfSinkWantsMaxAndTargetPixelCount) {
  VideoBroadcaster broadcaster;
  EXPECT_TRUE(!broadcaster.wants().target_pixel_count);

  FakeVideoRenderer sink1;
  VideoSinkWants wants1;
  wants1.target_pixel_count = 1280 * 720;

  broadcaster.AddOrUpdateSink(&sink1, wants1);
  EXPECT_EQ(1280 * 720, *broadcaster.wants().target_pixel_count);

  FakeVideoRenderer sink2;
  VideoSinkWants wants2;
  wants2.target_pixel_count = 640 * 360;
  broadcaster.AddOrUpdateSink(&sink2, wants2);
  EXPECT_EQ(640 * 360, *broadcaster.wants().target_pixel_count);

  broadcaster.RemoveSink(&sink2);
  EXPECT_EQ(1280 * 720, *broadcaster.wants().target_pixel_count);
}

TEST(VideoBroadcasterTest, AppliesMinOfSinkWantsMaxFramerate) {
  VideoBroadcaster broadcaster;
  EXPECT_EQ(std::numeric_limits<int>::max(),
            broadcaster.wants().max_framerate_fps);

  FakeVideoRenderer sink1;
  VideoSinkWants wants1;
  wants1.max_framerate_fps = 30;

  broadcaster.AddOrUpdateSink(&sink1, wants1);
  EXPECT_EQ(30, broadcaster.wants().max_framerate_fps);

  FakeVideoRenderer sink2;
  VideoSinkWants wants2;
  wants2.max_framerate_fps = 15;
  broadcaster.AddOrUpdateSink(&sink2, wants2);
  EXPECT_EQ(15, broadcaster.wants().max_framerate_fps);

  broadcaster.RemoveSink(&sink2);
  EXPECT_EQ(30, broadcaster.wants().max_framerate_fps);
}

TEST(VideoBroadcasterTest,
     AppliesLeastCommonMultipleOfSinkWantsResolutionAlignment) {
  VideoBroadcaster broadcaster;
  EXPECT_EQ(broadcaster.wants().resolution_alignment, 1);

  FakeVideoRenderer sink1;
  VideoSinkWants wants1;
  wants1.resolution_alignment = 2;
  broadcaster.AddOrUpdateSink(&sink1, wants1);
  EXPECT_EQ(broadcaster.wants().resolution_alignment, 2);

  FakeVideoRenderer sink2;
  VideoSinkWants wants2;
  wants2.resolution_alignment = 3;
  broadcaster.AddOrUpdateSink(&sink2, wants2);
  EXPECT_EQ(broadcaster.wants().resolution_alignment, 6);

  FakeVideoRenderer sink3;
  VideoSinkWants wants3;
  wants3.resolution_alignment = 4;
  broadcaster.AddOrUpdateSink(&sink3, wants3);
  EXPECT_EQ(broadcaster.wants().resolution_alignment, 12);

  broadcaster.RemoveSink(&sink2);
  EXPECT_EQ(broadcaster.wants().resolution_alignment, 4);
}

TEST(VideoBroadcasterTest, SinkWantsBlackFrames) {
  VideoBroadcaster broadcaster;
  EXPECT_TRUE(!broadcaster.wants().black_frames);

  FakeVideoRenderer sink1;
  VideoSinkWants wants1;
  wants1.black_frames = true;
  broadcaster.AddOrUpdateSink(&sink1, wants1);

  FakeVideoRenderer sink2;
  VideoSinkWants wants2;
  wants2.black_frames = false;
  broadcaster.AddOrUpdateSink(&sink2, wants2);

  rtc::scoped_refptr<webrtc::I420Buffer> buffer(
      webrtc::I420Buffer::Create(100, 200));
  // Makes it not all black.
  buffer->InitializeData();

  webrtc::VideoFrame frame1 = webrtc::VideoFrame::Builder()
                                  .set_video_frame_buffer(buffer)
                                  .set_rotation(webrtc::kVideoRotation_0)
                                  .set_timestamp_us(10)
                                  .build();
  broadcaster.OnFrame(frame1);
  EXPECT_TRUE(sink1.black_frame());
  EXPECT_EQ(10, sink1.timestamp_us());
  EXPECT_FALSE(sink2.black_frame());
  EXPECT_EQ(10, sink2.timestamp_us());

  // Switch the sink wants.
  wants1.black_frames = false;
  broadcaster.AddOrUpdateSink(&sink1, wants1);
  wants2.black_frames = true;
  broadcaster.AddOrUpdateSink(&sink2, wants2);

  webrtc::VideoFrame frame2 = webrtc::VideoFrame::Builder()
                                  .set_video_frame_buffer(buffer)
                                  .set_rotation(webrtc::kVideoRotation_0)
                                  .set_timestamp_us(30)
                                  .build();
  broadcaster.OnFrame(frame2);
  EXPECT_FALSE(sink1.black_frame());
  EXPECT_EQ(30, sink1.timestamp_us());
  EXPECT_TRUE(sink2.black_frame());
  EXPECT_EQ(30, sink2.timestamp_us());
}
