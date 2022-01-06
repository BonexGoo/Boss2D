/*
 *  Copyright 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef PC_PEER_CONNECTION_MESSAGE_HANDLER_H_
#define PC_PEER_CONNECTION_MESSAGE_HANDLER_H_

#include <functional>

#include BOSS_WEBRTC_U_api__jsep_h //original-code:"api/jsep.h"
#include BOSS_WEBRTC_U_api__media_stream_interface_h //original-code:"api/media_stream_interface.h"
#include BOSS_WEBRTC_U_api__peer_connection_interface_h //original-code:"api/peer_connection_interface.h"
#include BOSS_WEBRTC_U_api__rtc_error_h //original-code:"api/rtc_error.h"
#include BOSS_WEBRTC_U_api__stats_types_h //original-code:"api/stats_types.h"
#include BOSS_WEBRTC_U_pc__stats_collector_interface_h //original-code:"pc/stats_collector_interface.h"
#include BOSS_WEBRTC_U_rtc_base__message_handler_h //original-code:"rtc_base/message_handler.h"
#include BOSS_WEBRTC_U_rtc_base__thread_h //original-code:"rtc_base/thread.h"
#include BOSS_WEBRTC_U_rtc_base__thread_message_h //original-code:"rtc_base/thread_message.h"

namespace webrtc {

class CreateSessionDescriptionObserver;
class SetSessionDescriptionObserver;
class StatsCollectorInterface;
class StatsObserver;
class MediaStreamTrackInterface;

class PeerConnectionMessageHandler : public rtc::MessageHandler {
 public:
  explicit PeerConnectionMessageHandler(rtc::Thread* signaling_thread)
      : signaling_thread_(signaling_thread) {}
  ~PeerConnectionMessageHandler();

  // Implements MessageHandler.
  void OnMessage(rtc::Message* msg) override;
  void PostSetSessionDescriptionSuccess(
      SetSessionDescriptionObserver* observer);
  void PostSetSessionDescriptionFailure(SetSessionDescriptionObserver* observer,
                                        RTCError&& error);
  void PostCreateSessionDescriptionFailure(
      CreateSessionDescriptionObserver* observer,
      RTCError error);
  void PostGetStats(StatsObserver* observer,
                    StatsCollectorInterface* stats,
                    MediaStreamTrackInterface* track);
  void RequestUsagePatternReport(std::function<void()>, int delay_ms);

 private:
  rtc::Thread* signaling_thread() const { return signaling_thread_; }

  rtc::Thread* const signaling_thread_;
};

}  // namespace webrtc

#endif  // PC_PEER_CONNECTION_MESSAGE_HANDLER_H_
