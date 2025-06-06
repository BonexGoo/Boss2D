/*
 *  Copyright 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <string>

#include BOSS_WEBRTC_U_pc__audiotrack_h //original-code:"pc/audiotrack.h"
#include BOSS_WEBRTC_U_pc__mediastream_h //original-code:"pc/mediastream.h"
#include "pc/test/fakevideotracksource.h"
#include BOSS_WEBRTC_U_pc__videotrack_h //original-code:"pc/videotrack.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__refcount_h //original-code:"rtc_base/refcount.h"
#include "test/gmock.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

static const char kStreamLabel1[] = "local_stream_1";
static const char kVideoTrackId[] = "dummy_video_cam_1";
static const char kAudioTrackId[] = "dummy_microphone_1";

using rtc::scoped_refptr;
using ::testing::Exactly;

namespace webrtc {

// Helper class to test Observer.
class MockObserver : public ObserverInterface {
 public:
  explicit MockObserver(NotifierInterface* notifier) : notifier_(notifier) {
    notifier_->RegisterObserver(this);
  }

  ~MockObserver() { Unregister(); }

  void Unregister() {
    if (notifier_) {
      notifier_->UnregisterObserver(this);
      notifier_ = nullptr;
    }
  }

  MOCK_METHOD0(OnChanged, void());

 private:
  NotifierInterface* notifier_;
};

class MediaStreamTest: public testing::Test {
 protected:
  virtual void SetUp() {
    stream_ = MediaStream::Create(kStreamLabel1);
    ASSERT_TRUE(stream_.get() != NULL);

    video_track_ = VideoTrack::Create(
        kVideoTrackId, FakeVideoTrackSource::Create(), rtc::Thread::Current());
    ASSERT_TRUE(video_track_.get() != NULL);
    EXPECT_EQ(MediaStreamTrackInterface::kLive, video_track_->state());

    audio_track_ = AudioTrack::Create(kAudioTrackId, NULL);

    ASSERT_TRUE(audio_track_.get() != NULL);
    EXPECT_EQ(MediaStreamTrackInterface::kLive, audio_track_->state());

    EXPECT_TRUE(stream_->AddTrack(video_track_));
    EXPECT_FALSE(stream_->AddTrack(video_track_));
    EXPECT_TRUE(stream_->AddTrack(audio_track_));
    EXPECT_FALSE(stream_->AddTrack(audio_track_));
  }

  void ChangeTrack(MediaStreamTrackInterface* track) {
    MockObserver observer(track);

    EXPECT_CALL(observer, OnChanged())
        .Times(Exactly(1));
    track->set_enabled(false);
    EXPECT_FALSE(track->enabled());
  }

  scoped_refptr<MediaStreamInterface> stream_;
  scoped_refptr<AudioTrackInterface> audio_track_;
  scoped_refptr<VideoTrackInterface> video_track_;
};

TEST_F(MediaStreamTest, GetTrackInfo) {
  ASSERT_EQ(1u, stream_->GetVideoTracks().size());
  ASSERT_EQ(1u, stream_->GetAudioTracks().size());

  // Verify the video track.
  scoped_refptr<webrtc::MediaStreamTrackInterface> video_track(
      stream_->GetVideoTracks()[0]);
  EXPECT_EQ(0, video_track->id().compare(kVideoTrackId));
  EXPECT_TRUE(video_track->enabled());

  ASSERT_EQ(1u, stream_->GetVideoTracks().size());
  EXPECT_TRUE(stream_->GetVideoTracks()[0].get() == video_track.get());
  EXPECT_TRUE(stream_->FindVideoTrack(video_track->id()).get()
              == video_track.get());
  video_track = stream_->GetVideoTracks()[0];
  EXPECT_EQ(0, video_track->id().compare(kVideoTrackId));
  EXPECT_TRUE(video_track->enabled());

  // Verify the audio track.
  scoped_refptr<webrtc::MediaStreamTrackInterface> audio_track(
      stream_->GetAudioTracks()[0]);
  EXPECT_EQ(0, audio_track->id().compare(kAudioTrackId));
  EXPECT_TRUE(audio_track->enabled());
  ASSERT_EQ(1u, stream_->GetAudioTracks().size());
  EXPECT_TRUE(stream_->GetAudioTracks()[0].get() == audio_track.get());
  EXPECT_TRUE(stream_->FindAudioTrack(audio_track->id()).get()
              == audio_track.get());
  audio_track = stream_->GetAudioTracks()[0];
  EXPECT_EQ(0, audio_track->id().compare(kAudioTrackId));
  EXPECT_TRUE(audio_track->enabled());
}

TEST_F(MediaStreamTest, RemoveTrack) {
  MockObserver observer(stream_);

  EXPECT_CALL(observer, OnChanged())
      .Times(Exactly(2));

  EXPECT_TRUE(stream_->RemoveTrack(audio_track_));
  EXPECT_FALSE(stream_->RemoveTrack(audio_track_));
  EXPECT_EQ(0u, stream_->GetAudioTracks().size());
  EXPECT_EQ(0u, stream_->GetAudioTracks().size());

  EXPECT_TRUE(stream_->RemoveTrack(video_track_));
  EXPECT_FALSE(stream_->RemoveTrack(video_track_));

  EXPECT_EQ(0u, stream_->GetVideoTracks().size());
  EXPECT_EQ(0u, stream_->GetVideoTracks().size());

  EXPECT_FALSE(stream_->RemoveTrack(static_cast<AudioTrackInterface*>(NULL)));
  EXPECT_FALSE(stream_->RemoveTrack(static_cast<VideoTrackInterface*>(NULL)));
}

TEST_F(MediaStreamTest, ChangeVideoTrack) {
  scoped_refptr<webrtc::VideoTrackInterface> video_track(
      stream_->GetVideoTracks()[0]);
  ChangeTrack(video_track.get());
}

TEST_F(MediaStreamTest, ChangeAudioTrack) {
  scoped_refptr<webrtc::AudioTrackInterface> audio_track(
      stream_->GetAudioTracks()[0]);
  ChangeTrack(audio_track.get());
}

}  // namespace webrtc
