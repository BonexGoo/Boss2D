/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_RTP_TRANSCEIVER_H_
#define PC_RTP_TRANSCEIVER_H_

#include <stddef.h>

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__array_view_h //original-code:"api/array_view.h"
#include BOSS_WEBRTC_U_api__media_types_h //original-code:"api/media_types.h"
#include BOSS_WEBRTC_U_api__rtc_error_h //original-code:"api/rtc_error.h"
#include BOSS_WEBRTC_U_api__rtp_parameters_h //original-code:"api/rtp_parameters.h"
#include BOSS_WEBRTC_U_api__rtp_transceiver_direction_h //original-code:"api/rtp_transceiver_direction.h"
#include BOSS_WEBRTC_U_api__rtp_transceiver_interface_h //original-code:"api/rtp_transceiver_interface.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_base_h //original-code:"api/task_queue/task_queue_base.h"
#include BOSS_WEBRTC_U_pc__channel_interface_h //original-code:"pc/channel_interface.h"
#include BOSS_WEBRTC_U_pc__channel_manager_h //original-code:"pc/channel_manager.h"
#include BOSS_WEBRTC_U_pc__proxy_h //original-code:"pc/proxy.h"
#include BOSS_WEBRTC_U_pc__rtp_receiver_h //original-code:"pc/rtp_receiver.h"
#include BOSS_WEBRTC_U_pc__rtp_receiver_proxy_h //original-code:"pc/rtp_receiver_proxy.h"
#include BOSS_WEBRTC_U_pc__rtp_sender_h //original-code:"pc/rtp_sender.h"
#include BOSS_WEBRTC_U_pc__rtp_sender_proxy_h //original-code:"pc/rtp_sender_proxy.h"
#include BOSS_WEBRTC_U_rtc_base__ref_counted_object_h //original-code:"rtc_base/ref_counted_object.h"
#include BOSS_WEBRTC_U_rtc_base__task_utils__pending_task_safety_flag_h //original-code:"rtc_base/task_utils/pending_task_safety_flag.h"
#include BOSS_WEBRTC_U_rtc_base__third_party__sigslot__sigslot_h //original-code:"rtc_base/third_party/sigslot/sigslot.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"

