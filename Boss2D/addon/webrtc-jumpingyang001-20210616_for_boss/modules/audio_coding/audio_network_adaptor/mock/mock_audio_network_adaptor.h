/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_MOCK_MOCK_AUDIO_NETWORK_ADAPTOR_H_
#define MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_MOCK_MOCK_AUDIO_NETWORK_ADAPTOR_H_

#include BOSS_WEBRTC_U_modules__audio_coding__audio_network_adaptor__include__audio_network_adaptor_h //original-code:"modules/audio_coding/audio_network_adaptor/include/audio_network_adaptor.h"
#include BOSS_WEBRTC_U_test__gmock_h //original-code:"test/gmock.h"

namespace webrtc {

class MockAudioNetworkAdaptor : public AudioNetworkAdaptor {
 public:
  ~MockAudioNetworkAdaptor() override { Die(); }
  MOCK_METHOD(void, Die, ());

  MOCK_METHOD(void, SetUplinkBandwidth, (int uplink_bandwidth_bps), (override));

  MOCK_METHOD(void,
              SetUplinkPacketLossFraction,
              (float uplink_packet_loss_fraction),
              (override));

  MOCK_METHOD(void, SetRtt, (int rtt_ms), (override));

  MOCK_METHOD(void,
              SetTargetAudioBitrate,
              (int target_audio_bitrate_bps),
              (override));

  MOCK_METHOD(void,
              SetOverhead,
              (size_t overhead_bytes_per_packet),
              (override));

  MOCK_METHOD(AudioEncoderRuntimeConfig,
              GetEncoderRuntimeConfig,
              (),
              (override));

  MOCK_METHOD(void, StartDebugDump, (FILE * file_handle), (override));

  MOCK_METHOD(void, StopDebugDump, (), (override));

  MOCK_METHOD(ANAStats, GetStats, (), (const, override));
};

}  // namespace webrtc

#endif  // MODULES_AUDIO_CODING_AUDIO_NETWORK_ADAPTOR_MOCK_MOCK_AUDIO_NETWORK_ADAPTOR_H_
