/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_NETEQ_TOOLS_PACKET_H_
#define MODULES_AUDIO_CODING_NETEQ_TOOLS_PACKET_H_

#include <list>
#include <memory>

#include BOSS_WEBRTC_U_api__rtp_headers_h //original-code:"api/rtp_headers.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"     // NOLINT(build/include)
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"

namespace webrtc {

class RtpHeaderParser;

namespace test {

// Class for handling RTP packets in test applications.
class Packet {
 public:
  // Creates a packet, with the packet payload (including header bytes) in
  // |packet_memory|. The length of |packet_memory| is |allocated_bytes|.
  // The new object assumes ownership of |packet_memory| and will delete it
  // when the Packet object is deleted. The |time_ms| is an extra time
  // associated with this packet, typically used to denote arrival time.
  // The first bytes in |packet_memory| will be parsed using |parser|.
  Packet(uint8_t* packet_memory,
         size_t allocated_bytes,
         double time_ms,
         const RtpHeaderParser& parser);

  // Same as above, but with the extra argument |virtual_packet_length_bytes|.
  // This is typically used when reading RTP dump files that only contain the
  // RTP headers, and no payload (a.k.a RTP dummy files or RTP light). The
  // |virtual_packet_length_bytes| tells what size the packet had on wire,
  // including the now discarded payload, whereas |allocated_bytes| is the
  // length of the remaining payload (typically only the RTP header).
  Packet(uint8_t* packet_memory,
         size_t allocated_bytes,
         size_t virtual_packet_length_bytes,
         double time_ms,
         const RtpHeaderParser& parser);

  // The following two constructors are the same as above, but without a
  // parser. Note that when the object is constructed using any of these
  // methods, the header will be parsed using a default RtpHeaderParser object.
  // In particular, RTP header extensions won't be parsed.
  Packet(uint8_t* packet_memory, size_t allocated_bytes, double time_ms);

  Packet(uint8_t* packet_memory,
         size_t allocated_bytes,
         size_t virtual_packet_length_bytes,
         double time_ms);

  virtual ~Packet();

  // Parses the first bytes of the RTP payload, interpreting them as RED headers
  // according to RFC 2198. The headers will be inserted into |headers|. The
  // caller of the method assumes ownership of the objects in the list, and
  // must delete them properly.
  bool ExtractRedHeaders(std::list<RTPHeader*>* headers) const;

  // Deletes all RTPHeader objects in |headers|, but does not delete |headers|
  // itself.
  static void DeleteRedHeaders(std::list<RTPHeader*>* headers);

  const uint8_t* payload() const { return payload_; }

  size_t packet_length_bytes() const { return packet_length_bytes_; }

  size_t payload_length_bytes() const { return payload_length_bytes_; }

  size_t virtual_packet_length_bytes() const {
    return virtual_packet_length_bytes_;
  }

  size_t virtual_payload_length_bytes() const {
    return virtual_payload_length_bytes_;
  }

  const RTPHeader& header() const { return header_; }

  void set_time_ms(double time) { time_ms_ = time; }
  double time_ms() const { return time_ms_; }
  bool valid_header() const { return valid_header_; }

 private:
  bool ParseHeader(const RtpHeaderParser& parser);
  void CopyToHeader(RTPHeader* destination) const;

  RTPHeader header_;
  std::unique_ptr<uint8_t[]> payload_memory_;
  const uint8_t* payload_;            // First byte after header.
  const size_t packet_length_bytes_;  // Total length of packet.
  size_t payload_length_bytes_;  // Length of the payload, after RTP header.
                                 // Zero for dummy RTP packets.
  // Virtual lengths are used when parsing RTP header files (dummy RTP files).
  const size_t virtual_packet_length_bytes_;
  size_t virtual_payload_length_bytes_;
  double time_ms_;     // Used to denote a packet's arrival time.
  bool valid_header_;  // Set by the RtpHeaderParser.

  RTC_DISALLOW_COPY_AND_ASSIGN(Packet);
};

}  // namespace test
}  // namespace webrtc
#endif  // MODULES_AUDIO_CODING_NETEQ_TOOLS_PACKET_H_
