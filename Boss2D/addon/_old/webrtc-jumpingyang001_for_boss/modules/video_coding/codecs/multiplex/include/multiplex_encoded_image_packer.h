/*
 *  Copyright (c) 2018 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_CODECS_MULTIPLEX_INCLUDE_MULTIPLEX_ENCODED_IMAGE_PACKER_H_
#define MODULES_VIDEO_CODING_CODECS_MULTIPLEX_INCLUDE_MULTIPLEX_ENCODED_IMAGE_PACKER_H_

#include <vector>

#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_common_video__include__video_frame_h //original-code:"common_video/include/video_frame.h"

namespace webrtc {

// Struct describing the whole bundle of multiple frames of an image.
// This struct is expected to be the set in the beginning of a picture's
// bitstream data.
struct MultiplexImageHeader {
  // The number of frame components making up the complete picture data.
  // For example, |frame_count| = 2 for the case of YUV frame with Alpha frame.
  uint8_t component_count;

  // The increasing image ID given by the encoder. For different components
  // of a single picture, they have the same |picture_index|.
  uint16_t image_index;

  // The location of the first MultiplexImageComponentHeader in the bitstream,
  // in terms of byte from the beginning of the bitstream.
  uint32_t first_component_header_offset;
};
const int kMultiplexImageHeaderSize =
    sizeof(uint8_t) + sizeof(uint16_t) + sizeof(uint32_t);

// Struct describing the individual image component's content.
struct MultiplexImageComponentHeader {
  // The location of the next MultiplexImageComponentHeader in the bitstream,
  // in terms of the byte from the beginning of the bitstream;
  uint32_t next_component_header_offset;

  // Identifies which component this frame represent, i.e. YUV frame vs Alpha
  // frame.
  uint8_t component_index;

  // The location of the real encoded image data of the frame in the bitstream,
  // in terms of byte from the beginning of the bitstream.
  uint32_t bitstream_offset;

  // Indicates the number of bytes of the encoded image data.
  uint32_t bitstream_length;

  // Indicated the underlying VideoCodecType of the frame, i.e. VP9 or VP8 etc.
  VideoCodecType codec_type;

  // Indicated the underlying frame is a key frame or delta frame.
  FrameType frame_type;
};
const int kMultiplexImageComponentHeaderSize =
    sizeof(uint32_t) + sizeof(uint8_t) + sizeof(uint32_t) + sizeof(uint32_t) +
    sizeof(uint8_t) + sizeof(uint8_t);

// Struct holding the encoded image for one component.
struct MultiplexImageComponent {
  // Indicated the underlying VideoCodecType of the frame, i.e. VP9 or VP8 etc.
  VideoCodecType codec_type;

  // Identifies which component this frame represent, i.e. YUV frame vs Alpha
  // frame.
  uint8_t component_index;

  // Stores the actual frame data of the encoded image.
  EncodedImage encoded_image;
};

// Struct holding the whole frame bundle of components of an image.
struct MultiplexImage {
  uint16_t image_index;
  uint8_t component_count;
  std::vector<MultiplexImageComponent> image_components;

  MultiplexImage(uint16_t picture_index, uint8_t component_count);
};

// A utility class providing conversion between two representations of a
// multiplex image frame:
// 1. Packed version is just one encoded image, we pack all necessary metadata
//    in the bitstream as headers.
// 2. Unpacked version is essentially a list of encoded images, one for one
//    component.
class MultiplexEncodedImagePacker {
 public:
  // Note: It is caller responsibility to release the buffer of the result.
  static EncodedImage PackAndRelease(const MultiplexImage& image);

  // Note: The image components just share the memory with |combined_image|.
  static MultiplexImage Unpack(const EncodedImage& combined_image);
};

}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_CODECS_MULTIPLEX_INCLUDE_MULTIPLEX_ENCODED_IMAGE_PACKER_H_
