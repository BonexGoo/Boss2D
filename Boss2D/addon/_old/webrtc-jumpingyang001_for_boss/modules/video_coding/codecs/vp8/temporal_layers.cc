/* Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp8__temporal_layers_h //original-code:"modules/video_coding/codecs/vp8/temporal_layers.h"

#include <algorithm>
#include <memory>
#include <set>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__memory__memory_h //original-code:"absl/memory/memory.h"
#include BOSS_WEBRTC_U_modules__include__module_common_types_h //original-code:"modules/include/module_common_types.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp8__default_temporal_layers_h //original-code:"modules/video_coding/codecs/vp8/default_temporal_layers.h"
#include BOSS_WEBRTC_U_modules__video_coding__codecs__vp8__screenshare_layers_h //original-code:"modules/video_coding/codecs/vp8/screenshare_layers.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"
#include BOSS_WEBRTC_U_system_wrappers__include__field_trial_h //original-code:"system_wrappers/include/field_trial.h"

namespace webrtc {
namespace {
uint8_t NumTemporalLayers(const VideoCodec& codec, int spatial_id) {
  uint8_t num_temporal_layers =
      std::max<uint8_t>(1, codec.VP8().numberOfTemporalLayers);
  if (codec.numberOfSimulcastStreams > 0) {
    RTC_DCHECK_LT(spatial_id, codec.numberOfSimulcastStreams);
    num_temporal_layers =
        std::max(num_temporal_layers,
                 codec.simulcastStream[spatial_id].numberOfTemporalLayers);
  }
  return num_temporal_layers;
}

bool IsConferenceModeScreenshare(const VideoCodec& codec) {
  if (codec.mode != VideoCodecMode::kScreensharing ||
      NumTemporalLayers(codec, 0) != 2) {
    return false;
  }
  // Fixed default bitrates for legacy screenshare layers mode.
  return (codec.numberOfSimulcastStreams == 0 && codec.maxBitrate == 1000) ||
         (codec.numberOfSimulcastStreams >= 1 &&
          codec.simulcastStream[0].maxBitrate == 1000 &&
          codec.simulcastStream[0].targetBitrate == 200);
}
}  // namespace

std::unique_ptr<TemporalLayers> TemporalLayers::CreateTemporalLayers(
    const VideoCodec& codec,
    size_t spatial_id) {
  if (IsConferenceModeScreenshare(codec) && spatial_id == 0) {
    // Conference mode temporal layering for screen content in base stream.
    return absl::make_unique<ScreenshareLayers>(2, Clock::GetRealTimeClock());
  }

  return absl::make_unique<DefaultTemporalLayers>(
      NumTemporalLayers(codec, spatial_id));
}

std::unique_ptr<TemporalLayersChecker>
TemporalLayers::CreateTemporalLayersChecker(const VideoCodec& codec,
                                            size_t spatial_id) {
  if (IsConferenceModeScreenshare(codec) && spatial_id == 0) {
    // Conference mode temporal layering for screen content in base stream,
    // use generic checker.
    return absl::make_unique<TemporalLayersChecker>(2);
  }

  return absl::make_unique<DefaultTemporalLayersChecker>(
      NumTemporalLayers(codec, spatial_id));
}

TemporalLayersChecker::TemporalLayersChecker(int num_temporal_layers)
    : num_temporal_layers_(num_temporal_layers),
      sequence_number_(0),
      last_sync_sequence_number_(0),
      last_tl0_sequence_number_(0) {}

bool TemporalLayersChecker::CheckAndUpdateBufferState(
    BufferState* state,
    bool* need_sync,
    bool frame_is_keyframe,
    uint8_t temporal_layer,
    webrtc::TemporalLayers::BufferFlags flags,
    uint32_t sequence_number,
    uint32_t* lowest_sequence_referenced) {
  if (flags & TemporalLayers::BufferFlags::kReference) {
    if (state->temporal_layer > 0) {
      *need_sync = false;
    }
    if (!state->is_keyframe && !frame_is_keyframe &&
        state->sequence_number < *lowest_sequence_referenced) {
      *lowest_sequence_referenced = state->sequence_number;
    }
    if (!frame_is_keyframe && !state->is_keyframe &&
        state->temporal_layer > temporal_layer) {
      RTC_LOG(LS_ERROR) << "Frame is referencing higher temporal layer.";
      return false;
    }
  }
  if ((flags & TemporalLayers::BufferFlags::kUpdate)) {
    state->temporal_layer = temporal_layer;
    state->sequence_number = sequence_number;
    state->is_keyframe = frame_is_keyframe;
  }
  if (frame_is_keyframe)
    state->is_keyframe = true;
  return true;
}

bool TemporalLayersChecker::CheckTemporalConfig(
    bool frame_is_keyframe,
    const TemporalLayers::FrameConfig& frame_config) {
  if (frame_config.drop_frame) {
    return true;
  }
  ++sequence_number_;
  if (frame_config.packetizer_temporal_idx >= num_temporal_layers_ ||
      (frame_config.packetizer_temporal_idx == kNoTemporalIdx &&
       num_temporal_layers_ > 1)) {
    RTC_LOG(LS_ERROR) << "Incorrect temporal layer set for frame: "
                      << frame_config.packetizer_temporal_idx
                      << " num_temporal_layers: " << num_temporal_layers_;
    return false;
  }

  uint32_t lowest_sequence_referenced = sequence_number_;
  bool need_sync = frame_config.packetizer_temporal_idx > 0 &&
                   frame_config.packetizer_temporal_idx != kNoTemporalIdx;

  if (!CheckAndUpdateBufferState(
          &last_, &need_sync, frame_is_keyframe,
          frame_config.packetizer_temporal_idx, frame_config.last_buffer_flags,
          sequence_number_, &lowest_sequence_referenced)) {
    RTC_LOG(LS_ERROR) << "Error in the Last buffer";
    return false;
  }
  if (!CheckAndUpdateBufferState(&golden_, &need_sync, frame_is_keyframe,
                                 frame_config.packetizer_temporal_idx,
                                 frame_config.golden_buffer_flags,
                                 sequence_number_,
                                 &lowest_sequence_referenced)) {
    RTC_LOG(LS_ERROR) << "Error in the Golden buffer";
    return false;
  }
  if (!CheckAndUpdateBufferState(
          &arf_, &need_sync, frame_is_keyframe,
          frame_config.packetizer_temporal_idx, frame_config.arf_buffer_flags,
          sequence_number_, &lowest_sequence_referenced)) {
    RTC_LOG(LS_ERROR) << "Error in the Arf buffer";
    return false;
  }

  if (lowest_sequence_referenced < last_sync_sequence_number_ &&
      !frame_is_keyframe) {
    RTC_LOG(LS_ERROR) << "Reference past the last sync frame. Referenced "
                      << lowest_sequence_referenced << ", but sync was at "
                      << last_sync_sequence_number_;
    return false;
  }

  if (frame_config.packetizer_temporal_idx == 0) {
    last_tl0_sequence_number_ = sequence_number_;
  }

  if (frame_is_keyframe) {
    last_sync_sequence_number_ = sequence_number_;
  }

  if (need_sync) {
    last_sync_sequence_number_ = last_tl0_sequence_number_;
  }

  if (need_sync != frame_config.layer_sync) {
    RTC_LOG(LS_ERROR) << "Sync bit is set incorrectly on a frame. Expected: "
                      << need_sync << " Actual: " << frame_config.layer_sync;
    return false;
  }
  return true;
}
}  // namespace webrtc
