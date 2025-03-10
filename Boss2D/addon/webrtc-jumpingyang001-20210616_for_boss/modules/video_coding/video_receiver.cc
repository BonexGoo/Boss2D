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

#include BOSS_WEBRTC_U_api__rtp_headers_h //original-code:"api/rtp_headers.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_codec_h //original-code:"api/video_codecs/video_codec.h"
#include BOSS_WEBRTC_U_api__video_codecs__video_decoder_h //original-code:"api/video_codecs/video_decoder.h"
#include BOSS_WEBRTC_U_modules__utility__include__process_thread_h //original-code:"modules/utility/include/process_thread.h"
#include BOSS_WEBRTC_U_modules__video_coding__decoder_database_h //original-code:"modules/video_coding/decoder_database.h"
#include BOSS_WEBRTC_U_modules__video_coding__encoded_frame_h //original-code:"modules/video_coding/encoded_frame.h"
#include BOSS_WEBRTC_U_modules__video_coding__generic_decoder_h //original-code:"modules/video_coding/generic_decoder.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_coding_h //original-code:"modules/video_coding/include/video_coding.h"
#include BOSS_WEBRTC_U_modules__video_coding__include__video_coding_defines_h //original-code:"modules/video_coding/include/video_coding_defines.h"
#include BOSS_WEBRTC_U_modules__video_coding__internal_defines_h //original-code:"modules/video_coding/internal_defines.h"
#include BOSS_WEBRTC_U_modules__video_coding__jitter_buffer_h //original-code:"modules/video_coding/jitter_buffer.h"
#include BOSS_WEBRTC_U_modules__video_coding__media_opt_util_h //original-code:"modules/video_coding/media_opt_util.h"
#include BOSS_WEBRTC_U_modules__video_coding__packet_h //original-code:"modules/video_coding/packet.h"
#include BOSS_WEBRTC_U_modules__video_coding__receiver_h //original-code:"modules/video_coding/receiver.h"
#include BOSS_WEBRTC_U_modules__video_coding__timing_h //original-code:"modules/video_coding/timing.h"
#include BOSS_WEBRTC_U_modules__video_coding__video_coding_impl_h //original-code:"modules/video_coding/video_coding_impl.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__location_h //original-code:"rtc_base/location.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__one_time_event_h //original-code:"rtc_base/one_time_event.h"
#include BOSS_WEBRTC_U_rtc_base__trace_event_h //original-code:"rtc_base/trace_event.h"
#include BOSS_WEBRTC_U_system_wrappers__include__clock_h //original-code:"system_wrappers/include/clock.h"

