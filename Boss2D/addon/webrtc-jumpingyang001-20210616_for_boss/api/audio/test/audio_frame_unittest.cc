/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio__audio_frame_h //original-code:"api/audio/audio_frame.h"

#include <stdint.h>
#include <string.h>  // memcmp

#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

namespace {

bool AllSamplesAre(int16_t sample, const AudioFrame& frame) {
  const int16_t* frame_data = frame.data();
  for (size_t i = 0; i < frame.max_16bit_samples(); i++) {
    if (frame_data[i] != sample) {
      return false;
    }
  }
  return true;
}

constexpr uint32_t kTimestamp = 27;
constexpr int kSampleRateHz = 16000;
constexpr size_t kNumChannelsMono = 1;
constexpr size_t kNumChannelsStereo = 2;
constexpr size_t kNumChannels5_1 = 6;
constexpr size_t kSamplesPerChannel = kSampleRateHz / 100;

}  // namespace

TEST(AudioFrameTest, FrameStartsMuted) {
  AudioFrame frame;
  EXPECT_TRUE(frame.muted());
  EXPECT_TRUE(AllSamplesAre(0, frame));
}

TEST(AudioFrameTest, UnmutedFrameIsInitiallyZeroed) {
  AudioFrame frame;
  frame.mutable_data();
  EXPECT_FALSE(frame.muted());
  EXPECT_TRUE(AllSamplesAre(0, frame));
}

TEST(AudioFrameTest, MutedFrameBufferIsZeroed) {
  AudioFrame frame;
  int16_t* frame_data = frame.mutable_data();
  for (size_t i = 0; i < frame.max_16bit_samples(); i++) {
    frame_data[i] = 17;
  }
  ASSERT_TRUE(AllSamplesAre(17, frame));
  frame.Mute();
  EXPECT_TRUE(frame.muted());
  EXPECT_TRUE(AllSamplesAre(0, frame));
}

TEST(AudioFrameTest, UpdateFrameMono) {
  AudioFrame frame;
  int16_t samples[kNumChannelsMono * kSamplesPerChannel] = {17};
  frame.UpdateFrame(kTimestamp, samples, kSamplesPerChannel, kSampleRateHz,
                    AudioFrame::kPLC, AudioFrame::kVadActive, kNumChannelsMono);

  EXPECT_EQ(kTimestamp, frame.timestamp_);
  EXPECT_EQ(kSamplesPerChannel, frame.samples_per_channel());
  EXPECT_EQ(kSampleRateHz, frame.sample_rate_hz());
  EXPECT_EQ(AudioFrame::kPLC, frame.speech_type_);
  EXPECT_EQ(AudioFrame::kVadActive, frame.vad_activity_);
  EXPECT_EQ(kNumChannelsMono, frame.num_channels());
  EXPECT_EQ(CHANNEL_LAYOUT_MONO, frame.channel_layout());

  EXPECT_FALSE(frame.muted());
  EXPECT_EQ(0, memcmp(samples, frame.data(), sizeof(samples)));

  frame.UpdateFrame(kTimestamp, nullptr /* data*/, kSamplesPerChannel,
                    kSampleRateHz, AudioFrame::kPLC, AudioFrame::kVadActive,
                    kNumChannelsMono);
  EXPECT_TRUE(frame.muted());
  EXPECT_TRUE(AllSamplesAre(0, frame));
}

TEST(AudioFrameTest, UpdateFrameMultiChannel) {
  AudioFrame frame;
  frame.UpdateFrame(kTimestamp, nullptr /* data */, kSamplesPerChannel,
                    kSampleRateHz, AudioFrame::kPLC, AudioFrame::kVadActive,
                    kNumChannelsStereo);
  EXPECT_EQ(kSamplesPerChannel, frame.samples_per_channel());
  EXPECT_EQ(kNumChannelsStereo, frame.num_channels());
  EXPECT_EQ(CHANNEL_LAYOUT_STEREO, frame.channel_layout());

  frame.UpdateFrame(kTimestamp, nullptr /* data */, kSamplesPerChannel,
                    kSampleRateHz, AudioFrame::kPLC, AudioFrame::kVadActive,
                    kNumChannels5_1);
  EXPECT_EQ(kSamplesPerChannel, frame.samples_per_channel());
  EXPECT_EQ(kNumChannels5_1, frame.num_channels());
  EXPECT_EQ(CHANNEL_LAYOUT_5_1, frame.channel_layout());
}

