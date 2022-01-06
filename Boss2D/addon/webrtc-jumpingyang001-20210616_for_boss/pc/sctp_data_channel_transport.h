/*
 *  Copyright 2019 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_SCTP_DATA_CHANNEL_TRANSPORT_H_
#define PC_SCTP_DATA_CHANNEL_TRANSPORT_H_

#include BOSS_WEBRTC_U_api__rtc_error_h //original-code:"api/rtc_error.h"
#include BOSS_WEBRTC_U_api__transport__data_channel_transport_interface_h //original-code:"api/transport/data_channel_transport_interface.h"
#include BOSS_WEBRTC_U_media__base__media_channel_h //original-code:"media/base/media_channel.h"
#include BOSS_WEBRTC_U_media__sctp__sctp_transport_internal_h //original-code:"media/sctp/sctp_transport_internal.h"
#include BOSS_WEBRTC_U_rtc_base__copy_on_write_buffer_h //original-code:"rtc_base/copy_on_write_buffer.h"
#include BOSS_WEBRTC_U_rtc_base__third_party__sigslot__sigslot_h //original-code:"rtc_base/third_party/sigslot/sigslot.h"

namespace webrtc {

// SCTP implementation of DataChannelTransportInterface.
class SctpDataChannelTransport : public DataChannelTransportInterface,
                                 public sigslot::has_slots<> {
 public:
  explicit SctpDataChannelTransport(
      cricket::SctpTransportInternal* sctp_transport);

  RTCError OpenChannel(int channel_id) override;
  RTCError SendData(int channel_id,
                    const SendDataParams& params,
                    const rtc::CopyOnWriteBuffer& buffer) override;
  RTCError CloseChannel(int channel_id) override;
  void SetDataSink(DataChannelSink* sink) override;
  bool IsReadyToSend() const override;

 private:
  void OnReadyToSendData();
  void OnDataReceived(const cricket::ReceiveDataParams& params,
                      const rtc::CopyOnWriteBuffer& buffer);
  void OnClosingProcedureStartedRemotely(int channel_id);
  void OnClosingProcedureComplete(int channel_id);
  void OnClosedAbruptly();

  cricket::SctpTransportInternal* const sctp_transport_;

  DataChannelSink* sink_ = nullptr;
  bool ready_to_send_ = false;
};

}  // namespace webrtc

#endif  // PC_SCTP_DATA_CHANNEL_TRANSPORT_H_
