/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef CALL_RTP_VIDEO_SENDER_INTERFACE_H_
#define CALL_RTP_VIDEO_SENDER_INTERFACE_H_

#include <map>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__call__bitrate_allocation_h //original-code:"api/call/bitrate_allocation.h"
#include BOSS_WEBRTC_U_api__fec_controller_override_h //original-code:"api/fec_controller_override.h"
#include BOSS_WEBRTC_U_api__video__video_layers_allocation_h //original-code:"api/video/video_layers_allocation.h"
#include BOSS_WEBRTC_U_call__rtp_config_h //original-code:"call/rtp_config.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_sequence_number_map_h //original-code:"modules/rtp_rtcp/source/rtp_sequence_number_map.h"
#include BOSS_WEBRTC_U_modules__utility__include__process_thread_h //original-code:"modules/utility/include/process_thread.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_codec_interface_h //original-code:"modules/video_coding/include/video_codec_interface.h"

namespace webrtc {
class VideoBitrateAllocation;
struct FecProtectionParams;

class RtpVideoSenderInterface : public EncodedImageCallback,
                                public FecControllerOverride {
 public:
  virtual void RegisterProcessThread(ProcessThread* module_process_thread) = 0;
  virtual void DeRegisterProcessThread() = 0;

  // RtpVideoSender will only route packets if being active, all
  // packets will be dropped otherwise.
  virtual void SetActive(bool active) = 0;
  // Sets the sending status of the rtp modules and appropriately sets the
  // RtpVideoSender to active if any rtp modules are active.
  virtual void SetActiveModules(const std::vector<bool> active_modules) = 0;
  virtual bool IsActive() = 0;

  virtual void OnNetworkAvailability(bool network_available) = 0;
  virtual std::map<uint32_t, RtpState> GetRtpStates() const = 0;
  virtual std::map<uint32_t, RtpPayloadState> GetRtpPayloadStates() const = 0;

  virtual void DeliverRtcp(const uint8_t* packet, size_t length) = 0;

  virtual void OnBitrateAllocationUpdated(
      const VideoBitrateAllocation& bitrate) = 0;
  virtual void OnVideoLayersAllocationUpdated(
      const VideoLayersAllocation& allocation) = 0;
  virtual void OnBitrateUpdated(BitrateAllocationUpdate update,
                                int framerate) = 0;
  virtual void OnTransportOverheadChanged(
      size_t transport_overhead_bytes_per_packet) = 0;
  virtual uint32_t GetPayloadBitrateBps() const = 0;
  virtual uint32_t GetProtectionBitrateBps() const = 0;
  virtual void SetEncodingData(size_t width,
                               size_t height,
                               size_t num_temporal_layers) = 0;
  virtual std::vector<RtpSequenceNumberMap::Info> GetSentRtpPacketInfos(
      uint32_t ssrc,
      rtc::ArrayView<const uint16_t> sequence_numbers) const = 0;

  // Implements FecControllerOverride.
  void SetFecAllowed(bool fec_allowed) override = 0;
};
}  // namespace webrtc
#endif  // CALL_RTP_VIDEO_SENDER_INTERFACE_H_