namespace webrtc {
namespace vcm {

VideoReceiver::VideoReceiver(Clock* clock, VCMTiming* timing)
    : clock_(clock),
      _timing(timing),
      _receiver(_timing, clock_),
      _decodedFrameCallback(_timing, clock_),
      _frameTypeCallback(nullptr),
      _packetRequestCallback(nullptr),
      _scheduleKeyRequest(false),
      drop_frames_until_keyframe_(false),
      max_nack_list_size_(0),
      _codecDataBase(),
      _retransmissionTimer(10, clock_),
      _keyRequestTimer(500, clock_) {
  decoder_thread_checker_.Detach();
  module_thread_checker_.Detach();
}

VideoReceiver::~VideoReceiver() {
  RTC_DCHECK_RUN_ON(&construction_thread_checker_);
}

void VideoReceiver::Process() {
  RTC_DCHECK_RUN_ON(&module_thread_checker_);

  // Key frame requests
  if (_keyRequestTimer.TimeUntilProcess() == 0) {
    _keyRequestTimer.Processed();
    bool request_key_frame = _frameTypeCallback != nullptr;
    if (request_key_frame) {
      MutexLock lock(&process_mutex_);
      request_key_frame = _scheduleKeyRequest;
    }
    if (request_key_frame)
      RequestKeyFrame();
  }

  // Packet retransmission requests
  // TODO(holmer): Add API for changing Process interval and make sure it's
  // disabled when NACK is off.
  if (_retransmissionTimer.TimeUntilProcess() == 0) {
    _retransmissionTimer.Processed();
    bool callback_registered = _packetRequestCallback != nullptr;
    uint16_t length = max_nack_list_size_;
    if (callback_registered && length > 0) {
      // Collect sequence numbers from the default receiver.
      bool request_key_frame = false;
      std::vector<uint16_t> nackList = _receiver.NackList(&request_key_frame);
      int32_t ret = VCM_OK;
      if (request_key_frame) {
        ret = RequestKeyFrame();
      }
      if (ret == VCM_OK && !nackList.empty()) {
        MutexLock lock(&process_mutex_);
        if (_packetRequestCallback != nullptr) {
          _packetRequestCallback->ResendPackets(&nackList[0], nackList.size());
        }
      }
    }
  }
}

void VideoReceiver::ProcessThreadAttached(ProcessThread* process_thread) {
  RTC_DCHECK_RUN_ON(&construction_thread_checker_);
  if (process_thread) {
    is_attached_to_process_thread_ = true;
    RTC_DCHECK(!process_thread_ || process_thread_ == process_thread);
    process_thread_ = process_thread;
  } else {
    is_attached_to_process_thread_ = false;
  }
}

int64_t VideoReceiver::TimeUntilNextProcess() {
  RTC_DCHECK_RUN_ON(&module_thread_checker_);
  int64_t timeUntilNextProcess = _retransmissionTimer.TimeUntilProcess();

  timeUntilNextProcess =
      VCM_MIN(timeUntilNextProcess, _keyRequestTimer.TimeUntilProcess());

  return timeUntilNextProcess;
}

// Register a receive callback. Will be called whenever there is a new frame
// ready for rendering.
int32_t VideoReceiver::RegisterReceiveCallback(
    VCMReceiveCallback* receiveCallback) {
  RTC_DCHECK_RUN_ON(&construction_thread_checker_);
  // This value is set before the decoder thread starts and unset after
  // the decoder thread has been stopped.
  _decodedFrameCallback.SetUserReceiveCallback(receiveCallback);
  return VCM_OK;
}

// Register an externally defined decoder object.
void VideoReceiver::RegisterExternalDecoder(VideoDecoder* externalDecoder,
                                            uint8_t payloadType) {
  RTC_DCHECK_RUN_ON(&construction_thread_checker_);
  if (externalDecoder == nullptr) {
    RTC_CHECK(_codecDataBase.DeregisterExternalDecoder(payloadType));
    return;
  }
  _codecDataBase.RegisterExternalDecoder(externalDecoder, payloadType);
}

// Register a frame type request callback.
int32_t VideoReceiver::RegisterFrameTypeCallback(
    VCMFrameTypeCallback* frameTypeCallback) {
  RTC_DCHECK_RUN_ON(&construction_thread_checker_);
  RTC_DCHECK(!is_attached_to_process_thread_);
  // This callback is used on the module thread, but since we don't get
  // callbacks on the module thread while the decoder thread isn't running
  // (and this function must not be called when the decoder is running),
  // we don't need a lock here.
  _frameTypeCallback = frameTypeCallback;
  return VCM_OK;
}

int32_t VideoReceiver::RegisterPacketRequestCallback(
    VCMPacketRequestCallback* callback) {
  RTC_DCHECK_RUN_ON(&construction_thread_checker_);
  RTC_DCHECK(!is_attached_to_process_thread_);
  // This callback is used on the module thread, but since we don't get
  // callbacks on the module thread while the decoder thread isn't running
  // (and this function must not be called when the decoder is running),
  // we don't need a lock here.
  _packetRequestCallback = callback;
  return VCM_OK;
}

// Decode next frame, blocking.
// Should be called as often as possible to get the most out of the decoder.
int32_t VideoReceiver::Decode(uint16_t maxWaitTimeMs) {
  RTC_DCHECK_RUN_ON(&decoder_thread_checker_);
  VCMEncodedFrame* frame = _receiver.FrameForDecoding(maxWaitTimeMs, true);

  if (!frame)
    return VCM_FRAME_NOT_READY;

  bool drop_frame = false;
  {
    MutexLock lock(&process_mutex_);
    if (drop_frames_until_keyframe_) {
      // Still getting delta frames, schedule another keyframe request as if
      // decode failed.
      if (frame->FrameType() != VideoFrameType::kVideoFrameKey) {
        drop_frame = true;
        _scheduleKeyRequest = true;
        // TODO(tommi): Consider if we could instead post a task to the module
        // thread and call RequestKeyFrame directly. Here we call WakeUp so that
        // TimeUntilNextProcess() gets called straight away.
        process_thread_->WakeUp(this);
      } else {
        drop_frames_until_keyframe_ = false;
      }
    }
  }

  if (drop_frame) {
    _receiver.ReleaseFrame(frame);
    return VCM_FRAME_NOT_READY;
  }

  // If this frame was too late, we should adjust the delay accordingly
  _timing->UpdateCurrentDelay(frame->RenderTimeMs(),
                              clock_->TimeInMilliseconds());

  if (first_frame_received_()) {
    RTC_LOG(LS_INFO) << "Received first complete decodable video frame";
  }

  const int32_t ret = Decode(*frame);
  _receiver.ReleaseFrame(frame);
  return ret;
}

int32_t VideoReceiver::RequestKeyFrame() {
  RTC_DCHECK_RUN_ON(&module_thread_checker_);

  TRACE_EVENT0("webrtc", "RequestKeyFrame");
  if (_frameTypeCallback != nullptr) {
    const int32_t ret = _frameTypeCallback->RequestKeyFrame();
    if (ret < 0) {
      return ret;
    }
    MutexLock lock(&process_mutex_);
    _scheduleKeyRequest = false;
  } else {
    return VCM_MISSING_CALLBACK;
  }
  return VCM_OK;
}

// Must be called from inside the receive side critical section.
int32_t VideoReceiver::Decode(const VCMEncodedFrame& frame) {
  RTC_DCHECK_RUN_ON(&decoder_thread_checker_);
  TRACE_EVENT0("webrtc", "VideoReceiver::Decode");
  // Change decoder if payload type has changed
  VCMGenericDecoder* decoder =
      _codecDataBase.GetDecoder(frame, &_decodedFrameCallback);
  if (decoder == nullptr) {
    return VCM_NO_CODEC_REGISTERED;
  }
  return decoder->Decode(frame, clock_->CurrentTime());
}

// Register possible receive codecs, can be called multiple times
int32_t VideoReceiver::RegisterReceiveCodec(uint8_t payload_type,
                                            const VideoCodec* receiveCodec,
                                            int32_t numberOfCores) {
  RTC_DCHECK_RUN_ON(&construction_thread_checker_);
  if (receiveCodec == nullptr) {
    return VCM_PARAMETER_ERROR;
  }
  if (!_codecDataBase.RegisterReceiveCodec(payload_type, receiveCodec,
                                           numberOfCores)) {
    return -1;
  }
  return 0;
}

// Incoming packet from network parsed and ready for decode, non blocking.
int32_t VideoReceiver::IncomingPacket(const uint8_t* incomingPayload,
                                      size_t payloadLength,
                                      const RTPHeader& rtp_header,
                                      const RTPVideoHeader& video_header) {
  RTC_DCHECK_RUN_ON(&module_thread_checker_);
  if (video_header.frame_type == VideoFrameType::kVideoFrameKey) {
    TRACE_EVENT1("webrtc", "VCM::PacketKeyFrame", "seqnum",
                 rtp_header.sequenceNumber);
  }
  if (incomingPayload == nullptr) {
    // The jitter buffer doesn't handle non-zero payload lengths for packets
    // without payload.
    // TODO(holmer): We should fix this in the jitter buffer.
    payloadLength = 0;
  }
  // Callers don't provide any ntp time.
  const VCMPacket packet(incomingPayload, payloadLength, rtp_header,
                         video_header, /*ntp_time_ms=*/0,
                         clock_->CurrentTime());
  int32_t ret = _receiver.InsertPacket(packet);

  // TODO(holmer): Investigate if this somehow should use the key frame
  // request scheduling to throttle the requests.
  if (ret == VCM_FLUSH_INDICATOR) {
    {
      MutexLock lock(&process_mutex_);
      drop_frames_until_keyframe_ = true;
    }
    RequestKeyFrame();
  } else if (ret < 0) {
    return ret;
  }
  return VCM_OK;
}

void VideoReceiver::SetNackSettings(size_t max_nack_list_size,
                                    int max_packet_age_to_nack,
                                    int max_incomplete_time_ms) {
  RTC_DCHECK_RUN_ON(&construction_thread_checker_);
  if (max_nack_list_size != 0) {
    max_nack_list_size_ = max_nack_list_size;
  }
  _receiver.SetNackSettings(max_nack_list_size, max_packet_age_to_nack,
                            max_incomplete_time_ms);
}

}  // namespace vcm
}  // namespace webrtc
