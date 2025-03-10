/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef COMMON_VIDEO_H264_H264_BITSTREAM_PARSER_H_
#define COMMON_VIDEO_H264_H264_BITSTREAM_PARSER_H_
#include <stddef.h>
#include BOSS_FAKEWIN_V_stdint_h //original-code:<stdint.h>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_common_video__h264__pps_parser_h //original-code:"common_video/h264/pps_parser.h"
#include BOSS_WEBRTC_U_common_video__h264__sps_parser_h //original-code:"common_video/h264/sps_parser.h"

namespace rtc {
class BitBufferWriter;
}

namespace webrtc {

// Stateful H264 bitstream parser (due to SPS/PPS). Used to parse out QP values
// from the bitstream.
// TODO(pbos): Unify with RTP SPS parsing and only use one H264 parser.
// TODO(pbos): If/when this gets used on the receiver side CHECKs must be
// removed and gracefully abort as we have no control over receive-side
// bitstreams.
class H264BitstreamParser {
 public:
  enum Result {
    kOk,
    kInvalidStream,
    kUnsupportedStream,
  };

  H264BitstreamParser();
  virtual ~H264BitstreamParser();

  // Parse an additional chunk of H264 bitstream.
  void ParseBitstream(const uint8_t* bitstream, size_t length);

  // Get the last extracted QP value from the parsed bitstream.
  bool GetLastSliceQp(int* qp) const;

 protected:
  void ParseSlice(const uint8_t* slice, size_t length);
  Result ParseNonParameterSetNalu(const uint8_t* source,
                                  size_t source_length,
                                  uint8_t nalu_type);

  // SPS/PPS state, updated when parsing new SPS/PPS, used to parse slices.
  absl::optional<SpsParser::SpsState> sps_;
  absl::optional<PpsParser::PpsState> pps_;

  // Last parsed slice QP.
  absl::optional<int32_t> last_slice_qp_delta_;
};

}  // namespace webrtc

#endif  // COMMON_VIDEO_H264_H264_BITSTREAM_PARSER_H_