TEST(AudioFrameTest, CopyFrom) {
  AudioFrame frame1;
  AudioFrame frame2;

  int16_t samples[kNumChannelsMono * kSamplesPerChannel] = {17};
  frame2.UpdateFrame(kTimestamp, samples, kSamplesPerChannel, kSampleRateHz,
                     AudioFrame::kPLC, AudioFrame::kVadActive,
                     kNumChannelsMono);
  frame1.CopyFrom(frame2);

  EXPECT_EQ(frame2.timestamp_, frame1.timestamp_);
  EXPECT_EQ(frame2.samples_per_channel_, frame1.samples_per_channel_);
  EXPECT_EQ(frame2.sample_rate_hz_, frame1.sample_rate_hz_);
  EXPECT_EQ(frame2.speech_type_, frame1.speech_type_);
  EXPECT_EQ(frame2.vad_activity_, frame1.vad_activity_);
  EXPECT_EQ(frame2.num_channels_, frame1.num_channels_);

  EXPECT_EQ(frame2.muted(), frame1.muted());
  EXPECT_EQ(0, memcmp(frame2.data(), frame1.data(), sizeof(samples)));

  frame2.UpdateFrame(kTimestamp, nullptr /* data */, kSamplesPerChannel,
                     kSampleRateHz, AudioFrame::kPLC, AudioFrame::kVadActive,
                     kNumChannelsMono);
  frame1.CopyFrom(frame2);

  EXPECT_EQ(frame2.muted(), frame1.muted());
  EXPECT_EQ(0, memcmp(frame2.data(), frame1.data(), sizeof(samples)));
}

TEST(AudioFrameTest, SwapFrames) {
  AudioFrame frame1, frame2;
  int16_t samples1[kNumChannelsMono * kSamplesPerChannel];
  for (size_t i = 0; i < kNumChannelsMono * kSamplesPerChannel; ++i) {
    samples1[i] = i;
  }
  frame1.UpdateFrame(kTimestamp, samples1, kSamplesPerChannel, kSampleRateHz,
                     AudioFrame::kPLC, AudioFrame::kVadActive,
                     kNumChannelsMono);
  frame1.set_absolute_capture_timestamp_ms(12345678);
  const auto frame1_channel_layout = frame1.channel_layout();

  int16_t samples2[(kNumChannelsMono + 1) * (kSamplesPerChannel + 1)];
  for (size_t i = 0; i < (kNumChannelsMono + 1) * (kSamplesPerChannel + 1);
       ++i) {
    samples2[i] = 1000 + i;
  }
  frame2.UpdateFrame(kTimestamp + 1, samples2, kSamplesPerChannel + 1,
                     kSampleRateHz + 1, AudioFrame::kNormalSpeech,
                     AudioFrame::kVadPassive, kNumChannelsMono + 1);
  const auto frame2_channel_layout = frame2.channel_layout();

  swap(frame1, frame2);

  EXPECT_EQ(kTimestamp + 1, frame1.timestamp_);
  ASSERT_EQ(kSamplesPerChannel + 1, frame1.samples_per_channel_);
  EXPECT_EQ(kSampleRateHz + 1, frame1.sample_rate_hz_);
  EXPECT_EQ(AudioFrame::kNormalSpeech, frame1.speech_type_);
  EXPECT_EQ(AudioFrame::kVadPassive, frame1.vad_activity_);
  ASSERT_EQ(kNumChannelsMono + 1, frame1.num_channels_);
  for (size_t i = 0; i < (kNumChannelsMono + 1) * (kSamplesPerChannel + 1);
       ++i) {
    EXPECT_EQ(samples2[i], frame1.data()[i]);
  }
  EXPECT_FALSE(frame1.absolute_capture_timestamp_ms());
  EXPECT_EQ(frame2_channel_layout, frame1.channel_layout());

  EXPECT_EQ(kTimestamp, frame2.timestamp_);
  ASSERT_EQ(kSamplesPerChannel, frame2.samples_per_channel_);
  EXPECT_EQ(kSampleRateHz, frame2.sample_rate_hz_);
  EXPECT_EQ(AudioFrame::kPLC, frame2.speech_type_);
  EXPECT_EQ(AudioFrame::kVadActive, frame2.vad_activity_);
  ASSERT_EQ(kNumChannelsMono, frame2.num_channels_);
  for (size_t i = 0; i < kNumChannelsMono * kSamplesPerChannel; ++i) {
    EXPECT_EQ(samples1[i], frame2.data()[i]);
  }
  EXPECT_EQ(12345678, frame2.absolute_capture_timestamp_ms());
  EXPECT_EQ(frame1_channel_layout, frame2.channel_layout());
}

}  // namespace webrtc
