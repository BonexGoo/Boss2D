/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef TEST_PC_E2E_TEST_PEER_FACTORY_H_
#define TEST_PC_E2E_TEST_PEER_FACTORY_H_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__rtc_event_log__rtc_event_log_factory_h //original-code:"api/rtc_event_log/rtc_event_log_factory.h"
#include BOSS_WEBRTC_U_api__test__peerconnection_quality_test_fixture_h //original-code:"api/test/peerconnection_quality_test_fixture.h"
#include BOSS_WEBRTC_U_api__test__time_controller_h //original-code:"api/test/time_controller.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__test_audio_device_h //original-code:"modules/audio_device/include/test_audio_device.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"
#include "test/pc/e2e/analyzer/video/video_quality_analyzer_injection_helper.h"
#include "test/pc/e2e/peer_configurer.h"
#include "test/pc/e2e/peer_connection_quality_test_params.h"
#include "test/pc/e2e/test_peer.h"

namespace webrtc {
namespace webrtc_pc_e2e {

struct RemotePeerAudioConfig {
  explicit RemotePeerAudioConfig(
      PeerConnectionE2EQualityTestFixture::AudioConfig config)
      : sampling_frequency_in_hz(config.sampling_frequency_in_hz),
        output_file_name(config.output_dump_file_name) {}

  static absl::optional<RemotePeerAudioConfig> Create(
      absl::optional<PeerConnectionE2EQualityTestFixture::AudioConfig> config);

  int sampling_frequency_in_hz;
  absl::optional<std::string> output_file_name;
};

class TestPeerFactory {
 public:
  // Creates a test peer factory.
  // |signaling_thread| will be used as a signaling thread for all peers created
  // by this factory.
  // |time_controller| will be used to create required threads, task queue
  // factories and call factory.
  // |video_analyzer_helper| will be used to setup video quality analysis for
  // created peers.
  // |task_queue| will be used for AEC dump if it is requested.
  TestPeerFactory(rtc::Thread* signaling_thread,
                  TimeController& time_controller,
                  VideoQualityAnalyzerInjectionHelper* video_analyzer_helper,
                  rtc::TaskQueue* task_queue)
      : signaling_thread_(signaling_thread),
        time_controller_(time_controller),
        video_analyzer_helper_(video_analyzer_helper),
        task_queue_(task_queue) {}

  // Setups all components, that should be provided to WebRTC
  // PeerConnectionFactory and PeerConnection creation methods,
  // also will setup dependencies, that are required for media analyzers
  // injection.
  std::unique_ptr<TestPeer> CreateTestPeer(
      std::unique_ptr<PeerConfigurerImpl> configurer,
      std::unique_ptr<MockPeerConnectionObserver> observer,
      absl::optional<RemotePeerAudioConfig> remote_audio_config,
      double bitrate_multiplier,
      absl::optional<PeerConnectionE2EQualityTestFixture::EchoEmulationConfig>
          echo_emulation_config);

 private:
  rtc::Thread* signaling_thread_;
  TimeController& time_controller_;
  VideoQualityAnalyzerInjectionHelper* video_analyzer_helper_;
  rtc::TaskQueue* task_queue_;
};

}  // namespace webrtc_pc_e2e
}  // namespace webrtc

#endif  // TEST_PC_E2E_TEST_PEER_FACTORY_H_
