/*
 *  Copyright 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

// This file contains interfaces for DataChannels
// http://dev.w3.org/2011/webrtc/editor/webrtc.html#rtcdatachannel

#ifndef API_DATA_CHANNEL_INTERFACE_H_
#define API_DATA_CHANNEL_INTERFACE_H_

#include <stddef.h>
#include <stdint.h>

#include <string>

#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__priority_h //original-code:"api/priority.h"
#include BOSS_WEBRTC_U_api__rtc_error_h //original-code:"api/rtc_error.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__copy_on_write_buffer_h //original-code:"rtc_base/copy_on_write_buffer.h"
#include BOSS_WEBRTC_U_rtc_base__ref_count_h //original-code:"rtc_base/ref_count.h"
#include BOSS_WEBRTC_U_rtc_base__system__rtc_export_h //original-code:"rtc_base/system/rtc_export.h"

namespace webrtc {

// C++ version of: https://www.w3.org/TR/webrtc/#idl-def-rtcdatachannelinit
// TODO(deadbeef): Use absl::optional for the "-1 if unset" things.
struct DataChannelInit {
  // Deprecated. Reliability is assumed, and channel will be unreliable if
  // maxRetransmitTime or MaxRetransmits is set.
  bool reliable = false;

  // True if ordered delivery is required.
  bool ordered = true;

  // The max period of time in milliseconds in which retransmissions will be
  // sent. After this time, no more retransmissions will be sent.
  //
  // Cannot be set along with |maxRetransmits|.
  // This is called |maxPacketLifeTime| in the WebRTC JS API.
  // Negative values are ignored, and positive values are clamped to [0-65535]
  absl::optional<int> maxRetransmitTime;

  // The max number of retransmissions.
  //
  // Cannot be set along with |maxRetransmitTime|.
  // Negative values are ignored, and positive values are clamped to [0-65535]
  absl::optional<int> maxRetransmits;

  // This is set by the application and opaque to the WebRTC implementation.
  std::string protocol;

  // True if the channel has been externally negotiated and we do not send an
  // in-band signalling in the form of an "open" message. If this is true, |id|
  // below must be set; otherwise it should be unset and will be negotiated
  // in-band.
  bool negotiated = false;

  // The stream id, or SID, for SCTP data channels. -1 if unset (see above).
  int id = -1;

  // https://w3c.github.io/webrtc-priority/#new-rtcdatachannelinit-member
  absl::optional<Priority> priority;
};

// At the JavaScript level, data can be passed in as a string or a blob, so
// this structure's |binary| flag tells whether the data should be interpreted
// as binary or text.
struct DataBuffer {
  DataBuffer(const rtc::CopyOnWriteBuffer& data, bool binary)
      : data(data), binary(binary) {}
  // For convenience for unit tests.
  explicit DataBuffer(const std::string& text)
      : data(text.data(), text.length()), binary(false) {}
  size_t size() const { return data.size(); }

  rtc::CopyOnWriteBuffer data;
  // Indicates if the received data contains UTF-8 or binary data.
  // Note that the upper layers are left to verify the UTF-8 encoding.
  // TODO(jiayl): prefer to use an enum instead of a bool.
  bool binary;
};

// Used to implement RTCDataChannel events.
//
// The code responding to these callbacks should unwind the stack before
// using any other webrtc APIs; re-entrancy is not supported.
class DataChannelObserver {
 public:
  // The data channel state have changed.
  virtual void OnStateChange() = 0;
  //  A data buffer was successfully received.
  virtual void OnMessage(const DataBuffer& buffer) = 0;
  // The data channel's buffered_amount has changed.
  virtual void OnBufferedAmountChange(uint64_t sent_data_size) {}

 protected:
  virtual ~DataChannelObserver() = default;
};

class RTC_EXPORT DataChannelInterface : public rtc::RefCountInterface {
 public:
  // C++ version of: https://www.w3.org/TR/webrtc/#idl-def-rtcdatachannelstate
  // Unlikely to change, but keep in sync with DataChannel.java:State and
  // RTCDataChannel.h:RTCDataChannelState.
  enum DataState {
    kConnecting,
    kOpen,  // The DataChannel is ready to send data.
    kClosing,
    kClosed
  };

  static const char* DataStateString(DataState state) {
    switch (state) {
      case kConnecting:
        return "connecting";
      case kOpen:
        return "open";
      case kClosing:
        return "closing";
      case kClosed:
        return "closed";
    }
    RTC_CHECK(false) << "Unknown DataChannel state: " << state;
    return "";
  }

  // Used to receive events from the data channel. Only one observer can be
  // registered at a time. UnregisterObserver should be called before the
  // observer object is destroyed.
  virtual void RegisterObserver(DataChannelObserver* observer) = 0;
  virtual void UnregisterObserver() = 0;

  // The label attribute represents a label that can be used to distinguish this
  // DataChannel object from other DataChannel objects.
  virtual std::string label() const = 0;

  // The accessors below simply return the properties from the DataChannelInit
  // the data channel was constructed with.
  virtual bool reliable() const = 0;
  // TODO(deadbeef): Remove these dummy implementations when all classes have
  // implemented these APIs. They should all just return the values the
  // DataChannel was created with.
  virtual bool ordered() const;
  // TODO(hta): Deprecate and remove the following two functions.
  virtual uint16_t maxRetransmitTime() const;
  virtual uint16_t maxRetransmits() const;
  virtual absl::optional<int> maxRetransmitsOpt() const;
  virtual absl::optional<int> maxPacketLifeTime() const;
  virtual std::string protocol() const;
  virtual bool negotiated() const;

  // Returns the ID from the DataChannelInit, if it was negotiated out-of-band.
  // If negotiated in-band, this ID will be populated once the DTLS role is
  // determined, and until then this will return -1.
  virtual int id() const = 0;
  virtual Priority priority() const { return Priority::kLow; }
  virtual DataState state() const = 0;
  // When state is kClosed, and the DataChannel was not closed using
  // the closing procedure, returns the error information about the closing.
  // The default implementation returns "no error".
  virtual RTCError error() const { return RTCError(); }
  virtual uint32_t messages_sent() const = 0;
  virtual uint64_t bytes_sent() const = 0;
  virtual uint32_t messages_received() const = 0;
  virtual uint64_t bytes_received() const = 0;

  // Returns the number of bytes of application data (UTF-8 text and binary
  // data) that have been queued using Send but have not yet been processed at
  // the SCTP level. See comment above Send below.
  virtual uint64_t buffered_amount() const = 0;

  // Begins the graceful data channel closing procedure. See:
  // https://tools.ietf.org/html/draft-ietf-rtcweb-data-channel-13#section-6.7
  virtual void Close() = 0;

  // Sends |data| to the remote peer. If the data can't be sent at the SCTP
  // level (due to congestion control), it's buffered at the data channel level,
  // up to a maximum of 16MB. If Send is called while this buffer is full, the
  // data channel will be closed abruptly.
  //
  // So, it's important to use buffered_amount() and OnBufferedAmountChange to
  // ensure the data channel is used efficiently but without filling this
  // buffer.
  virtual bool Send(const DataBuffer& buffer) = 0;

 protected:
  ~DataChannelInterface() override = default;
};

}  // namespace webrtc

#endif  // API_DATA_CHANNEL_INTERFACE_H_
