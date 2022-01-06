/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_BITRATE_CONTROLLER_H_
#define MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_BITRATE_CONTROLLER_H_

#include <stddef.h>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_modules__audio_coding__audio_network_adaptor__controller_h //original-code:"modules/audio_coding/audio_network_adaptor/controller.h"
#include BOSS_WEBRTC_U_modules__audio_coding__audio_network_adaptor__include__audio_network_adaptor_config_h //original-code:"modules/audio_coding/audio_network_adaptor/include/audio_network_adaptor_config.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {
namespace audio_network_adaptor {

class BitrateController final : public Controller {
 public:
  struct Config {
    Config(int initial_bitrate_bps,
           int initial_frame_length_ms,
           int fl_increase_overhead_offset,
           int fl_decrease_overhead_offset);
    ~Config();
    int initial_bitrate_bps;
    int initial_frame_length_ms;
    int fl_increase_overhead_offset;
    int fl_decrease_overhead_offset;
  };

  explicit BitrateController(const Config& config);

  ~BitrateController() override;

  void UpdateNetworkMetrics(const NetworkMetrics& network_metrics) override;

  void MakeDecision(AudioEncoderRuntimeConfig* config) override;

 private:
  const Config config_;
  int bitrate_bps_;
  int frame_length_ms_;
  absl::optional<int> target_audio_bitrate_bps_;
  absl::optional<size_t> overhead_bytes_per_packet_;
  RTC_DISALLOW_COPY_AND_ASSIGN(BitrateController);
};

}  // namespace audio_network_adaptor
}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_BITRATE_CONTROLLER_H_
