/*
 *  Copyright (c) 2020 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */
#ifndef RTC_BASE_THREAD_MESSAGE_H_
#define RTC_BASE_THREAD_MESSAGE_H_

#include <list>
#include <memory>
#include <utility>

#include BOSS_WEBRTC_U_api__scoped_refptr_h //original-code:"api/scoped_refptr.h"
#include BOSS_WEBRTC_U_rtc_base__location_h //original-code:"rtc_base/location.h"
#include BOSS_WEBRTC_U_rtc_base__message_handler_h //original-code:"rtc_base/message_handler.h"

namespace rtc {

// Derive from this for specialized data
// App manages lifetime, except when messages are purged

class MessageData {
 public:
  MessageData() {}
  virtual ~MessageData() {}
};

template <class T>
class TypedMessageData : public MessageData {
 public:
  explicit TypedMessageData(const T& data) : data_(data) {}
  const T& data() const { return data_; }
  T& data() { return data_; }

 private:
  T data_;
};

// Like TypedMessageData, but for pointers that require a delete.
template <class T>
class ScopedMessageData : public MessageData {
 public:
  explicit ScopedMessageData(std::unique_ptr<T> data)
      : data_(std::move(data)) {}
  // Deprecated.
  // TODO(deadbeef): Remove this once downstream applications stop using it.
  explicit ScopedMessageData(T* data) : data_(data) {}
  // Deprecated.
  // TODO(deadbeef): Returning a reference to a unique ptr? Why. Get rid of
  // this once downstream applications stop using it, then rename inner_data to
  // just data.
  const std::unique_ptr<T>& data() const { return data_; }
  std::unique_ptr<T>& data() { return data_; }

  const T& inner_data() const { return *data_; }
  T& inner_data() { return *data_; }

 private:
  std::unique_ptr<T> data_;
};

// Like ScopedMessageData, but for reference counted pointers.
template <class T>
class ScopedRefMessageData : public MessageData {
 public:
  explicit ScopedRefMessageData(T* data) : data_(data) {}
  const scoped_refptr<T>& data() const { return data_; }
  scoped_refptr<T>& data() { return data_; }

 private:
  scoped_refptr<T> data_;
};

template <class T>
inline MessageData* WrapMessageData(const T& data) {
  return new TypedMessageData<T>(data);
}

template <class T>
inline const T& UseMessageData(MessageData* data) {
  return static_cast<TypedMessageData<T>*>(data)->data();
}

template <class T>
class DisposeData : public MessageData {
 public:
  explicit DisposeData(T* data) : data_(data) {}
  virtual ~DisposeData() { delete data_; }

 private:
  T* data_;
};

const uint32_t MQID_ANY = static_cast<uint32_t>(-1);
const uint32_t MQID_DISPOSE = static_cast<uint32_t>(-2);

// No destructor

struct Message {
  Message() : phandler(nullptr), message_id(0), pdata(nullptr) {}
  inline bool Match(MessageHandler* handler, uint32_t id) const {
    return (handler == nullptr || handler == phandler) &&
           (id == MQID_ANY || id == message_id);
  }
  Location posted_from;
  MessageHandler* phandler;
  uint32_t message_id;
  MessageData* pdata;
};

typedef std::list<Message> MessageList;
}  // namespace rtc
#endif  // RTC_BASE_THREAD_MESSAGE_H_
