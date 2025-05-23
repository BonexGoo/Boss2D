/*
 *  Copyright 2021 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_RTP_SENDER_PROXY_H_
#define PC_RTP_SENDER_PROXY_H_

#include <string>
#include <vector>

#include BOSS_WEBRTC_U_api__rtp_sender_interface_h //original-code:"api/rtp_sender_interface.h"
#include BOSS_WEBRTC_U_pc__proxy_h //original-code:"pc/proxy.h"

namespace webrtc {

// Define proxy for RtpSenderInterface.
// TODO(deadbeef): Move this to .cc file. What threads methods are called on is
// an implementation detail.
BEGIN_PRIMARY_PROXY_MAP(RtpSender)
PROXY_PRIMARY_THREAD_DESTRUCTOR()
PROXY_METHOD1(bool, SetTrack, MediaStreamTrackInterface*)
PROXY_CONSTMETHOD0(rtc::scoped_refptr<MediaStreamTrackInterface>, track)
PROXY_CONSTMETHOD0(rtc::scoped_refptr<DtlsTransportInterface>, dtls_transport)
PROXY_CONSTMETHOD0(uint32_t, ssrc)
BYPASS_PROXY_CONSTMETHOD0(cricket::MediaType, media_type)
BYPASS_PROXY_CONSTMETHOD0(std::string, id)
PROXY_CONSTMETHOD0(std::vector<std::string>, stream_ids)
PROXY_CONSTMETHOD0(std::vector<RtpEncodingParameters>, init_send_encodings)
PROXY_CONSTMETHOD0(RtpParameters, GetParameters)
PROXY_METHOD1(RTCError, SetParameters, const RtpParameters&)
PROXY_CONSTMETHOD0(rtc::scoped_refptr<DtmfSenderInterface>, GetDtmfSender)
PROXY_METHOD1(void,
              SetFrameEncryptor,
              rtc::scoped_refptr<FrameEncryptorInterface>)
PROXY_CONSTMETHOD0(rtc::scoped_refptr<FrameEncryptorInterface>,
                   GetFrameEncryptor)
PROXY_METHOD1(void, SetStreams, const std::vector<std::string>&)
PROXY_METHOD1(void,
              SetEncoderToPacketizerFrameTransformer,
              rtc::scoped_refptr<FrameTransformerInterface>)
END_PROXY_MAP(RtpSender)

}  // namespace webrtc

#endif  // PC_RTP_SENDER_PROXY_H_
