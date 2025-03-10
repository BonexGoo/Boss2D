/*
 *  Copyright (c) 2017 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef CALL_RTP_STREAM_RECEIVER_CONTROLLER_H_
#define CALL_RTP_STREAM_RECEIVER_CONTROLLER_H_

#include <memory>

#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_call__rtp_demuxer_h //original-code:"call/rtp_demuxer.h"
#include BOSS_WEBRTC_U_call__rtp_stream_receiver_controller_interface_h //original-code:"call/rtp_stream_receiver_controller_interface.h"

namespace webrtc {

class RtpPacketReceived;

// This class represents the RTP receive parsing and demuxing, for a
// single RTP session.
// TODO(nisse): Add RTCP processing, we should aim to terminate RTCP
// and not leave any RTCP processing to individual receive streams.
// TODO(nisse): Extract per-packet processing, including parsing and
// demuxing, into a separate class.
class RtpStreamReceiverController
    : public RtpStreamReceiverControllerInterface {
 public:
  RtpStreamReceiverController();
  ~RtpStreamReceiverController() override;

  // Implements RtpStreamReceiverControllerInterface.
  std::unique_ptr<RtpStreamReceiverInterface> CreateReceiver(
      uint32_t ssrc,
      RtpPacketSinkInterface* sink) override;

  // Thread-safe wrappers for the corresponding RtpDemuxer methods.
  bool AddSink(uint32_t ssrc, RtpPacketSinkInterface* sink) override;
  size_t RemoveSink(const RtpPacketSinkInterface* sink) override;

  // TODO(nisse): Not yet responsible for parsing.
  bool OnRtpPacket(const RtpPacketReceived& packet);

 private:
  class Receiver : public RtpStreamReceiverInterface {
   public:
    Receiver(RtpStreamReceiverController* controller,
             uint32_t ssrc,
             RtpPacketSinkInterface* sink);

    ~Receiver() override;

   private:
    RtpStreamReceiverController* const controller_;
    RtpPacketSinkInterface* const sink_;
  };

  // TODO(bugs.webrtc.org/11993): We expect construction and all methods to be
  // called on the same thread/tq. Currently this is the worker thread
  // (including OnRtpPacket) but a more natural fit would be the network thread.
  // Using a sequence checker to ensure that usage is correct but at the same
  // time not require a specific thread/tq, an instance of this class + the
  // associated functionality should be easily moved from one execution context
  // to another (i.e. when network packets don't hop to the worker thread inside
  // of Call).
  SequenceChecker demuxer_sequence_;
  // At this level the demuxer is only configured to demux by SSRC, so don't
  // worry about MIDs (MIDs are handled by upper layers).
  RtpDemuxer demuxer_ RTC_GUARDED_BY(&demuxer_sequence_){false /*use_mid*/};
};

}  // namespace webrtc

#endif  // CALL_RTP_STREAM_RECEIVER_CONTROLLER_H_
