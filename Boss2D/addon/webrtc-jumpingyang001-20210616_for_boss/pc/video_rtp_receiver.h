/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_VIDEO_RTP_RECEIVER_H_
#define PC_VIDEO_RTP_RECEIVER_H_

#include <stdint.h>

#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__crypto__frame_decryptor_interface_h //original-code:"api/crypto/frame_decryptor_interface.h"
#include BOSS_WEBRTC_U_api__dtls_transport_interface_h //original-code:"api/dtls_transport_interface.h"
#include BOSS_WEBRTC_U_api__frame_transformer_interface_h //original-code:"api/frame_transformer_interface.h"
#include BOSS_WEBRTC_U_api__media_stream_interface_h //original-code:"api/media_stream_interface.h"
#include BOSS_WEBRTC_U_api__media_types_h //original-code:"api/media_types.h"
#include BOSS_WEBRTC_U_api__rtp_parameters_h //original-code:"api/rtp_parameters.h"
#include BOSS_WEBRTC_U_api__rtp_receiver_interface_h //original-code:"api/rtp_receiver_interface.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__transport__rtp__rtp_source_h //original-code:"api/transport/rtp/rtp_source.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video__video_sink_interface_h //original-code:"api/video/video_sink_interface.h"
#include BOSS_WEBRTC_U_api__video__video_source_interface_h //original-code:"api/video/video_source_interface.h"
#include BOSS_WEBRTC_U_media__base__media_channel_h //original-code:"media/base/media_channel.h"
#include BOSS_WEBRTC_U_pc__jitter_buffer_delay_h //original-code:"pc/jitter_buffer_delay.h"
#include BOSS_WEBRTC_U_pc__media_stream_track_proxy_h //original-code:"pc/media_stream_track_proxy.h"
#include BOSS_WEBRTC_U_pc__rtp_receiver_h //original-code:"pc/rtp_receiver.h"
#include BOSS_WEBRTC_U_pc__video_rtp_track_source_h //original-code:"pc/video_rtp_track_source.h"
#include BOSS_WEBRTC_U_pc__video_track_h //original-code:"pc/video_track.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__system__no_unique_address_h //original-code:"rtc_base/system/no_unique_address.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

class VideoRtpReceiver : public RtpReceiverInternal {
 public:
  // An SSRC of 0 will create a receiver that will match the first SSRC it
  // sees. Must be called on signaling thread.
  VideoRtpReceiver(rtc::Thread* worker_thread,
                   std::string receiver_id,
                   std::vector<std::string> streams_ids);
  // TODO(hbos): Remove this when streams() is removed.
  // https://crbug.com/webrtc/9480
  VideoRtpReceiver(
      rtc::Thread* worker_thread,
      const std::string& receiver_id,
      const std::vector<rtc::scoped_refptr<MediaStreamInterface>>& streams);

  virtual ~VideoRtpReceiver();

  rtc::scoped_refptr<VideoTrackInterface> video_track() const { return track_; }

  // RtpReceiverInterface implementation
  rtc::scoped_refptr<MediaStreamTrackInterface> track() const override {
    return track_;
  }
  rtc::scoped_refptr<DtlsTransportInterface> dtls_transport() const override;
  std::vector<std::string> stream_ids() const override;
  std::vector<rtc::scoped_refptr<MediaStreamInterface>> streams()
      const override;
  cricket::MediaType media_type() const override {
    return cricket::MEDIA_TYPE_VIDEO;
  }

  std::string id() const override { return id_; }

  RtpParameters GetParameters() const override;

  void SetFrameDecryptor(
      rtc::scoped_refptr<FrameDecryptorInterface> frame_decryptor) override;

  rtc::scoped_refptr<FrameDecryptorInterface> GetFrameDecryptor()
      const override;

  void SetDepacketizerToDecoderFrameTransformer(
      rtc::scoped_refptr<FrameTransformerInterface> frame_transformer) override;

  // RtpReceiverInternal implementation.
  void Stop() override;
  void StopAndEndTrack() override;
  void SetupMediaChannel(uint32_t ssrc) override;
  void SetupUnsignaledMediaChannel() override;
  uint32_t ssrc() const override;
  void NotifyFirstPacketReceived() override;
  void set_stream_ids(std::vector<std::string> stream_ids) override;
  void set_transport(
      rtc::scoped_refptr<DtlsTransportInterface> dtls_transport) override;
  void SetStreams(const std::vector<rtc::scoped_refptr<MediaStreamInterface>>&
                      streams) override;

