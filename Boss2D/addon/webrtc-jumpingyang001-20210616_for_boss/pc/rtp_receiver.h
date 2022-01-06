/*
 *  Copyright 2015 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file contains classes that implement RtpReceiverInterface.
// An RtpReceiver associates a MediaStreamTrackInterface with an underlying
// transport (provided by cricket::VoiceChannel/cricket::VideoChannel)

#ifndef PC_RTP_RECEIVER_H_
#define PC_RTP_RECEIVER_H_

#include <stdint.h>

#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__crypto__frame_decryptor_interface_h //original-code:"api/crypto/frame_decryptor_interface.h"
#include BOSS_WEBRTC_U_api__dtls_transport_interface_h //original-code:"api/dtls_transport_interface.h"
#include BOSS_WEBRTC_U_api__media_stream_interface_h //original-code:"api/media_stream_interface.h"
#include BOSS_WEBRTC_U_api__media_types_h //original-code:"api/media_types.h"
#include BOSS_WEBRTC_U_api__rtp_parameters_h //original-code:"api/rtp_parameters.h"
#include BOSS_WEBRTC_U_api__rtp_receiver_interface_h //original-code:"api/rtp_receiver_interface.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__video__video_frame_h //original-code:"api/video/video_frame.h"
#include BOSS_WEBRTC_U_api__video__video_sink_interface_h //original-code:"api/video/video_sink_interface.h"
#include BOSS_WEBRTC_U_api__video__video_source_interface_h //original-code:"api/video/video_source_interface.h"
#include BOSS_WEBRTC_U_media__base__media_channel_h //original-code:"media/base/media_channel.h"
#include BOSS_WEBRTC_U_media__base__video_broadcaster_h //original-code:"media/base/video_broadcaster.h"
#include BOSS_WEBRTC_U_pc__video_track_source_h //original-code:"pc/video_track_source.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"

namespace webrtc {

// Internal class used by PeerConnection.
class RtpReceiverInternal : public RtpReceiverInterface {
 public:
  // Stops receiving. The track may be reactivated.
  virtual void Stop() = 0;
  // Stops the receiver permanently.
  // Causes the associated track to enter kEnded state. Cannot be reversed.
  virtual void StopAndEndTrack() = 0;

  // Sets the underlying MediaEngine channel associated with this RtpSender.
  // A VoiceMediaChannel should be used for audio RtpSenders and
  // a VideoMediaChannel should be used for video RtpSenders.
  // Must call SetMediaChannel(nullptr) before the media channel is destroyed.
  virtual void SetMediaChannel(cricket::MediaChannel* media_channel) = 0;

  // Configures the RtpReceiver with the underlying media channel, with the
  // given SSRC as the stream identifier.
  virtual void SetupMediaChannel(uint32_t ssrc) = 0;

  // Configures the RtpReceiver with the underlying media channel to receive an
  // unsignaled receive stream.
  virtual void SetupUnsignaledMediaChannel() = 0;

  virtual void set_transport(
      rtc::scoped_refptr<DtlsTransportInterface> dtls_transport) = 0;
  // This SSRC is used as an identifier for the receiver between the API layer
  // and the WebRtcVideoEngine, WebRtcVoiceEngine layer.
  virtual uint32_t ssrc() const = 0;

  // Call this to notify the RtpReceiver when the first packet has been received
  // on the corresponding channel.
  virtual void NotifyFirstPacketReceived() = 0;

  // Set the associated remote media streams for this receiver. The remote track
  // will be removed from any streams that are no longer present and added to
  // any new streams.
  virtual void set_stream_ids(std::vector<std::string> stream_ids) = 0;
  // TODO(https://crbug.com/webrtc/9480): Remove SetStreams() in favor of
  // set_stream_ids() as soon as downstream projects are no longer dependent on
  // stream objects.
  virtual void SetStreams(
      const std::vector<rtc::scoped_refptr<MediaStreamInterface>>& streams) = 0;

  // Returns an ID that changes if the attached track changes, but
  // otherwise remains constant. Used to generate IDs for stats.
  // The special value zero means that no track is attached.
  virtual int AttachmentId() const = 0;

 protected:
  static int GenerateUniqueId();

  static std::vector<rtc::scoped_refptr<MediaStreamInterface>>
  CreateStreamsFromIds(std::vector<std::string> stream_ids);
};

}  // namespace webrtc

#endif  // PC_RTP_RECEIVER_H_
