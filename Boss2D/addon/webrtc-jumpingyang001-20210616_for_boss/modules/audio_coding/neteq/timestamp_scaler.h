/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_NETEQ_TIMESTAMP_SCALER_H_
#define MODULES_AUDIO_CODING_NETEQ_TIMESTAMP_SCALER_H_

#include BOSS_WEBRTC_U_modules__audio_coding__neteq__packet_h //original-code:"modules/audio_coding/neteq/packet.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"

namespace webrtc {

// Forward declaration.
class DecoderDatabase;

// This class scales timestamps for codecs that need timestamp scaling.
// This is done for codecs where one RTP timestamp does not correspond to
// one sample.
class TimestampScaler {
 public:
  explicit TimestampScaler(const DecoderDatabase& decoder_database)
      : first_packet_received_(false),
        numerator_(1),
        denominator_(1),
        external_ref_(0),
        internal_ref_(0),
        decoder_database_(decoder_database) {}

  virtual ~TimestampScaler() {}

  // Start over.
  virtual void Reset();

  // Scale the timestamp in |packet| from external to internal.
  virtual void ToInternal(Packet* packet);

  // Scale the timestamp for all packets in |packet_list| from external to
  // internal.
  virtual void ToInternal(PacketList* packet_list);

  // Returns the internal equivalent of |external_timestamp|, given the
  // RTP payload type |rtp_payload_type|.
  virtual uint32_t ToInternal(uint32_t external_timestamp,
                              uint8_t rtp_payload_type);

  // Scales back to external timestamp. This is the inverse of ToInternal().
  virtual uint32_t ToExternal(uint32_t internal_timestamp) const;

 private:
  bool first_packet_received_;
  int numerator_;
  int denominator_;
  uint32_t external_ref_;
  uint32_t internal_ref_;
  const DecoderDatabase& decoder_database_;

  RTC_DISALLOW_COPY_AND_ASSIGN(TimestampScaler);
};

}  // namespace webrtc
#endif  // MODULES_AUDIO_CODING_NETEQ_TIMESTAMP_SCALER_H_
