/*
 *  Copyright (c) 2011 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MEDIA_BASE_RTP_UTILS_H_
#define MEDIA_BASE_RTP_UTILS_H_

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_rtc_base__byte_order_h //original-code:"rtc_base/byte_order.h"
#include BOSS_WEBRTC_U_rtc_base__system__rtc_export_h //original-code:"rtc_base/system/rtc_export.h"

namespace rtc {
struct PacketTimeUpdateParams;
}  // namespace rtc

namespace cricket {

const size_t kMinRtpPacketLen = 12;
const size_t kMaxRtpPacketLen = 2048;
const size_t kMinRtcpPacketLen = 4;

struct RtpHeader {
  int payload_type;
  int seq_num;
  uint32_t timestamp;
  uint32_t ssrc;
};

enum RtcpTypes {
  kRtcpTypeSR = 200,     // Sender report payload type.
  kRtcpTypeRR = 201,     // Receiver report payload type.
  kRtcpTypeSDES = 202,   // SDES payload type.
  kRtcpTypeBye = 203,    // BYE payload type.
  kRtcpTypeApp = 204,    // APP payload type.
  kRtcpTypeRTPFB = 205,  // Transport layer Feedback message payload type.
  kRtcpTypePSFB = 206,   // Payload-specific Feedback message payload type.
};

enum class RtpPacketType {
  kRtp,
  kRtcp,
  kUnknown,
};

bool GetRtpPayloadType(const void* data, size_t len, int* value);
bool GetRtpSeqNum(const void* data, size_t len, int* value);
bool GetRtpTimestamp(const void* data, size_t len, uint32_t* value);
bool GetRtpSsrc(const void* data, size_t len, uint32_t* value);
bool GetRtpHeaderLen(const void* data, size_t len, size_t* value);
bool GetRtcpType(const void* data, size_t len, int* value);
bool GetRtcpSsrc(const void* data, size_t len, uint32_t* value);
bool GetRtpHeader(const void* data, size_t len, RtpHeader* header);

bool SetRtpSsrc(void* data, size_t len, uint32_t value);
// Assumes version 2, no padding, no extensions, no csrcs.
bool SetRtpHeader(void* data, size_t len, const RtpHeader& header);

bool IsRtpPacket(rtc::ArrayView<const char> packet);

bool IsRtcpPacket(rtc::ArrayView<const char> packet);
// Checks the packet header to determine if it can be an RTP or RTCP packet.
RtpPacketType InferRtpPacketType(rtc::ArrayView<const char> packet);
// True if |payload type| is 0-127.
bool IsValidRtpPayloadType(int payload_type);

// True if |size| is appropriate for the indicated packet type.
bool IsValidRtpPacketSize(RtpPacketType packet_type, size_t size);

// Returns "RTCP", "RTP" or "Unknown" according to |packet_type|.
absl::string_view RtpPacketTypeToString(RtpPacketType packet_type);

// Verifies that a packet has a valid RTP header.
bool RTC_EXPORT ValidateRtpHeader(const uint8_t* rtp,
                                  size_t length,
                                  size_t* header_length);

// Helper method which updates the absolute send time extension if present.
bool UpdateRtpAbsSendTimeExtension(uint8_t* rtp,
                                   size_t length,
                                   int extension_id,
                                   uint64_t time_us);

// Applies specified |options| to the packet. It updates the absolute send time
// extension header if it is present present then updates HMAC.
bool RTC_EXPORT
ApplyPacketOptions(uint8_t* data,
                   size_t length,
                   const rtc::PacketTimeUpdateParams& packet_time_params,
                   uint64_t time_us);

}  // namespace cricket

#endif  // MEDIA_BASE_RTP_UTILS_H_
