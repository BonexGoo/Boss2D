/*
 *  Copyright 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file contains mock implementations of observers used in PeerConnection.
// TODO(steveanton): These aren't really mocks and should be renamed.

#ifndef PC_TEST_MOCKPEERCONNECTIONOBSERVERS_H_
#define PC_TEST_MOCKPEERCONNECTIONOBSERVERS_H_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include BOSS_WEBRTC_U_api__datachannelinterface_h //original-code:"api/datachannelinterface.h"
#include BOSS_WEBRTC_U_api__jsepicecandidate_h //original-code:"api/jsepicecandidate.h"
#include BOSS_WEBRTC_U_pc__streamcollection_h //original-code:"pc/streamcollection.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__ptr_util_h //original-code:"rtc_base/ptr_util.h"

namespace webrtc {

class MockPeerConnectionObserver : public PeerConnectionObserver {
 public:
  struct AddTrackEvent {
    explicit AddTrackEvent(
        rtc::scoped_refptr<RtpReceiverInterface> event_receiver,
        std::vector<rtc::scoped_refptr<MediaStreamInterface>> event_streams)
        : receiver(std::move(event_receiver)),
          streams(std::move(event_streams)) {
      for (auto stream : streams) {
        std::vector<rtc::scoped_refptr<MediaStreamTrackInterface>> tracks;
        for (auto audio_track : stream->GetAudioTracks()) {
          tracks.push_back(audio_track);
        }
        for (auto video_track : stream->GetVideoTracks()) {
          tracks.push_back(video_track);
        }
        snapshotted_stream_tracks[stream] = tracks;
      }
    }

    rtc::scoped_refptr<RtpReceiverInterface> receiver;
    std::vector<rtc::scoped_refptr<MediaStreamInterface>> streams;
    // This map records the tracks present in each stream at the time the
    // OnAddTrack callback was issued.
    std::map<rtc::scoped_refptr<MediaStreamInterface>,
             std::vector<rtc::scoped_refptr<MediaStreamTrackInterface>>>
        snapshotted_stream_tracks;
  };

  MockPeerConnectionObserver() : remote_streams_(StreamCollection::Create()) {}
  virtual ~MockPeerConnectionObserver() {}
  void SetPeerConnectionInterface(PeerConnectionInterface* pc) {
    pc_ = pc;
    if (pc) {
      state_ = pc_->signaling_state();
    }
  }
  void OnSignalingChange(
      PeerConnectionInterface::SignalingState new_state) override {
    RTC_DCHECK(pc_->signaling_state() == new_state);
    state_ = new_state;
  }

  MediaStreamInterface* RemoteStream(const std::string& label) {
    return remote_streams_->find(label);
  }
  StreamCollectionInterface* remote_streams() const { return remote_streams_; }
  void OnAddStream(rtc::scoped_refptr<MediaStreamInterface> stream) override {
    last_added_stream_ = stream;
    remote_streams_->AddStream(stream);
  }
  void OnRemoveStream(
      rtc::scoped_refptr<MediaStreamInterface> stream) override {
    last_removed_stream_ = stream;
    remote_streams_->RemoveStream(stream);
  }
  void OnRenegotiationNeeded() override { renegotiation_needed_ = true; }
  void OnDataChannel(
      rtc::scoped_refptr<DataChannelInterface> data_channel) override {
    last_datachannel_ = data_channel;
  }

  void OnIceConnectionChange(
      PeerConnectionInterface::IceConnectionState new_state) override {
    RTC_DCHECK(pc_->ice_connection_state() == new_state);
    ice_connected_ =
        (new_state == PeerConnectionInterface::kIceConnectionConnected);
    callback_triggered_ = true;
  }
  void OnIceGatheringChange(
      PeerConnectionInterface::IceGatheringState new_state) override {
    RTC_DCHECK(pc_->ice_gathering_state() == new_state);
    ice_gathering_complete_ =
        new_state == PeerConnectionInterface::kIceGatheringComplete;
    callback_triggered_ = true;
  }
  void OnIceCandidate(const IceCandidateInterface* candidate) override {
    RTC_DCHECK(PeerConnectionInterface::kIceGatheringNew !=
               pc_->ice_gathering_state());
    candidates_.push_back(rtc::MakeUnique<JsepIceCandidate>(
        candidate->sdp_mid(), candidate->sdp_mline_index(),
        candidate->candidate()));
    callback_triggered_ = true;
  }

  void OnIceCandidatesRemoved(
      const std::vector<cricket::Candidate>& candidates) override {
    num_candidates_removed_++;
    callback_triggered_ = true;
  }

  void OnIceConnectionReceivingChange(bool receiving) override {
    callback_triggered_ = true;
  }

  void OnAddTrack(rtc::scoped_refptr<RtpReceiverInterface> receiver,
                  const std::vector<rtc::scoped_refptr<MediaStreamInterface>>&
                      streams) override {
    RTC_DCHECK(receiver);
    num_added_tracks_++;
    last_added_track_label_ = receiver->id();
    add_track_events_.push_back(AddTrackEvent(receiver, streams));
  }

  void OnRemoveTrack(
      rtc::scoped_refptr<RtpReceiverInterface> receiver) override {
    remove_track_events_.push_back(receiver);
  }

  std::vector<rtc::scoped_refptr<RtpReceiverInterface>> GetAddTrackReceivers() {
    std::vector<rtc::scoped_refptr<RtpReceiverInterface>> receivers;
    for (const AddTrackEvent& event : add_track_events_) {
      receivers.push_back(event.receiver);
    }
    return receivers;
  }

  // Returns the label of the last added stream.
  // Empty string if no stream have been added.
  std::string GetLastAddedStreamLabel() {
    if (last_added_stream_.get())
      return last_added_stream_->label();
    return "";
  }
  std::string GetLastRemovedStreamLabel() {
    if (last_removed_stream_.get())
      return last_removed_stream_->label();
    return "";
  }

  IceCandidateInterface* last_candidate() {
    if (candidates_.empty()) {
      return nullptr;
    } else {
      return candidates_.back().get();
    }
  }

  std::vector<IceCandidateInterface*> GetCandidatesByMline(int mline_index) {
    std::vector<IceCandidateInterface*> candidates;
    for (const auto& candidate : candidates_) {
      if (candidate->sdp_mline_index() == mline_index) {
        candidates.push_back(candidate.get());
      }
    }
    return candidates;
  }

  bool negotiation_needed() const { return renegotiation_needed_; }
  void clear_negotiation_needed() { renegotiation_needed_ = false; }

  rtc::scoped_refptr<PeerConnectionInterface> pc_;
  PeerConnectionInterface::SignalingState state_;
  std::vector<std::unique_ptr<IceCandidateInterface>> candidates_;
  rtc::scoped_refptr<DataChannelInterface> last_datachannel_;
  rtc::scoped_refptr<StreamCollection> remote_streams_;
  bool renegotiation_needed_ = false;
  bool ice_gathering_complete_ = false;
  bool ice_connected_ = false;
  bool callback_triggered_ = false;
  int num_added_tracks_ = 0;
  std::string last_added_track_label_;
  std::vector<AddTrackEvent> add_track_events_;
  std::vector<rtc::scoped_refptr<RtpReceiverInterface>> remove_track_events_;
  int num_candidates_removed_ = 0;

 private:
  rtc::scoped_refptr<MediaStreamInterface> last_added_stream_;
  rtc::scoped_refptr<MediaStreamInterface> last_removed_stream_;
};

class MockCreateSessionDescriptionObserver
    : public webrtc::CreateSessionDescriptionObserver {
 public:
  MockCreateSessionDescriptionObserver()
      : called_(false),
        error_("MockCreateSessionDescriptionObserver not called") {}
  virtual ~MockCreateSessionDescriptionObserver() {}
  virtual void OnSuccess(SessionDescriptionInterface* desc) {
    called_ = true;
    error_ = "";
    desc_.reset(desc);
  }
  virtual void OnFailure(const std::string& error) {
    called_ = true;
    error_ = error;
  }
  bool called() const { return called_; }
  bool result() const { return error_.empty(); }
  const std::string& error() const { return error_; }
  std::unique_ptr<SessionDescriptionInterface> MoveDescription() {
    return std::move(desc_);
  }

 private:
  bool called_;
  std::string error_;
  std::unique_ptr<SessionDescriptionInterface> desc_;
};

class MockSetSessionDescriptionObserver
    : public webrtc::SetSessionDescriptionObserver {
 public:
  MockSetSessionDescriptionObserver()
      : called_(false),
        error_("MockSetSessionDescriptionObserver not called") {}
  ~MockSetSessionDescriptionObserver() override {}
  void OnSuccess() override {
    called_ = true;
    error_ = "";
  }
  void OnFailure(const std::string& error) override {
    called_ = true;
    error_ = error;
  }
  bool called() const { return called_; }
  bool result() const { return error_.empty(); }
  const std::string& error() const { return error_; }

 private:
  bool called_;
  std::string error_;
};

class MockSetRemoteDescriptionObserver
    : public rtc::RefCountedObject<SetRemoteDescriptionObserverInterface> {
 public:
  bool called() const { return error_.has_value(); }
  RTCError& error() {
    RTC_DCHECK(error_.has_value());
    return *error_;
  }

  // SetRemoteDescriptionObserverInterface implementation.
  void OnSetRemoteDescriptionComplete(RTCError error) override {
    error_ = std::move(error);
  }

 private:
  // Set on complete, on success this is set to an RTCError::OK() error.
  rtc::Optional<RTCError> error_;
};

class MockDataChannelObserver : public webrtc::DataChannelObserver {
 public:
  explicit MockDataChannelObserver(webrtc::DataChannelInterface* channel)
      : channel_(channel) {
    channel_->RegisterObserver(this);
    state_ = channel_->state();
  }
  virtual ~MockDataChannelObserver() {
    channel_->UnregisterObserver();
  }

  void OnBufferedAmountChange(uint64_t previous_amount) override {}

  void OnStateChange() override { state_ = channel_->state(); }
  void OnMessage(const DataBuffer& buffer) override {
    messages_.push_back(
        std::string(buffer.data.data<char>(), buffer.data.size()));
  }

  bool IsOpen() const { return state_ == DataChannelInterface::kOpen; }
  std::vector<std::string> messages() const { return messages_; }
  std::string last_message() const {
    return messages_.empty() ? std::string() : messages_.back();
  }
  size_t received_message_count() const { return messages_.size(); }

 private:
  rtc::scoped_refptr<webrtc::DataChannelInterface> channel_;
  DataChannelInterface::DataState state_;
  std::vector<std::string> messages_;
};

class MockStatsObserver : public webrtc::StatsObserver {
 public:
  MockStatsObserver() : called_(false), stats_() {}
  virtual ~MockStatsObserver() {}

  virtual void OnComplete(const StatsReports& reports) {
    RTC_CHECK(!called_);
    called_ = true;
    stats_.Clear();
    stats_.number_of_reports = reports.size();
    for (const auto* r : reports) {
      if (r->type() == StatsReport::kStatsReportTypeSsrc) {
        stats_.timestamp = r->timestamp();
        GetIntValue(r, StatsReport::kStatsValueNameAudioOutputLevel,
            &stats_.audio_output_level);
        GetIntValue(r, StatsReport::kStatsValueNameAudioInputLevel,
            &stats_.audio_input_level);
        GetIntValue(r, StatsReport::kStatsValueNameBytesReceived,
            &stats_.bytes_received);
        GetIntValue(r, StatsReport::kStatsValueNameBytesSent,
            &stats_.bytes_sent);
        GetInt64Value(r, StatsReport::kStatsValueNameCaptureStartNtpTimeMs,
            &stats_.capture_start_ntp_time);
      } else if (r->type() == StatsReport::kStatsReportTypeBwe) {
        stats_.timestamp = r->timestamp();
        GetIntValue(r, StatsReport::kStatsValueNameAvailableReceiveBandwidth,
            &stats_.available_receive_bandwidth);
      } else if (r->type() == StatsReport::kStatsReportTypeComponent) {
        stats_.timestamp = r->timestamp();
        GetStringValue(r, StatsReport::kStatsValueNameDtlsCipher,
            &stats_.dtls_cipher);
        GetStringValue(r, StatsReport::kStatsValueNameSrtpCipher,
            &stats_.srtp_cipher);
      }
    }
  }

  bool called() const { return called_; }
  size_t number_of_reports() const { return stats_.number_of_reports; }
  double timestamp() const { return stats_.timestamp; }

  int AudioOutputLevel() const {
    RTC_CHECK(called_);
    return stats_.audio_output_level;
  }

  int AudioInputLevel() const {
    RTC_CHECK(called_);
    return stats_.audio_input_level;
  }

  int BytesReceived() const {
    RTC_CHECK(called_);
    return stats_.bytes_received;
  }

  int BytesSent() const {
    RTC_CHECK(called_);
    return stats_.bytes_sent;
  }

  int64_t CaptureStartNtpTime() const {
    RTC_CHECK(called_);
    return stats_.capture_start_ntp_time;
  }

  int AvailableReceiveBandwidth() const {
    RTC_CHECK(called_);
    return stats_.available_receive_bandwidth;
  }

  std::string DtlsCipher() const {
    RTC_CHECK(called_);
    return stats_.dtls_cipher;
  }

  std::string SrtpCipher() const {
    RTC_CHECK(called_);
    return stats_.srtp_cipher;
  }

 private:
  bool GetIntValue(const StatsReport* report,
                   StatsReport::StatsValueName name,
                   int* value) {
    const StatsReport::Value* v = report->FindValue(name);
    if (v) {
      // TODO(tommi): We should really just be using an int here :-/
      *value = rtc::FromString<int>(v->ToString());
    }
    return v != nullptr;
  }

  bool GetInt64Value(const StatsReport* report,
                   StatsReport::StatsValueName name,
                   int64_t* value) {
    const StatsReport::Value* v = report->FindValue(name);
    if (v) {
      // TODO(tommi): We should really just be using an int here :-/
      *value = rtc::FromString<int64_t>(v->ToString());
    }
    return v != nullptr;
  }

  bool GetStringValue(const StatsReport* report,
                      StatsReport::StatsValueName name,
                      std::string* value) {
    const StatsReport::Value* v = report->FindValue(name);
    if (v)
      *value = v->ToString();
    return v != nullptr;
  }

  bool called_;
  struct {
    void Clear() {
      number_of_reports = 0;
      timestamp = 0;
      audio_output_level = 0;
      audio_input_level = 0;
      bytes_received = 0;
      bytes_sent = 0;
      capture_start_ntp_time = 0;
      available_receive_bandwidth = 0;
      dtls_cipher.clear();
      srtp_cipher.clear();
    }

    size_t number_of_reports;
    double timestamp;
    int audio_output_level;
    int audio_input_level;
    int bytes_received;
    int bytes_sent;
    int64_t capture_start_ntp_time;
    int available_receive_bandwidth;
    std::string dtls_cipher;
    std::string srtp_cipher;
  } stats_;
};

// Helper class that just stores the report from the callback.
class MockRTCStatsCollectorCallback : public webrtc::RTCStatsCollectorCallback {
 public:
  rtc::scoped_refptr<const RTCStatsReport> report() { return report_; }

  bool called() const { return called_; }

 protected:
  void OnStatsDelivered(
      const rtc::scoped_refptr<const RTCStatsReport>& report) override {
    report_ = report;
    called_ = true;
  }

 private:
  bool called_ = false;
  rtc::scoped_refptr<const RTCStatsReport> report_;
};

}  // namespace webrtc

#endif  // PC_TEST_MOCKPEERCONNECTIONOBSERVERS_H_
