/*
 *  Copyright (c) 2013 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include <stddef.h>

#include <cstdint>
#include <vector>

#include BOSS_WEBRTC_U_modules__video_coding__video_receiver2_h //original-code:"modules/video_coding/video_receiver2.h"

#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_modules__video_coding__decoder_database_h //original-code:"modules/video_coding/decoder_database.h"
#include BOSS_WEBRTC_U_modules__video_coding__encoded_frame_h //original-code:"modules/video_coding/encoded_frame.h"
#include BOSS_WEBRTC_U_modules__video_coding__generic_decoder_h //original-code:"modules/video_coding/generic_decoder.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_coding_defines_h //original-code:"modules/video_coding/include/video_coding_defines.h"
#include BOSS_WEBRTC_U_modules__video_coding__timing_h //original-code:"modules/video_coding/timing.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__trace_event_h //original-code:"rtc_base/trace_event.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {

VideoReceiver2::VideoReceiver2(Clock* clock, VCMTiming* timing)
    : clock_(clock),
      timing_(timing),
      decodedFrameCallback_(timing_, clock_),
      codecDataBase_() {
  decoder_sequence_checker_.Detach();
}

VideoReceiver2::~VideoReceiver2() {
  RTC_DCHECK_RUN_ON(&construction_sequence_checker_);
}

// Register a receive callback. Will be called whenever there is a new frame
// ready for rendering.
int32_t VideoReceiver2::RegisterReceiveCallback(
    VCMReceiveCallback* receiveCallback) {
  RTC_DCHECK_RUN_ON(&construction_sequence_checker_);
  RTC_DCHECK(!IsDecoderThreadRunning());
  // This value is set before the decoder thread starts and unset after
  // the decoder thread has been stopped.
  decodedFrameCallback_.SetUserReceiveCallback(receiveCallback);
  return VCM_OK;
}

// Register an externally defined decoder object. This may be called on either
// the construction sequence or the decoder sequence to allow for lazy creation
// of video decoders. If called on the decoder sequence |externalDecoder| cannot
// be a nullptr. It's the responsibility of the caller to make sure that the
// access from the two sequences are mutually exclusive.
void VideoReceiver2::RegisterExternalDecoder(VideoDecoder* externalDecoder,
                                             uint8_t payloadType) {
  if (IsDecoderThreadRunning()) {
    RTC_DCHECK_RUN_ON(&decoder_sequence_checker_);
    // Don't allow deregistering decoders on the decoder thread.
    RTC_DCHECK(externalDecoder != nullptr);
  } else {
    RTC_DCHECK_RUN_ON(&construction_sequence_checker_);
  }

  if (externalDecoder == nullptr) {
    codecDataBase_.DeregisterExternalDecoder(payloadType);
    return;
  }
  codecDataBase_.RegisterExternalDecoder(externalDecoder, payloadType);
}

bool VideoReceiver2::IsExternalDecoderRegistered(uint8_t payloadType) const {
  RTC_DCHECK_RUN_ON(&decoder_sequence_checker_);
  return codecDataBase_.IsExternalDecoderRegistered(payloadType);
}

void VideoReceiver2::DecoderThreadStarting() {
  RTC_DCHECK_RUN_ON(&construction_sequence_checker_);
  RTC_DCHECK(!IsDecoderThreadRunning());
#if RTC_DCHECK_IS_ON
  decoder_thread_is_running_ = true;
#endif
}

void VideoReceiver2::DecoderThreadStopped() {
  RTC_DCHECK_RUN_ON(&construction_sequence_checker_);
  RTC_DCHECK(IsDecoderThreadRunning());
#if RTC_DCHECK_IS_ON
  decoder_thread_is_running_ = false;
  decoder_sequence_checker_.Detach();
#endif
}

// Must be called from inside the receive side critical section.
int32_t VideoReceiver2::Decode(const VCMEncodedFrame* frame) {
  RTC_DCHECK_RUN_ON(&decoder_sequence_checker_);
  TRACE_EVENT0("webrtc", "VideoReceiver2::Decode");
  // Change decoder if payload type has changed
  VCMGenericDecoder* decoder =
      codecDataBase_.GetDecoder(*frame, &decodedFrameCallback_);
  if (decoder == nullptr) {
    return VCM_NO_CODEC_REGISTERED;
  }
  return decoder->Decode(*frame, clock_->CurrentTime());
}

// Register possible receive codecs, can be called multiple times
int32_t VideoReceiver2::RegisterReceiveCodec(uint8_t payload_type,
                                             const VideoCodec* receiveCodec,
                                             int32_t numberOfCores) {
  RTC_DCHECK_RUN_ON(&construction_sequence_checker_);
  RTC_DCHECK(!IsDecoderThreadRunning());
  if (receiveCodec == nullptr) {
    return VCM_PARAMETER_ERROR;
  }
  if (!codecDataBase_.RegisterReceiveCodec(payload_type, receiveCodec,
                                           numberOfCores)) {
    return -1;
  }
  return 0;
}

bool VideoReceiver2::IsDecoderThreadRunning() {
#if RTC_DCHECK_IS_ON
  return decoder_thread_is_running_;
#else
  return true;
#endif
}

}  // namespace webrtc