namespace webrtc {

// Implementation of the public RtpTransceiverInterface.
//
// The RtpTransceiverInterface is only intended to be used with a PeerConnection
// that enables Unified Plan SDP. Thus, the methods that only need to implement
// public API features and are not used internally can assume exactly one sender
// and receiver.
//
// Since the RtpTransceiver is used internally by PeerConnection for tracking
// RtpSenders, RtpReceivers, and BaseChannels, and PeerConnection needs to be
// backwards compatible with Plan B SDP, this implementation is more flexible
// than that required by the WebRTC specification.
//
// With Plan B SDP, an RtpTransceiver can have any number of senders and
// receivers which map to a=ssrc lines in the m= section.
// With Unified Plan SDP, an RtpTransceiver will have exactly one sender and one
// receiver which are encapsulated by the m= section.
//
// This class manages the RtpSenders, RtpReceivers, and BaseChannel associated
// with this m= section. Since the transceiver, senders, and receivers are
// reference counted and can be referenced from JavaScript (in Chromium), these
// objects must be ready to live for an arbitrary amount of time. The
// BaseChannel is not reference counted and is owned by the ChannelManager, so
// the PeerConnection must take care of creating/deleting the BaseChannel and
// setting the channel reference in the transceiver to null when it has been
// deleted.
//
// The RtpTransceiver is specialized to either audio or video according to the
// MediaType specified in the constructor. Audio RtpTransceivers will have
// AudioRtpSenders, AudioRtpReceivers, and a VoiceChannel. Video RtpTransceivers
// will have VideoRtpSenders, VideoRtpReceivers, and a VideoChannel.
class RtpTransceiver final
    : public rtc::RefCountedObject<RtpTransceiverInterface>,
      public sigslot::has_slots<> {
 public:
  // Construct a Plan B-style RtpTransceiver with no senders, receivers, or
  // channel set.
  // |media_type| specifies the type of RtpTransceiver (and, by transitivity,
  // the type of senders, receivers, and channel). Can either by audio or video.
  RtpTransceiver(cricket::MediaType media_type,
                 cricket::ChannelManager* channel_manager);
  // Construct a Unified Plan-style RtpTransceiver with the given sender and
  // receiver. The media type will be derived from the media types of the sender
  // and receiver. The sender and receiver should have the same media type.
  // |HeaderExtensionsToOffer| is used for initializing the return value of
  // HeaderExtensionsToOffer().
  RtpTransceiver(
      rtc::scoped_refptr<RtpSenderProxyWithInternal<RtpSenderInternal>> sender,
      rtc::scoped_refptr<RtpReceiverProxyWithInternal<RtpReceiverInternal>>
          receiver,
      cricket::ChannelManager* channel_manager,
      std::vector<RtpHeaderExtensionCapability> HeaderExtensionsToOffer,
      std::function<void()> on_negotiation_needed);
  ~RtpTransceiver() override;

  // Returns the Voice/VideoChannel set for this transceiver. May be null if
  // the transceiver is not in the currently set local/remote description.
  cricket::ChannelInterface* channel() const { return channel_; }

  // Sets the Voice/VideoChannel. The caller must pass in the correct channel
  // implementation based on the type of the transceiver.
  void SetChannel(cricket::ChannelInterface* channel);

  // Adds an RtpSender of the appropriate type to be owned by this transceiver.
  // Must not be null.
  void AddSender(
      rtc::scoped_refptr<RtpSenderProxyWithInternal<RtpSenderInternal>> sender);

  // Removes the given RtpSender. Returns false if the sender is not owned by
  // this transceiver.
  bool RemoveSender(RtpSenderInterface* sender);

  // Returns a vector of the senders owned by this transceiver.
  std::vector<rtc::scoped_refptr<RtpSenderProxyWithInternal<RtpSenderInternal>>>
  senders() const {
    return senders_;
  }

  // Adds an RtpReceiver of the appropriate type to be owned by this
  // transceiver. Must not be null.
  void AddReceiver(
      rtc::scoped_refptr<RtpReceiverProxyWithInternal<RtpReceiverInternal>>
          receiver);

  // Removes the given RtpReceiver. Returns false if the sender is not owned by
  // this transceiver.
  bool RemoveReceiver(RtpReceiverInterface* receiver);

  // Returns a vector of the receivers owned by this transceiver.
  std::vector<
      rtc::scoped_refptr<RtpReceiverProxyWithInternal<RtpReceiverInternal>>>
  receivers() const {
    return receivers_;
  }

  // Returns the backing object for the transceiver's Unified Plan sender.
  rtc::scoped_refptr<RtpSenderInternal> sender_internal() const;

  // Returns the backing object for the transceiver's Unified Plan receiver.
  rtc::scoped_refptr<RtpReceiverInternal> receiver_internal() const;

  // RtpTransceivers are not associated until they have a corresponding media
  // section set in SetLocalDescription or SetRemoteDescription. Therefore,
  // when setting a local offer we need a way to remember which transceiver was
  // used to create which media section in the offer. Storing the mline index
  // in CreateOffer is specified in JSEP to allow us to do that.
  absl::optional<size_t> mline_index() const { return mline_index_; }
  void set_mline_index(absl::optional<size_t> mline_index) {
    mline_index_ = mline_index;
  }

  // Sets the MID for this transceiver. If the MID is not null, then the
  // transceiver is considered "associated" with the media section that has the
  // same MID.
  void set_mid(const absl::optional<std::string>& mid) { mid_ = mid; }

  // Sets the intended direction for this transceiver. Intended to be used
  // internally over SetDirection since this does not trigger a negotiation
  // needed callback.
  void set_direction(RtpTransceiverDirection direction) {
    direction_ = direction;
  }

  // Sets the current direction for this transceiver as negotiated in an offer/
  // answer exchange. The current direction is null before an answer with this
  // transceiver has been set.
  void set_current_direction(RtpTransceiverDirection direction);

  // Sets the fired direction for this transceiver. The fired direction is null
  // until SetRemoteDescription is called or an answer is set (either local or
  // remote).
  void set_fired_direction(RtpTransceiverDirection direction);

  // According to JSEP rules for SetRemoteDescription, RtpTransceivers can be
  // reused only if they were added by AddTrack.
  void set_created_by_addtrack(bool created_by_addtrack) {
    created_by_addtrack_ = created_by_addtrack;
  }
  // If AddTrack has been called then transceiver can't be removed during
  // rollback.
  void set_reused_for_addtrack(bool reused_for_addtrack) {
    reused_for_addtrack_ = reused_for_addtrack;
  }

  bool created_by_addtrack() const { return created_by_addtrack_; }

  bool reused_for_addtrack() const { return reused_for_addtrack_; }

  // Returns true if this transceiver has ever had the current direction set to
  // sendonly or sendrecv.
  bool has_ever_been_used_to_send() const {
    return has_ever_been_used_to_send_;
  }

  // Informs the transceiver that its owning
  // PeerConnection is closed.
  void SetPeerConnectionClosed();

  // Executes the "stop the RTCRtpTransceiver" procedure from
  // the webrtc-pc specification, described under the stop() method.
  void StopTransceiverProcedure();

  // Fired when the RtpTransceiver state changes such that negotiation is now
  // needed (e.g., in response to a direction change).
  //  sigslot::signal0<> SignalNegotiationNeeded;

  // RtpTransceiverInterface implementation.
  cricket::MediaType media_type() const override;
  absl::optional<std::string> mid() const override;
  rtc::scoped_refptr<RtpSenderInterface> sender() const override;
  rtc::scoped_refptr<RtpReceiverInterface> receiver() const override;
  bool stopped() const override;
  bool stopping() const override;
  RtpTransceiverDirection direction() const override;
  RTCError SetDirectionWithError(
      RtpTransceiverDirection new_direction) override;
  absl::optional<RtpTransceiverDirection> current_direction() const override;
  absl::optional<RtpTransceiverDirection> fired_direction() const override;
  RTCError StopStandard() override;
  void StopInternal() override;
  RTCError SetCodecPreferences(
      rtc::ArrayView<RtpCodecCapability> codecs) override;
  std::vector<RtpCodecCapability> codec_preferences() const override {
    return codec_preferences_;
  }
  std::vector<RtpHeaderExtensionCapability> HeaderExtensionsToOffer()
      const override;
  std::vector<RtpHeaderExtensionCapability> HeaderExtensionsNegotiated()
      const override;
  RTCError SetOfferedRtpHeaderExtensions(
      rtc::ArrayView<const RtpHeaderExtensionCapability>
          header_extensions_to_offer) override;

  // Called on the signaling thread when the local or remote content description
  // is updated. Used to update the negotiated header extensions.
  // TODO(tommi): The implementation of this method is currently very simple and
  // only used for updating the negotiated headers. However, we're planning to
  // move all the updates done on the channel from the transceiver into this
  // method. This will happen with the ownership of the channel object being
  // moved into the transceiver.
  void OnNegotiationUpdate(SdpType sdp_type,
                           const cricket::MediaContentDescription* content);

 private:
  void OnFirstPacketReceived();
  void StopSendingAndReceiving();

  // Enforce that this object is created, used and destroyed on one thread.
  TaskQueueBase* const thread_;
  const bool unified_plan_;
  const cricket::MediaType media_type_;
  rtc::scoped_refptr<PendingTaskSafetyFlag> signaling_thread_safety_;
  std::vector<rtc::scoped_refptr<RtpSenderProxyWithInternal<RtpSenderInternal>>>
      senders_;
  std::vector<
      rtc::scoped_refptr<RtpReceiverProxyWithInternal<RtpReceiverInternal>>>
      receivers_;

  bool stopped_ RTC_GUARDED_BY(thread_) = false;
  bool stopping_ RTC_GUARDED_BY(thread_) = false;
  bool is_pc_closed_ = false;
  RtpTransceiverDirection direction_ = RtpTransceiverDirection::kInactive;
  absl::optional<RtpTransceiverDirection> current_direction_;
  absl::optional<RtpTransceiverDirection> fired_direction_;
  absl::optional<std::string> mid_;
  absl::optional<size_t> mline_index_;
  bool created_by_addtrack_ = false;
  bool reused_for_addtrack_ = false;
  bool has_ever_been_used_to_send_ = false;

  cricket::ChannelInterface* channel_ = nullptr;
  cricket::ChannelManager* channel_manager_ = nullptr;
  std::vector<RtpCodecCapability> codec_preferences_;
  std::vector<RtpHeaderExtensionCapability> header_extensions_to_offer_;

  // |negotiated_header_extensions_| is read and written to on the signaling
  // thread from the SdpOfferAnswerHandler class (e.g.
  // PushdownMediaDescription().
  cricket::RtpHeaderExtensions negotiated_header_extensions_
      RTC_GUARDED_BY(thread_);

  const std::function<void()> on_negotiation_needed_;
};

BEGIN_PRIMARY_PROXY_MAP(RtpTransceiver)

PROXY_PRIMARY_THREAD_DESTRUCTOR()
BYPASS_PROXY_CONSTMETHOD0(cricket::MediaType, media_type)
PROXY_CONSTMETHOD0(absl::optional<std::string>, mid)
PROXY_CONSTMETHOD0(rtc::scoped_refptr<RtpSenderInterface>, sender)
PROXY_CONSTMETHOD0(rtc::scoped_refptr<RtpReceiverInterface>, receiver)
PROXY_CONSTMETHOD0(bool, stopped)
PROXY_CONSTMETHOD0(bool, stopping)
PROXY_CONSTMETHOD0(RtpTransceiverDirection, direction)
PROXY_METHOD1(webrtc::RTCError, SetDirectionWithError, RtpTransceiverDirection)
PROXY_CONSTMETHOD0(absl::optional<RtpTransceiverDirection>, current_direction)
PROXY_CONSTMETHOD0(absl::optional<RtpTransceiverDirection>, fired_direction)
PROXY_METHOD0(webrtc::RTCError, StopStandard)
PROXY_METHOD0(void, StopInternal)
PROXY_METHOD1(webrtc::RTCError,
              SetCodecPreferences,
              rtc::ArrayView<RtpCodecCapability>)
PROXY_CONSTMETHOD0(std::vector<RtpCodecCapability>, codec_preferences)
PROXY_CONSTMETHOD0(std::vector<RtpHeaderExtensionCapability>,
                   HeaderExtensionsToOffer)
PROXY_CONSTMETHOD0(std::vector<RtpHeaderExtensionCapability>,
                   HeaderExtensionsNegotiated)
PROXY_METHOD1(webrtc::RTCError,
              SetOfferedRtpHeaderExtensions,
              rtc::ArrayView<const RtpHeaderExtensionCapability>)
END_PROXY_MAP(RtpTransceiver)

}  // namespace webrtc

#endif  // PC_RTP_TRANSCEIVER_H_
