/*
 *  Copyright (c) 2019 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef API_RTP_PACKET_INFO_H_
#define API_RTP_PACKET_INFO_H_

#include <cstdint>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__rtp_headers_h //original-code:"api/rtp_headers.h"
#include BOSS_WEBRTC_U_api__units__timestamp_h //original-code:"api/units/timestamp.h"
#include BOSS_WEBRTC_U_rtc_base__system__rtc_export_h //original-code:"rtc_base/system/rtc_export.h"

namespace webrtc {

//
// Structure to hold information about a received |RtpPacket|. It is primarily
// used to carry per-packet information from when a packet is received until
// the information is passed to |SourceTracker|.
//
class RTC_EXPORT RtpPacketInfo {
 public:
  RtpPacketInfo();

  RtpPacketInfo(uint32_t ssrc,
                std::vector<uint32_t> csrcs,
                uint32_t rtp_timestamp,
                absl::optional<uint8_t> audio_level,
                absl::optional<AbsoluteCaptureTime> absolute_capture_time,
                Timestamp receive_time);

  RtpPacketInfo(const RTPHeader& rtp_header, Timestamp receive_time);

  // TODO(bugs.webrtc.org/12722): Deprecated, remove once downstream projects
  // are updated.
  RtpPacketInfo(uint32_t ssrc,
                std::vector<uint32_t> csrcs,
                uint32_t rtp_timestamp,
                absl::optional<uint8_t> audio_level,
                absl::optional<AbsoluteCaptureTime> absolute_capture_time,
                int64_t receive_time_ms);
  RtpPacketInfo(const RTPHeader& rtp_header, int64_t receive_time_ms);

  RtpPacketInfo(const RtpPacketInfo& other) = default;
  RtpPacketInfo(RtpPacketInfo&& other) = default;
  RtpPacketInfo& operator=(const RtpPacketInfo& other) = default;
  RtpPacketInfo& operator=(RtpPacketInfo&& other) = default;

  uint32_t ssrc() const { return ssrc_; }
  void set_ssrc(uint32_t value) { ssrc_ = value; }

  const std::vector<uint32_t>& csrcs() const { return csrcs_; }
  void set_csrcs(std::vector<uint32_t> value) { csrcs_ = std::move(value); }

  uint32_t rtp_timestamp() const { return rtp_timestamp_; }
  void set_rtp_timestamp(uint32_t value) { rtp_timestamp_ = value; }

  absl::optional<uint8_t> audio_level() const { return audio_level_; }
  void set_audio_level(absl::optional<uint8_t> value) { audio_level_ = value; }

  const absl::optional<AbsoluteCaptureTime>& absolute_capture_time() const {
    return absolute_capture_time_;
  }
  void set_absolute_capture_time(
      const absl::optional<AbsoluteCaptureTime>& value) {
    absolute_capture_time_ = value;
  }

  const absl::optional<int64_t>& local_capture_clock_offset() const {
    return local_capture_clock_offset_;
  }

  void set_local_capture_clock_offset(const absl::optional<int64_t>& value) {
    local_capture_clock_offset_ = value;
  }

  Timestamp receive_time() const { return receive_time_; }
  void set_receive_time(Timestamp value) { receive_time_ = value; }
  // TODO(bugs.webrtc.org/12722): Deprecated, remove once downstream projects
  // are updated.
  int64_t receive_time_ms() const { return receive_time_.ms(); }

 private:
  // Fields from the RTP header:
  // https://tools.ietf.org/html/rfc3550#section-5.1
  uint32_t ssrc_;
  std::vector<uint32_t> csrcs_;
  uint32_t rtp_timestamp_;

  // Fields from the Audio Level header extension:
  // https://tools.ietf.org/html/rfc6464#section-3
  absl::optional<uint8_t> audio_level_;

  // Fields from the Absolute Capture Time header extension:
  // http://www.webrtc.org/experiments/rtp-hdrext/abs-capture-time
  // To not be confused with |local_capture_clock_offset_|, the
  // |estimated_capture_clock_offset| in |absolute_capture_time_| should
  // represent the clock offset between a remote sender and the capturer, and
  // thus equals to the corresponding values in the received RTP packets,
  // subjected to possible interpolations.
  absl::optional<AbsoluteCaptureTime> absolute_capture_time_;

  // Clock offset against capturer's clock. Should be derived from the estimated
  // capture clock offset defined in the Absolute Capture Time header extension.
  absl::optional<int64_t> local_capture_clock_offset_;

  // Local |webrtc::Clock|-based timestamp of when the packet was received.
  Timestamp receive_time_;
};

bool operator==(const RtpPacketInfo& lhs, const RtpPacketInfo& rhs);

inline bool operator!=(const RtpPacketInfo& lhs, const RtpPacketInfo& rhs) {
  return !(lhs == rhs);
}

}  // namespace webrtc

#endif  // API_RTP_PACKET_INFO_H_
