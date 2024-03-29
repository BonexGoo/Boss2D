/*
 *  Copyright 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_pc__peer_connection_message_handler_h //original-code:"pc/peer_connection_message_handler.h"

#include <utility>

#include BOSS_WEBRTC_U_api__jsep_h //original-code:"api/jsep.h"
#include BOSS_WEBRTC_U_api__media_stream_interface_h //original-code:"api/media_stream_interface.h"
#include BOSS_WEBRTC_U_api__peer_connection_interface_h //original-code:"api/peer_connection_interface.h"
#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_api__sequence_checker_h //original-code:"api/sequence_checker.h"
#include BOSS_WEBRTC_U_api__stats_types_h //original-code:"api/stats_types.h"
#include BOSS_WEBRTC_U_pc__stats_collector_interface_h //original-code:"pc/stats_collector_interface.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__location_h //original-code:"rtc_base/location.h"

namespace webrtc {

namespace {

enum {
  MSG_SET_SESSIONDESCRIPTION_SUCCESS = 0,
  MSG_SET_SESSIONDESCRIPTION_FAILED,
  MSG_CREATE_SESSIONDESCRIPTION_FAILED,
  MSG_GETSTATS,
  MSG_REPORT_USAGE_PATTERN,
};

struct SetSessionDescriptionMsg : public rtc::MessageData {
  explicit SetSessionDescriptionMsg(
      webrtc::SetSessionDescriptionObserver* observer)
      : observer(observer) {}

  rtc::scoped_refptr<webrtc::SetSessionDescriptionObserver> observer;
  RTCError error;
};

struct CreateSessionDescriptionMsg : public rtc::MessageData {
  explicit CreateSessionDescriptionMsg(
      webrtc::CreateSessionDescriptionObserver* observer)
      : observer(observer) {}

  rtc::scoped_refptr<webrtc::CreateSessionDescriptionObserver> observer;
  RTCError error;
};

struct GetStatsMsg : public rtc::MessageData {
  GetStatsMsg(webrtc::StatsObserver* observer,
              StatsCollectorInterface* stats,
              webrtc::MediaStreamTrackInterface* track)
      : observer(observer), stats(stats), track(track) {}
  rtc::scoped_refptr<webrtc::StatsObserver> observer;
  StatsCollectorInterface* stats;
  rtc::scoped_refptr<webrtc::MediaStreamTrackInterface> track;
};

struct RequestUsagePatternMsg : public rtc::MessageData {
  explicit RequestUsagePatternMsg(std::function<void()> func)
      : function(func) {}
  std::function<void()> function;
};

}  // namespace

PeerConnectionMessageHandler::~PeerConnectionMessageHandler() {
  // Process all pending notifications in the message queue. If we don't do
  // this, requests will linger and not know they succeeded or failed.
  rtc::MessageList list;
  signaling_thread()->Clear(this, rtc::MQID_ANY, &list);
  for (auto& msg : list) {
    if (msg.message_id == MSG_CREATE_SESSIONDESCRIPTION_FAILED) {
      // Processing CreateOffer() and CreateAnswer() messages ensures their
      // observers are invoked even if the PeerConnection is destroyed early.
      OnMessage(&msg);
    } else {
      // TODO(hbos): Consider processing all pending messages. This would mean
      // that SetLocalDescription() and SetRemoteDescription() observers are
      // informed of successes and failures; this is currently NOT the case.
      delete msg.pdata;
    }
  }
}

void PeerConnectionMessageHandler::OnMessage(rtc::Message* msg) {
  RTC_DCHECK_RUN_ON(signaling_thread());
  switch (msg->message_id) {
    case MSG_SET_SESSIONDESCRIPTION_SUCCESS: {
      SetSessionDescriptionMsg* param =
          static_cast<SetSessionDescriptionMsg*>(msg->pdata);
      param->observer->OnSuccess();
      delete param;
      break;
    }
    case MSG_SET_SESSIONDESCRIPTION_FAILED: {
      SetSessionDescriptionMsg* param =
          static_cast<SetSessionDescriptionMsg*>(msg->pdata);
      param->observer->OnFailure(std::move(param->error));
      delete param;
      break;
    }
    case MSG_CREATE_SESSIONDESCRIPTION_FAILED: {
      CreateSessionDescriptionMsg* param =
          static_cast<CreateSessionDescriptionMsg*>(msg->pdata);
      param->observer->OnFailure(std::move(param->error));
      delete param;
      break;
    }
    case MSG_GETSTATS: {
      GetStatsMsg* param = static_cast<GetStatsMsg*>(msg->pdata);
      StatsReports reports;
      param->stats->GetStats(param->track, &reports);
      param->observer->OnComplete(reports);
      delete param;
      break;
    }
    case MSG_REPORT_USAGE_PATTERN: {
      RequestUsagePatternMsg* param =
          static_cast<RequestUsagePatternMsg*>(msg->pdata);
      param->function();
      delete param;
      break;
    }
    default:
      RTC_NOTREACHED() << "Not implemented";
      break;
  }
}

void PeerConnectionMessageHandler::PostSetSessionDescriptionSuccess(
    SetSessionDescriptionObserver* observer) {
  SetSessionDescriptionMsg* msg = new SetSessionDescriptionMsg(observer);
  signaling_thread()->Post(RTC_FROM_HERE, this,
                           MSG_SET_SESSIONDESCRIPTION_SUCCESS, msg);
}

void PeerConnectionMessageHandler::PostSetSessionDescriptionFailure(
    SetSessionDescriptionObserver* observer,
    RTCError&& error) {
  RTC_DCHECK(!error.ok());
  SetSessionDescriptionMsg* msg = new SetSessionDescriptionMsg(observer);
  msg->error = std::move(error);
  signaling_thread()->Post(RTC_FROM_HERE, this,
                           MSG_SET_SESSIONDESCRIPTION_FAILED, msg);
}

void PeerConnectionMessageHandler::PostCreateSessionDescriptionFailure(
    CreateSessionDescriptionObserver* observer,
    RTCError error) {
  RTC_DCHECK(!error.ok());
  CreateSessionDescriptionMsg* msg = new CreateSessionDescriptionMsg(observer);
  msg->error = std::move(error);
  signaling_thread()->Post(RTC_FROM_HERE, this,
                           MSG_CREATE_SESSIONDESCRIPTION_FAILED, msg);
}

void PeerConnectionMessageHandler::PostGetStats(
    StatsObserver* observer,
    StatsCollectorInterface* stats,
    MediaStreamTrackInterface* track) {
  signaling_thread()->Post(RTC_FROM_HERE, this, MSG_GETSTATS,
                           new GetStatsMsg(observer, stats, track));
}

void PeerConnectionMessageHandler::RequestUsagePatternReport(
    std::function<void()> func,
    int delay_ms) {
  signaling_thread()->PostDelayed(RTC_FROM_HERE, delay_ms, this,
                                  MSG_REPORT_USAGE_PATTERN,
                                  new RequestUsagePatternMsg(func));
}

}  // namespace webrtc
