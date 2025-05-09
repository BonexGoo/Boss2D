/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_initializer_h //original-code:"modules/video_coding/include/video_codec_initializer.h"

#include <stdint.h>
#include <string.h>

#include <algorithm>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__units__data_rate_h //original-code:"api/units/data_rate.h"
#include BOSS_WEBRTC_U_api__video__video_bitrate_allocation_h //original-code:"api/video/video_bitrate_allocation.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_h //original-code:"api/video_codecs/video_encoder.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__av1__av1_svc_config_h //original-code:"modules/video_coding/codecs/av1/av1_svc_config.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp9__svc_config_h //original-code:"modules/video_coding/codecs/vp9/svc_config.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_coding_defines_h //original-code:"modules/video_coding/include/video_coding_defines.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__min_video_bitrate_experiment_h //original-code:"rtc_base/experiments/min_video_bitrate_experiment.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"

namespace webrtc {

bool VideoCodecInitializer::SetupCodec(const VideoEncoderConfig& config,
                                       const std::vector<VideoStream>& streams,
                                       VideoCodec* codec) {
  if (config.codec_type == kVideoCodecMultiplex) {
    VideoEncoderConfig associated_config = config.Copy();
    associated_config.codec_type = kVideoCodecVP9;
    if (!SetupCodec(associated_config, streams, codec)) {
      RTC_LOG(LS_ERROR) << "Failed to create stereo encoder configuration.";
      return false;
    }
    codec->codecType = kVideoCodecMultiplex;
    return true;
  }

  *codec = VideoEncoderConfigToVideoCodec(config, streams);
  return true;
}

// TODO(sprang): Split this up and separate the codec specific parts.
VideoCodec VideoCodecInitializer::VideoEncoderConfigToVideoCodec(
    const VideoEncoderConfig& config,
    const std::vector<VideoStream>& streams) {
  static const int kEncoderMinBitrateKbps = 30;
  RTC_DCHECK(!streams.empty());
  RTC_DCHECK_GE(config.min_transmit_bitrate_bps, 0);

  VideoCodec video_codec;
  video_codec.codecType = config.codec_type;

  switch (config.content_type) {
    case VideoEncoderConfig::ContentType::kRealtimeVideo:
      video_codec.mode = VideoCodecMode::kRealtimeVideo;
      break;
    case VideoEncoderConfig::ContentType::kScreen:
      video_codec.mode = VideoCodecMode::kScreensharing;
      break;
  }

  video_codec.legacy_conference_mode =
      config.content_type == VideoEncoderConfig::ContentType::kScreen &&
      config.legacy_conference_mode;

  video_codec.numberOfSimulcastStreams =
      static_cast<unsigned char>(streams.size());
  video_codec.minBitrate = streams[0].min_bitrate_bps / 1000;
  bool codec_active = false;
  // Active configuration might not be fully copied to |streams| for SVC yet.
  // Therefore the |config| is checked here.
  for (const VideoStream& stream : config.simulcast_layers) {
    if (stream.active) {
      codec_active = true;
      break;
    }
  }
  // Set active for the entire video codec for the non simulcast case.
  video_codec.active = codec_active;
  if (video_codec.minBitrate < kEncoderMinBitrateKbps)
    video_codec.minBitrate = kEncoderMinBitrateKbps;
  video_codec.timing_frame_thresholds = {kDefaultTimingFramesDelayMs,
                                         kDefaultOutlierFrameSizePercent};
  RTC_DCHECK_LE(streams.size(), kMaxSimulcastStreams);

  int max_framerate = 0;

  absl::optional<std::string> scalability_mode = streams[0].scalability_mode;
  for (size_t i = 0; i < streams.size(); ++i) {
    SpatialLayer* sim_stream = &video_codec.simulcastStream[i];
    RTC_DCHECK_GT(streams[i].width, 0);
    RTC_DCHECK_GT(streams[i].height, 0);
    RTC_DCHECK_GT(streams[i].max_framerate, 0);
    RTC_DCHECK_GE(streams[i].min_bitrate_bps, 0);
    RTC_DCHECK_GE(streams[i].target_bitrate_bps, streams[i].min_bitrate_bps);
    RTC_DCHECK_GE(streams[i].max_bitrate_bps, streams[i].target_bitrate_bps);
    RTC_DCHECK_GE(streams[i].max_qp, 0);

    sim_stream->width = static_cast<uint16_t>(streams[i].width);
    sim_stream->height = static_cast<uint16_t>(streams[i].height);
    sim_stream->maxFramerate = streams[i].max_framerate;
    sim_stream->minBitrate = streams[i].min_bitrate_bps / 1000;
    sim_stream->targetBitrate = streams[i].target_bitrate_bps / 1000;
    sim_stream->maxBitrate = streams[i].max_bitrate_bps / 1000;
    sim_stream->qpMax = streams[i].max_qp;
    sim_stream->numberOfTemporalLayers =
        static_cast<unsigned char>(streams[i].num_temporal_layers.value_or(1));
    sim_stream->active = streams[i].active;

    video_codec.width =
        std::max(video_codec.width, static_cast<uint16_t>(streams[i].width));
    video_codec.height =
        std::max(video_codec.height, static_cast<uint16_t>(streams[i].height));
    video_codec.minBitrate =
        std::min(static_cast<uint16_t>(video_codec.minBitrate),
                 static_cast<uint16_t>(streams[i].min_bitrate_bps / 1000));
    video_codec.maxBitrate += streams[i].max_bitrate_bps / 1000;
    video_codec.qpMax = std::max(video_codec.qpMax,
                                 static_cast<unsigned int>(streams[i].max_qp));
    max_framerate = std::max(max_framerate, streams[i].max_framerate);

    if (streams[0].scalability_mode != streams[i].scalability_mode) {
      RTC_LOG(LS_WARNING) << "Inconsistent scalability modes configured.";
      scalability_mode.reset();
    }
  }

  if (scalability_mode.has_value()) {
    video_codec.SetScalabilityMode(*scalability_mode);
  }

  if (video_codec.maxBitrate == 0) {
    // Unset max bitrate -> cap to one bit per pixel.
    video_codec.maxBitrate =
        (video_codec.width * video_codec.height * video_codec.maxFramerate) /
        1000;
  }
  if (video_codec.maxBitrate < kEncoderMinBitrateKbps)
    video_codec.maxBitrate = kEncoderMinBitrateKbps;

  video_codec.maxFramerate = max_framerate;
  video_codec.spatialLayers[0] = {0};
  video_codec.spatialLayers[0].width = video_codec.width;
  video_codec.spatialLayers[0].height = video_codec.height;
  video_codec.spatialLayers[0].maxFramerate = max_framerate;
  video_codec.spatialLayers[0].numberOfTemporalLayers =
      streams[0].num_temporal_layers.value_or(1);

  // Set codec specific options
  if (config.encoder_specific_settings)
    config.encoder_specific_settings->FillEncoderSpecificSettings(&video_codec);

  switch (video_codec.codecType) {
    case kVideoCodecVP8: {
      if (!config.encoder_specific_settings) {
        *video_codec.VP8() = VideoEncoder::GetDefaultVp8Settings();
      }

      video_codec.VP8()->numberOfTemporalLayers = static_cast<unsigned char>(
          streams.back().num_temporal_layers.value_or(
              video_codec.VP8()->numberOfTemporalLayers));
      RTC_DCHECK_GE(video_codec.VP8()->numberOfTemporalLayers, 1);
      RTC_DCHECK_LE(video_codec.VP8()->numberOfTemporalLayers,
                    kMaxTemporalStreams);

      break;
    }
    case kVideoCodecVP9: {
      // Force the first stream to always be active.
      video_codec.simulcastStream[0].active = codec_active;

      if (!config.encoder_specific_settings) {
        *video_codec.VP9() = VideoEncoder::GetDefaultVp9Settings();
      }

      video_codec.VP9()->numberOfTemporalLayers = static_cast<unsigned char>(
          streams.back().num_temporal_layers.value_or(
              video_codec.VP9()->numberOfTemporalLayers));
      RTC_DCHECK_GE(video_codec.VP9()->numberOfTemporalLayers, 1);
      RTC_DCHECK_LE(video_codec.VP9()->numberOfTemporalLayers,
                    kMaxTemporalStreams);

      RTC_DCHECK(config.spatial_layers.empty() ||
                 config.spatial_layers.size() ==
                     video_codec.VP9()->numberOfSpatialLayers);

      std::vector<SpatialLayer> spatial_layers;
      if (!config.spatial_layers.empty()) {
        // Layering is set explicitly.
        spatial_layers = config.spatial_layers;
      } else {
        size_t first_active_layer = 0;
        for (size_t spatial_idx = 0;
             spatial_idx < config.simulcast_layers.size(); ++spatial_idx) {
          if (config.simulcast_layers[spatial_idx].active) {
            first_active_layer = spatial_idx;
            break;
          }
        }

        spatial_layers = GetSvcConfig(
            video_codec.width, video_codec.height, video_codec.maxFramerate,
            first_active_layer, video_codec.VP9()->numberOfSpatialLayers,
            video_codec.VP9()->numberOfTemporalLayers,
            video_codec.mode == VideoCodecMode::kScreensharing);

        // If there was no request for spatial layering, don't limit bitrate
        // of single spatial layer.
        const bool no_spatial_layering =
            video_codec.VP9()->numberOfSpatialLayers <= 1;
        if (no_spatial_layering) {
          // Use codec's bitrate limits.
          spatial_layers.back().minBitrate = video_codec.minBitrate;
          spatial_layers.back().targetBitrate = video_codec.maxBitrate;
          spatial_layers.back().maxBitrate = video_codec.maxBitrate;
        }

        for (size_t spatial_idx = first_active_layer;
             spatial_idx < config.simulcast_layers.size() &&
             spatial_idx < spatial_layers.size() + first_active_layer;
             ++spatial_idx) {
          spatial_layers[spatial_idx - first_active_layer].active =
              config.simulcast_layers[spatial_idx].active;
        }
      }

      RTC_DCHECK(!spatial_layers.empty());
      for (size_t i = 0; i < spatial_layers.size(); ++i) {
        video_codec.spatialLayers[i] = spatial_layers[i];
      }

      // The top spatial layer dimensions may not be equal to the input
      // resolution because of the rounding or explicit configuration.
      // This difference must be propagated to the stream configuration.
      video_codec.width = spatial_layers.back().width;
      video_codec.height = spatial_layers.back().height;
      video_codec.simulcastStream[0].width = spatial_layers.back().width;
      video_codec.simulcastStream[0].height = spatial_layers.back().height;

      // Update layering settings.
      video_codec.VP9()->numberOfSpatialLayers =
          static_cast<unsigned char>(spatial_layers.size());
      RTC_DCHECK_GE(video_codec.VP9()->numberOfSpatialLayers, 1);
      RTC_DCHECK_LE(video_codec.VP9()->numberOfSpatialLayers,
                    kMaxSpatialLayers);

      video_codec.VP9()->numberOfTemporalLayers = static_cast<unsigned char>(
          spatial_layers.back().numberOfTemporalLayers);
      RTC_DCHECK_GE(video_codec.VP9()->numberOfTemporalLayers, 1);
      RTC_DCHECK_LE(video_codec.VP9()->numberOfTemporalLayers,
                    kMaxTemporalStreams);

      break;
    }
    case kVideoCodecAV1:
      if (SetAv1SvcConfig(video_codec)) {
        for (size_t i = 0; i < config.spatial_layers.size(); ++i) {
          video_codec.spatialLayers[i].active = config.spatial_layers[i].active;
        }
      } else {
        RTC_LOG(LS_WARNING) << "Failed to configure svc bitrates for av1.";
      }
      break;
    case kVideoCodecH264: {
      if (!config.encoder_specific_settings)
        *video_codec.H264() = VideoEncoder::GetDefaultH264Settings();
      video_codec.H264()->numberOfTemporalLayers = static_cast<unsigned char>(
          streams.back().num_temporal_layers.value_or(
              video_codec.H264()->numberOfTemporalLayers));
      RTC_DCHECK_GE(video_codec.H264()->numberOfTemporalLayers, 1);
      RTC_DCHECK_LE(video_codec.H264()->numberOfTemporalLayers,
                    kMaxTemporalStreams);
      break;
    }
    default:
      // TODO(pbos): Support encoder_settings codec-agnostically.
      RTC_DCHECK(!config.encoder_specific_settings)
          << "Encoder-specific settings for codec type not wired up.";
      break;
  }

  const absl::optional<DataRate> experimental_min_bitrate =
      GetExperimentalMinVideoBitrate(video_codec.codecType);
  if (experimental_min_bitrate) {
    const int experimental_min_bitrate_kbps =
        rtc::saturated_cast<int>(experimental_min_bitrate->kbps());
    video_codec.minBitrate = experimental_min_bitrate_kbps;
    video_codec.simulcastStream[0].minBitrate = experimental_min_bitrate_kbps;
    if (video_codec.codecType == kVideoCodecVP9) {
      video_codec.spatialLayers[0].minBitrate = experimental_min_bitrate_kbps;
    }
  }

  return video_codec;
}

}  // namespace webrtc
