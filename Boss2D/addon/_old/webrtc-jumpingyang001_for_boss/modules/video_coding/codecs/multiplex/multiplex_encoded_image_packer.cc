/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__video_coding__codecs__multiplex__include__multiplex_encoded_image_packer_h //original-code:"modules/video_coding/codecs/multiplex/include/multiplex_encoded_image_packer.h"

#include <cstring>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__byte_io_h //original-code:"modules/rtp_rtcp/source/byte_io.h"

namespace webrtc {
int PackHeader(uint8_t* buffer, MultiplexImageHeader header) {
  int offset = 0;
  ByteWriter<uint8_t>::WriteBigEndian(buffer + offset, header.component_count);
  offset += sizeof(uint8_t);

  ByteWriter<uint16_t>::WriteBigEndian(buffer + offset, header.image_index);
  offset += sizeof(uint16_t);

  ByteWriter<uint32_t>::WriteBigEndian(buffer + offset,
                                       header.first_component_header_offset);
  offset += sizeof(uint32_t);

  RTC_DCHECK_EQ(offset, kMultiplexImageHeaderSize);
  return offset;
}

MultiplexImageHeader UnpackHeader(uint8_t* buffer) {
  MultiplexImageHeader header;
  int offset = 0;
  header.component_count = ByteReader<uint8_t>::ReadBigEndian(buffer + offset);
  offset += sizeof(uint8_t);

  header.image_index = ByteReader<uint16_t>::ReadBigEndian(buffer + offset);
  offset += sizeof(uint16_t);

  header.first_component_header_offset =
      ByteReader<uint32_t>::ReadBigEndian(buffer + offset);
  offset += sizeof(uint32_t);

  RTC_DCHECK_EQ(offset, kMultiplexImageHeaderSize);
  return header;
}

int PackFrameHeader(uint8_t* buffer,
                    MultiplexImageComponentHeader frame_header) {
  int offset = 0;
  ByteWriter<uint32_t>::WriteBigEndian(
      buffer + offset, frame_header.next_component_header_offset);
  offset += sizeof(uint32_t);

  ByteWriter<uint8_t>::WriteBigEndian(buffer + offset,
                                      frame_header.component_index);
  offset += sizeof(uint8_t);

  ByteWriter<uint32_t>::WriteBigEndian(buffer + offset,
                                       frame_header.bitstream_offset);
  offset += sizeof(uint32_t);

  ByteWriter<uint32_t>::WriteBigEndian(buffer + offset,
                                       frame_header.bitstream_length);
  offset += sizeof(uint32_t);

  ByteWriter<uint8_t>::WriteBigEndian(buffer + offset, frame_header.codec_type);
  offset += sizeof(uint8_t);

  ByteWriter<uint8_t>::WriteBigEndian(buffer + offset, frame_header.frame_type);
  offset += sizeof(uint8_t);

  RTC_DCHECK_EQ(offset, kMultiplexImageComponentHeaderSize);
  return offset;
}

MultiplexImageComponentHeader UnpackFrameHeader(uint8_t* buffer) {
  MultiplexImageComponentHeader frame_header;
  int offset = 0;

  frame_header.next_component_header_offset =
      ByteReader<uint32_t>::ReadBigEndian(buffer + offset);
  offset += sizeof(uint32_t);

  frame_header.component_index =
      ByteReader<uint8_t>::ReadBigEndian(buffer + offset);
  offset += sizeof(uint8_t);

  frame_header.bitstream_offset =
      ByteReader<uint32_t>::ReadBigEndian(buffer + offset);
  offset += sizeof(uint32_t);

  frame_header.bitstream_length =
      ByteReader<uint32_t>::ReadBigEndian(buffer + offset);
  offset += sizeof(uint32_t);

  frame_header.codec_type = static_cast<VideoCodecType>(
      ByteReader<uint8_t>::ReadBigEndian(buffer + offset));
  offset += sizeof(uint8_t);

  frame_header.frame_type = static_cast<FrameType>(
      ByteReader<uint8_t>::ReadBigEndian(buffer + offset));
  offset += sizeof(uint8_t);

  RTC_DCHECK_EQ(offset, kMultiplexImageComponentHeaderSize);
  return frame_header;
}

void PackBitstream(uint8_t* buffer, MultiplexImageComponent image) {
  memcpy(buffer, image.encoded_image._buffer, image.encoded_image._length);
}

MultiplexImage::MultiplexImage(uint16_t picture_index, uint8_t frame_count)
    : image_index(picture_index), component_count(frame_count) {}

EncodedImage MultiplexEncodedImagePacker::PackAndRelease(
    const MultiplexImage& multiplex_image) {
  MultiplexImageHeader header;
  std::vector<MultiplexImageComponentHeader> frame_headers;

  header.component_count = multiplex_image.component_count;
  header.image_index = multiplex_image.image_index;
  int header_offset = kMultiplexImageHeaderSize;
  header.first_component_header_offset = header_offset;
  int bitstream_offset = header_offset + kMultiplexImageComponentHeaderSize *
                                             header.component_count;

  const std::vector<MultiplexImageComponent>& images =
      multiplex_image.image_components;
  EncodedImage combined_image = images[0].encoded_image;
  for (size_t i = 0; i < images.size(); i++) {
    MultiplexImageComponentHeader frame_header;
    header_offset += kMultiplexImageComponentHeaderSize;
    frame_header.next_component_header_offset =
        (i == images.size() - 1) ? 0 : header_offset;
    frame_header.component_index = images[i].component_index;

    frame_header.bitstream_offset = bitstream_offset;
    const size_t padding =
        EncodedImage::GetBufferPaddingBytes(images[i].codec_type);
    frame_header.bitstream_length =
        static_cast<uint32_t>(images[i].encoded_image._length + padding);
    bitstream_offset += frame_header.bitstream_length;

    frame_header.codec_type = images[i].codec_type;
    frame_header.frame_type = images[i].encoded_image._frameType;

    // As long as one component is delta frame, we have to mark the combined
    // frame as delta frame, because it is necessary for all components to be
    // key frame so as to decode the whole image without previous frame data.
    // Thus only when all components are key frames, we can mark the combined
    // frame as key frame.
    if (frame_header.frame_type == FrameType::kVideoFrameDelta) {
      combined_image._frameType = FrameType::kVideoFrameDelta;
    }

    frame_headers.push_back(frame_header);
  }

  combined_image._length = combined_image._size = bitstream_offset;
  combined_image._buffer = new uint8_t[combined_image._length];

  // header
  header_offset = PackHeader(combined_image._buffer, header);
  RTC_DCHECK_EQ(header.first_component_header_offset,
                kMultiplexImageHeaderSize);

  // Frame Header
  for (size_t i = 0; i < images.size(); i++) {
    int relative_offset = PackFrameHeader(
        combined_image._buffer + header_offset, frame_headers[i]);
    RTC_DCHECK_EQ(relative_offset, kMultiplexImageComponentHeaderSize);

    header_offset = frame_headers[i].next_component_header_offset;
    RTC_DCHECK_EQ(header_offset,
                  (i == images.size() - 1)
                      ? 0
                      : (kMultiplexImageHeaderSize +
                         kMultiplexImageComponentHeaderSize * (i + 1)));
  }

  // Bitstreams
  for (size_t i = 0; i < images.size(); i++) {
    PackBitstream(combined_image._buffer + frame_headers[i].bitstream_offset,
                  images[i]);
    delete[] images[i].encoded_image._buffer;
  }

  return combined_image;
}

MultiplexImage MultiplexEncodedImagePacker::Unpack(
    const EncodedImage& combined_image) {
  const MultiplexImageHeader& header = UnpackHeader(combined_image._buffer);

  MultiplexImage multiplex_image(header.image_index, header.component_count);
  std::vector<MultiplexImageComponentHeader> frame_headers;
  int header_offset = header.first_component_header_offset;

  while (header_offset > 0) {
    frame_headers.push_back(
        UnpackFrameHeader(combined_image._buffer + header_offset));
    header_offset = frame_headers.back().next_component_header_offset;
  }

  RTC_DCHECK_LE(frame_headers.size(), header.component_count);
  for (size_t i = 0; i < frame_headers.size(); i++) {
    MultiplexImageComponent image_component;
    image_component.component_index = frame_headers[i].component_index;
    image_component.codec_type = frame_headers[i].codec_type;

    EncodedImage encoded_image = combined_image;
    encoded_image._timeStamp = combined_image._timeStamp;
    encoded_image._frameType = frame_headers[i].frame_type;
    encoded_image._size =
        static_cast<size_t>(frame_headers[i].bitstream_length);
    const size_t padding =
        EncodedImage::GetBufferPaddingBytes(image_component.codec_type);
    encoded_image._length = encoded_image._size - padding;
    encoded_image._buffer =
        combined_image._buffer + frame_headers[i].bitstream_offset;

    image_component.encoded_image = encoded_image;

    multiplex_image.image_components.push_back(image_component);
  }

  return multiplex_image;
}

}  // namespace webrtc
