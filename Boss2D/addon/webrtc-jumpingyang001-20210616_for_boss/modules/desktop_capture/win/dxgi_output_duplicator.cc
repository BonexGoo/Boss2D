/*
 *  Copyright (c) 2016 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__desktop_capture__win__dxgi_output_duplicator_h //original-code:"modules/desktop_capture/win/dxgi_output_duplicator.h"

#include <dxgi.h>
#include <dxgiformat.h>
#include <string.h>
#include <unknwn.h>
#include <windows.h>

#include <algorithm>

#include BOSS_WEBRTC_U_modules__desktop_capture__win__desktop_capture_utils_h //original-code:"modules/desktop_capture/win/desktop_capture_utils.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__win__dxgi_texture_mapping_h //original-code:"modules/desktop_capture/win/dxgi_texture_mapping.h"
#include BOSS_WEBRTC_U_modules__desktop_capture__win__dxgi_texture_staging_h //original-code:"modules/desktop_capture/win/dxgi_texture_staging.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__string_utils_h //original-code:"rtc_base/string_utils.h"
#include BOSS_WEBRTC_U_rtc_base__win32_h //original-code:"rtc_base/win32.h"

namespace webrtc {

using Microsoft::WRL::ComPtr;

namespace {

// Timeout for AcquireNextFrame() call.
// DxgiDuplicatorController leverages external components to do the capture
// scheduling. So here DxgiOutputDuplicator does not need to actively wait for a
// new frame.
const int kAcquireTimeoutMs = 0;

DesktopRect RECTToDesktopRect(const RECT& rect) {
  return DesktopRect::MakeLTRB(rect.left, rect.top, rect.right, rect.bottom);
}

Rotation DxgiRotationToRotation(DXGI_MODE_ROTATION rotation) {
  switch (rotation) {
    case DXGI_MODE_ROTATION_IDENTITY:
    case DXGI_MODE_ROTATION_UNSPECIFIED:
      return Rotation::CLOCK_WISE_0;
    case DXGI_MODE_ROTATION_ROTATE90:
      return Rotation::CLOCK_WISE_90;
    case DXGI_MODE_ROTATION_ROTATE180:
      return Rotation::CLOCK_WISE_180;
    case DXGI_MODE_ROTATION_ROTATE270:
      return Rotation::CLOCK_WISE_270;
  }

  RTC_NOTREACHED();
  return Rotation::CLOCK_WISE_0;
}

}  // namespace

DxgiOutputDuplicator::DxgiOutputDuplicator(const D3dDevice& device,
                                           const ComPtr<IDXGIOutput1>& output,
                                           const DXGI_OUTPUT_DESC& desc)
    : device_(device),
      output_(output),
      device_name_(rtc::ToUtf8(desc.DeviceName)),
      desktop_rect_(RECTToDesktopRect(desc.DesktopCoordinates)) {
  RTC_DCHECK(output_);
  RTC_DCHECK(!desktop_rect_.is_empty());
  RTC_DCHECK_GT(desktop_rect_.width(), 0);
  RTC_DCHECK_GT(desktop_rect_.height(), 0);
}

DxgiOutputDuplicator::DxgiOutputDuplicator(DxgiOutputDuplicator&& other) =
    default;

DxgiOutputDuplicator::~DxgiOutputDuplicator() {
  if (duplication_) {
    duplication_->ReleaseFrame();
  }
  texture_.reset();
}

bool DxgiOutputDuplicator::Initialize() {
  if (DuplicateOutput()) {
    if (desc_.DesktopImageInSystemMemory) {
      texture_.reset(new DxgiTextureMapping(duplication_.Get()));
    } else {
      texture_.reset(new DxgiTextureStaging(device_));
    }
    return true;
  } else {
    duplication_.Reset();
    return false;
  }
}

bool DxgiOutputDuplicator::DuplicateOutput() {
  RTC_DCHECK(!duplication_);
  _com_error error =
      output_->DuplicateOutput(static_cast<IUnknown*>(device_.d3d_device()),
                               duplication_.GetAddressOf());
  if (error.Error() != S_OK || !duplication_) {
    RTC_LOG(LS_WARNING) << "Failed to duplicate output from IDXGIOutput1: "
                        << desktop_capture::utils::ComErrorToString(error);
    return false;
  }

  memset(&desc_, 0, sizeof(desc_));
  duplication_->GetDesc(&desc_);
  if (desc_.ModeDesc.Format != DXGI_FORMAT_B8G8R8A8_UNORM) {
    RTC_LOG(LS_ERROR) << "IDXGIDuplicateOutput does not use RGBA (8 bit) "
                      << "format, which is required by downstream components, "
                      << "format is " << desc_.ModeDesc.Format;
    return false;
  }

  if (static_cast<int>(desc_.ModeDesc.Width) != desktop_rect_.width() ||
      static_cast<int>(desc_.ModeDesc.Height) != desktop_rect_.height()) {
    RTC_LOG(LS_ERROR)
        << "IDXGIDuplicateOutput does not return a same size as its "
        << "IDXGIOutput1, size returned by IDXGIDuplicateOutput is "
        << desc_.ModeDesc.Width << " x " << desc_.ModeDesc.Height
        << ", size returned by IDXGIOutput1 is " << desktop_rect_.width()
        << " x " << desktop_rect_.height();
    return false;
  }

  rotation_ = DxgiRotationToRotation(desc_.Rotation);
  unrotated_size_ = RotateSize(desktop_size(), ReverseRotation(rotation_));

  return true;
}

bool DxgiOutputDuplicator::ReleaseFrame() {
  RTC_DCHECK(duplication_);
  _com_error error = duplication_->ReleaseFrame();
  if (error.Error() != S_OK) {
    RTC_LOG(LS_ERROR) << "Failed to release frame from IDXGIOutputDuplication: "
                      << desktop_capture::utils::ComErrorToString(error);
    return false;
  }
  return true;
}

bool DxgiOutputDuplicator::Duplicate(Context* context,
                                     DesktopVector offset,
                                     SharedDesktopFrame* target) {
  RTC_DCHECK(duplication_);
  RTC_DCHECK(texture_);
  RTC_DCHECK(target);
  if (!DesktopRect::MakeSize(target->size())
           .ContainsRect(GetTranslatedDesktopRect(offset))) {
    // target size is not large enough to cover current output region.
    return false;
  }

  DXGI_OUTDUPL_FRAME_INFO frame_info;
  memset(&frame_info, 0, sizeof(frame_info));
  ComPtr<IDXGIResource> resource;
  _com_error error = duplication_->AcquireNextFrame(
      kAcquireTimeoutMs, &frame_info, resource.GetAddressOf());
  if (error.Error() != S_OK && error.Error() != DXGI_ERROR_WAIT_TIMEOUT) {
    RTC_LOG(LS_ERROR) << "Failed to capture frame: "
                      << desktop_capture::utils::ComErrorToString(error);
    return false;
  }

  // We need to merge updated region with the one from context, but only spread
  // updated region from current frame. So keeps a copy of updated region from
  // context here. The |updated_region| always starts from (0, 0).
  DesktopRegion updated_region;
  updated_region.Swap(&context->updated_region);
  if (error.Error() == S_OK && frame_info.AccumulatedFrames > 0 && resource) {
    DetectUpdatedRegion(frame_info, &context->updated_region);
    SpreadContextChange(context);
    if (!texture_->CopyFrom(frame_info, resource.Get())) {
      return false;
    }
    updated_region.AddRegion(context->updated_region);
    // TODO(zijiehe): Figure out why clearing context->updated_region() here
    // triggers screen flickering?

    const DesktopFrame& source = texture_->AsDesktopFrame();
    if (rotation_ != Rotation::CLOCK_WISE_0) {
      for (DesktopRegion::Iterator it(updated_region); !it.IsAtEnd();
           it.Advance()) {
        // The |updated_region| returned by Windows is rotated, but the |source|
        // frame is not. So we need to rotate it reversely.
        const DesktopRect source_rect =
            RotateRect(it.rect(), desktop_size(), ReverseRotation(rotation_));
        RotateDesktopFrame(source, source_rect, rotation_, offset, target);
      }
    } else {
      for (DesktopRegion::Iterator it(updated_region); !it.IsAtEnd();
           it.Advance()) {
        // The DesktopRect in |target|, starts from offset.
        DesktopRect dest_rect = it.rect();
        dest_rect.Translate(offset);
        target->CopyPixelsFrom(source, it.rect().top_left(), dest_rect);
      }
    }
    last_frame_ = target->Share();
    last_frame_offset_ = offset;
    updated_region.Translate(offset.x(), offset.y());
    target->mutable_updated_region()->AddRegion(updated_region);
    num_frames_captured_++;
    return texture_->Release() && ReleaseFrame();
  }

  if (last_frame_) {
    // No change since last frame or AcquireNextFrame() timed out, we will
    // export last frame to the target.
    for (DesktopRegion::Iterator it(updated_region); !it.IsAtEnd();
         it.Advance()) {
      // The DesktopRect in |source|, starts from last_frame_offset_.
      DesktopRect source_rect = it.rect();
      // The DesktopRect in |target|, starts from offset.
      DesktopRect target_rect = source_rect;
      source_rect.Translate(last_frame_offset_);
      target_rect.Translate(offset);
      target->CopyPixelsFrom(*last_frame_, source_rect.top_left(), target_rect);
    }
    updated_region.Translate(offset.x(), offset.y());
    target->mutable_updated_region()->AddRegion(updated_region);
  } else {
    // If we were at the very first frame, and capturing failed, the
    // context->updated_region should be kept unchanged for next attempt.
    context->updated_region.Swap(&updated_region);
  }
  // If AcquireNextFrame() failed with timeout error, we do not need to release
  // the frame.
  return error.Error() == DXGI_ERROR_WAIT_TIMEOUT || ReleaseFrame();
}

DesktopRect DxgiOutputDuplicator::GetTranslatedDesktopRect(
    DesktopVector offset) const {
  DesktopRect result(DesktopRect::MakeSize(desktop_size()));
  result.Translate(offset);
  return result;
}

DesktopRect DxgiOutputDuplicator::GetUntranslatedDesktopRect() const {
  return DesktopRect::MakeSize(desktop_size());
}

void DxgiOutputDuplicator::DetectUpdatedRegion(
    const DXGI_OUTDUPL_FRAME_INFO& frame_info,
    DesktopRegion* updated_region) {
  if (DoDetectUpdatedRegion(frame_info, updated_region)) {
    // Make sure even a region returned by Windows API is out of the scope of
    // desktop_rect_, we still won't export it to the target DesktopFrame.
    updated_region->IntersectWith(GetUntranslatedDesktopRect());
  } else {
    updated_region->SetRect(GetUntranslatedDesktopRect());
  }
}

bool DxgiOutputDuplicator::DoDetectUpdatedRegion(
    const DXGI_OUTDUPL_FRAME_INFO& frame_info,
    DesktopRegion* updated_region) {
  RTC_DCHECK(updated_region);
  updated_region->Clear();
  if (frame_info.TotalMetadataBufferSize == 0) {
    // This should not happen, since frame_info.AccumulatedFrames > 0.
    RTC_LOG(LS_ERROR) << "frame_info.AccumulatedFrames > 0, "
                      << "but TotalMetadataBufferSize == 0";
    return false;
  }

  if (metadata_.size() < frame_info.TotalMetadataBufferSize) {
    metadata_.clear();  // Avoid data copy
    metadata_.resize(frame_info.TotalMetadataBufferSize);
  }

  UINT buff_size = 0;
  DXGI_OUTDUPL_MOVE_RECT* move_rects =
      reinterpret_cast<DXGI_OUTDUPL_MOVE_RECT*>(metadata_.data());
  size_t move_rects_count = 0;
  _com_error error = duplication_->GetFrameMoveRects(
      static_cast<UINT>(metadata_.size()), move_rects, &buff_size);
  if (error.Error() != S_OK) {
    RTC_LOG(LS_ERROR) << "Failed to get move rectangles: "
                      << desktop_capture::utils::ComErrorToString(error);
    return false;
  }
  move_rects_count = buff_size / sizeof(DXGI_OUTDUPL_MOVE_RECT);

  RECT* dirty_rects = reinterpret_cast<RECT*>(metadata_.data() + buff_size);
  size_t dirty_rects_count = 0;
  error = duplication_->GetFrameDirtyRects(
      static_cast<UINT>(metadata_.size()) - buff_size, dirty_rects, &buff_size);
  if (error.Error() != S_OK) {
    RTC_LOG(LS_ERROR) << "Failed to get dirty rectangles: "
                      << desktop_capture::utils::ComErrorToString(error);
    return false;
  }
  dirty_rects_count = buff_size / sizeof(RECT);

  while (move_rects_count > 0) {
    // DirectX capturer API may randomly return unmoved move_rects, which should
    // be skipped to avoid unnecessary wasting of differing and encoding
    // resources.
    // By using testing application it2me_standalone_host_main, this check
    // reduces average capture time by 0.375% (4.07 -> 4.055), and average
    // encode time by 0.313% (8.042 -> 8.016) without other impacts.
    if (move_rects->SourcePoint.x != move_rects->DestinationRect.left ||
        move_rects->SourcePoint.y != move_rects->DestinationRect.top) {
      updated_region->AddRect(
          RotateRect(DesktopRect::MakeXYWH(move_rects->SourcePoint.x,
                                           move_rects->SourcePoint.y,
                                           move_rects->DestinationRect.right -
                                               move_rects->DestinationRect.left,
                                           move_rects->DestinationRect.bottom -
                                               move_rects->DestinationRect.top),
                     unrotated_size_, rotation_));
      updated_region->AddRect(
          RotateRect(DesktopRect::MakeLTRB(move_rects->DestinationRect.left,
                                           move_rects->DestinationRect.top,
                                           move_rects->DestinationRect.right,
                                           move_rects->DestinationRect.bottom),
                     unrotated_size_, rotation_));
    } else {
      RTC_LOG(LS_INFO) << "Unmoved move_rect detected, ["
                       << move_rects->DestinationRect.left << ", "
                       << move_rects->DestinationRect.top << "] - ["
                       << move_rects->DestinationRect.right << ", "
                       << move_rects->DestinationRect.bottom << "].";
    }
    move_rects++;
    move_rects_count--;
  }

  while (dirty_rects_count > 0) {
    updated_region->AddRect(RotateRect(
        DesktopRect::MakeLTRB(dirty_rects->left, dirty_rects->top,
                              dirty_rects->right, dirty_rects->bottom),
        unrotated_size_, rotation_));
    dirty_rects++;
    dirty_rects_count--;
  }

  return true;
}

void DxgiOutputDuplicator::Setup(Context* context) {
  RTC_DCHECK(context->updated_region.is_empty());
  // Always copy entire monitor during the first Duplicate() function call.
  context->updated_region.AddRect(GetUntranslatedDesktopRect());
  RTC_DCHECK(std::find(contexts_.begin(), contexts_.end(), context) ==
             contexts_.end());
  contexts_.push_back(context);
}

void DxgiOutputDuplicator::Unregister(const Context* const context) {
  auto it = std::find(contexts_.begin(), contexts_.end(), context);
  RTC_DCHECK(it != contexts_.end());
  contexts_.erase(it);
}

void DxgiOutputDuplicator::SpreadContextChange(const Context* const source) {
  for (Context* dest : contexts_) {
    RTC_DCHECK(dest);
    if (dest != source) {
      dest->updated_region.AddRegion(source->updated_region);
    }
  }
}

DesktopSize DxgiOutputDuplicator::desktop_size() const {
  return desktop_rect_.size();
}

int64_t DxgiOutputDuplicator::num_frames_captured() const {
#if !defined(NDEBUG)
  RTC_DCHECK_EQ(!!last_frame_, num_frames_captured_ > 0);
#endif
  return num_frames_captured_;
}

void DxgiOutputDuplicator::TranslateRect(const DesktopVector& position) {
  desktop_rect_.Translate(position);
  RTC_DCHECK_GE(desktop_rect_.left(), 0);
  RTC_DCHECK_GE(desktop_rect_.top(), 0);
}

}  // namespace webrtc
