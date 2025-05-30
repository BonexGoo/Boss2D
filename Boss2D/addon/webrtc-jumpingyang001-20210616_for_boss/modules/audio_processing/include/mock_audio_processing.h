/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_PROCESSING_INCLUDE_MOCK_AUDIO_PROCESSING_H_
#define MODULES_AUDIO_PROCESSING_INCLUDE_MOCK_AUDIO_PROCESSING_H_

#include <memory>

#include BOSS_WEBRTC_U_modules__audio_processing__include__aec_dump_h //original-code:"modules/audio_processing/include/aec_dump.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_h //original-code:"modules/audio_processing/include/audio_processing.h"
#include BOSS_WEBRTC_U_modules__audio_processing__include__audio_processing_statistics_h //original-code:"modules/audio_processing/include/audio_processing_statistics.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

namespace test {
class MockCustomProcessing : public CustomProcessing {
 public:
  virtual ~MockCustomProcessing() {}
  MOCK_METHOD(void,
              Initialize,
              (int sample_rate_hz, int num_channels),
              (override));
  MOCK_METHOD(void, Process, (AudioBuffer * audio), (override));
  MOCK_METHOD(void,
              SetRuntimeSetting,
              (AudioProcessing::RuntimeSetting setting),
              (override));
  MOCK_METHOD(std::string, ToString, (), (const, override));
};

class MockCustomAudioAnalyzer : public CustomAudioAnalyzer {
 public:
  virtual ~MockCustomAudioAnalyzer() {}
  MOCK_METHOD(void,
              Initialize,
              (int sample_rate_hz, int num_channels),
              (override));
  MOCK_METHOD(void, Analyze, (const AudioBuffer* audio), (override));
  MOCK_METHOD(std::string, ToString, (), (const, override));
};

class MockEchoControl : public EchoControl {
 public:
  virtual ~MockEchoControl() {}
  MOCK_METHOD(void, AnalyzeRender, (AudioBuffer * render), (override));
  MOCK_METHOD(void, AnalyzeCapture, (AudioBuffer * capture), (override));
  MOCK_METHOD(void,
              ProcessCapture,
              (AudioBuffer * capture, bool echo_path_change),
              (override));
  MOCK_METHOD(void,
              ProcessCapture,
              (AudioBuffer * capture,
               AudioBuffer* linear_output,
               bool echo_path_change),
              (override));
  MOCK_METHOD(Metrics, GetMetrics, (), (const, override));
  MOCK_METHOD(void, SetAudioBufferDelay, (int delay_ms), (override));
  MOCK_METHOD(bool, ActiveProcessing, (), (const, override));
};

class MockAudioProcessing : public AudioProcessing {
 public:
  MockAudioProcessing() {}

  virtual ~MockAudioProcessing() {}

  MOCK_METHOD(int, Initialize, (), (override));
  MOCK_METHOD(int,
              Initialize,
              (int capture_input_sample_rate_hz,
               int capture_output_sample_rate_hz,
               int render_sample_rate_hz,
               ChannelLayout capture_input_layout,
               ChannelLayout capture_output_layout,
               ChannelLayout render_input_layout),
              (override));
  MOCK_METHOD(int,
              Initialize,
              (const ProcessingConfig& processing_config),
              (override));
  MOCK_METHOD(void, ApplyConfig, (const Config& config), (override));
  MOCK_METHOD(int, proc_sample_rate_hz, (), (const, override));
  MOCK_METHOD(int, proc_split_sample_rate_hz, (), (const, override));
  MOCK_METHOD(size_t, num_input_channels, (), (const, override));
  MOCK_METHOD(size_t, num_proc_channels, (), (const, override));
  MOCK_METHOD(size_t, num_output_channels, (), (const, override));
  MOCK_METHOD(size_t, num_reverse_channels, (), (const, override));
  MOCK_METHOD(void, set_output_will_be_muted, (bool muted), (override));
  MOCK_METHOD(void, SetRuntimeSetting, (RuntimeSetting setting), (override));
  MOCK_METHOD(bool, PostRuntimeSetting, (RuntimeSetting setting), (override));
  MOCK_METHOD(int,
              ProcessStream,
              (const int16_t* const src,
               const StreamConfig& input_config,
               const StreamConfig& output_config,
               int16_t* const dest),
              (override));
  MOCK_METHOD(int,
              ProcessStream,
              (const float* const* src,
               const StreamConfig& input_config,
               const StreamConfig& output_config,
               float* const* dest),
              (override));
  MOCK_METHOD(int,
              ProcessReverseStream,
              (const int16_t* const src,
               const StreamConfig& input_config,
               const StreamConfig& output_config,
               int16_t* const dest),
              (override));
  MOCK_METHOD(int,
              AnalyzeReverseStream,
              (const float* const* data, const StreamConfig& reverse_config),
              (override));
  MOCK_METHOD(int,
              ProcessReverseStream,
              (const float* const* src,
               const StreamConfig& input_config,
               const StreamConfig& output_config,
               float* const* dest),
              (override));
  MOCK_METHOD(bool,
              GetLinearAecOutput,
              ((rtc::ArrayView<std::array<float, 160>> linear_output)),
              (const, override));
  MOCK_METHOD(int, set_stream_delay_ms, (int delay), (override));
  MOCK_METHOD(int, stream_delay_ms, (), (const, override));
  MOCK_METHOD(void, set_stream_key_pressed, (bool key_pressed), (override));
  MOCK_METHOD(void, set_stream_analog_level, (int), (override));
  MOCK_METHOD(int, recommended_stream_analog_level, (), (const, override));
  MOCK_METHOD(bool,
              CreateAndAttachAecDump,
              (const std::string& file_name,
               int64_t max_log_size_bytes,
               rtc::TaskQueue* worker_queue),
              (override));
  MOCK_METHOD(bool,
              CreateAndAttachAecDump,
              (FILE * handle,
               int64_t max_log_size_bytes,
               rtc::TaskQueue* worker_queue),
              (override));
  MOCK_METHOD(void, AttachAecDump, (std::unique_ptr<AecDump>), (override));
  MOCK_METHOD(void, DetachAecDump, (), (override));

  MOCK_METHOD(AudioProcessingStats, GetStatistics, (), (override));
  MOCK_METHOD(AudioProcessingStats, GetStatistics, (bool), (override));

  MOCK_METHOD(AudioProcessing::Config, GetConfig, (), (const, override));
};

}  // namespace test
}  // namespace webrtc

#endif  // MODULES_AUDIO_PROCESSING_INCLUDE_MOCK_AUDIO_PROCESSING_H_
