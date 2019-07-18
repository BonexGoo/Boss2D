/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_RTP_RTCP_SOURCE_RTP_PACKET_HISTORY_H_
#define MODULES_RTP_RTCP_SOURCE_RTP_PACKET_HISTORY_H_

#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_rtcp_defines_h //original-code:"modules/rtp_rtcp/include/rtp_rtcp_defines.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"
#include BOSS_WEBRTC_U_rtc_base__criticalsection_h //original-code:"rtc_base/criticalsection.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"
#include BOSS_WEBRTC_U_typedefs_h //original-code:"typedefs.h"  // NOLINT(build/include)

namespace webrtc {

class Clock;
class RtpPacketToSend;

class RtpPacketHistory {
 public:
  static constexpr size_t kMaxCapacity = 9600;
  explicit RtpPacketHistory(Clock* clock);
  ~RtpPacketHistory();

  void SetStorePacketsStatus(bool enable, uint16_t number_to_store);
  bool StorePackets() const;

  void PutRtpPacket(std::unique_ptr<RtpPacketToSend> packet,
                    StorageType type,
                    bool sent);

  // Set RTT, used to avoid premature retransmission and to prevent over-writing
  // a packet in the history before we are reasonably sure it has been received.
  void SetRtt(int64_t rtt_ms);

  // Gets stored RTP packet corresponding to the input |sequence number|.
  // Returns nullptr if packet is not found.
  // |min_elapsed_time_ms| is the minimum time that must have elapsed since
  // the last time the packet was resent (parameter is ignored if set to zero).
  // If the packet is found but the minimum time has not elapsed, returns
  // nullptr.
  std::unique_ptr<RtpPacketToSend> GetPacketAndSetSendTime(
      uint16_t sequence_number,
      int64_t min_elapsed_time_ms,
      bool retransmit);

  std::unique_ptr<RtpPacketToSend> GetBestFittingPacket(
      size_t packet_size) const;

  bool HasRtpPacket(uint16_t sequence_number) const;

 private:
  struct StoredPacket {
    uint16_t sequence_number = 0;
    int64_t send_time = 0;
    StorageType storage_type = kDontRetransmit;
    bool has_been_retransmitted = false;

    std::unique_ptr<RtpPacketToSend> packet;
  };

  std::unique_ptr<RtpPacketToSend> GetPacket(int index) const
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critsect_);
  void Allocate(size_t number_to_store) RTC_EXCLUSIVE_LOCKS_REQUIRED(critsect_);
  void Free() RTC_EXCLUSIVE_LOCKS_REQUIRED(critsect_);
  bool FindSeqNum(uint16_t sequence_number, int* index) const
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critsect_);
  int FindBestFittingPacket(size_t size) const
      RTC_EXCLUSIVE_LOCKS_REQUIRED(critsect_);

  Clock* clock_;
  rtc::CriticalSection critsect_;
  bool store_ RTC_GUARDED_BY(critsect_);
  size_t prev_index_ RTC_GUARDED_BY(critsect_);
  std::vector<StoredPacket> stored_packets_ RTC_GUARDED_BY(critsect_);
  int64_t rtt_ms_ RTC_GUARDED_BY(critsect_);
  RTC_DISALLOW_IMPLICIT_CONSTRUCTORS(RtpPacketHistory);
};
}  // namespace webrtc
#endif  // MODULES_RTP_RTCP_SOURCE_RTP_PACKET_HISTORY_H_