  void SetObserver(RtpReceiverObserverInterface* observer) override;

  void SetJitterBufferMinimumDelay(
      absl::optional<double> delay_seconds) override;

  void SetMediaChannel(cricket::MediaChannel* media_channel) override;

  int AttachmentId() const override { return attachment_id_; }

  std::vector<RtpSource> GetSources() const override;

 private:
  void RestartMediaChannel(absl::optional<uint32_t> ssrc);
  void SetSink(rtc::VideoSinkInterface<VideoFrame>* sink)
      RTC_RUN_ON(worker_thread_);
  void SetMediaChannel_w(cricket::MediaChannel* media_channel)
      RTC_RUN_ON(worker_thread_);

  // VideoRtpTrackSource::Callback
  void OnGenerateKeyFrame();
  void OnEncodedSinkEnabled(bool enable);

  void SetEncodedSinkEnabled(bool enable) RTC_RUN_ON(worker_thread_);

  class SourceCallback : public VideoRtpTrackSource::Callback {
   public:
    explicit SourceCallback(VideoRtpReceiver* receiver) : receiver_(receiver) {}
    ~SourceCallback() override = default;

   private:
    void OnGenerateKeyFrame() override { receiver_->OnGenerateKeyFrame(); }
    void OnEncodedSinkEnabled(bool enable) override {
      receiver_->OnEncodedSinkEnabled(enable);
    }

    VideoRtpReceiver* const receiver_;
  } source_callback_{this};

  RTC_NO_UNIQUE_ADDRESS SequenceChecker signaling_thread_checker_;
  rtc::Thread* const worker_thread_;

  const std::string id_;
  // See documentation for `stopped_` below for when a valid media channel
  // has been assigned and when this pointer will be null.
  cricket::VideoMediaChannel* media_channel_ RTC_GUARDED_BY(worker_thread_) =
      nullptr;
  absl::optional<uint32_t> ssrc_ RTC_GUARDED_BY(worker_thread_);
  // |source_| is held here to be able to change the state of the source when
  // the VideoRtpReceiver is stopped.
  const rtc::scoped_refptr<VideoRtpTrackSource> source_;
  const rtc::scoped_refptr<VideoTrackProxyWithInternal<VideoTrack>> track_;
  std::vector<rtc::scoped_refptr<MediaStreamInterface>> streams_
      RTC_GUARDED_BY(&signaling_thread_checker_);
  // `stopped` is state that's used on the signaling thread to indicate whether
  // a valid `media_channel_` has been assigned and configured. When an instance
  // of VideoRtpReceiver is initially created, `stopped_` is true and will
  // remain true until either `SetupMediaChannel` or
  // `SetupUnsignaledMediaChannel` is called after assigning a media channel.
  // After that, `stopped_` will remain false until `Stop()` is called.
  // Note, for checking the state of the class on the worker thread,
  // check `media_channel_` instead, as that's the main worker thread state.
  bool stopped_ RTC_GUARDED_BY(&signaling_thread_checker_) = true;
  RtpReceiverObserverInterface* observer_
      RTC_GUARDED_BY(&signaling_thread_checker_) = nullptr;
  bool received_first_packet_ RTC_GUARDED_BY(&signaling_thread_checker_) =
      false;
  const int attachment_id_;
  rtc::scoped_refptr<FrameDecryptorInterface> frame_decryptor_
      RTC_GUARDED_BY(worker_thread_);
  rtc::scoped_refptr<DtlsTransportInterface> dtls_transport_
      RTC_GUARDED_BY(&signaling_thread_checker_);
  rtc::scoped_refptr<FrameTransformerInterface> frame_transformer_
      RTC_GUARDED_BY(worker_thread_);
  // Stores the minimum jitter buffer delay. Handles caching cases
  // if |SetJitterBufferMinimumDelay| is called before start.
  JitterBufferDelay delay_ RTC_GUARDED_BY(worker_thread_);

  // Records if we should generate a keyframe when |media_channel_| gets set up
  // or switched.
  bool saved_generate_keyframe_ RTC_GUARDED_BY(worker_thread_) = false;
  bool saved_encoded_sink_enabled_ RTC_GUARDED_BY(worker_thread_) = false;
};

}  // namespace webrtc

#endif  // PC_VIDEO_RTP_RECEIVER_H_
