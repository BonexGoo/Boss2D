/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/desktop_capture/win/dxgi_texture_mapping.h"

#include <DXGI.h>
#include <DXGI1_2.h>
#include <comdef.h>

#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

namespace webrtc {

DxgiTextureMapping::DxgiTextureMapping(IDXGIOutputDuplication* duplication)
    : duplication_(duplication) {
  RTC_DCHECK(duplication_);
}

DxgiTextureMapping::~DxgiTextureMapping() = default;

bool DxgiTextureMapping::CopyFromTexture(
    const DXGI_OUTDUPL_FRAME_INFO& frame_info,
    ID3D11Texture2D* texture) {
  RTC_DCHECK_GT(frame_info.AccumulatedFrames, 0);
  RTC_DCHECK(texture);
  *rect() = {0};
  _com_error error = duplication_->MapDesktopSurface(rect());
  if (error.Error() != S_OK) {
    *rect() = {0};
    RTC_LOG(LS_ERROR)
        << "Failed to map the IDXGIOutputDuplication to a bitmap, "
           "error "
        << error.ErrorMessage() << ", code " << error.Error();
    return false;
  }

  return true;
}

bool DxgiTextureMapping::DoRelease() {
  _com_error error = duplication_->UnMapDesktopSurface();
  if (error.Error() != S_OK) {
    RTC_LOG(LS_ERROR) << "Failed to unmap the IDXGIOutputDuplication, error "
                      << error.ErrorMessage() << ", code " << error.Error();
    return false;
  }
  return true;
}

}  // namespace webrtc
