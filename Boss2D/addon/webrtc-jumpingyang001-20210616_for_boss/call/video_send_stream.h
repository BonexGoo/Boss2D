/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef CALL_VIDEO_SEND_STREAM_H_
#define CALL_VIDEO_SEND_STREAM_H_

#include <stdint.h>

#include <map>
#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__adaptation__resource_h //original-code:"api/adaptation/resource.h"
#include BOSS_WEBRTC_U_api__call__transport_h //original-code:"api/call/transport.h"
#include BOSS_WEBRTC_U_api__crypto__crypto_options_h //original-code:"api/crypto/crypto_options.h"
#include BOSS_WEBRTC_U_api__frame_transformer_interface_h //original-code:"api/frame_transformer_interface.h"
#include BOSS_WEBRTC_U_api__rtp_parameters_h //original-code:"api/rtp_parameters.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__video__video_content_type_h //original-code:"api/video/video_content_type.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video__video_sink_interface_h //original-code:"api/video/video_sink_interface.h"
#include BOSS_WEBRTC_U_api__video__video_source_interface_h //original-code:"api/video/video_source_interface.h"
#include BOSS_WEBRTC_U_api__video__video_stream_encoder_settings_h //original-code:"api/video/video_stream_encoder_settings.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_config_h //original-code:"api/video_codecs/video_encoder_config.h"
#include BOSS_WEBRTC_U_call__rtp_config_h //original-code:"call/rtp_config.h"
#include BOSS_WEBRTC_U_common_video__frame_counts_h //original-code:"common_video/frame_counts.h"
#include BOSS_WEBRTC_U_common_video__include__quality_limitation_reason_h //original-code:"common_video/include/quality_limitation_reason.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__report_block_data_h //original-code:"modules/rtp_rtcp/include/report_block_data.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtcp_statistics_h //original-code:"modules/rtp_rtcp/include/rtcp_statistics.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"

namespace webrtc {

class FrameEncryptorInterface;

class VideoSendStream {
 public:
  // Multiple StreamStats objects are present if simulcast is used (multiple
  // kMedia streams) or if RTX or FlexFEC is negotiated. Multiple SVC layers, on
  // the other hand, does not cause additional StreamStats.
  struct StreamStats {
    enum class StreamType {
      // A media stream is an RTP stream for audio or video. Retransmissions and
      // FEC is either sent over the same SSRC or negotiated to be sent over
      // separate SSRCs, in which case separate StreamStats objects exist with
      // references to this media stream's SSRC.
      kMedia,
      // RTX streams are streams dedicated to retransmissions. They have a
      // dependency on a single kMedia stream: |referenced_media_ssrc|.
      kRtx,
      // FlexFEC streams are streams dedicated to FlexFEC. They have a
      // dependency on a single kMedia stream: |referenced_media_ssrc|.
      kFlexfec,
    };

    StreamStats();
    ~StreamStats();

    std::string ToString() const;

    StreamType type = StreamType::kMedia;
    // If |type| is kRtx or kFlexfec this value is present. The referenced SSRC
    // is the kMedia stream that this stream is performing retransmissions or
    // FEC for. If |type| is kMedia, this value is null.
    absl::optional<uint32_t> referenced_media_ssrc;
    FrameCounts frame_counts;
    int width = 0;
    int height = 0;
    // TODO(holmer): Move bitrate_bps out to the webrtc::Call layer.
    int total_bitrate_bps = 0;
    int retransmit_bitrate_bps = 0;
    int avg_delay_ms = 0;
    int max_delay_ms = 0;
    uint64_t total_packet_send_delay_ms = 0;
    StreamDataCounters rtp_stats;
    RtcpPacketTypeCounter rtcp_packet_type_counts;
    // A snapshot of the most recent Report Block with additional data of
    // interest to statistics. Used to implement RTCRemoteInboundRtpStreamStats.
    absl::optional<ReportBlockData> report_block_data;
    double encode_frame_rate = 0.0;
    int frames_encoded = 0;
    absl::optional<uint64_t> qp_sum;
    uint64_t total_encode_time_ms = 0;
    uint64_t total_encoded_bytes_target = 0;
    uint32_t huge_frames_sent = 0;
  };

