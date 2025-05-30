/*
 *  Copyright 2016 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_RTC_STATS_COLLECTOR_H_
#define PC_RTC_STATS_COLLECTOR_H_

#include <stdint.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__data_channel_interface_h //original-code:"api/data_channel_interface.h"
#include BOSS_WEBRTC_U_api__media_types_h //original-code:"api/media_types.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__stats__rtc_stats_collector_callback_h //original-code:"api/stats/rtc_stats_collector_callback.h"
#include BOSS_WEBRTC_U_api__stats__rtc_stats_report_h //original-code:"api/stats/rtc_stats_report.h"
#include BOSS_WEBRTC_U_api__stats__rtcstats_objects_h //original-code:"api/stats/rtcstats_objects.h"
#include BOSS_WEBRTC_U_call__call_h //original-code:"call/call.h"
#include BOSS_WEBRTC_U_media__base__media_channel_h //original-code:"media/base/media_channel.h"
#include BOSS_WEBRTC_U_pc__data_channel_utils_h //original-code:"pc/data_channel_utils.h"
#include BOSS_WEBRTC_U_pc__peer_connection_internal_h //original-code:"pc/peer_connection_internal.h"
#include BOSS_WEBRTC_U_pc__rtp_receiver_h //original-code:"pc/rtp_receiver.h"
#include BOSS_WEBRTC_U_pc__rtp_sender_h //original-code:"pc/rtp_sender.h"
#include BOSS_WEBRTC_U_pc__rtp_transceiver_h //original-code:"pc/rtp_transceiver.h"
#include BOSS_WEBRTC_U_pc__sctp_data_channel_h //original-code:"pc/sctp_data_channel.h"
#include BOSS_WEBRTC_U_pc__track_media_info_map_h //original-code:"pc/track_media_info_map.h"
#include BOSS_WEBRTC_U_pc__transport_stats_h //original-code:"pc/transport_stats.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"
#include BOSS_WEBRTC_U_rtc_base__ref_count_h //original-code:"rtc_base/ref_count.h"
#include BOSS_WEBRTC_U_rtc_base__ssl_certificate_h //original-code:"rtc_base/ssl_certificate.h"
#include BOSS_WEBRTC_U_rtc_base__ssl_identity_h //original-code:"rtc_base/ssl_identity.h"
#include BOSS_WEBRTC_U_rtc_base__third_party__sigslot__sigslot_h //original-code:"rtc_base/third_party/sigslot/sigslot.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__time_utils_h //original-code:"rtc_base/time_utils.h"

namespace webrtc {

class RtpSenderInternal;
class RtpReceiverInternal;

// All public methods of the collector are to be called on the signaling thread.
// Stats are gathered on the signaling, worker and network threads
// asynchronously. The callback is invoked on the signaling thread. Resulting
// reports are cached for |cache_lifetime_| ms.
class RTCStatsCollector : public rtc::RefCountInterface,
                          public sigslot::has_slots<> {
 public:
  static rtc::scoped_refptr<RTCStatsCollector> Create(
      PeerConnectionInternal* pc,
      int64_t cache_lifetime_us = 50 * rtc::kNumMicrosecsPerMillisec);

  // Gets a recent stats report. If there is a report cached that is still fresh
  // it is returned, otherwise new stats are gathered and returned. A report is
  // considered fresh for |cache_lifetime_| ms. const RTCStatsReports are safe
  // to use across multiple threads and may be destructed on any thread.
  // If the optional selector argument is used, stats are filtered according to
  // stats selection algorithm before delivery.
  // https://w3c.github.io/webrtc-pc/#dfn-stats-selection-algorithm
  void GetStatsReport(rtc::scoped_refptr<RTCStatsCollectorCallback> callback);
  // If |selector| is null the selection algorithm is still applied (interpreted
  // as: no RTP streams are sent by selector). The result is empty.
  void GetStatsReport(rtc::scoped_refptr<RtpSenderInternal> selector,
                      rtc::scoped_refptr<RTCStatsCollectorCallback> callback);
  // If |selector| is null the selection algorithm is still applied (interpreted
  // as: no RTP streams are received by selector). The result is empty.
  void GetStatsReport(rtc::scoped_refptr<RtpReceiverInternal> selector,
                      rtc::scoped_refptr<RTCStatsCollectorCallback> callback);
  // Clears the cache's reference to the most recent stats report. Subsequently
  // calling |GetStatsReport| guarantees fresh stats.
  void ClearCachedStatsReport();

  // If there is a |GetStatsReport| requests in-flight, waits until it has been
  // completed. Must be called on the signaling thread.
  void WaitForPendingRequest();

 protected:
  RTCStatsCollector(PeerConnectionInternal* pc, int64_t cache_lifetime_us);
  ~RTCStatsCollector();

  struct CertificateStatsPair {
    std::unique_ptr<rtc::SSLCertificateStats> local;
    std::unique_ptr<rtc::SSLCertificateStats> remote;
  };

  // Stats gathering on a particular thread. Virtual for the sake of testing.
  virtual void ProducePartialResultsOnSignalingThreadImpl(
      int64_t timestamp_us,
      RTCStatsReport* partial_report);
  virtual void ProducePartialResultsOnNetworkThreadImpl(
      int64_t timestamp_us,
      const std::map<std::string, cricket::TransportStats>&
          transport_stats_by_name,
      const std::map<std::string, CertificateStatsPair>& transport_cert_stats,
      RTCStatsReport* partial_report);

 private:
  class RequestInfo {
   public:
    enum class FilterMode { kAll, kSenderSelector, kReceiverSelector };

    // Constructs with FilterMode::kAll.
    explicit RequestInfo(
        rtc::scoped_refptr<RTCStatsCollectorCallback> callback);
    // Constructs with FilterMode::kSenderSelector. The selection algorithm is
    // applied even if |selector| is null, resulting in an empty report.
    RequestInfo(rtc::scoped_refptr<RtpSenderInternal> selector,
                rtc::scoped_refptr<RTCStatsCollectorCallback> callback);
    // Constructs with FilterMode::kReceiverSelector. The selection algorithm is
    // applied even if |selector| is null, resulting in an empty report.
    RequestInfo(rtc::scoped_refptr<RtpReceiverInternal> selector,
                rtc::scoped_refptr<RTCStatsCollectorCallback> callback);

    FilterMode filter_mode() const { return filter_mode_; }
    rtc::scoped_refptr<RTCStatsCollectorCallback> callback() const {
      return callback_;
    }
    rtc::scoped_refptr<RtpSenderInternal> sender_selector() const {
      RTC_DCHECK(filter_mode_ == FilterMode::kSenderSelector);
      return sender_selector_;
    }
    rtc::scoped_refptr<RtpReceiverInternal> receiver_selector() const {
      RTC_DCHECK(filter_mode_ == FilterMode::kReceiverSelector);
      return receiver_selector_;
    }

   private:
    RequestInfo(FilterMode filter_mode,
                rtc::scoped_refptr<RTCStatsCollectorCallback> callback,
                rtc::scoped_refptr<RtpSenderInternal> sender_selector,
                rtc::scoped_refptr<RtpReceiverInternal> receiver_selector);

    FilterMode filter_mode_;
    rtc::scoped_refptr<RTCStatsCollectorCallback> callback_;
    rtc::scoped_refptr<RtpSenderInternal> sender_selector_;
    rtc::scoped_refptr<RtpReceiverInternal> receiver_selector_;
  };

  void GetStatsReportInternal(RequestInfo request);

  // Structure for tracking stats about each RtpTransceiver managed by the
  // PeerConnection. This can either by a Plan B style or Unified Plan style
  // transceiver (i.e., can have 0 or many senders and receivers).
  // Some fields are copied from the RtpTransceiver/BaseChannel object so that
  // they can be accessed safely on threads other than the signaling thread.
  // If a BaseChannel is not available (e.g., if signaling has not started),
  // then |mid| and |transport_name| will be null.
  struct RtpTransceiverStatsInfo {
    rtc::scoped_refptr<RtpTransceiver> transceiver;
    cricket::MediaType media_type;
    absl::optional<std::string> mid;
    absl::optional<std::string> transport_name;
    std::unique_ptr<TrackMediaInfoMap> track_media_info_map;
  };

  void DeliverCachedReport(
      rtc::scoped_refptr<const RTCStatsReport> cached_report,
      std::vector<RequestInfo> requests);

  // Produces |RTCCertificateStats|.
  void ProduceCertificateStats_n(
      int64_t timestamp_us,
      const std::map<std::string, CertificateStatsPair>& transport_cert_stats,
      RTCStatsReport* report) const;
  // Produces |RTCCodecStats|.
  void ProduceCodecStats_n(
      int64_t timestamp_us,
      const std::vector<RtpTransceiverStatsInfo>& transceiver_stats_infos,
      RTCStatsReport* report) const;
  // Produces |RTCDataChannelStats|.
  void ProduceDataChannelStats_s(int64_t timestamp_us,
                                 RTCStatsReport* report) const;
  // Produces |RTCIceCandidatePairStats| and |RTCIceCandidateStats|.
  void ProduceIceCandidateAndPairStats_n(
      int64_t timestamp_us,
      const std::map<std::string, cricket::TransportStats>&
          transport_stats_by_name,
      const Call::Stats& call_stats,
      RTCStatsReport* report) const;
  // Produces |RTCMediaStreamStats|.
  void ProduceMediaStreamStats_s(int64_t timestamp_us,
                                 RTCStatsReport* report) const;
  // Produces |RTCMediaStreamTrackStats|.
  void ProduceMediaStreamTrackStats_s(int64_t timestamp_us,
                                      RTCStatsReport* report) const;
  // Produces RTCMediaSourceStats, including RTCAudioSourceStats and
  // RTCVideoSourceStats.
  void ProduceMediaSourceStats_s(int64_t timestamp_us,
                                 RTCStatsReport* report) const;
  // Produces |RTCPeerConnectionStats|.
  void ProducePeerConnectionStats_s(int64_t timestamp_us,
                                    RTCStatsReport* report) const;
  // Produces |RTCInboundRTPStreamStats| and |RTCOutboundRTPStreamStats|.
  // This has to be invoked after codecs and transport stats have been created
  // because some metrics are calculated through lookup of other metrics.
  void ProduceRTPStreamStats_n(
      int64_t timestamp_us,
      const std::vector<RtpTransceiverStatsInfo>& transceiver_stats_infos,
      RTCStatsReport* report) const;
  void ProduceAudioRTPStreamStats_n(int64_t timestamp_us,
                                    const RtpTransceiverStatsInfo& stats,
                                    RTCStatsReport* report) const;
  void ProduceVideoRTPStreamStats_n(int64_t timestamp_us,
                                    const RtpTransceiverStatsInfo& stats,
                                    RTCStatsReport* report) const;
  // Produces |RTCTransportStats|.
  void ProduceTransportStats_n(
      int64_t timestamp_us,
      const std::map<std::string, cricket::TransportStats>&
          transport_stats_by_name,
      const std::map<std::string, CertificateStatsPair>& transport_cert_stats,
      RTCStatsReport* report) const;

  // Helper function to stats-producing functions.
  std::map<std::string, CertificateStatsPair>
  PrepareTransportCertificateStats_n(
      const std::map<std::string, cricket::TransportStats>&
          transport_stats_by_name) const;
  // The results are stored in |transceiver_stats_infos_| and |call_stats_|.
  void PrepareTransceiverStatsInfosAndCallStats_s_w_n();

  // Stats gathering on a particular thread.
  void ProducePartialResultsOnSignalingThread(int64_t timestamp_us);
  void ProducePartialResultsOnNetworkThread(
      int64_t timestamp_us,
      absl::optional<std::string> sctp_transport_name);
  // Merges |network_report_| into |partial_report_| and completes the request.
  // This is a NO-OP if |network_report_| is null.
  void MergeNetworkReport_s();

  // Slots for signals (sigslot) that are wired up to |pc_|.
  void OnSctpDataChannelCreated(SctpDataChannel* channel);
  // Slots for signals (sigslot) that are wired up to |channel|.
  void OnDataChannelOpened(DataChannelInterface* channel);
  void OnDataChannelClosed(DataChannelInterface* channel);

  PeerConnectionInternal* const pc_;
  rtc::Thread* const signaling_thread_;
  rtc::Thread* const worker_thread_;
  rtc::Thread* const network_thread_;

  int num_pending_partial_reports_;
  int64_t partial_report_timestamp_us_;
  // Reports that are produced on the signaling thread or the network thread are
  // merged into this report. It is only touched on the signaling thread. Once
  // all partial reports are merged this is the result of a request.
  rtc::scoped_refptr<RTCStatsReport> partial_report_;
  std::vector<RequestInfo> requests_;
  // Holds the result of ProducePartialResultsOnNetworkThread(). It is merged
  // into |partial_report_| on the signaling thread and then nulled by
  // MergeNetworkReport_s(). Thread-safety is ensured by using
  // |network_report_event_|.
  rtc::scoped_refptr<RTCStatsReport> network_report_;
  // If set, it is safe to touch the |network_report_| on the signaling thread.
  // This is reset before async-invoking ProducePartialResultsOnNetworkThread()
  // and set when ProducePartialResultsOnNetworkThread() is complete, after it
  // has updated the value of |network_report_|.
  rtc::Event network_report_event_;

  // Cleared and set in `PrepareTransceiverStatsInfosAndCallStats_s_w_n`,
  // starting out on the signaling thread, then network. Later read on the
  // network and signaling threads as part of collecting stats and finally
  // reset when the work is done. Initially this variable was added and not
  // passed around as an arguments to avoid copies. This is thread safe due to
  // how operations are sequenced and we don't start the stats collection
  // sequence if one is in progress. As a future improvement though, we could
  // now get rid of the variable and keep the data scoped within a stats
  // collection sequence.
  std::vector<RtpTransceiverStatsInfo> transceiver_stats_infos_;

  Call::Stats call_stats_;

  // A timestamp, in microseconds, that is based on a timer that is
  // monotonically increasing. That is, even if the system clock is modified the
  // difference between the timer and this timestamp is how fresh the cached
  // report is.
  int64_t cache_timestamp_us_;
  int64_t cache_lifetime_us_;
  rtc::scoped_refptr<const RTCStatsReport> cached_report_;

  // Data recorded and maintained by the stats collector during its lifetime.
  // Some stats are produced from this record instead of other components.
  struct InternalRecord {
    InternalRecord() : data_channels_opened(0), data_channels_closed(0) {}

    // The opened count goes up when a channel is fully opened and the closed
    // count goes up if a previously opened channel has fully closed. The opened
    // count does not go down when a channel closes, meaning (opened - closed)
    // is the number of channels currently opened. A channel that is closed
    // before reaching the open state does not affect these counters.
    uint32_t data_channels_opened;
    uint32_t data_channels_closed;
    // Identifies by address channels that have been opened, which remain in the
    // set until they have been fully closed.
    std::set<uintptr_t> opened_data_channels;
  };
  InternalRecord internal_record_;
};

const char* CandidateTypeToRTCIceCandidateTypeForTesting(
    const std::string& type);
const char* DataStateToRTCDataChannelStateForTesting(
    DataChannelInterface::DataState state);

}  // namespace webrtc

#endif  // PC_RTC_STATS_COLLECTOR_H_
