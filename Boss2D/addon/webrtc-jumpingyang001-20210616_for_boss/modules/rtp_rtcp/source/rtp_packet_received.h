/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef MODULES_RTP_RTCP_SOURCE_RTP_PACKET_RECEIVED_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_PACKET_RECEIVED_H_

#include <stdint.h>

#include <utility>

#include BOSS_ABSEILCPP_U_absl__base__attributes_h //original-code:"absl/base/attributes.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__ref_counted_base_h //original-code:"api/ref_counted_base.h"
#include BOSS_WEBRTC_U_api__rtp_headers_h //original-code:"api/rtp_headers.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__units__timestamp_h //original-code:"api/units/timestamp.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_packet_h //original-code:"modules/rtp_rtcp/source/rtp_packet.h"

namespace webrtc {
// Class to hold rtp packet with metadata for receiver side.
// The metadata is not parsed from the rtp packet, but may be derived from the
// data that is parsed from the rtp packet.
class RtpPacketReceived : public RtpPacket {
 public:
  RtpPacketReceived();
  explicit RtpPacketReceived(
      const ExtensionManager* extensions,
      webrtc::Timestamp arrival_time = webrtc::Timestamp::MinusInfinity());
  RtpPacketReceived(const RtpPacketReceived& packet);
  RtpPacketReceived(RtpPacketReceived&& packet);

  RtpPacketReceived& operator=(const RtpPacketReceived& packet);
  RtpPacketReceived& operator=(RtpPacketReceived&& packet);

  ~RtpPacketReceived();

  // TODO(danilchap): Remove this function when all code update to use RtpPacket
  // directly. Function is there just for easier backward compatibilty.
  void GetHeader(RTPHeader* header) const;

  // Time in local time base as close as it can to packet arrived on the
  // network.
  webrtc::Timestamp arrival_time() const { return arrival_time_; }
  void set_arrival_time(webrtc::Timestamp time) { arrival_time_ = time; }

  ABSL_DEPRECATED("Use arrival_time() instead")
  int64_t arrival_time_ms() const {
    return arrival_time_.IsMinusInfinity() ? -1 : arrival_time_.ms();
  }
  ABSL_DEPRECATED("Use set_arrival_time() instead")
  void set_arrival_time_ms(int64_t time) {
    arrival_time_ = webrtc::Timestamp::Millis(time);
  }

  // Flag if packet was recovered via RTX or FEC.
  bool recovered() const { return recovered_; }
  void set_recovered(bool value) { recovered_ = value; }

  int payload_type_frequency() const { return payload_type_frequency_; }
  void set_payload_type_frequency(int value) {
    payload_type_frequency_ = value;
  }

  // An application can attach arbitrary data to an RTP packet using
  // `additional_data`. The additional data does not affect WebRTC processing.
  rtc::scoped_refptr<rtc::RefCountedBase> additional_data() const {
    return additional_data_;
  }
  void set_additional_data(rtc::scoped_refptr<rtc::RefCountedBase> data) {
    additional_data_ = std::move(data);
  }

 private:
  webrtc::Timestamp arrival_time_ = Timestamp::MinusInfinity();
  int payload_type_frequency_ = 0;
  bool recovered_ = false;
  rtc::scoped_refptr<rtc::RefCountedBase> additional_data_;
};

}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_RTP_PACKET_RECEIVED_H_
