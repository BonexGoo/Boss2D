/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/audio_coding/neteq/red_payload_splitter.h"

#include <assert.h>
#include <vector>

#include "modules/audio_coding/neteq/decoder_database.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"

namespace webrtc {

// The method loops through a list of packets {A, B, C, ...}. Each packet is
// split into its corresponding RED payloads, {A1, A2, ...}, which is
// temporarily held in the list |new_packets|.
// When the first packet in |packet_list| has been processed, the orignal packet
// is replaced by the new ones in |new_packets|, so that |packet_list| becomes:
// {A1, A2, ..., B, C, ...}. The method then continues with B, and C, until all
// the original packets have been replaced by their split payloads.
bool RedPayloadSplitter::SplitRed(PacketList* packet_list) {
  // Too many RED blocks indicates that something is wrong. Clamp it at some
  // reasonable value.
  const size_t kMaxRedBlocks = 32;
  bool ret = true;
  PacketList::iterator it = packet_list->begin();
  while (it != packet_list->end()) {
    const Packet& red_packet = *it;
    assert(!red_packet.payload.empty());
    const uint8_t* payload_ptr = red_packet.payload.data();

    // Read RED headers (according to RFC 2198):
    //
    //    0                   1                   2                   3
    //    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
    //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    //   |F|   block PT  |  timestamp offset         |   block length    |
    //   +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
    // Last RED header:
    //    0 1 2 3 4 5 6 7
    //   +-+-+-+-+-+-+-+-+
    //   |0|   Block PT  |
    //   +-+-+-+-+-+-+-+-+

    struct RedHeader {
      uint8_t payload_type;
      uint32_t timestamp;
      size_t payload_length;
    };

    std::vector<RedHeader> new_headers;
    bool last_block = false;
    size_t sum_length = 0;
    while (!last_block) {
      RedHeader new_header;
      // Check the F bit. If F == 0, this was the last block.
      last_block = ((*payload_ptr & 0x80) == 0);
      // Bits 1 through 7 are payload type.
      new_header.payload_type = payload_ptr[0] & 0x7F;
      if (last_block) {
        // No more header data to read.
        ++sum_length;  // Account for RED header size of 1 byte.
        new_header.timestamp = red_packet.timestamp;
        new_header.payload_length = red_packet.payload.size() - sum_length;
        payload_ptr += 1;  // Advance to first payload byte.
      } else {
        // Bits 8 through 21 are timestamp offset.
        int timestamp_offset =
            (payload_ptr[1] << 6) + ((payload_ptr[2] & 0xFC) >> 2);
        new_header.timestamp = red_packet.timestamp - timestamp_offset;
        // Bits 22 through 31 are payload length.
        new_header.payload_length =
            ((payload_ptr[2] & 0x03) << 8) + payload_ptr[3];
        payload_ptr += 4;  // Advance to next RED header.
      }
      sum_length += new_header.payload_length;
      sum_length += 4;  // Account for RED header size of 4 bytes.
      // Store in new list of packets.
      new_headers.push_back(new_header);
    }

    if (new_headers.size() <= kMaxRedBlocks) {
      // Populate the new packets with payload data.
      // |payload_ptr| now points at the first payload byte.
      PacketList new_packets;  // An empty list to store the split packets in.
      for (size_t i = 0; i != new_headers.size(); ++i) {
        const auto& new_header = new_headers[i];
        size_t payload_length = new_header.payload_length;
        if (payload_ptr + payload_length >
            red_packet.payload.data() + red_packet.payload.size()) {
          // The block lengths in the RED headers do not match the overall
          // packet length. Something is corrupt. Discard this and the remaining
          // payloads from this packet.
          RTC_LOG(LS_WARNING) << "SplitRed length mismatch";
          ret = false;
          break;
        }

        Packet new_packet;
        new_packet.timestamp = new_header.timestamp;
        new_packet.payload_type = new_header.payload_type;
        new_packet.sequence_number = red_packet.sequence_number;
        new_packet.priority.red_level =
            rtc::dchecked_cast<int>((new_headers.size() - 1) - i);
        new_packet.payload.SetData(payload_ptr, payload_length);
        new_packets.push_front(std::move(new_packet));
        payload_ptr += payload_length;
      }
      // Insert new packets into original list, before the element pointed to by
      // iterator |it|.
      packet_list->splice(it, std::move(new_packets));
    } else {
      RTC_LOG(LS_WARNING) << "SplitRed too many blocks: " << new_headers.size();
      ret = false;
    }
    // Remove |it| from the packet list. This operation effectively moves the
    // iterator |it| to the next packet in the list. Thus, we do not have to
    // increment it manually.
    it = packet_list->erase(it);
  }
  return ret;
}

int RedPayloadSplitter::CheckRedPayloads(
    PacketList* packet_list,
    const DecoderDatabase& decoder_database) {
  int main_payload_type = -1;
  int num_deleted_packets = 0;
  for (auto it = packet_list->begin(); it != packet_list->end(); /* */) {
    uint8_t this_payload_type = it->payload_type;
    if (!decoder_database.IsDtmf(this_payload_type) &&
        !decoder_database.IsComfortNoise(this_payload_type)) {
      if (main_payload_type == -1) {
        // This is the first packet in the list which is non-DTMF non-CNG.
        main_payload_type = this_payload_type;
      } else {
        if (this_payload_type != main_payload_type) {
          // We do not allow redundant payloads of a different type.
          // Remove |it| from the packet list. This operation effectively
          // moves the iterator |it| to the next packet in the list. Thus, we
          // do not have to increment it manually.
          it = packet_list->erase(it);
          ++num_deleted_packets;
          continue;
        }
      }
    }
    ++it;
  }
  return num_deleted_packets;
}

}  // namespace webrtc
