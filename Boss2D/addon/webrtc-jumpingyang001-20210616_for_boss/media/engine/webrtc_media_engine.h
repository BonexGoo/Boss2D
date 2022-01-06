/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MEDIA_ENGINE_WEBRTC_MEDIA_ENGINE_H_
#define MEDIA_ENGINE_WEBRTC_MEDIA_ENGINE_H_

#include <memory>
#include <string>
#include <vector>

#include BOSS_WEBRTC_U_api__audio__audio_frame_processor_h //original-code:"api/audio/audio_frame_processor.h"
#include BOSS_WEBRTC_U_api__audio__audio_mixer_h //original-code:"api/audio/audio_mixer.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_decoder_factory_h //original-code:"api/audio_codecs/audio_decoder_factory.h"
#include BOSS_WEBRTC_U_api__audio_codecs__audio_encoder_factory_h //original-code:"api/audio_codecs/audio_encoder_factory.h"
#include BOSS_WEBRTC_U_api__rtp_parameters_h //original-code:"api/rtp_parameters.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_factory_h //original-code:"api/task_queue/task_queue_factory.h"
#include BOSS_WEBRTC_U_api__transport__bitrate_settings_h //original-code:"api/transport/bitrate_settings.h"
#include BOSS_WEBRTC_U_api__transport__field_trial_based_config_h //original-code:"api/transport/field_trial_based_config.h"
#include BOSS_WEBRTC_U_api__transport__webrtc_key_value_config_h //original-code:"api/transport/webrtc_key_value_config.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_factory_h //original-code:"api/video_codecs/video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_encoder_factory_h //original-code:"api/video_codecs/video_encoder_factory.h"
#include BOSS_WEBRTC_U_media__base__codec_h //original-code:"media/base/codec.h"
#include BOSS_WEBRTC_U_media__base__media_engine_h //original-code:"media/base/media_engine.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__audio_device_h //original-code:"modules/audio_device/include/audio_device.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_rtc_base__system__rtc_export_h //original-code:"rtc_base/system/rtc_export.h"

namespace cricket {

struct MediaEngineDependencies {
  MediaEngineDependencies() = default;
  MediaEngineDependencies(const MediaEngineDependencies&) = delete;
  MediaEngineDependencies(MediaEngineDependencies&&) = default;
  MediaEngineDependencies& operator=(const MediaEngineDependencies&) = delete;
  MediaEngineDependencies& operator=(MediaEngineDependencies&&) = default;
  ~MediaEngineDependencies() = default;

  webrtc::TaskQueueFactory* task_queue_factory = nullptr;
  rtc::scoped_refptr<webrtc::AudioDeviceModule> adm;
  rtc::scoped_refptr<webrtc::AudioEncoderFactory> audio_encoder_factory;
  rtc::scoped_refptr<webrtc::AudioDecoderFactory> audio_decoder_factory;
  rtc::scoped_refptr<webrtc::AudioMixer> audio_mixer;
  rtc::scoped_refptr<webrtc::AudioProcessing> audio_processing;
  webrtc::AudioFrameProcessor* audio_frame_processor = nullptr;

  std::unique_ptr<webrtc::VideoEncoderFactory> video_encoder_factory;
  std::unique_ptr<webrtc::VideoDecoderFactory> video_decoder_factory;

  const webrtc::WebRtcKeyValueConfig* trials = nullptr;
};

// CreateMediaEngine may be called on any thread, though the engine is
// only expected to be used on one thread, internally called the "worker
// thread". This is the thread Init must be called on.
RTC_EXPORT std::unique_ptr<MediaEngineInterface> CreateMediaEngine(
    MediaEngineDependencies dependencies);

// Verify that extension IDs are within 1-byte extension range and are not
// overlapping.
bool ValidateRtpExtensions(const std::vector<webrtc::RtpExtension>& extensions);

// Discard any extensions not validated by the 'supported' predicate. Duplicate
// extensions are removed if 'filter_redundant_extensions' is set, and also any
// mutually exclusive extensions (see implementation for details) are removed.
std::vector<webrtc::RtpExtension> FilterRtpExtensions(
    const std::vector<webrtc::RtpExtension>& extensions,
    bool (*supported)(absl::string_view),
    bool filter_redundant_extensions,
    const webrtc::WebRtcKeyValueConfig& trials);

webrtc::BitrateConstraints GetBitrateConfigForCodec(const Codec& codec);

}  // namespace cricket

#endif  // MEDIA_ENGINE_WEBRTC_MEDIA_ENGINE_H_
