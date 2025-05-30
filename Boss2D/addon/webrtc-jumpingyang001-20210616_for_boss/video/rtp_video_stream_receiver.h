/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef VIDEO_RTP_VIDEO_STREAM_RECEIVER_H_
#define VIDEO_RTP_VIDEO_STREAM_RECEIVER_H_

#include <atomic>
#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__crypto__frame_decryptor_interface_h //original-code:"api/crypto/frame_decryptor_interface.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__units__timestamp_h //original-code:"api/units/timestamp.h"
#include BOSS_WEBRTC_U_api__video__color_space_h //original-code:"api/video/color_space.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_call__rtp_packet_sink_interface_h //original-code:"call/rtp_packet_sink_interface.h"
#include BOSS_WEBRTC_U_call__syncable_h //original-code:"call/syncable.h"
#include BOSS_WEBRTC_U_call__video_receive_stream_h //original-code:"call/video_receive_stream.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__receive_statistics_h //original-code:"modules/rtp_rtcp/include/receive_statistics.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__remote_ntp_time_estimator_h //original-code:"modules/rtp_rtcp/include/remote_ntp_time_estimator.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_header_extension_map_h //original-code:"modules/rtp_rtcp/include/rtp_header_extension_map.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__absolute_capture_time_interpolator_h //original-code:"modules/rtp_rtcp/source/absolute_capture_time_interpolator.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__capture_clock_offset_updater_h //original-code:"modules/rtp_rtcp/source/capture_clock_offset_updater.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_dependency_descriptor_extension_h //original-code:"modules/rtp_rtcp/source/rtp_dependency_descriptor_extension.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_received_h //original-code:"modules/rtp_rtcp/source/rtp_packet_received.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_video_header_h //original-code:"modules/rtp_rtcp/source/rtp_video_header.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__video_rtp_depacketizer_h //original-code:"modules/rtp_rtcp/source/video_rtp_depacketizer.h"
#include BOSS_WEBRTC_U_modules__video_coding__h264_sps_pps_tracker_h //original-code:"modules/video_coding/h264_sps_pps_tracker.h"
#include BOSS_WEBRTC_U_modules__video_coding__loss_notification_controller_h //original-code:"modules/video_coding/loss_notification_controller.h"
#include BOSS_WEBRTC_U_modules__video_coding__packet_buffer_h //original-code:"modules/video_coding/packet_buffer.h"
#include BOSS_WEBRTC_U_modules__video_coding__rtp_frame_reference_finder_h //original-code:"modules/video_coding/rtp_frame_reference_finder.h"
#include BOSS_WEBRTC_U_modules__video_coding__unique_timestamp_counter_h //original-code:"modules/video_coding/unique_timestamp_counter.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__field_trial_parser_h //original-code:"rtc_base/experiments/field_trial_parser.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__sequence_number_util_h //original-code:"rtc_base/numerics/sequence_number_util.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__system__no_unique_address_h //original-code:"rtc_base/system/no_unique_address.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"
#include BOSS_WEBRTC_U_video__buffered_frame_decryptor_h //original-code:"video/buffered_frame_decryptor.h"
#include BOSS_WEBRTC_U_video__rtp_video_stream_receiver_frame_transformer_delegate_h //original-code:"video/rtp_video_stream_receiver_frame_transformer_delegate.h"

