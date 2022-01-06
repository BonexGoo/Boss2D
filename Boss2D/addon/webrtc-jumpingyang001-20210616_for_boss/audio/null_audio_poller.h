/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef AUDIO_NULL_AUDIO_POLLER_H_
#define AUDIO_NULL_AUDIO_POLLER_H_

#include <stdint.h>

#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_modules__audio_device__include__audio_device_defines_h //original-code:"modules/audio_device/include/audio_device_defines.h"
#include BOSS_WEBRTC_U_rtc_base__message_handler_h //original-code:"rtc_base/message_handler.h"

namespace webrtc {
namespace internal {

class NullAudioPoller final : public rtc::MessageHandler {
 public:
  explicit NullAudioPoller(AudioTransport* audio_transport);
  ~NullAudioPoller() override;

 protected:
  void OnMessage(rtc::Message* msg) override;

 private:
  SequenceChecker thread_checker_;
  AudioTransport* const audio_transport_;
  int64_t reschedule_at_;
};

}  // namespace internal
}  // namespace webrtc

#endif  // AUDIO_NULL_AUDIO_POLLER_H_
