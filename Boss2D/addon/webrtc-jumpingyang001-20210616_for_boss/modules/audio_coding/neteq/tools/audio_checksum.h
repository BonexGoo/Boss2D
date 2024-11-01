/*
 *  Copyright (c) 2014 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_NETEQ_TOOLS_AUDIO_CHECKSUM_H_
#define MODULES_AUDIO_CODING_NETEQ_TOOLS_AUDIO_CHECKSUM_H_

#include <memory>
#include <string>

#include BOSS_WEBRTC_U_modules__audio_coding__neteq__tools__audio_sink_h //original-code:"modules/audio_coding/neteq/tools/audio_sink.h"
#include BOSS_WEBRTC_U_rtc_base__buffer_h //original-code:"rtc_base/buffer.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__message_digest_h //original-code:"rtc_base/message_digest.h"
#include BOSS_WEBRTC_U_rtc_base__string_encode_h //original-code:"rtc_base/string_encode.h"
#include BOSS_WEBRTC_U_rtc_base__system__arch_h //original-code:"rtc_base/system/arch.h"

namespace webrtc {
namespace test {

class AudioChecksum : public AudioSink {
 public:
  AudioChecksum()
      : checksum_(rtc::MessageDigestFactory::Create(rtc::DIGEST_MD5)),
        checksum_result_(checksum_->Size()),
        finished_(false) {}

  bool WriteArray(const int16_t* audio, size_t num_samples) override {
    if (finished_)
      return false;

#ifndef WEBRTC_ARCH_LITTLE_ENDIAN
#error "Big-endian gives a different checksum"
#endif
    checksum_->Update(audio, num_samples * sizeof(*audio));
    return true;
  }

  // Finalizes the computations, and returns the checksum.
  std::string Finish() {
    if (!finished_) {
      finished_ = true;
      checksum_->Finish(checksum_result_.data(), checksum_result_.size());
    }
    return rtc::hex_encode(checksum_result_.data<char>(),
                           checksum_result_.size());
  }

 private:
  std::unique_ptr<rtc::MessageDigest> checksum_;
  rtc::Buffer checksum_result_;
  bool finished_;

  RTC_DISALLOW_COPY_AND_ASSIGN(AudioChecksum);
};

}  // namespace test
}  // namespace webrtc
#endif  // MODULES_AUDIO_CODING_NETEQ_TOOLS_AUDIO_CHECKSUM_H_
