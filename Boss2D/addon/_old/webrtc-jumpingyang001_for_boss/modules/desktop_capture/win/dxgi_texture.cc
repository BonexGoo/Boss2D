/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "modules/desktop_capture/win/dxgi_texture.h"

#include <D3D11.h>
#include <comdef.h>
#include <wrl/client.h>

#include BOSS_WEBRTC_U_modules__desktop_capture__desktop_region_h //original-code:"modules/desktop_capture/desktop_region.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"

using Microsoft::WRL::ComPtr;

namespace webrtc {

namespace {

class DxgiDesktopFrame : public DesktopFrame {
 public:
  explicit DxgiDesktopFrame(const DxgiTexture& texture)
      : DesktopFrame(texture.desktop_size(),
                     texture.pitch(),
                     texture.bits(),
                     nullptr) {}

  ~DxgiDesktopFrame() override = default;
};

}  // namespace

DxgiTexture::DxgiTexture() = default;
DxgiTexture::~DxgiTexture() = default;

bool DxgiTexture::CopyFrom(const DXGI_OUTDUPL_FRAME_INFO& frame_info,
                           IDXGIResource* resource) {
  RTC_DCHECK_GT(frame_info.AccumulatedFrames, 0);
  RTC_DCHECK(resource);
  ComPtr<ID3D11Texture2D> texture;
  _com_error error = resource->QueryInterface(
      __uuidof(ID3D11Texture2D),
      reinterpret_cast<void**>(texture.GetAddressOf()));
  if (error.Error() != S_OK || !texture) {
    RTC_LOG(LS_ERROR) << "Failed to convert IDXGIResource to ID3D11Texture2D, "
                         "error "
                      << error.ErrorMessage() << ", code " << error.Error();
    return false;
  }

  D3D11_TEXTURE2D_DESC desc = {0};
  texture->GetDesc(&desc);
  desktop_size_.set(desc.Width, desc.Height);

  return CopyFromTexture(frame_info, texture.Get());
}

const DesktopFrame& DxgiTexture::AsDesktopFrame() {
  if (!frame_) {
    frame_.reset(new DxgiDesktopFrame(*this));
  }
  return *frame_;
}

bool DxgiTexture::Release() {
  frame_.reset();
  return DoRelease();
}

DXGI_MAPPED_RECT* DxgiTexture::rect() {
  return &rect_;
}

}  // namespace webrtc
