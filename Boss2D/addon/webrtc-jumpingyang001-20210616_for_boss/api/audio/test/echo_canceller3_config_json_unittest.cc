/*
 *  Copyright 2018 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_json_h //original-code:"api/audio/echo_canceller3_config_json.h"

#include BOSS_WEBRTC_U_api__audio__echo_canceller3_config_h //original-code:"api/audio/echo_canceller3_config.h"
#include BOSS_WEBRTC_U_test__gtest_h //original-code:"test/gtest.h"

namespace webrtc {

TEST(EchoCanceller3JsonHelpers, ToStringAndParseJson) {
  EchoCanceller3Config cfg;
  cfg.delay.down_sampling_factor = 1u;
  cfg.delay.log_warning_on_delay_changes = true;
  cfg.filter.refined.error_floor = 2.f;
  cfg.filter.coarse_initial.length_blocks = 3u;
  cfg.filter.high_pass_filter_echo_reference =
      !cfg.filter.high_pass_filter_echo_reference;
  cfg.comfort_noise.noise_floor_dbfs = 100.f;
  cfg.echo_model.model_reverb_in_nonlinear_mode = false;
  cfg.suppressor.normal_tuning.mask_hf.enr_suppress = .5f;
  cfg.suppressor.subband_nearend_detection.nearend_average_blocks = 3;
  cfg.suppressor.subband_nearend_detection.subband1 = {1, 3};
  cfg.suppressor.subband_nearend_detection.subband1 = {4, 5};
  cfg.suppressor.subband_nearend_detection.nearend_threshold = 2.f;
  cfg.suppressor.subband_nearend_detection.snr_threshold = 100.f;
  std::string json_string = Aec3ConfigToJsonString(cfg);
  EchoCanceller3Config cfg_transformed = Aec3ConfigFromJsonString(json_string);

  // Expect unchanged values to remain default.
  EXPECT_EQ(cfg.ep_strength.default_len,
            cfg_transformed.ep_strength.default_len);
  EXPECT_EQ(cfg.suppressor.normal_tuning.mask_lf.enr_suppress,
            cfg_transformed.suppressor.normal_tuning.mask_lf.enr_suppress);

  // Expect changed values to carry through the transformation.
  EXPECT_EQ(cfg.delay.down_sampling_factor,
            cfg_transformed.delay.down_sampling_factor);
  EXPECT_EQ(cfg.delay.log_warning_on_delay_changes,
            cfg_transformed.delay.log_warning_on_delay_changes);
  EXPECT_EQ(cfg.filter.coarse_initial.length_blocks,
            cfg_transformed.filter.coarse_initial.length_blocks);
  EXPECT_EQ(cfg.filter.refined.error_floor,
            cfg_transformed.filter.refined.error_floor);
  EXPECT_EQ(cfg.filter.high_pass_filter_echo_reference,
            cfg_transformed.filter.high_pass_filter_echo_reference);
  EXPECT_EQ(cfg.comfort_noise.noise_floor_dbfs,
            cfg_transformed.comfort_noise.noise_floor_dbfs);
  EXPECT_EQ(cfg.echo_model.model_reverb_in_nonlinear_mode,
            cfg_transformed.echo_model.model_reverb_in_nonlinear_mode);
  EXPECT_EQ(cfg.suppressor.normal_tuning.mask_hf.enr_suppress,
            cfg_transformed.suppressor.normal_tuning.mask_hf.enr_suppress);
  EXPECT_EQ(cfg.suppressor.subband_nearend_detection.nearend_average_blocks,
            cfg_transformed.suppressor.subband_nearend_detection
                .nearend_average_blocks);
  EXPECT_EQ(cfg.suppressor.subband_nearend_detection.subband1.low,
            cfg_transformed.suppressor.subband_nearend_detection.subband1.low);
  EXPECT_EQ(cfg.suppressor.subband_nearend_detection.subband1.high,
            cfg_transformed.suppressor.subband_nearend_detection.subband1.high);
  EXPECT_EQ(cfg.suppressor.subband_nearend_detection.subband2.low,
            cfg_transformed.suppressor.subband_nearend_detection.subband2.low);
  EXPECT_EQ(cfg.suppressor.subband_nearend_detection.subband2.high,
            cfg_transformed.suppressor.subband_nearend_detection.subband2.high);
  EXPECT_EQ(
      cfg.suppressor.subband_nearend_detection.nearend_threshold,
      cfg_transformed.suppressor.subband_nearend_detection.nearend_threshold);
  EXPECT_EQ(cfg.suppressor.subband_nearend_detection.snr_threshold,
            cfg_transformed.suppressor.subband_nearend_detection.snr_threshold);
}
}  // namespace webrtc
