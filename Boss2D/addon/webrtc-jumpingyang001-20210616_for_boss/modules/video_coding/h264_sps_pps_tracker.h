/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_VIDEO_CODING_H264_SPS_PPS_TRACKER_H_
#define MODULES_VIDEO_CODING_H264_SPS_PPS_TRACKER_H_

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <vector>

#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_video_header_h //original-code:"modules/rtp_rtcp/source/rtp_video_header.h"
#include BOSS_WEBRTC_U_rtc_base__copy_on_write_buffer_h //original-code:"rtc_base/copy_on_write_buffer.h"

namespace webrtc {
namespace video_coding {

class H264SpsPpsTracker {
 public:
  enum PacketAction { kInsert, kDrop, kRequestKeyframe };
  struct FixedBitstream {
    PacketAction action;
    rtc::CopyOnWriteBuffer bitstream;
  };

  H264SpsPpsTracker();
  ~H264SpsPpsTracker();

  // Returns fixed bitstream and modifies |video_header|.
  FixedBitstream CopyAndFixBitstream(rtc::ArrayView<const uint8_t> bitstream,
                                     RTPVideoHeader* video_header);

  void InsertSpsPpsNalus(const std::vector<uint8_t>& sps,
                         const std::vector<uint8_t>& pps);

 private:
  struct PpsInfo {
    PpsInfo();
    PpsInfo(PpsInfo&& rhs);
    PpsInfo& operator=(PpsInfo&& rhs);
    ~PpsInfo();

    int sps_id = -1;
    size_t size = 0;
    std::unique_ptr<uint8_t[]> data;
  };

  struct SpsInfo {
    SpsInfo();
    SpsInfo(SpsInfo&& rhs);
    SpsInfo& operator=(SpsInfo&& rhs);
    ~SpsInfo();

    size_t size = 0;
    int width = -1;
    int height = -1;
    std::unique_ptr<uint8_t[]> data;
  };

  std::map<uint32_t, PpsInfo> pps_data_;
  std::map<uint32_t, SpsInfo> sps_data_;
};

}  // namespace video_coding
}  // namespace webrtc

#endif  // MODULES_VIDEO_CODING_H264_SPS_PPS_TRACKER_H_