namespace webrtc {

class DEPRECATED_NackModule;
class PacketRouter;
class ProcessThread;
class ReceiveStatistics;
class ReceiveStatisticsProxy;
class RtcpRttStats;
class RtpPacketReceived;
class Transport;
class UlpfecReceiver;

class RtpVideoStreamReceiver : public LossNotificationSender,
                               public RecoveredPacketReceiver,
                               public RtpPacketSinkInterface,
                               public KeyFrameRequestSender,
                               public OnDecryptedFrameCallback,
                               public OnDecryptionStatusChangeCallback,
                               public RtpVideoFrameReceiver {
 public:
  // A complete frame is a frame which has received all its packets and all its
  // references are known.
  class OnCompleteFrameCallback {
   public:
    virtual ~OnCompleteFrameCallback() {}
    virtual void OnCompleteFrame(std::unique_ptr<EncodedFrame> frame) = 0;
  };

  // DEPRECATED due to dependency on ReceiveStatisticsProxy.
  RtpVideoStreamReceiver(
      Clock* clock,
      Transport* transport,
      RtcpRttStats* rtt_stats,
      // The packet router is optional; if provided, the RtpRtcp module for this
      // stream is registered as a candidate for sending REMB and transport
      // feedback.
      PacketRouter* packet_router,
      const VideoReceiveStream::Config* config,
      ReceiveStatistics* rtp_receive_statistics,
      ReceiveStatisticsProxy* receive_stats_proxy,
      ProcessThread* process_thread,
      NackSender* nack_sender,
      // The KeyFrameRequestSender is optional; if not provided, key frame
      // requests are sent via the internal RtpRtcp module.
      KeyFrameRequestSender* keyframe_request_sender,
      OnCompleteFrameCallback* complete_frame_callback,
      rtc::scoped_refptr<FrameDecryptorInterface> frame_decryptor,
      rtc::scoped_refptr<FrameTransformerInterface> frame_transformer);

  RtpVideoStreamReceiver(
      Clock* clock,
      Transport* transport,
      RtcpRttStats* rtt_stats,
      // The packet router is optional; if provided, the RtpRtcp module for this
      // stream is registered as a candidate for sending REMB and transport
      // feedback.
      PacketRouter* packet_router,
      const VideoReceiveStream::Config* config,
      ReceiveStatistics* rtp_receive_statistics,
      RtcpPacketTypeCounterObserver* rtcp_packet_type_counter_observer,
      RtcpCnameCallback* rtcp_cname_callback,
      ProcessThread* process_thread,
      NackSender* nack_sender,
      // The KeyFrameRequestSender is optional; if not provided, key frame
      // requests are sent via the internal RtpRtcp module.
      KeyFrameRequestSender* keyframe_request_sender,
      OnCompleteFrameCallback* complete_frame_callback,
      rtc::scoped_refptr<FrameDecryptorInterface> frame_decryptor,
      rtc::scoped_refptr<FrameTransformerInterface> frame_transformer);
  ~RtpVideoStreamReceiver() override;

  void AddReceiveCodec(uint8_t payload_type,
                       const VideoCodec& video_codec,
                       const std::map<std::string, std::string>& codec_params,
                       bool raw_payload);

  void StartReceive();
  void StopReceive();

  // Produces the transport-related timestamps; current_delay_ms is left unset.
  absl::optional<Syncable::Info> GetSyncInfo() const;

  bool DeliverRtcp(const uint8_t* rtcp_packet, size_t rtcp_packet_length);

  void FrameContinuous(int64_t seq_num);

  void FrameDecoded(int64_t seq_num);

  void SignalNetworkState(NetworkState state);

  // Returns number of different frames seen.
  int GetUniqueFramesSeen() const {
    RTC_DCHECK_RUN_ON(&worker_task_checker_);
    return frame_counter_.GetUniqueSeen();
  }

  // Implements RtpPacketSinkInterface.
  void OnRtpPacket(const RtpPacketReceived& packet) override;

  // Public only for tests.
  void OnReceivedPayloadData(rtc::CopyOnWriteBuffer codec_payload,
                             const RtpPacketReceived& rtp_packet,
                             const RTPVideoHeader& video);

  // Implements RecoveredPacketReceiver.
  void OnRecoveredPacket(const uint8_t* packet, size_t packet_length) override;

  // Send an RTCP keyframe request.
  void RequestKeyFrame() override;

  // Implements LossNotificationSender.
  void SendLossNotification(uint16_t last_decoded_seq_num,
                            uint16_t last_received_seq_num,
                            bool decodability_flag,
                            bool buffering_allowed) override;

  bool IsUlpfecEnabled() const;
  bool IsRetransmissionsEnabled() const;

  // Returns true if a decryptor is attached and frames can be decrypted.
  // Updated by OnDecryptionStatusChangeCallback. Note this refers to Frame
  // Decryption not SRTP.
  bool IsDecryptable() const;

  // Don't use, still experimental.
  void RequestPacketRetransmit(const std::vector<uint16_t>& sequence_numbers);

  void OnCompleteFrames(RtpFrameReferenceFinder::ReturnVector frames);

  // Implements OnDecryptedFrameCallback.
  void OnDecryptedFrame(std::unique_ptr<RtpFrameObject> frame) override;

  // Implements OnDecryptionStatusChangeCallback.
  void OnDecryptionStatusChange(
      FrameDecryptorInterface::Status status) override;

  // Optionally set a frame decryptor after a stream has started. This will not
  // reset the decoder state.
  void SetFrameDecryptor(
      rtc::scoped_refptr<FrameDecryptorInterface> frame_decryptor);

  // Sets a frame transformer after a stream has started, if no transformer
  // has previously been set. Does not reset the decoder state.
  void SetDepacketizerToDecoderFrameTransformer(
      rtc::scoped_refptr<FrameTransformerInterface> frame_transformer);

  // Called by VideoReceiveStream when stats are updated.
  void UpdateRtt(int64_t max_rtt_ms);

  absl::optional<int64_t> LastReceivedPacketMs() const;
  absl::optional<int64_t> LastReceivedKeyframePacketMs() const;

  // RtpDemuxer only forwards a given RTP packet to one sink. However, some
  // sinks, such as FlexFEC, might wish to be informed of all of the packets
  // a given sink receives (or any set of sinks). They may do so by registering
  // themselves as secondary sinks.
  void AddSecondarySink(RtpPacketSinkInterface* sink);
  void RemoveSecondarySink(const RtpPacketSinkInterface* sink);

 private:
  // Implements RtpVideoFrameReceiver.
  void ManageFrame(std::unique_ptr<RtpFrameObject> frame) override;

  // Used for buffering RTCP feedback messages and sending them all together.
  // Note:
  // 1. Key frame requests and NACKs are mutually exclusive, with the
  //    former taking precedence over the latter.
  // 2. Loss notifications are orthogonal to either. (That is, may be sent
  //    alongside either.)
  class RtcpFeedbackBuffer : public KeyFrameRequestSender,
                             public NackSender,
                             public LossNotificationSender {
   public:
    RtcpFeedbackBuffer(KeyFrameRequestSender* key_frame_request_sender,
                       NackSender* nack_sender,
                       LossNotificationSender* loss_notification_sender);

    ~RtcpFeedbackBuffer() override = default;

    // KeyFrameRequestSender implementation.
    void RequestKeyFrame() RTC_LOCKS_EXCLUDED(mutex_) override;

    // NackSender implementation.
    void SendNack(const std::vector<uint16_t>& sequence_numbers,
                  bool buffering_allowed) RTC_LOCKS_EXCLUDED(mutex_) override;

    // LossNotificationSender implementation.
    void SendLossNotification(uint16_t last_decoded_seq_num,
                              uint16_t last_received_seq_num,
                              bool decodability_flag,
                              bool buffering_allowed)
        RTC_LOCKS_EXCLUDED(mutex_) override;

    // Send all RTCP feedback messages buffered thus far.
    void SendBufferedRtcpFeedback() RTC_LOCKS_EXCLUDED(mutex_);

   private:
    // LNTF-related state.
    struct LossNotificationState {
      LossNotificationState(uint16_t last_decoded_seq_num,
                            uint16_t last_received_seq_num,
                            bool decodability_flag)
          : last_decoded_seq_num(last_decoded_seq_num),
            last_received_seq_num(last_received_seq_num),
            decodability_flag(decodability_flag) {}

      uint16_t last_decoded_seq_num;
      uint16_t last_received_seq_num;
      bool decodability_flag;
    };
    struct ConsumedRtcpFeedback {
      bool request_key_frame = false;
      std::vector<uint16_t> nack_sequence_numbers;
      absl::optional<LossNotificationState> lntf_state;
    };

    ConsumedRtcpFeedback ConsumeRtcpFeedback() RTC_LOCKS_EXCLUDED(mutex_);
    ConsumedRtcpFeedback ConsumeRtcpFeedbackLocked()
        RTC_EXCLUSIVE_LOCKS_REQUIRED(mutex_);
    // This method is called both with and without mutex_ held.
    void SendRtcpFeedback(ConsumedRtcpFeedback feedback);

    KeyFrameRequestSender* const key_frame_request_sender_;
    NackSender* const nack_sender_;
    LossNotificationSender* const loss_notification_sender_;

    // NACKs are accessible from two threads due to nack_module_ being a module.
    Mutex mutex_;

    // Key-frame-request-related state.
    bool request_key_frame_ RTC_GUARDED_BY(mutex_);

    // NACK-related state.
    std::vector<uint16_t> nack_sequence_numbers_ RTC_GUARDED_BY(mutex_);

    absl::optional<LossNotificationState> lntf_state_ RTC_GUARDED_BY(mutex_);
  };
  enum ParseGenericDependenciesResult {
    kDropPacket,
    kHasGenericDescriptor,
    kNoGenericDescriptor
  };

  // Entry point doing non-stats work for a received packet. Called
  // for the same packet both before and after RED decapsulation.
  void ReceivePacket(const RtpPacketReceived& packet);
  // Parses and handles RED headers.
  // This function assumes that it's being called from only one thread.
  void ParseAndHandleEncapsulatingHeader(const RtpPacketReceived& packet);
  void NotifyReceiverOfEmptyPacket(uint16_t seq_num);
  void UpdateHistograms();
  bool IsRedEnabled() const;
  void InsertSpsPpsIntoTracker(uint8_t payload_type);
  void OnInsertedPacket(video_coding::PacketBuffer::InsertResult result);
  ParseGenericDependenciesResult ParseGenericDependenciesExtension(
      const RtpPacketReceived& rtp_packet,
      RTPVideoHeader* video_header) RTC_RUN_ON(worker_task_checker_);
  void OnAssembledFrame(std::unique_ptr<RtpFrameObject> frame)
      RTC_LOCKS_EXCLUDED(packet_buffer_lock_);
  void UpdatePacketReceiveTimestamps(const RtpPacketReceived& packet,
                                     bool is_keyframe)
      RTC_RUN_ON(worker_task_checker_);

  Clock* const clock_;
  // Ownership of this object lies with VideoReceiveStream, which owns |this|.
  const VideoReceiveStream::Config& config_;
  PacketRouter* const packet_router_;
  ProcessThread* const process_thread_;

  RemoteNtpTimeEstimator ntp_estimator_;

  RtpHeaderExtensionMap rtp_header_extensions_;
  // Set by the field trial WebRTC-ForcePlayoutDelay to override any playout
  // delay that is specified in the received packets.
  FieldTrialOptional<int> forced_playout_delay_max_ms_;
  FieldTrialOptional<int> forced_playout_delay_min_ms_;
  ReceiveStatistics* const rtp_receive_statistics_;
  std::unique_ptr<UlpfecReceiver> ulpfec_receiver_;

  RTC_NO_UNIQUE_ADDRESS SequenceChecker worker_task_checker_;
  bool receiving_ RTC_GUARDED_BY(worker_task_checker_);
  int64_t last_packet_log_ms_ RTC_GUARDED_BY(worker_task_checker_);

  const std::unique_ptr<RtpRtcp> rtp_rtcp_;

  OnCompleteFrameCallback* complete_frame_callback_;
  KeyFrameRequestSender* const keyframe_request_sender_;

  RtcpFeedbackBuffer rtcp_feedback_buffer_;
  std::unique_ptr<DEPRECATED_NackModule> nack_module_;
  std::unique_ptr<LossNotificationController> loss_notification_controller_;

  mutable Mutex packet_buffer_lock_;
  video_coding::PacketBuffer packet_buffer_ RTC_GUARDED_BY(packet_buffer_lock_);
  UniqueTimestampCounter frame_counter_ RTC_GUARDED_BY(worker_task_checker_);
  SeqNumUnwrapper<uint16_t> frame_id_unwrapper_
      RTC_GUARDED_BY(worker_task_checker_);

  // Video structure provided in the dependency descriptor in a first packet
  // of a key frame. It is required to parse dependency descriptor in the
  // following delta packets.
  std::unique_ptr<FrameDependencyStructure> video_structure_
      RTC_GUARDED_BY(worker_task_checker_);
  // Frame id of the last frame with the attached video structure.
  // absl::nullopt when `video_structure_ == nullptr`;
  absl::optional<int64_t> video_structure_frame_id_
      RTC_GUARDED_BY(worker_task_checker_);

  Mutex reference_finder_lock_;
  std::unique_ptr<RtpFrameReferenceFinder> reference_finder_
      RTC_GUARDED_BY(reference_finder_lock_);
  absl::optional<VideoCodecType> current_codec_;
  uint32_t last_assembled_frame_rtp_timestamp_;

  Mutex last_seq_num_mutex_;
  std::map<int64_t, uint16_t> last_seq_num_for_pic_id_
      RTC_GUARDED_BY(last_seq_num_mutex_);
  video_coding::H264SpsPpsTracker tracker_;

  // Maps payload id to the depacketizer.
  std::map<uint8_t, std::unique_ptr<VideoRtpDepacketizer>> payload_type_map_;

  // TODO(johan): Remove pt_codec_params_ once
  // https://bugs.chromium.org/p/webrtc/issues/detail?id=6883 is resolved.
  // Maps a payload type to a map of out-of-band supplied codec parameters.
  std::map<uint8_t, std::map<std::string, std::string>> pt_codec_params_;
  int16_t last_payload_type_ = -1;

  bool has_received_frame_;

  std::vector<RtpPacketSinkInterface*> secondary_sinks_
      RTC_GUARDED_BY(worker_task_checker_);

  // Info for GetSyncInfo is updated on network or worker thread, and queried on
  // the worker thread.
  mutable Mutex sync_info_lock_;
  absl::optional<uint32_t> last_received_rtp_timestamp_
      RTC_GUARDED_BY(sync_info_lock_);
  absl::optional<uint32_t> last_received_keyframe_rtp_timestamp_
      RTC_GUARDED_BY(sync_info_lock_);
  absl::optional<Timestamp> last_received_rtp_system_time_
      RTC_GUARDED_BY(sync_info_lock_);
  absl::optional<Timestamp> last_received_keyframe_rtp_system_time_
      RTC_GUARDED_BY(sync_info_lock_);

  // Used to validate the buffered frame decryptor is always run on the correct
  // thread.
  SequenceChecker network_tc_;
  // Handles incoming encrypted frames and forwards them to the
  // rtp_reference_finder if they are decryptable.
  std::unique_ptr<BufferedFrameDecryptor> buffered_frame_decryptor_
      RTC_PT_GUARDED_BY(network_tc_);
  std::atomic<bool> frames_decryptable_;
  absl::optional<ColorSpace> last_color_space_;

  AbsoluteCaptureTimeInterpolator absolute_capture_time_interpolator_
      RTC_GUARDED_BY(worker_task_checker_);

  CaptureClockOffsetUpdater capture_clock_offset_updater_
      RTC_GUARDED_BY(worker_task_checker_);

  int64_t last_completed_picture_id_ = 0;

  rtc::scoped_refptr<RtpVideoStreamReceiverFrameTransformerDelegate>
      frame_transformer_delegate_;

  SeqNumUnwrapper<uint16_t> rtp_seq_num_unwrapper_
      RTC_GUARDED_BY(packet_buffer_lock_);
  std::map<int64_t, RtpPacketInfo> packet_infos_
      RTC_GUARDED_BY(packet_buffer_lock_);
};

}  // namespace webrtc

#endif  // VIDEO_RTP_VIDEO_STREAM_RECEIVER_H_
