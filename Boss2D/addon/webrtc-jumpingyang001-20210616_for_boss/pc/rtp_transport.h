/*
 *  Copyright 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_RTP_TRANSPORT_H_
#define PC_RTP_TRANSPORT_H_

#include <stddef.h>
#include <stdint.h>

#include <string>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_call__rtp_demuxer_h //original-code:"call/rtp_demuxer.h"
#include BOSS_WEBRTC_U_modules__rtp_rtcp__include__rtp_header_extension_map_h //original-code:"modules/rtp_rtcp/include/rtp_header_extension_map.h"
#include BOSS_WEBRTC_U_p2p__base__packet_transport_internal_h //original-code:"p2p/base/packet_transport_internal.h"
#include BOSS_WEBRTC_U_pc__rtp_transport_internal_h //original-code:"pc/rtp_transport_internal.h"
#include BOSS_WEBRTC_U_pc__session_description_h //original-code:"pc/session_description.h"
#include BOSS_WEBRTC_U_rtc_base__async_packet_socket_h //original-code:"rtc_base/async_packet_socket.h"
#include BOSS_WEBRTC_U_rtc_base__copy_on_write_buffer_h //original-code:"rtc_base/copy_on_write_buffer.h"
#include BOSS_WEBRTC_U_rtc_base__network__sent_packet_h //original-code:"rtc_base/network/sent_packet.h"
#include BOSS_WEBRTC_U_rtc_base__network_route_h //original-code:"rtc_base/network_route.h"
#include BOSS_WEBRTC_U_rtc_base__socket_h //original-code:"rtc_base/socket.h"
#include BOSS_WEBRTC_U_rtc_base__third_party__sigslot__sigslot_h //original-code:"rtc_base/third_party/sigslot/sigslot.h"

namespace rtc {

class CopyOnWriteBuffer;
struct PacketOptions;
class PacketTransportInternal;

}  // namespace rtc

namespace webrtc {

class RtpTransport : public RtpTransportInternal {
 public:
  RtpTransport(const RtpTransport&) = delete;
  RtpTransport& operator=(const RtpTransport&) = delete;

  explicit RtpTransport(bool rtcp_mux_enabled)
      : rtcp_mux_enabled_(rtcp_mux_enabled) {}

  bool rtcp_mux_enabled() const override { return rtcp_mux_enabled_; }
  void SetRtcpMuxEnabled(bool enable) override;

  const std::string& transport_name() const override;

  int SetRtpOption(rtc::Socket::Option opt, int value) override;
  int SetRtcpOption(rtc::Socket::Option opt, int value) override;

  rtc::PacketTransportInternal* rtp_packet_transport() const {
    return rtp_packet_transport_;
  }
  void SetRtpPacketTransport(rtc::PacketTransportInternal* rtp);

  rtc::PacketTransportInternal* rtcp_packet_transport() const {
    return rtcp_packet_transport_;
  }
  void SetRtcpPacketTransport(rtc::PacketTransportInternal* rtcp);

  bool IsReadyToSend() const override { return ready_to_send_; }

  bool IsWritable(bool rtcp) const override;

  bool SendRtpPacket(rtc::CopyOnWriteBuffer* packet,
                     const rtc::PacketOptions& options,
                     int flags) override;

  bool SendRtcpPacket(rtc::CopyOnWriteBuffer* packet,
                      const rtc::PacketOptions& options,
                      int flags) override;

  bool IsSrtpActive() const override { return false; }

  void UpdateRtpHeaderExtensionMap(
      const cricket::RtpHeaderExtensions& header_extensions) override;

  bool RegisterRtpDemuxerSink(const RtpDemuxerCriteria& criteria,
                              RtpPacketSinkInterface* sink) override;

  bool UnregisterRtpDemuxerSink(RtpPacketSinkInterface* sink) override;

 protected:
  // These methods will be used in the subclasses.
  void DemuxPacket(rtc::CopyOnWriteBuffer packet, int64_t packet_time_us);

  bool SendPacket(bool rtcp,
                  rtc::CopyOnWriteBuffer* packet,
                  const rtc::PacketOptions& options,
                  int flags);

  // Overridden by SrtpTransport.
  virtual void OnNetworkRouteChanged(
      absl::optional<rtc::NetworkRoute> network_route);
  virtual void OnRtpPacketReceived(rtc::CopyOnWriteBuffer packet,
                                   int64_t packet_time_us);
  virtual void OnRtcpPacketReceived(rtc::CopyOnWriteBuffer packet,
                                    int64_t packet_time_us);
  // Overridden by SrtpTransport and DtlsSrtpTransport.
  virtual void OnWritableState(rtc::PacketTransportInternal* packet_transport);

 private:
  void OnReadyToSend(rtc::PacketTransportInternal* transport);
  void OnSentPacket(rtc::PacketTransportInternal* packet_transport,
                    const rtc::SentPacket& sent_packet);
  void OnReadPacket(rtc::PacketTransportInternal* transport,
                    const char* data,
                    size_t len,
                    const int64_t& packet_time_us,
                    int flags);

  // Updates "ready to send" for an individual channel and fires
  // SignalReadyToSend.
  void SetReadyToSend(bool rtcp, bool ready);

  void MaybeSignalReadyToSend();

  bool IsTransportWritable();

  bool rtcp_mux_enabled_;

  rtc::PacketTransportInternal* rtp_packet_transport_ = nullptr;
  rtc::PacketTransportInternal* rtcp_packet_transport_ = nullptr;

  bool ready_to_send_ = false;
  bool rtp_ready_to_send_ = false;
  bool rtcp_ready_to_send_ = false;

  RtpDemuxer rtp_demuxer_;

  // Used for identifying the MID for RtpDemuxer.
  RtpHeaderExtensionMap header_extension_map_;
};

}  // namespace webrtc

#endif  // PC_RTP_TRANSPORT_H_
