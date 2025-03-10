/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef AUDIO_CHANNEL_SEND_FRAME_TRANSFORMER_DELEGATE_H_
#define AUDIO_CHANNEL_SEND_FRAME_TRANSFORMER_DELEGATE_H_

#include <memory>

#include BOSS_WEBRTC_U_api__frame_transformer_interface_h //original-code:"api/frame_transformer_interface.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_modules__audio_coding__include__audio_coding_module_typedefs_h //original-code:"modules/audio_coding/include/audio_coding_module_typedefs.h"
#include BOSS_WEBRTC_U_rtc_base__buffer_h //original-code:"rtc_base/buffer.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__task_queue_h //original-code:"rtc_base/task_queue.h"

namespace webrtc {

// Delegates calls to FrameTransformerInterface to transform frames, and to
// ChannelSend to send the transformed frames using |send_frame_callback_| on
// the |encoder_queue_|.
// OnTransformedFrame() can be called from any thread, the delegate ensures
// thread-safe access to the ChannelSend callback.
class ChannelSendFrameTransformerDelegate : public TransformedFrameCallback {
 public:
  using SendFrameCallback =
      std::function<int32_t(AudioFrameType frameType,
                            uint8_t payloadType,
                            uint32_t rtp_timestamp,
                            rtc::ArrayView<const uint8_t> payload,
                            int64_t absolute_capture_timestamp_ms)>;
  ChannelSendFrameTransformerDelegate(
      SendFrameCallback send_frame_callback,
      rtc::scoped_refptr<FrameTransformerInterface> frame_transformer,
      rtc::TaskQueue* encoder_queue);

  // Registers |this| as callback for |frame_transformer_|, to get the
  // transformed frames.
  void Init();

  // Unregisters and releases the |frame_transformer_| reference, and resets
  // |send_frame_callback_| under lock. Called from ChannelSend destructor to
  // prevent running the callback on a dangling channel.
  void Reset();

  // Delegates the call to FrameTransformerInterface::TransformFrame, to
  // transform the frame asynchronously.
  void Transform(AudioFrameType frame_type,
                 uint8_t payload_type,
                 uint32_t rtp_timestamp,
                 uint32_t rtp_start_timestamp,
                 const uint8_t* payload_data,
                 size_t payload_size,
                 int64_t absolute_capture_timestamp_ms,
                 uint32_t ssrc);

  // Implements TransformedFrameCallback. Can be called on any thread.
  void OnTransformedFrame(
      std::unique_ptr<TransformableFrameInterface> frame) override;

  // Delegates the call to ChannelSend::SendRtpAudio on the |encoder_queue_|,
  // by calling |send_audio_callback_|.
  void SendFrame(std::unique_ptr<TransformableFrameInterface> frame) const;

 protected:
  ~ChannelSendFrameTransformerDelegate() override = default;

 private:
  mutable Mutex send_lock_;
  SendFrameCallback send_frame_callback_ RTC_GUARDED_BY(send_lock_);
  rtc::scoped_refptr<FrameTransformerInterface> frame_transformer_;
  rtc::TaskQueue* encoder_queue_ RTC_GUARDED_BY(send_lock_);
};
}  // namespace webrtc
#endif  // AUDIO_CHANNEL_SEND_FRAME_TRANSFORMER_DELEGATE_H_
