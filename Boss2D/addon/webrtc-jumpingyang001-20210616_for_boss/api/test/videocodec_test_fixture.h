/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_TEST_VIDEOCODEC_TEST_FIXTURE_H_
#define API_TEST_VIDEOCODEC_TEST_FIXTURE_H_

#include <string>
#include <vector>

#include BOSS_WEBRTC_U_api__test__videocodec_test_stats_h //original-code:"api/test/videocodec_test_stats.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_factory_h //original-code:"api/video_codecs/video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_factory_h //original-code:"api/video_codecs/video_encoder_factory.h"
#include BOSS_WEBRTC_U_media__base__h264_profile_level_id_h //original-code:"media/base/h264_profile_level_id.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"

namespace webrtc {
namespace test {

// Rates for the encoder and the frame number when to apply profile.
struct RateProfile {
  size_t target_kbps;
  double input_fps;
  size_t frame_num;
};

struct RateControlThresholds {
  double max_avg_bitrate_mismatch_percent;
  double max_time_to_reach_target_bitrate_sec;
  // TODO(ssilkin): Use absolute threshold for framerate.
  double max_avg_framerate_mismatch_percent;
  double max_avg_buffer_level_sec;
  double max_max_key_frame_delay_sec;
  double max_max_delta_frame_delay_sec;
  size_t max_num_spatial_resizes;
  size_t max_num_key_frames;
};

struct QualityThresholds {
  double min_avg_psnr;
  double min_min_psnr;
  double min_avg_ssim;
  double min_min_ssim;
};

struct BitstreamThresholds {
  size_t max_max_nalu_size_bytes;
};

// NOTE: This class is still under development and may change without notice.
class VideoCodecTestFixture {
 public:
  class EncodedFrameChecker {
   public:
    virtual ~EncodedFrameChecker() = default;
    virtual void CheckEncodedFrame(VideoCodecType codec,
                                   const EncodedImage& encoded_frame) const = 0;
  };

  struct Config {
    Config();
    void SetCodecSettings(std::string codec_name,
                          size_t num_simulcast_streams,
                          size_t num_spatial_layers,
                          size_t num_temporal_layers,
                          bool denoising_on,
                          bool frame_dropper_on,
                          bool spatial_resize_on,
                          size_t width,
                          size_t height);

    size_t NumberOfCores() const;
    size_t NumberOfTemporalLayers() const;
    size_t NumberOfSpatialLayers() const;
    size_t NumberOfSimulcastStreams() const;

    std::string ToString() const;
    std::string CodecName() const;

    // Name of this config, to be used for accounting by the test runner.
    std::string test_name;

    // Plain name of YUV file to process without file extension.
    std::string filename;
    // Dimensions of test clip. Falls back to (codec_settings.width/height) if
    // not set.
    absl::optional<int> clip_width;
    absl::optional<int> clip_height;
    // Framerate of input clip. Defaults to 30fps if not set.
    absl::optional<int> clip_fps;

    // The resolution at which psnr/ssim comparisons should be made. Frames
    // will be scaled to this size if different.
    absl::optional<int> reference_width;
    absl::optional<int> reference_height;

    // File to process. This must be a video file in the YUV format.
    std::string filepath;

    // Number of frames to process.
    size_t num_frames = 0;

    // Bitstream constraints.
    size_t max_payload_size_bytes = 1440;

    // Should we decode the encoded frames?
    bool decode = true;

    // Force the encoder and decoder to use a single core for processing.
    bool use_single_core = false;

    // Should cpu usage be measured?
    // If set to true, the encoding will run in real-time.
    bool measure_cpu = false;

    // Simulate frames arriving in real-time by adding delays between frames.
    bool encode_in_real_time = false;

    // Codec settings to use.
    VideoCodec codec_settings;

    // Name of the codec being tested.
    std::string codec_name;

    // H.264 specific settings.
    struct H264CodecSettings {
      H264Profile profile = H264Profile::kProfileConstrainedBaseline;
      H264PacketizationMode packetization_mode =
          H264PacketizationMode::NonInterleaved;
    } h264_codec_settings;

    // Custom checker that will be called for each frame.
    const EncodedFrameChecker* encoded_frame_checker = nullptr;

    // Print out frame level stats.
    bool print_frame_level_stats = false;

    // Path to a directory where encoded or/and decoded video should be saved.
    std::string output_path;

    // Should video be saved persistently to disk for post-run visualization?
    struct VisualizationParams {
      bool save_encoded_ivf = false;
      bool save_decoded_y4m = false;
    } visualization_params;

    // Enables quality analysis for dropped frames.
    bool analyze_quality_of_dropped_frames = false;
  };

  virtual ~VideoCodecTestFixture() = default;

  virtual void RunTest(const std::vector<RateProfile>& rate_profiles,
                       const std::vector<RateControlThresholds>* rc_thresholds,
                       const std::vector<QualityThresholds>* quality_thresholds,
                       const BitstreamThresholds* bs_thresholds) = 0;
  virtual VideoCodecTestStats& GetStats() = 0;
};

}  // namespace test
}  // namespace webrtc

#endif  // API_TEST_VIDEOCODEC_TEST_FIXTURE_H_