  struct Stats {
    Stats();
    ~Stats();
    std::string ToString(int64_t time_ms) const;
    std::string encoder_implementation_name = "unknown";
    int input_frame_rate = 0;
    int encode_frame_rate = 0;
    int avg_encode_time_ms = 0;
    int encode_usage_percent = 0;
    uint32_t frames_encoded = 0;
    // https://w3c.github.io/webrtc-stats/#dom-rtcoutboundrtpstreamstats-totalencodetime
    uint64_t total_encode_time_ms = 0;
    // https://w3c.github.io/webrtc-stats/#dom-rtcoutboundrtpstreamstats-totalencodedbytestarget
    uint64_t total_encoded_bytes_target = 0;
    uint32_t frames = 0;
    uint32_t frames_dropped_by_capturer = 0;
    uint32_t frames_dropped_by_encoder_queue = 0;
    uint32_t frames_dropped_by_rate_limiter = 0;
    uint32_t frames_dropped_by_congestion_window = 0;
    uint32_t frames_dropped_by_encoder = 0;
    // Bitrate the encoder is currently configured to use due to bandwidth
    // limitations.
    int target_media_bitrate_bps = 0;
    // Bitrate the encoder is actually producing.
    int media_bitrate_bps = 0;
    bool suspended = false;
    bool bw_limited_resolution = false;
    bool cpu_limited_resolution = false;
    bool bw_limited_framerate = false;
    bool cpu_limited_framerate = false;
    // https://w3c.github.io/webrtc-stats/#dom-rtcoutboundrtpstreamstats-qualitylimitationreason
    QualityLimitationReason quality_limitation_reason =
        QualityLimitationReason::kNone;
    // https://w3c.github.io/webrtc-stats/#dom-rtcoutboundrtpstreamstats-qualitylimitationdurations
    std::map<QualityLimitationReason, int64_t> quality_limitation_durations_ms;
    // https://w3c.github.io/webrtc-stats/#dom-rtcoutboundrtpstreamstats-qualitylimitationresolutionchanges
    uint32_t quality_limitation_resolution_changes = 0;
    // Total number of times resolution as been requested to be changed due to
    // CPU/quality adaptation.
    int number_of_cpu_adapt_changes = 0;
    int number_of_quality_adapt_changes = 0;
    bool has_entered_low_resolution = false;
    std::map<uint32_t, StreamStats> substreams;
    webrtc::VideoContentType content_type =
        webrtc::VideoContentType::UNSPECIFIED;
    uint32_t frames_sent = 0;
    uint32_t huge_frames_sent = 0;
  };

  struct Config {
   public:
    Config() = delete;
    Config(Config&&);
    explicit Config(Transport* send_transport);

    Config& operator=(Config&&);
    Config& operator=(const Config&) = delete;

    ~Config();

    // Mostly used by tests.  Avoid creating copies if you can.
    Config Copy() const { return Config(*this); }

    std::string ToString() const;

    RtpConfig rtp;

    VideoStreamEncoderSettings encoder_settings;

    // Time interval between RTCP report for video
    int rtcp_report_interval_ms = 1000;

    // Transport for outgoing packets.
    Transport* send_transport = nullptr;

    // Expected delay needed by the renderer, i.e. the frame will be delivered
    // this many milliseconds, if possible, earlier than expected render time.
    // Only valid if |local_renderer| is set.
    int render_delay_ms = 0;

    // Target delay in milliseconds. A positive value indicates this stream is
    // used for streaming instead of a real-time call.
    int target_delay_ms = 0;

    // True if the stream should be suspended when the available bitrate fall
    // below the minimum configured bitrate. If this variable is false, the
    // stream may send at a rate higher than the estimated available bitrate.
    bool suspend_below_min_bitrate = false;

    // Enables periodic bandwidth probing in application-limited region.
    bool periodic_alr_bandwidth_probing = false;

    // An optional custom frame encryptor that allows the entire frame to be
    // encrypted in whatever way the caller chooses. This is not required by
    // default.
    rtc::scoped_refptr<webrtc::FrameEncryptorInterface> frame_encryptor;

    // Per PeerConnection cryptography options.
    CryptoOptions crypto_options;

    rtc::scoped_refptr<webrtc::FrameTransformerInterface> frame_transformer;

   private:
    // Access to the copy constructor is private to force use of the Copy()
    // method for those exceptional cases where we do use it.
    Config(const Config&);
  };

  // Updates the sending state for all simulcast layers that the video send
  // stream owns. This can mean updating the activity one or for multiple
  // layers. The ordering of active layers is the order in which the
  // rtp modules are stored in the VideoSendStream.
  // Note: This starts stream activity if it is inactive and one of the layers
  // is active. This stops stream activity if it is active and all layers are
  // inactive.
  virtual void UpdateActiveSimulcastLayers(
      const std::vector<bool> active_layers) = 0;

  // Starts stream activity.
  // When a stream is active, it can receive, process and deliver packets.
  virtual void Start() = 0;
  // Stops stream activity.
  // When a stream is stopped, it can't receive, process or deliver packets.
  virtual void Stop() = 0;

  // If the resource is overusing, the VideoSendStream will try to reduce
  // resolution or frame rate until no resource is overusing.
  // TODO(https://crbug.com/webrtc/11565): When the ResourceAdaptationProcessor
  // is moved to Call this method could be deleted altogether in favor of
  // Call-level APIs only.
  virtual void AddAdaptationResource(rtc::scoped_refptr<Resource> resource) = 0;
  virtual std::vector<rtc::scoped_refptr<Resource>>
  GetAdaptationResources() = 0;

  virtual void SetSource(
      rtc::VideoSourceInterface<webrtc::VideoFrame>* source,
      const DegradationPreference& degradation_preference) = 0;

  // Set which streams to send. Must have at least as many SSRCs as configured
  // in the config. Encoder settings are passed on to the encoder instance along
  // with the VideoStream settings.
  virtual void ReconfigureVideoEncoder(VideoEncoderConfig config) = 0;

  virtual Stats GetStats() = 0;

 protected:
  virtual ~VideoSendStream() {}
};

}  // namespace webrtc

#endif  // CALL_VIDEO_SEND_STREAM_H_
