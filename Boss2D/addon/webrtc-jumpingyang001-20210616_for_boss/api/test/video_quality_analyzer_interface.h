/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_TEST_VIDEO_QUALITY_ANALYZER_INTERFACE_H_
#define API_TEST_VIDEO_QUALITY_ANALYZER_INTERFACE_H_

#include <memory>
#include <string>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__test__stats_observer_interface_h //original-code:"api/test/stats_observer_interface.h"
#include BOSS_WEBRTC_U_api__video__encoded_image_h //original-code:"api/video/encoded_image.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"

namespace webrtc {
namespace webrtc_pc_e2e {

// API is in development and can be changed without notice.

// Base interface for video quality analyzer for peer connection level end-2-end
// tests. Interface has only one abstract method, which have to return frame id.
// Other methods have empty implementation by default, so user can override only
// required parts.
//
// VideoQualityAnalyzerInterface will be injected into WebRTC pipeline on both
// sides of the call. Here is video data flow in WebRTC pipeline
//
// Alice:
//  ___________       ________       _________
// |           |     |        |     |         |
// |   Frame   |-(A)→| WebRTC |-(B)→| Video   |-(C)┐
// | Generator |     | Stack  |     | Decoder |    |
//  ¯¯¯¯¯¯¯¯¯¯¯       ¯¯¯¯¯¯¯¯       ¯¯¯¯¯¯¯¯¯     |
//                                               __↓________
//                                              | Transport |
//                                              |     &     |
//                                              |  Network  |
//                                               ¯¯|¯¯¯¯¯¯¯¯
// Bob:                                            |
//  _______       ________       _________         |
// |       |     |        |     |         |        |
// | Video |←(F)-| WebRTC |←(E)-| Video   |←(D)----┘
// | Sink  |     | Stack  |     | Decoder |
//  ¯¯¯¯¯¯¯       ¯¯¯¯¯¯¯¯       ¯¯¯¯¯¯¯¯¯
// The analyzer will be injected in all points from A to F.
class VideoQualityAnalyzerInterface : public StatsObserverInterface {
 public:
  // Contains extra statistic provided by video encoder.
  struct EncoderStats {
    // TODO(hbos) https://crbug.com/webrtc/9547,
    // https://crbug.com/webrtc/11443: improve stats API to make available
    // there.
    uint32_t target_encode_bitrate;
  };
  // Contains extra statistic provided by video decoder.
  struct DecoderStats {
    // Decode time provided by decoder itself. If decoder doesn’t produce such
    // information can be omitted.
    absl::optional<int32_t> decode_time_ms;
  };

  ~VideoQualityAnalyzerInterface() override = default;

  // Will be called by framework before test.
  // |test_case_name| is name of test case, that should be used to report all
  // video metrics.
  // |threads_count| is number of threads that analyzer can use for heavy
  // calculations. Analyzer can perform simple calculations on the calling
  // thread in each method, but should remember, that it is the same thread,
  // that is used in video pipeline.
  virtual void Start(std::string test_case_name,
                     rtc::ArrayView<const std::string> peer_names,
                     int max_threads_count) {}

  // Will be called when frame was generated from the input stream.
  // |peer_name| is name of the peer on which side frame was captured.
  // Returns frame id, that will be set by framework to the frame.
  virtual uint16_t OnFrameCaptured(absl::string_view peer_name,
                                   const std::string& stream_label,
                                   const VideoFrame& frame) = 0;
  // Will be called before calling the encoder.
  // |peer_name| is name of the peer on which side frame came to encoder.
  virtual void OnFramePreEncode(absl::string_view peer_name,
                                const VideoFrame& frame) {}
  // Will be called for each EncodedImage received from encoder. Single
  // VideoFrame can produce multiple EncodedImages. Each encoded image will
  // have id from VideoFrame.
  // |peer_name| is name of the peer on which side frame was encoded.
  virtual void OnFrameEncoded(absl::string_view peer_name,
                              uint16_t frame_id,
                              const EncodedImage& encoded_image,
                              const EncoderStats& stats) {}
  // Will be called for each frame dropped by encoder.
  // |peer_name| is name of the peer on which side frame drop was detected.
  virtual void OnFrameDropped(absl::string_view peer_name,
                              EncodedImageCallback::DropReason reason) {}
  // Will be called before calling the decoder.
  // |peer_name| is name of the peer on which side frame was received.
  virtual void OnFramePreDecode(absl::string_view peer_name,
                                uint16_t frame_id,
                                const EncodedImage& encoded_image) {}
  // Will be called after decoding the frame.
  // |peer_name| is name of the peer on which side frame was decoded.
  virtual void OnFrameDecoded(absl::string_view peer_name,
                              const VideoFrame& frame,
                              const DecoderStats& stats) {}
  // Will be called when frame will be obtained from PeerConnection stack.
  // |peer_name| is name of the peer on which side frame was rendered.
  virtual void OnFrameRendered(absl::string_view peer_name,
                               const VideoFrame& frame) {}
  // Will be called if encoder return not WEBRTC_VIDEO_CODEC_OK.
  // All available codes are listed in
  // modules/video_coding/include/video_error_codes.h
  // |peer_name| is name of the peer on which side error acquired.
  virtual void OnEncoderError(absl::string_view peer_name,
                              const VideoFrame& frame,
                              int32_t error_code) {}
  // Will be called if decoder return not WEBRTC_VIDEO_CODEC_OK.
  // All available codes are listed in
  // modules/video_coding/include/video_error_codes.h
  // |peer_name| is name of the peer on which side error acquired.
  virtual void OnDecoderError(absl::string_view peer_name,
                              uint16_t frame_id,
                              int32_t error_code) {}
  // Will be called every time new stats reports are available for the
  // Peer Connection identified by |pc_label|.
  void OnStatsReports(
      absl::string_view pc_label,
      const rtc::scoped_refptr<const RTCStatsReport>& report) override {}

  // Will be called before test adds new participant in the middle of a call.
  virtual void RegisterParticipantInCall(absl::string_view peer_name) {}

  // Tells analyzer that analysis complete and it should calculate final
  // statistics.
  virtual void Stop() {}

  virtual std::string GetStreamLabel(uint16_t frame_id) = 0;
};

}  // namespace webrtc_pc_e2e
}  // namespace webrtc

#endif  // API_TEST_VIDEO_QUALITY_ANALYZER_INTERFACE_H_
