/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#include "modules/video_coding/utility/vp9_uncompressed_header_parser.h"

#include BOSS_WEBRTC_U_rtc_base__bitbuffer_h //original-code:"rtc_base/bitbuffer.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace webrtc {

#define RETURN_FALSE_IF_ERROR(x) \
  if (!(x)) {                    \
    return false;                \
  }

namespace vp9 {
namespace {
const size_t kVp9NumRefsPerFrame = 3;
const size_t kVp9MaxRefLFDeltas = 4;
const size_t kVp9MaxModeLFDeltas = 2;

bool Vp9ReadProfile(rtc::BitBuffer* br, uint8_t* profile) {
  uint32_t high_bit;
  uint32_t low_bit;
  RETURN_FALSE_IF_ERROR(br->ReadBits(&low_bit, 1));
  RETURN_FALSE_IF_ERROR(br->ReadBits(&high_bit, 1));
  *profile = (high_bit << 1) + low_bit;
  if (*profile > 2) {
    uint32_t reserved_bit;
    RETURN_FALSE_IF_ERROR(br->ReadBits(&reserved_bit, 1));
    if (reserved_bit) {
      RTC_LOG(LS_WARNING) << "Failed to get QP. Unsupported bitstream profile.";
      return false;
    }
  }
  return true;
}

bool Vp9ReadSyncCode(rtc::BitBuffer* br) {
  uint32_t sync_code;
  RETURN_FALSE_IF_ERROR(br->ReadBits(&sync_code, 24));
  if (sync_code != 0x498342) {
    RTC_LOG(LS_WARNING) << "Failed to get QP. Invalid sync code.";
    return false;
  }
  return true;
}

bool Vp9ReadColorConfig(rtc::BitBuffer* br, uint8_t profile) {
  if (profile == 2 || profile == 3) {
    // Bitdepth.
    RETURN_FALSE_IF_ERROR(br->ConsumeBits(1));
  }
  uint32_t color_space;
  RETURN_FALSE_IF_ERROR(br->ReadBits(&color_space, 3));

  // SRGB is 7.
  if (color_space != 7) {
    // YUV range flag.
    RETURN_FALSE_IF_ERROR(br->ConsumeBits(1));
    if (profile == 1 || profile == 3) {
      // 1 bit: subsampling x.
      // 1 bit: subsampling y.
      RETURN_FALSE_IF_ERROR(br->ConsumeBits(2));
      uint32_t reserved_bit;
      RETURN_FALSE_IF_ERROR(br->ReadBits(&reserved_bit, 1));
      if (reserved_bit) {
        RTC_LOG(LS_WARNING) << "Failed to get QP. Reserved bit set.";
        return false;
      }
    }
  } else {
    if (profile == 1 || profile == 3) {
      uint32_t reserved_bit;
      RETURN_FALSE_IF_ERROR(br->ReadBits(&reserved_bit, 1));
      if (reserved_bit) {
        RTC_LOG(LS_WARNING) << "Failed to get QP. Reserved bit set.";
        return false;
      }
    } else {
      RTC_LOG(LS_WARNING) << "Failed to get QP. 4:4:4 color not supported in "
                             "profile 0 or 2.";
      return false;
    }
  }

  return true;
}

bool Vp9ReadFrameSize(rtc::BitBuffer* br) {
  // 2 bytes: frame width.
  // 2 bytes: frame height.
  return br->ConsumeBytes(4);
}

bool Vp9ReadRenderSize(rtc::BitBuffer* br) {
  uint32_t bit;
  RETURN_FALSE_IF_ERROR(br->ReadBits(&bit, 1));
  if (bit) {
    // 2 bytes: render width.
    // 2 bytes: render height.
    RETURN_FALSE_IF_ERROR(br->ConsumeBytes(4));
  }
  return true;
}

bool Vp9ReadFrameSizeFromRefs(rtc::BitBuffer* br) {
  uint32_t found_ref = 0;
  for (size_t i = 0; i < kVp9NumRefsPerFrame; i++) {
    // Size in refs.
    RETURN_FALSE_IF_ERROR(br->ReadBits(&found_ref, 1));
    if (found_ref)
      break;
  }

  if (!found_ref) {
    if (!Vp9ReadFrameSize(br)) {
      return false;
    }
  }
  return Vp9ReadRenderSize(br);
}

bool Vp9ReadInterpolationFilter(rtc::BitBuffer* br) {
  uint32_t bit;
  RETURN_FALSE_IF_ERROR(br->ReadBits(&bit, 1));
  if (bit)
    return true;

  return br->ConsumeBits(2);
}

bool Vp9ReadLoopfilter(rtc::BitBuffer* br) {
  // 6 bits: filter level.
  // 3 bits: sharpness level.
  RETURN_FALSE_IF_ERROR(br->ConsumeBits(9));

  uint32_t mode_ref_delta_enabled;
  RETURN_FALSE_IF_ERROR(br->ReadBits(&mode_ref_delta_enabled, 1));
  if (mode_ref_delta_enabled) {
    uint32_t mode_ref_delta_update;
    RETURN_FALSE_IF_ERROR(br->ReadBits(&mode_ref_delta_update, 1));
    if (mode_ref_delta_update) {
      uint32_t bit;
      for (size_t i = 0; i < kVp9MaxRefLFDeltas; i++) {
        RETURN_FALSE_IF_ERROR(br->ReadBits(&bit, 1));
        if (bit) {
          RETURN_FALSE_IF_ERROR(br->ConsumeBits(7));
        }
      }
      for (size_t i = 0; i < kVp9MaxModeLFDeltas; i++) {
        RETURN_FALSE_IF_ERROR(br->ReadBits(&bit, 1));
        if (bit) {
          RETURN_FALSE_IF_ERROR(br->ConsumeBits(7));
        }
      }
    }
  }
  return true;
}
}  // namespace

bool GetQp(const uint8_t* buf, size_t length, int* qp) {
  rtc::BitBuffer br(buf, length);

  // Frame marker.
  uint32_t frame_marker;
  RETURN_FALSE_IF_ERROR(br.ReadBits(&frame_marker, 2));
  if (frame_marker != 0x2) {
    RTC_LOG(LS_WARNING) << "Failed to get QP. Frame marker should be 2.";
    return false;
  }

  // Profile.
  uint8_t profile;
  if (!Vp9ReadProfile(&br, &profile))
    return false;

  // Show existing frame.
  uint32_t show_existing_frame;
  RETURN_FALSE_IF_ERROR(br.ReadBits(&show_existing_frame, 1));
  if (show_existing_frame)
    return false;

  // Frame type: KEY_FRAME(0), INTER_FRAME(1).
  uint32_t frame_type;
  uint32_t show_frame;
  uint32_t error_resilient;
  RETURN_FALSE_IF_ERROR(br.ReadBits(&frame_type, 1));
  RETURN_FALSE_IF_ERROR(br.ReadBits(&show_frame, 1));
  RETURN_FALSE_IF_ERROR(br.ReadBits(&error_resilient, 1));

  if (!frame_type) {
    if (!Vp9ReadSyncCode(&br))
      return false;
    if (!Vp9ReadColorConfig(&br, profile))
      return false;
    if (!Vp9ReadFrameSize(&br))
      return false;
    if (!Vp9ReadRenderSize(&br))
      return false;

  } else {
    uint32_t intra_only = 0;
    if (!show_frame)
      RETURN_FALSE_IF_ERROR(br.ReadBits(&intra_only, 1));
    if (!error_resilient)
      RETURN_FALSE_IF_ERROR(br.ConsumeBits(2));  // Reset frame context.

    if (intra_only) {
      if (!Vp9ReadSyncCode(&br))
        return false;

      if (profile > 0) {
        if (!Vp9ReadColorConfig(&br, profile))
          return false;
      }
      // Refresh frame flags.
      RETURN_FALSE_IF_ERROR(br.ConsumeBits(8));
      if (!Vp9ReadFrameSize(&br))
        return false;
      if (!Vp9ReadRenderSize(&br))
        return false;
    } else {
      // Refresh frame flags.
      RETURN_FALSE_IF_ERROR(br.ConsumeBits(8));

      for (size_t i = 0; i < kVp9NumRefsPerFrame; i++) {
        // 3 bits: Ref frame index.
        // 1 bit: Ref frame sign biases.
        RETURN_FALSE_IF_ERROR(br.ConsumeBits(4));
      }

      if (!Vp9ReadFrameSizeFromRefs(&br))
        return false;

      // Allow high precision mv.
      RETURN_FALSE_IF_ERROR(br.ConsumeBits(1));
      // Interpolation filter.
      if (!Vp9ReadInterpolationFilter(&br))
        return false;
    }
  }

  if (!error_resilient) {
    // 1 bit: Refresh frame context.
    // 1 bit: Frame parallel decoding mode.
    RETURN_FALSE_IF_ERROR(br.ConsumeBits(2));
  }

  // Frame context index.
  RETURN_FALSE_IF_ERROR(br.ConsumeBits(2));

  if (!Vp9ReadLoopfilter(&br))
    return false;

  // Base QP.
  uint8_t base_q0;
  RETURN_FALSE_IF_ERROR(br.ReadUInt8(&base_q0));
  *qp = base_q0;
  return true;
}

}  // namespace vp9

}  // namespace webrtc
