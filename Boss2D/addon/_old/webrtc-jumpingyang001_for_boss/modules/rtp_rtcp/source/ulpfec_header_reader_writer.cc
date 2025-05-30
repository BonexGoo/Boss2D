/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__ulpfec_header_reader_writer_h //original-code:"modules/rtp_rtcp/source/ulpfec_header_reader_writer.h"

#include <utility>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__byte_io_h //original-code:"modules/rtp_rtcp/source/byte_io.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__forward_error_correction_internal_h //original-code:"modules/rtp_rtcp/source/forward_error_correction_internal.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"

namespace webrtc {

namespace {

// Maximum number of media packets that can be protected in one batch.
constexpr size_t kMaxMediaPackets = 48;

// Maximum number of FEC packets stored inside ForwardErrorCorrection.
constexpr size_t kMaxFecPackets = kMaxMediaPackets;

// FEC Level 0 header size in bytes.
constexpr size_t kFecLevel0HeaderSize = 10;

// FEC Level 1 (ULP) header size in bytes (L bit is set).
constexpr size_t kFecLevel1HeaderSizeLBitSet = 2 + kUlpfecPacketMaskSizeLBitSet;

// FEC Level 1 (ULP) header size in bytes (L bit is cleared).
constexpr size_t kFecLevel1HeaderSizeLBitClear =
    2 + kUlpfecPacketMaskSizeLBitClear;

constexpr size_t kPacketMaskOffset = kFecLevel0HeaderSize + 2;

size_t UlpfecHeaderSize(size_t packet_mask_size) {
  RTC_DCHECK_LE(packet_mask_size, kUlpfecPacketMaskSizeLBitSet);
  if (packet_mask_size <= kUlpfecPacketMaskSizeLBitClear) {
    return kFecLevel0HeaderSize + kFecLevel1HeaderSizeLBitClear;
  } else {
    return kFecLevel0HeaderSize + kFecLevel1HeaderSizeLBitSet;
  }
}

}  // namespace

UlpfecHeaderReader::UlpfecHeaderReader()
    : FecHeaderReader(kMaxMediaPackets, kMaxFecPackets) {}

UlpfecHeaderReader::~UlpfecHeaderReader() = default;

bool UlpfecHeaderReader::ReadFecHeader(
    ForwardErrorCorrection::ReceivedFecPacket* fec_packet) const {
  bool l_bit = (fec_packet->pkt->data[0] & 0x40) != 0u;
  size_t packet_mask_size =
      l_bit ? kUlpfecPacketMaskSizeLBitSet : kUlpfecPacketMaskSizeLBitClear;
  fec_packet->fec_header_size = UlpfecHeaderSize(packet_mask_size);
  uint16_t seq_num_base =
      ByteReader<uint16_t>::ReadBigEndian(&fec_packet->pkt->data[2]);
  fec_packet->protected_ssrc = fec_packet->ssrc;  // Due to RED.
  fec_packet->seq_num_base = seq_num_base;
  fec_packet->packet_mask_offset = kPacketMaskOffset;
  fec_packet->packet_mask_size = packet_mask_size;
  fec_packet->protection_length =
      ByteReader<uint16_t>::ReadBigEndian(&fec_packet->pkt->data[10]);

  // Store length recovery field in temporary location in header.
  // This makes the header "compatible" with the corresponding
  // FlexFEC location of the length recovery field, thus simplifying
  // the XORing operations.
  memcpy(&fec_packet->pkt->data[2], &fec_packet->pkt->data[8], 2);

  return true;
}

UlpfecHeaderWriter::UlpfecHeaderWriter()
    : FecHeaderWriter(kMaxMediaPackets,
                      kMaxFecPackets,
                      kFecLevel0HeaderSize + kFecLevel1HeaderSizeLBitSet) {}

UlpfecHeaderWriter::~UlpfecHeaderWriter() = default;

// TODO(brandtr): Consider updating this implementation (which actually
// returns a bound on the sequence number spread), if logic is added to
// UlpfecHeaderWriter::FinalizeFecHeader to truncate packet masks which end
// in a string of zeroes. (Similar to how it is done in the FlexFEC case.)
size_t UlpfecHeaderWriter::MinPacketMaskSize(const uint8_t* packet_mask,
                                             size_t packet_mask_size) const {
  return packet_mask_size;
}

size_t UlpfecHeaderWriter::FecHeaderSize(size_t packet_mask_size) const {
  return UlpfecHeaderSize(packet_mask_size);
}

void UlpfecHeaderWriter::FinalizeFecHeader(
    uint32_t /* media_ssrc */,
    uint16_t seq_num_base,
    const uint8_t* packet_mask,
    size_t packet_mask_size,
    ForwardErrorCorrection::Packet* fec_packet) const {
  // Set E bit to zero.
  fec_packet->data[0] &= 0x7f;
  // Set L bit based on packet mask size. (Note that the packet mask
  // can only take on two discrete values.)
  bool l_bit = (packet_mask_size == kUlpfecPacketMaskSizeLBitSet);
  if (l_bit) {
    fec_packet->data[0] |= 0x40;  // Set the L bit.
  } else {
    RTC_DCHECK_EQ(packet_mask_size, kUlpfecPacketMaskSizeLBitClear);
    fec_packet->data[0] &= 0xbf;  // Clear the L bit.
  }
  // Copy length recovery field from temporary location.
  memcpy(&fec_packet->data[8], &fec_packet->data[2], 2);
  // Write sequence number base.
  ByteWriter<uint16_t>::WriteBigEndian(&fec_packet->data[2], seq_num_base);
  // Protection length is set to entire packet. (This is not
  // required in general.)
  const size_t fec_header_size = FecHeaderSize(packet_mask_size);
  ByteWriter<uint16_t>::WriteBigEndian(&fec_packet->data[10],
                                       fec_packet->length - fec_header_size);
  // Copy the packet mask.
  memcpy(&fec_packet->data[12], packet_mask, packet_mask_size);
}

}  // namespace webrtc
