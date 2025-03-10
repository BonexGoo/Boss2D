/*
 *  Copyright 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_SDP_OFFER_ANSWER_H_
#define PC_SDP_OFFER_ANSWER_H_

#include <stddef.h>
#include <stdint.h>
#include <functional>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__audio_options_h //original-code:"api/audio_options.h"
#include BOSS_WEBRTC_U_api__candidate_h //original-code:"api/candidate.h"
#include BOSS_WEBRTC_U_api__jsep_h //original-code:"api/jsep.h"
#include BOSS_WEBRTC_U_api__jsep_ice_candidate_h //original-code:"api/jsep_ice_candidate.h"
#include BOSS_WEBRTC_U_api__media_stream_interface_h //original-code:"api/media_stream_interface.h"
#include BOSS_WEBRTC_U_api__media_types_h //original-code:"api/media_types.h"
#include BOSS_WEBRTC_U_api__peer_connection_interface_h //original-code:"api/peer_connection_interface.h"
#include BOSS_WEBRTC_U_api__rtc_error_h //original-code:"api/rtc_error.h"
#include BOSS_WEBRTC_U_api__rtp_transceiver_direction_h //original-code:"api/rtp_transceiver_direction.h"
#include BOSS_WEBRTC_U_api__rtp_transceiver_interface_h //original-code:"api/rtp_transceiver_interface.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__set_local_description_observer_interface_h //original-code:"api/set_local_description_observer_interface.h"
#include BOSS_WEBRTC_U_api__set_remote_description_observer_interface_h //original-code:"api/set_remote_description_observer_interface.h"
#include BOSS_WEBRTC_U_api__transport__data_channel_transport_interface_h //original-code:"api/transport/data_channel_transport_interface.h"
#include BOSS_WEBRTC_U_api__turn_customizer_h //original-code:"api/turn_customizer.h"
#include BOSS_WEBRTC_U_api__uma_metrics_h //original-code:"api/uma_metrics.h"
#include BOSS_WEBRTC_U_api__video__video_bitrate_allocator_factory_h //original-code:"api/video/video_bitrate_allocator_factory.h"
#include BOSS_WEBRTC_U_media__base__media_channel_h //original-code:"media/base/media_channel.h"
#include BOSS_WEBRTC_U_media__base__stream_params_h //original-code:"media/base/stream_params.h"
#include BOSS_WEBRTC_U_p2p__base__port_allocator_h //original-code:"p2p/base/port_allocator.h"
#include BOSS_WEBRTC_U_pc__channel_h //original-code:"pc/channel.h"
#include BOSS_WEBRTC_U_pc__channel_interface_h //original-code:"pc/channel_interface.h"
#include BOSS_WEBRTC_U_pc__channel_manager_h //original-code:"pc/channel_manager.h"
#include BOSS_WEBRTC_U_pc__data_channel_controller_h //original-code:"pc/data_channel_controller.h"
#include BOSS_WEBRTC_U_pc__ice_server_parsing_h //original-code:"pc/ice_server_parsing.h"
#include BOSS_WEBRTC_U_pc__jsep_transport_controller_h //original-code:"pc/jsep_transport_controller.h"
#include BOSS_WEBRTC_U_pc__media_session_h //original-code:"pc/media_session.h"
#include BOSS_WEBRTC_U_pc__media_stream_observer_h //original-code:"pc/media_stream_observer.h"
#include BOSS_WEBRTC_U_pc__peer_connection_factory_h //original-code:"pc/peer_connection_factory.h"
#include BOSS_WEBRTC_U_pc__peer_connection_internal_h //original-code:"pc/peer_connection_internal.h"
#include BOSS_WEBRTC_U_pc__rtc_stats_collector_h //original-code:"pc/rtc_stats_collector.h"
#include BOSS_WEBRTC_U_pc__rtp_receiver_h //original-code:"pc/rtp_receiver.h"
#include BOSS_WEBRTC_U_pc__rtp_sender_h //original-code:"pc/rtp_sender.h"
#include BOSS_WEBRTC_U_pc__rtp_transceiver_h //original-code:"pc/rtp_transceiver.h"
#include BOSS_WEBRTC_U_pc__rtp_transmission_manager_h //original-code:"pc/rtp_transmission_manager.h"
#include BOSS_WEBRTC_U_pc__sctp_transport_h //original-code:"pc/sctp_transport.h"
#include BOSS_WEBRTC_U_pc__sdp_state_provider_h //original-code:"pc/sdp_state_provider.h"
#include BOSS_WEBRTC_U_pc__session_description_h //original-code:"pc/session_description.h"
#include BOSS_WEBRTC_U_pc__stats_collector_h //original-code:"pc/stats_collector.h"
#include BOSS_WEBRTC_U_pc__stream_collection_h //original-code:"pc/stream_collection.h"
#include BOSS_WEBRTC_U_pc__transceiver_list_h //original-code:"pc/transceiver_list.h"
#include BOSS_WEBRTC_U_pc__webrtc_session_description_factory_h //original-code:"pc/webrtc_session_description_factory.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__experiments__field_trial_parser_h //original-code:"rtc_base/experiments/field_trial_parser.h"
#include BOSS_WEBRTC_U_rtc_base__operations_chain_h //original-code:"rtc_base/operations_chain.h"
#include BOSS_WEBRTC_U_rtc_base__race_checker_h //original-code:"rtc_base/race_checker.h"
#include BOSS_WEBRTC_U_rtc_base__rtc_certificate_h //original-code:"rtc_base/rtc_certificate.h"
#include BOSS_WEBRTC_U_rtc_base__ssl_stream_adapter_h //original-code:"rtc_base/ssl_stream_adapter.h"
#include BOSS_WEBRTC_U_rtc_base__third_party__sigslot__sigslot_h //original-code:"rtc_base/third_party/sigslot/sigslot.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__thread_annotations_h //original-code:"rtc_base/thread_annotations.h"
#include BOSS_WEBRTC_U_rtc_base__unique_id_generator_h //original-code:"rtc_base/unique_id_generator.h"
#include BOSS_WEBRTC_U_rtc_base__weak_ptr_h //original-code:"rtc_base/weak_ptr.h"

namespace webrtc {

// SdpOfferAnswerHandler is a component
// of the PeerConnection object as defined
// by the PeerConnectionInterface API surface.
// The class is responsible for the following:
// - Parsing and interpreting SDP.
// - Generating offers and answers based on the current state.
// This class lives on the signaling thread.
class SdpOfferAnswerHandler : public SdpStateProvider,
                              public sigslot::has_slots<> {
 public:
  ~SdpOfferAnswerHandler();

  // Creates an SdpOfferAnswerHandler. Modifies dependencies.
  static std::unique_ptr<SdpOfferAnswerHandler> Create(
      PeerConnection* pc,
      const PeerConnectionInterface::RTCConfiguration& configuration,
      PeerConnectionDependencies& dependencies);

  void ResetSessionDescFactory() {
    RTC_DCHECK_RUN_ON(signaling_thread());
    webrtc_session_desc_factory_.reset();
  }
  const WebRtcSessionDescriptionFactory* webrtc_session_desc_factory() const {
    RTC_DCHECK_RUN_ON(signaling_thread());
    return webrtc_session_desc_factory_.get();
  }

  // Change signaling state to Closed, and perform appropriate actions.
  void Close();

  // Called as part of destroying the owning PeerConnection.
  void PrepareForShutdown();

  // Implementation of SdpStateProvider
  PeerConnectionInterface::SignalingState signaling_state() const override;

  const SessionDescriptionInterface* local_description() const override;
  const SessionDescriptionInterface* remote_description() const override;
  const SessionDescriptionInterface* current_local_description() const override;
  const SessionDescriptionInterface* current_remote_description()
      const override;
  const SessionDescriptionInterface* pending_local_description() const override;
  const SessionDescriptionInterface* pending_remote_description()
      const override;

  bool NeedsIceRestart(const std::string& content_name) const override;
  bool IceRestartPending(const std::string& content_name) const override;
  absl::optional<rtc::SSLRole> GetDtlsRole(
      const std::string& mid) const override;

  void RestartIce();

  // JSEP01
  void CreateOffer(
      CreateSessionDescriptionObserver* observer,
      const PeerConnectionInterface::RTCOfferAnswerOptions& options);
  void CreateAnswer(
      CreateSessionDescriptionObserver* observer,
      const PeerConnectionInterface::RTCOfferAnswerOptions& options);

  void SetLocalDescription(
      std::unique_ptr<SessionDescriptionInterface> desc,
      rtc::scoped_refptr<SetLocalDescriptionObserverInterface> observer);
  void SetLocalDescription(
      rtc::scoped_refptr<SetLocalDescriptionObserverInterface> observer);
  void SetLocalDescription(SetSessionDescriptionObserver* observer,
                           SessionDescriptionInterface* desc);
  void SetLocalDescription(SetSessionDescriptionObserver* observer);

  void SetRemoteDescription(
      std::unique_ptr<SessionDescriptionInterface> desc,
      rtc::scoped_refptr<SetRemoteDescriptionObserverInterface> observer);
  void SetRemoteDescription(SetSessionDescriptionObserver* observer,
                            SessionDescriptionInterface* desc);

  PeerConnectionInterface::RTCConfiguration GetConfiguration();
  RTCError SetConfiguration(
      const PeerConnectionInterface::RTCConfiguration& configuration);
  bool AddIceCandidate(const IceCandidateInterface* candidate);
  void AddIceCandidate(std::unique_ptr<IceCandidateInterface> candidate,
                       std::function<void(RTCError)> callback);
  bool RemoveIceCandidates(const std::vector<cricket::Candidate>& candidates);
  // Adds a locally generated candidate to the local description.
  void AddLocalIceCandidate(const JsepIceCandidate* candidate);
  void RemoveLocalIceCandidates(
      const std::vector<cricket::Candidate>& candidates);
  bool ShouldFireNegotiationNeededEvent(uint32_t event_id);

  bool AddStream(MediaStreamInterface* local_stream);
  void RemoveStream(MediaStreamInterface* local_stream);

  absl::optional<bool> is_caller();
  bool HasNewIceCredentials();
  void UpdateNegotiationNeeded();

  // Destroys all BaseChannels and destroys the SCTP data channel, if present.
  void DestroyAllChannels();

  rtc::scoped_refptr<StreamCollectionInterface> local_streams();
  rtc::scoped_refptr<StreamCollectionInterface> remote_streams();

 private:
  class ImplicitCreateSessionDescriptionObserver;

  friend class ImplicitCreateSessionDescriptionObserver;
  class SetSessionDescriptionObserverAdapter;

  friend class SetSessionDescriptionObserverAdapter;

  enum class SessionError {
    kNone,       // No error.
    kContent,    // Error in BaseChannel SetLocalContent/SetRemoteContent.
    kTransport,  // Error from the underlying transport.
  };

  // Represents the [[LocalIceCredentialsToReplace]] internal slot in the spec.
  // It makes the next CreateOffer() produce new ICE credentials even if
  // RTCOfferAnswerOptions::ice_restart is false.
  // https://w3c.github.io/webrtc-pc/#dfn-localufragstoreplace
  // TODO(hbos): When JsepTransportController/JsepTransport supports rollback,
  // move this type of logic to JsepTransportController/JsepTransport.
  class LocalIceCredentialsToReplace;

  // Only called by the Create() function.
  explicit SdpOfferAnswerHandler(PeerConnection* pc);
  // Called from the `Create()` function. Can only be called
  // once. Modifies dependencies.
  void Initialize(
      const PeerConnectionInterface::RTCConfiguration& configuration,
      PeerConnectionDependencies& dependencies);

  rtc::Thread* signaling_thread() const;
  // Non-const versions of local_description()/remote_description(), for use
  // internally.
  SessionDescriptionInterface* mutable_local_description()
      RTC_RUN_ON(signaling_thread()) {
    return pending_local_description_ ? pending_local_description_.get()
                                      : current_local_description_.get();
  }
  SessionDescriptionInterface* mutable_remote_description()
      RTC_RUN_ON(signaling_thread()) {
    return pending_remote_description_ ? pending_remote_description_.get()
                                       : current_remote_description_.get();
  }

  // Synchronous implementations of SetLocalDescription/SetRemoteDescription
  // that return an RTCError instead of invoking a callback.
  RTCError ApplyLocalDescription(
      std::unique_ptr<SessionDescriptionInterface> desc,
      const std::map<std::string, const cricket::ContentGroup*>&
          bundle_groups_by_mid);
  RTCError ApplyRemoteDescription(
      std::unique_ptr<SessionDescriptionInterface> desc,
      const std::map<std::string, const cricket::ContentGroup*>&
          bundle_groups_by_mid);

  // Implementation of the offer/answer exchange operations. These are chained
  // onto the |operations_chain_| when the public CreateOffer(), CreateAnswer(),
  // SetLocalDescription() and SetRemoteDescription() methods are invoked.
  void DoCreateOffer(
      const PeerConnectionInterface::RTCOfferAnswerOptions& options,
      rtc::scoped_refptr<CreateSessionDescriptionObserver> observer);
  void DoCreateAnswer(
      const PeerConnectionInterface::RTCOfferAnswerOptions& options,
      rtc::scoped_refptr<CreateSessionDescriptionObserver> observer);
  void DoSetLocalDescription(
      std::unique_ptr<SessionDescriptionInterface> desc,
      rtc::scoped_refptr<SetLocalDescriptionObserverInterface> observer);
  void DoSetRemoteDescription(
      std::unique_ptr<SessionDescriptionInterface> desc,
      rtc::scoped_refptr<SetRemoteDescriptionObserverInterface> observer);

  // Update the state, signaling if necessary.
  void ChangeSignalingState(
      PeerConnectionInterface::SignalingState signaling_state);

  RTCError UpdateSessionState(
      SdpType type,
      cricket::ContentSource source,
      const cricket::SessionDescription* description,
      const std::map<std::string, const cricket::ContentGroup*>&
          bundle_groups_by_mid);

  bool IsUnifiedPlan() const RTC_RUN_ON(signaling_thread());

  // Signals from MediaStreamObserver.
  void OnAudioTrackAdded(AudioTrackInterface* track,
                         MediaStreamInterface* stream)
      RTC_RUN_ON(signaling_thread());
  void OnAudioTrackRemoved(AudioTrackInterface* track,
                           MediaStreamInterface* stream)
      RTC_RUN_ON(signaling_thread());
  void OnVideoTrackAdded(VideoTrackInterface* track,
                         MediaStreamInterface* stream)
      RTC_RUN_ON(signaling_thread());
  void OnVideoTrackRemoved(VideoTrackInterface* track,
                           MediaStreamInterface* stream)
      RTC_RUN_ON(signaling_thread());

  // | desc_type | is the type of the description that caused the rollback.
  RTCError Rollback(SdpType desc_type);
  void OnOperationsChainEmpty();

  // Runs the algorithm **set the associated remote streams** specified in
  // https://w3c.github.io/webrtc-pc/#set-associated-remote-streams.
  void SetAssociatedRemoteStreams(
      rtc::scoped_refptr<RtpReceiverInternal> receiver,
      const std::vector<std::string>& stream_ids,
      std::vector<rtc::scoped_refptr<MediaStreamInterface>>* added_streams,
      std::vector<rtc::scoped_refptr<MediaStreamInterface>>* removed_streams);

  bool CheckIfNegotiationIsNeeded();
  void GenerateNegotiationNeededEvent();
  // Helper method which verifies SDP.
  RTCError ValidateSessionDescription(
      const SessionDescriptionInterface* sdesc,
      cricket::ContentSource source,
      const std::map<std::string, const cricket::ContentGroup*>&
          bundle_groups_by_mid) RTC_RUN_ON(signaling_thread());

  // Updates the local RtpTransceivers according to the JSEP rules. Called as
  // part of setting the local/remote description.
  RTCError UpdateTransceiversAndDataChannels(
      cricket::ContentSource source,
      const SessionDescriptionInterface& new_session,
      const SessionDescriptionInterface* old_local_description,
      const SessionDescriptionInterface* old_remote_description,
      const std::map<std::string, const cricket::ContentGroup*>&
          bundle_groups_by_mid);

  // Associate the given transceiver according to the JSEP rules.
  RTCErrorOr<
      rtc::scoped_refptr<RtpTransceiverProxyWithInternal<RtpTransceiver>>>
  AssociateTransceiver(cricket::ContentSource source,
                       SdpType type,
                       size_t mline_index,
                       const cricket::ContentInfo& content,
                       const cricket::ContentInfo* old_local_content,
                       const cricket::ContentInfo* old_remote_content)
      RTC_RUN_ON(signaling_thread());

  // Returns the media section in the given session description that is
  // associated with the RtpTransceiver. Returns null if none found or this
  // RtpTransceiver is not associated. Logic varies depending on the
  // SdpSemantics specified in the configuration.
  const cricket::ContentInfo* FindMediaSectionForTransceiver(
      const RtpTransceiver* transceiver,
      const SessionDescriptionInterface* sdesc) const;

  // Either creates or destroys the transceiver's BaseChannel according to the
  // given media section.
  RTCError UpdateTransceiverChannel(
      rtc::scoped_refptr<RtpTransceiverProxyWithInternal<RtpTransceiver>>
          transceiver,
      const cricket::ContentInfo& content,
      const cricket::ContentGroup* bundle_group) RTC_RUN_ON(signaling_thread());

  // Either creates or destroys the local data channel according to the given
  // media section.
  RTCError UpdateDataChannel(cricket::ContentSource source,
                             const cricket::ContentInfo& content,
                             const cricket::ContentGroup* bundle_group)
      RTC_RUN_ON(signaling_thread());
  // Check if a call to SetLocalDescription is acceptable with a session
  // description of the given type.
  bool ExpectSetLocalDescription(SdpType type);
  // Check if a call to SetRemoteDescription is acceptable with a session
  // description of the given type.
  bool ExpectSetRemoteDescription(SdpType type);

  // The offer/answer machinery assumes the media section MID is present and
  // unique. To support legacy end points that do not supply a=mid lines, this
  // method will modify the session description to add MIDs generated according
  // to the SDP semantics.
  void FillInMissingRemoteMids(cricket::SessionDescription* remote_description);

  // Returns an RtpTransciever, if available, that can be used to receive the
  // given media type according to JSEP rules.
  rtc::scoped_refptr<RtpTransceiverProxyWithInternal<RtpTransceiver>>
  FindAvailableTransceiverToReceive(cricket::MediaType media_type) const;

  // Returns a MediaSessionOptions struct with options decided by |options|,
  // the local MediaStreams and DataChannels.
  void GetOptionsForOffer(const PeerConnectionInterface::RTCOfferAnswerOptions&
                              offer_answer_options,
                          cricket::MediaSessionOptions* session_options);
  void GetOptionsForPlanBOffer(
      const PeerConnectionInterface::RTCOfferAnswerOptions&
          offer_answer_options,
      cricket::MediaSessionOptions* session_options)
      RTC_RUN_ON(signaling_thread());
  void GetOptionsForUnifiedPlanOffer(
      const PeerConnectionInterface::RTCOfferAnswerOptions&
          offer_answer_options,
      cricket::MediaSessionOptions* session_options)
      RTC_RUN_ON(signaling_thread());

  // Returns a MediaSessionOptions struct with options decided by
  // |constraints|, the local MediaStreams and DataChannels.
  void GetOptionsForAnswer(const PeerConnectionInterface::RTCOfferAnswerOptions&
                               offer_answer_options,
                           cricket::MediaSessionOptions* session_options);
  void GetOptionsForPlanBAnswer(
      const PeerConnectionInterface::RTCOfferAnswerOptions&
          offer_answer_options,
      cricket::MediaSessionOptions* session_options)
      RTC_RUN_ON(signaling_thread());
  void GetOptionsForUnifiedPlanAnswer(
      const PeerConnectionInterface::RTCOfferAnswerOptions&
          offer_answer_options,
      cricket::MediaSessionOptions* session_options)
      RTC_RUN_ON(signaling_thread());

  const char* SessionErrorToString(SessionError error) const;
  std::string GetSessionErrorMsg();
  // Returns the last error in the session. See the enum above for details.
  SessionError session_error() const {
    RTC_DCHECK_RUN_ON(signaling_thread());
    return session_error_;
  }
  const std::string& session_error_desc() const { return session_error_desc_; }

  RTCError HandleLegacyOfferOptions(
      const PeerConnectionInterface::RTCOfferAnswerOptions& options);
  void RemoveRecvDirectionFromReceivingTransceiversOfType(
      cricket::MediaType media_type) RTC_RUN_ON(signaling_thread());
  void AddUpToOneReceivingTransceiverOfType(cricket::MediaType media_type);

  std::vector<
      rtc::scoped_refptr<RtpTransceiverProxyWithInternal<RtpTransceiver>>>
  GetReceivingTransceiversOfType(cricket::MediaType media_type)
      RTC_RUN_ON(signaling_thread());

  // Runs the algorithm specified in
  // https://w3c.github.io/webrtc-pc/#process-remote-track-removal
  // This method will update the following lists:
  // |remove_list| is the list of transceivers for which the receiving track is
  //     being removed.
  // |removed_streams| is the list of streams which no longer have a receiving
  //     track so should be removed.
  void ProcessRemovalOfRemoteTrack(
      const rtc::scoped_refptr<RtpTransceiverProxyWithInternal<RtpTransceiver>>
          transceiver,
      std::vector<rtc::scoped_refptr<RtpTransceiverInterface>>* remove_list,
      std::vector<rtc::scoped_refptr<MediaStreamInterface>>* removed_streams);

  void RemoveRemoteStreamsIfEmpty(
      const std::vector<rtc::scoped_refptr<MediaStreamInterface>>&
          remote_streams,
      std::vector<rtc::scoped_refptr<MediaStreamInterface>>* removed_streams);

  // Remove all local and remote senders of type |media_type|.
  // Called when a media type is rejected (m-line set to port 0).
  void RemoveSenders(cricket::MediaType media_type);

  // Loops through the vector of |streams| and finds added and removed
  // StreamParams since last time this method was called.
  // For each new or removed StreamParam, OnLocalSenderSeen or
  // OnLocalSenderRemoved is invoked.
  void UpdateLocalSenders(const std::vector<cricket::StreamParams>& streams,
                          cricket::MediaType media_type);

  // Makes sure a MediaStreamTrack is created for each StreamParam in |streams|,
  // and existing MediaStreamTracks are removed if there is no corresponding
  // StreamParam. If |default_track_needed| is true, a default MediaStreamTrack
  // is created if it doesn't exist; if false, it's removed if it exists.
  // |media_type| is the type of the |streams| and can be either audio or video.
  // If a new MediaStream is created it is added to |new_streams|.
  void UpdateRemoteSendersList(
      const std::vector<cricket::StreamParams>& streams,
      bool default_track_needed,
      cricket::MediaType media_type,
      StreamCollection* new_streams);

  // Enables media channels to allow sending of media.
  // This enables media to flow on all configured audio/video channels.
  void EnableSending();
  // Push the media parts of the local or remote session description
  // down to all of the channels.
  RTCError PushdownMediaDescription(
      SdpType type,
      cricket::ContentSource source,
      const std::map<std::string, const cricket::ContentGroup*>&
          bundle_groups_by_mid);

  RTCError PushdownTransportDescription(cricket::ContentSource source,
                                        SdpType type);
  // Helper function to remove stopped transceivers.
  void RemoveStoppedTransceivers();
  // Deletes the corresponding channel of contents that don't exist in |desc|.
  // |desc| can be null. This means that all channels are deleted.
  void RemoveUnusedChannels(const cricket::SessionDescription* desc);

  // Report inferred negotiated SDP semantics from a local/remote answer to the
  // UMA observer.
  void ReportNegotiatedSdpSemantics(const SessionDescriptionInterface& answer);

  // Finds remote MediaStreams without any tracks and removes them from
  // |remote_streams_| and notifies the observer that the MediaStreams no longer
  // exist.
  void UpdateEndedRemoteMediaStreams();

  // Uses all remote candidates in |remote_desc| in this session.
  bool UseCandidatesInSessionDescription(
      const SessionDescriptionInterface* remote_desc);
  // Uses |candidate| in this session.
  bool UseCandidate(const IceCandidateInterface* candidate);
  // Returns true if we are ready to push down the remote candidate.
  // |remote_desc| is the new remote description, or NULL if the current remote
  // description should be used. Output |valid| is true if the candidate media
  // index is valid.
  bool ReadyToUseRemoteCandidate(const IceCandidateInterface* candidate,
                                 const SessionDescriptionInterface* remote_desc,
                                 bool* valid);

  RTCErrorOr<const cricket::ContentInfo*> FindContentInfo(
      const SessionDescriptionInterface* description,
      const IceCandidateInterface* candidate) RTC_RUN_ON(signaling_thread());

  // Functions for dealing with transports.
  // Note that cricket code uses the term "channel" for what other code
  // refers to as "transport".

  // Allocates media channels based on the |desc|. If |desc| doesn't have
  // the BUNDLE option, this method will disable BUNDLE in PortAllocator.
  // This method will also delete any existing media channels before creating.
  RTCError CreateChannels(const cricket::SessionDescription& desc);

  // Helper methods to create media channels.
  cricket::VoiceChannel* CreateVoiceChannel(const std::string& mid);
  cricket::VideoChannel* CreateVideoChannel(const std::string& mid);
  bool CreateDataChannel(const std::string& mid);

  // Destroys and clears the BaseChannel associated with the given transceiver,
  // if such channel is set.
  void DestroyTransceiverChannel(
      rtc::scoped_refptr<RtpTransceiverProxyWithInternal<RtpTransceiver>>
          transceiver);

  // Destroys the RTP data channel transport and/or the SCTP data channel
  // transport and clears it.
  void DestroyDataChannelTransport();

  // Destroys the given ChannelInterface.
  // The channel cannot be accessed after this method is called.
  void DestroyChannelInterface(cricket::ChannelInterface* channel);
  // Generates MediaDescriptionOptions for the |session_opts| based on existing
  // local description or remote description.

  void GenerateMediaDescriptionOptions(
      const SessionDescriptionInterface* session_desc,
      RtpTransceiverDirection audio_direction,
      RtpTransceiverDirection video_direction,
      absl::optional<size_t>* audio_index,
      absl::optional<size_t>* video_index,
      absl::optional<size_t>* data_index,
      cricket::MediaSessionOptions* session_options);

  // Generates the active MediaDescriptionOptions for the local data channel
  // given the specified MID.
  cricket::MediaDescriptionOptions GetMediaDescriptionOptionsForActiveData(
      const std::string& mid) const;

  // Generates the rejected MediaDescriptionOptions for the local data channel
  // given the specified MID.
  cricket::MediaDescriptionOptions GetMediaDescriptionOptionsForRejectedData(
      const std::string& mid) const;

  // Based on number of transceivers per media type, enabled or disable
  // payload type based demuxing in the affected channels.
  bool UpdatePayloadTypeDemuxingState(
      cricket::ContentSource source,
      const std::map<std::string, const cricket::ContentGroup*>&
          bundle_groups_by_mid);

  // ==================================================================
  // Access to pc_ variables
  cricket::ChannelManager* channel_manager() const;
  TransceiverList* transceivers();
  const TransceiverList* transceivers() const;
  DataChannelController* data_channel_controller();
  const DataChannelController* data_channel_controller() const;
  cricket::PortAllocator* port_allocator();
  const cricket::PortAllocator* port_allocator() const;
  RtpTransmissionManager* rtp_manager();
  const RtpTransmissionManager* rtp_manager() const;
  JsepTransportController* transport_controller();
  const JsepTransportController* transport_controller() const;
  // ===================================================================
  const cricket::AudioOptions& audio_options() { return audio_options_; }
  const cricket::VideoOptions& video_options() { return video_options_; }

  PeerConnection* const pc_;

  std::unique_ptr<WebRtcSessionDescriptionFactory> webrtc_session_desc_factory_
      RTC_GUARDED_BY(signaling_thread());

  std::unique_ptr<SessionDescriptionInterface> current_local_description_
      RTC_GUARDED_BY(signaling_thread());
  std::unique_ptr<SessionDescriptionInterface> pending_local_description_
      RTC_GUARDED_BY(signaling_thread());
  std::unique_ptr<SessionDescriptionInterface> current_remote_description_
      RTC_GUARDED_BY(signaling_thread());
  std::unique_ptr<SessionDescriptionInterface> pending_remote_description_
      RTC_GUARDED_BY(signaling_thread());

  PeerConnectionInterface::SignalingState signaling_state_
      RTC_GUARDED_BY(signaling_thread()) = PeerConnectionInterface::kStable;

  // Whether this peer is the caller. Set when the local description is applied.
  absl::optional<bool> is_caller_ RTC_GUARDED_BY(signaling_thread());

  // Streams added via AddStream.
  const rtc::scoped_refptr<StreamCollection> local_streams_
      RTC_GUARDED_BY(signaling_thread());
  // Streams created as a result of SetRemoteDescription.
  const rtc::scoped_refptr<StreamCollection> remote_streams_
      RTC_GUARDED_BY(signaling_thread());

  std::vector<std::unique_ptr<MediaStreamObserver>> stream_observers_
      RTC_GUARDED_BY(signaling_thread());

  // The operations chain is used by the offer/answer exchange methods to ensure
  // they are executed in the right order. For example, if
  // SetRemoteDescription() is invoked while CreateOffer() is still pending, the
  // SRD operation will not start until CreateOffer() has completed. See
  // https://w3c.github.io/webrtc-pc/#dfn-operations-chain.
  rtc::scoped_refptr<rtc::OperationsChain> operations_chain_
      RTC_GUARDED_BY(signaling_thread());

  // One PeerConnection has only one RTCP CNAME.
  // https://tools.ietf.org/html/draft-ietf-rtcweb-rtp-usage-26#section-4.9
  const std::string rtcp_cname_;

  // MIDs will be generated using this generator which will keep track of
  // all the MIDs that have been seen over the life of the PeerConnection.
  rtc::UniqueStringGenerator mid_generator_ RTC_GUARDED_BY(signaling_thread());

  // List of content names for which the remote side triggered an ICE restart.
  std::set<std::string> pending_ice_restarts_
      RTC_GUARDED_BY(signaling_thread());

  std::unique_ptr<LocalIceCredentialsToReplace>
      local_ice_credentials_to_replace_ RTC_GUARDED_BY(signaling_thread());

  bool remote_peer_supports_msid_ RTC_GUARDED_BY(signaling_thread()) = false;
  bool is_negotiation_needed_ RTC_GUARDED_BY(signaling_thread()) = false;
  uint32_t negotiation_needed_event_id_ = 0;
  bool update_negotiation_needed_on_empty_chain_
      RTC_GUARDED_BY(signaling_thread()) = false;
  // If PT demuxing is successfully negotiated one time we will allow PT
  // demuxing for the rest of the session so that PT-based apps default to PT
  // demuxing in follow-up O/A exchanges.
  bool pt_demuxing_has_been_used_audio_ = false;
  bool pt_demuxing_has_been_used_video_ = false;

  // In Unified Plan, if we encounter remote SDP that does not contain an a=msid
  // line we create and use a stream with a random ID for our receivers. This is
  // to support legacy endpoints that do not support the a=msid attribute (as
  // opposed to streamless tracks with "a=msid:-").
  rtc::scoped_refptr<MediaStreamInterface> missing_msid_default_stream_
      RTC_GUARDED_BY(signaling_thread());

  // Updates the error state, signaling if necessary.
  void SetSessionError(SessionError error, const std::string& error_desc);

  // Implements AddIceCandidate without reporting usage, but returns the
  // particular success/error value that should be reported (and can be utilized
  // for other purposes).
  AddIceCandidateResult AddIceCandidateInternal(
      const IceCandidateInterface* candidate);

  SessionError session_error_ RTC_GUARDED_BY(signaling_thread()) =
      SessionError::kNone;
  std::string session_error_desc_ RTC_GUARDED_BY(signaling_thread());

  // Member variables for caching global options.
  cricket::AudioOptions audio_options_ RTC_GUARDED_BY(signaling_thread());
  cricket::VideoOptions video_options_ RTC_GUARDED_BY(signaling_thread());

  // This object should be used to generate any SSRC that is not explicitly
  // specified by the user (or by the remote party).
  // The generator is not used directly, instead it is passed on to the
  // channel manager and the session description factory.
  // TODO(bugs.webrtc.org/12666): This variable is used from both the signaling
  // and worker threads. See if we can't restrict usage to a single thread.
  rtc::UniqueRandomIdGenerator ssrc_generator_;

  // A video bitrate allocator factory.
  // This can be injected using the PeerConnectionDependencies,
  // or else the CreateBuiltinVideoBitrateAllocatorFactory() will be called.
  // Note that one can still choose to override this in a MediaEngine
  // if one wants too.
  std::unique_ptr<webrtc::VideoBitrateAllocatorFactory>
      video_bitrate_allocator_factory_;

  rtc::WeakPtrFactory<SdpOfferAnswerHandler> weak_ptr_factory_
      RTC_GUARDED_BY(signaling_thread());
};

}  // namespace webrtc

#endif  // PC_SDP_OFFER_ANSWER_H_
