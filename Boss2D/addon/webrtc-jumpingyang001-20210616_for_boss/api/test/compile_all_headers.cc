/*
 *  Copyright 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file verifies that all include files in this directory can be
// compiled without errors or other required includes.

// Note: The following header files are not not tested here, as their
// associated targets are not included in all configurations.
// "api/test/audioproc_float.h"
// "api/test/create_video_quality_test_fixture.h"
// "api/test/neteq_simulator_factory.h"
// "api/test/video_quality_test_fixture.h"
// The following header files are also not tested:
// "api/test/create_simulcast_test_fixture.h"
// "api/test/create_videocodec_test_fixture.h"
// "api/test/neteq_simulator.h"
// "api/test/simulated_network.h"
// "api/test/simulcast_test_fixture.h"
// "api/test/test_dependency_factory.h"
// "api/test/videocodec_test_fixture.h"
// "api/test/videocodec_test_stats.h"

#include BOSS_WEBRTC_U_api__test__dummy_peer_connection_h //original-code:"api/test/dummy_peer_connection.h"
#include BOSS_WEBRTC_U_api__test__fake_frame_decryptor_h //original-code:"api/test/fake_frame_decryptor.h"
#include BOSS_WEBRTC_U_api__test__fake_frame_encryptor_h //original-code:"api/test/fake_frame_encryptor.h"
#include BOSS_WEBRTC_U_api__test__mock_async_dns_resolver_h //original-code:"api/test/mock_async_dns_resolver.h"
#include BOSS_WEBRTC_U_api__test__mock_audio_mixer_h //original-code:"api/test/mock_audio_mixer.h"
#include BOSS_WEBRTC_U_api__test__mock_data_channel_h //original-code:"api/test/mock_data_channel.h"
#include BOSS_WEBRTC_U_api__test__mock_frame_decryptor_h //original-code:"api/test/mock_frame_decryptor.h"
#include BOSS_WEBRTC_U_api__test__mock_frame_encryptor_h //original-code:"api/test/mock_frame_encryptor.h"
#include BOSS_WEBRTC_U_api__test__mock_media_stream_interface_h //original-code:"api/test/mock_media_stream_interface.h"
#include BOSS_WEBRTC_U_api__test__mock_peer_connection_factory_interface_h //original-code:"api/test/mock_peer_connection_factory_interface.h"
#include BOSS_WEBRTC_U_api__test__mock_peerconnectioninterface_h //original-code:"api/test/mock_peerconnectioninterface.h"
#include BOSS_WEBRTC_U_api__test__mock_rtp_transceiver_h //original-code:"api/test/mock_rtp_transceiver.h"
#include BOSS_WEBRTC_U_api__test__mock_rtpreceiver_h //original-code:"api/test/mock_rtpreceiver.h"
#include BOSS_WEBRTC_U_api__test__mock_rtpsender_h //original-code:"api/test/mock_rtpsender.h"
#include BOSS_WEBRTC_U_api__test__mock_transformable_video_frame_h //original-code:"api/test/mock_transformable_video_frame.h"
#include BOSS_WEBRTC_U_api__test__mock_video_bitrate_allocator_h //original-code:"api/test/mock_video_bitrate_allocator.h"
#include BOSS_WEBRTC_U_api__test__mock_video_bitrate_allocator_factory_h //original-code:"api/test/mock_video_bitrate_allocator_factory.h"
#include BOSS_WEBRTC_U_api__test__mock_video_decoder_h //original-code:"api/test/mock_video_decoder.h"
#include BOSS_WEBRTC_U_api__test__mock_video_decoder_factory_h //original-code:"api/test/mock_video_decoder_factory.h"
#include BOSS_WEBRTC_U_api__test__mock_video_encoder_h //original-code:"api/test/mock_video_encoder.h"
#include BOSS_WEBRTC_U_api__test__mock_video_encoder_factory_h //original-code:"api/test/mock_video_encoder_factory.h"
