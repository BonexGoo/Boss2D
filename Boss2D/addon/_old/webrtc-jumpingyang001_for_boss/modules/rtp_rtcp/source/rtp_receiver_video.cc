/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_receiver_video_h //original-code:"modules/rtp_rtcp/source/rtp_receiver_video.h"

#include <assert.h>
#include <string.h>

#include <memory>

#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_cvo_h //original-code:"modules/rtp_rtcp/include/rtp_cvo.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_payload_registry_h //original-code:"modules/rtp_rtcp/include/rtp_payload_registry.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_format_h //original-code:"modules/rtp_rtcp/source/rtp_format.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_format_video_generic_h //original-code:"modules/rtp_rtcp/source/rtp_format_video_generic.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__source__rtp_utility_h //original-code:"modules/rtp_rtcp/source/rtp_utility.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__trace_event_h //original-code:"rtc_base/trace_event.h"

namespace webrtc {

RTPReceiverStrategy* RTPReceiverStrategy::CreateVideoStrategy(
    RtpData* data_callback) {
  return new RTPReceiverVideo(data_callback);
}

RTPReceiverVideo::RTPReceiverVideo(RtpData* data_callback)
    : RTPReceiverStrategy(data_callback) {}

RTPReceiverVideo::~RTPReceiverVideo() {}

int32_t RTPReceiverVideo::ParseRtpPacket(WebRtcRTPHeader* rtp_header,
                                         const PayloadUnion& specific_payload,
                                         const uint8_t* payload,
                                         size_t payload_length,
                                         int64_t timestamp_ms) {
  rtp_header->video_header().codec =
      specific_payload.video_payload().videoCodecType;

  RTC_DCHECK_GE(payload_length, rtp_header->header.paddingLength);
  const size_t payload_data_length =
      payload_length - rtp_header->header.paddingLength;

  if (payload == NULL || payload_data_length == 0) {
    return data_callback_->OnReceivedPayloadData(NULL, 0, rtp_header) == 0 ? 0
                                                                           : -1;
  }

  if (first_packet_received_()) {
    RTC_LOG(LS_INFO) << "Received first video RTP packet";
  }

  // We are not allowed to hold a critical section when calling below functions.
  std::unique_ptr<RtpDepacketizer> depacketizer(
      RtpDepacketizer::Create(rtp_header->video_header().codec));
  if (depacketizer.get() == NULL) {
    RTC_LOG(LS_ERROR) << "Failed to create depacketizer.";
    return -1;
  }

  RtpDepacketizer::ParsedPayload parsed_payload;
  if (!depacketizer->Parse(&parsed_payload, payload, payload_data_length))
    return -1;

  rtp_header->frameType = parsed_payload.frame_type;
  rtp_header->video_header() = parsed_payload.video_header();
  rtp_header->video_header().rotation = kVideoRotation_0;
  rtp_header->video_header().content_type = VideoContentType::UNSPECIFIED;
  rtp_header->video_header().video_timing.flags = VideoSendTiming::kInvalid;

  // Retrieve the video rotation information.
  if (rtp_header->header.extension.hasVideoRotation) {
    rtp_header->video_header().rotation =
        rtp_header->header.extension.videoRotation;
  }

  if (rtp_header->header.extension.hasVideoContentType) {
    rtp_header->video_header().content_type =
        rtp_header->header.extension.videoContentType;
  }

  if (rtp_header->header.extension.has_video_timing) {
    rtp_header->video_header().video_timing =
        rtp_header->header.extension.video_timing;
  }

  rtp_header->video_header().playout_delay =
      rtp_header->header.extension.playout_delay;

  return data_callback_->OnReceivedPayloadData(parsed_payload.payload,
                                               parsed_payload.payload_length,
                                               rtp_header) == 0
             ? 0
             : -1;
}

}  // namespace webrtc
