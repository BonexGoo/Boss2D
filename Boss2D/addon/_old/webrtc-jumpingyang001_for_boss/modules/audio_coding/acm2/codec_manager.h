/*
 *  Copyright (c) 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef MODULES_AUDIO_CODING_ACM2_CODEC_MANAGER_H_
#define MODULES_AUDIO_CODING_ACM2_CODEC_MANAGER_H_

#include <map>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_common_types_h //original-code:"common_types.h"  // NOLINT(build/include)
#include BOSS_WEBRTC_U_modules__audio_coding__acm2__rent_a_codec_h //original-code:"modules/audio_coding/acm2/rent_a_codec.h"
#include BOSS_WEBRTC_U_modules__audio_coding__include__audio_coding_module_h //original-code:"modules/audio_coding/include/audio_coding_module.h"
#include BOSS_WEBRTC_U_modules__audio_coding__include__audio_coding_module_typedefs_h //original-code:"modules/audio_coding/include/audio_coding_module_typedefs.h"
#include BOSS_WEBRTC_U_rtc_base__constructormagic_h //original-code:"rtc_base/constructormagic.h"
#include BOSS_WEBRTC_U_rtc_base__thread_checker_h //original-code:"rtc_base/thread_checker.h"

namespace webrtc {

class AudioDecoder;
class AudioEncoder;

namespace acm2 {

class CodecManager final {
 public:
  CodecManager();
  ~CodecManager();

  // Parses the given specification. On success, returns true and updates the
  // stored CodecInst and stack parameters; on error, returns false.
  bool RegisterEncoder(const CodecInst& send_codec);

  static CodecInst ForgeCodecInst(const AudioEncoder* external_speech_encoder);

  const CodecInst* GetCodecInst() const {
    return send_codec_inst_ ? &*send_codec_inst_ : nullptr;
  }

  void UnsetCodecInst() { send_codec_inst_ = absl::nullopt; }

  const RentACodec::StackParameters* GetStackParams() const {
    return &codec_stack_params_;
  }
  RentACodec::StackParameters* GetStackParams() { return &codec_stack_params_; }

  bool SetCopyRed(bool enable);

  bool SetVAD(bool enable, ACMVADMode mode);

  bool SetCodecFEC(bool enable_codec_fec);

  // Uses the provided Rent-A-Codec to create a new encoder stack, if we have a
  // complete specification; if so, it is then passed to set_encoder. On error,
  // returns false.
  bool MakeEncoder(RentACodec* rac, AudioCodingModule* acm);

 private:
  rtc::ThreadChecker thread_checker_;
  absl::optional<CodecInst> send_codec_inst_;
  RentACodec::StackParameters codec_stack_params_;
  bool recreate_encoder_ = true;  // Need to recreate encoder?

  RTC_DISALLOW_COPY_AND_ASSIGN(CodecManager);
};

}  // namespace acm2
}  // namespace webrtc
#endif  // MODULES_AUDIO_CODING_ACM2_CODEC_MANAGER_H_
