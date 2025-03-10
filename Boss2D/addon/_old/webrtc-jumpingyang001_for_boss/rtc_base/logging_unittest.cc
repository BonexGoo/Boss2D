/*
 *  Copyright 2004 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__arraysize_h //original-code:"rtc_base/arraysize.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"
#include BOSS_WEBRTC_U_rtc_base__gunit_h //original-code:"rtc_base/gunit.h"
#include BOSS_WEBRTC_U_rtc_base__platform_thread_h //original-code:"rtc_base/platform_thread.h"
#include BOSS_WEBRTC_U_rtc_base__stream_h //original-code:"rtc_base/stream.h"
#include "test/testsupport/fileutils.h"

namespace rtc {

namespace {

class StringStream : public StreamInterface {
 public:
  explicit StringStream(std::string* str);
  explicit StringStream(const std::string& str);

  StreamState GetState() const override;
  StreamResult Read(void* buffer,
                    size_t buffer_len,
                    size_t* read,
                    int* error) override;
  StreamResult Write(const void* data,
                     size_t data_len,
                     size_t* written,
                     int* error) override;
  void Close() override;
  bool SetPosition(size_t position) override;
  bool GetPosition(size_t* position) const override;
  bool GetSize(size_t* size) const override;
  bool GetAvailable(size_t* size) const override;
  bool ReserveSize(size_t size) override;

 private:
  std::string& str_;
  size_t read_pos_;
  bool read_only_;
};

StringStream::StringStream(std::string* str)
    : str_(*str), read_pos_(0), read_only_(false) {}

StringStream::StringStream(const std::string& str)
    : str_(const_cast<std::string&>(str)), read_pos_(0), read_only_(true) {}

StreamState StringStream::GetState() const {
  return SS_OPEN;
}

StreamResult StringStream::Read(void* buffer,
                                size_t buffer_len,
                                size_t* read,
                                int* error) {
  size_t available = std::min(buffer_len, str_.size() - read_pos_);
  if (!available)
    return SR_EOS;
  memcpy(buffer, str_.data() + read_pos_, available);
  read_pos_ += available;
  if (read)
    *read = available;
  return SR_SUCCESS;
}

StreamResult StringStream::Write(const void* data,
                                 size_t data_len,
                                 size_t* written,
                                 int* error) {
  if (read_only_) {
    if (error) {
      *error = -1;
    }
    return SR_ERROR;
  }
  str_.append(static_cast<const char*>(data),
              static_cast<const char*>(data) + data_len);
  if (written)
    *written = data_len;
  return SR_SUCCESS;
}

void StringStream::Close() {}

bool StringStream::SetPosition(size_t position) {
  if (position > str_.size())
    return false;
  read_pos_ = position;
  return true;
}

bool StringStream::GetPosition(size_t* position) const {
  if (position)
    *position = read_pos_;
  return true;
}

bool StringStream::GetSize(size_t* size) const {
  if (size)
    *size = str_.size();
  return true;
}

bool StringStream::GetAvailable(size_t* size) const {
  if (size)
    *size = str_.size() - read_pos_;
  return true;
}

bool StringStream::ReserveSize(size_t size) {
  if (read_only_)
    return false;
  str_.reserve(size);
  return true;
}

}  // namespace

template <typename Base>
class LogSinkImpl : public LogSink, public Base {
 public:
  LogSinkImpl() {}

  template <typename P>
  explicit LogSinkImpl(P* p) : Base(p) {}

 private:
  void OnLogMessage(const std::string& message) override {
    static_cast<Base*>(this)->WriteAll(message.data(), message.size(), nullptr,
                                       nullptr);
  }
};

class LogMessageForTesting : public LogMessage {
 public:
  LogMessageForTesting(const char* file,
                       int line,
                       LoggingSeverity sev,
                       LogErrorContext err_ctx = ERRCTX_NONE,
                       int err = 0)
      : LogMessage(file, line, sev, err_ctx, err) {}

  const std::string& get_extra() const { return extra_; }
  bool is_noop() const { return is_noop_; }
#if defined(WEBRTC_ANDROID)
  const char* get_tag() const { return tag_; }
#endif

  // Returns the contents of the internal log stream.
  // Note that parts of the stream won't (as is) be available until *after* the
  // dtor of the parent class has run. So, as is, this only represents a
  // partially built stream.
  std::string GetPrintStream() {
    RTC_DCHECK(!is_finished_);
    is_finished_ = true;
    FinishPrintStream();
    std::string ret = print_stream_.str();
    // Just to make an error even more clear if the stream gets used after this.
    print_stream_.clear();
    return ret;
  }

 private:
  bool is_finished_ = false;
};

// Test basic logging operation. We should get the INFO log but not the VERBOSE.
// We should restore the correct global state at the end.
TEST(LogTest, SingleStream) {
  int sev = LogMessage::GetLogToStream(nullptr);

  std::string str;
  LogSinkImpl<StringStream> stream(&str);
  LogMessage::AddLogToStream(&stream, LS_INFO);
  EXPECT_EQ(LS_INFO, LogMessage::GetLogToStream(&stream));

  RTC_LOG(LS_INFO) << "INFO";
  RTC_LOG(LS_VERBOSE) << "VERBOSE";
  EXPECT_NE(std::string::npos, str.find("INFO"));
  EXPECT_EQ(std::string::npos, str.find("VERBOSE"));

  LogMessage::RemoveLogToStream(&stream);
  EXPECT_EQ(LS_NONE, LogMessage::GetLogToStream(&stream));

  EXPECT_EQ(sev, LogMessage::GetLogToStream(nullptr));
}

#if GTEST_HAS_DEATH_TEST && !defined(WEBRTC_ANDROID)
TEST(LogTest, Checks) {
  EXPECT_DEATH(FATAL() << "message",
               "\n\n#\n"
               "# Fatal error in: \\S+, line \\w+\n"
               "# last system error: \\w+\n"
               "# Check failed: FATAL\\(\\)\n"
               "# message");

  int a = 1, b = 2;
  EXPECT_DEATH(RTC_CHECK_EQ(a, b) << 1 << 2u,
               "\n\n#\n"
               "# Fatal error in: \\S+, line \\w+\n"
               "# last system error: \\w+\n"
               "# Check failed: a == b \\(1 vs. 2\\)\n"
               "# 12");
  RTC_CHECK_EQ(5, 5);

  RTC_CHECK(true) << "Shouldn't crash" << 1;
  EXPECT_DEATH(RTC_CHECK(false) << "Hi there!",
               "\n\n#\n"
               "# Fatal error in: \\S+, line \\w+\n"
               "# last system error: \\w+\n"
               "# Check failed: false\n"
               "# Hi there!");
}
#endif

// Test using multiple log streams. The INFO stream should get the INFO message,
// the VERBOSE stream should get the INFO and the VERBOSE.
// We should restore the correct global state at the end.
TEST(LogTest, MultipleStreams) {
  int sev = LogMessage::GetLogToStream(nullptr);

  std::string str1, str2;
  LogSinkImpl<StringStream> stream1(&str1), stream2(&str2);
  LogMessage::AddLogToStream(&stream1, LS_INFO);
  LogMessage::AddLogToStream(&stream2, LS_VERBOSE);
  EXPECT_EQ(LS_INFO, LogMessage::GetLogToStream(&stream1));
  EXPECT_EQ(LS_VERBOSE, LogMessage::GetLogToStream(&stream2));

  RTC_LOG(LS_INFO) << "INFO";
  RTC_LOG(LS_VERBOSE) << "VERBOSE";

  EXPECT_NE(std::string::npos, str1.find("INFO"));
  EXPECT_EQ(std::string::npos, str1.find("VERBOSE"));
  EXPECT_NE(std::string::npos, str2.find("INFO"));
  EXPECT_NE(std::string::npos, str2.find("VERBOSE"));

  LogMessage::RemoveLogToStream(&stream2);
  LogMessage::RemoveLogToStream(&stream1);
  EXPECT_EQ(LS_NONE, LogMessage::GetLogToStream(&stream2));
  EXPECT_EQ(LS_NONE, LogMessage::GetLogToStream(&stream1));

  EXPECT_EQ(sev, LogMessage::GetLogToStream(nullptr));
}

class LogThread {
 public:
  LogThread() : thread_(&ThreadEntry, this, "LogThread") {}
  ~LogThread() { thread_.Stop(); }

  void Start() { thread_.Start(); }

 private:
  void Run() {
    // LS_SENSITIVE by default to avoid cluttering up any real logging going on.
    RTC_LOG(LS_SENSITIVE) << "RTC_LOG";
  }

  static void ThreadEntry(void* p) { static_cast<LogThread*>(p)->Run(); }

  PlatformThread thread_;
  Event event_{false, false};
};

// Ensure we don't crash when adding/removing streams while threads are going.
// We should restore the correct global state at the end.
TEST(LogTest, MultipleThreads) {
  int sev = LogMessage::GetLogToStream(nullptr);

  LogThread thread1, thread2, thread3;
  thread1.Start();
  thread2.Start();
  thread3.Start();

  std::string s1, s2, s3;
  LogSinkImpl<StringStream> stream1(&s1), stream2(&s2), stream3(&s3);
  for (int i = 0; i < 1000; ++i) {
    LogMessage::AddLogToStream(&stream1, LS_INFO);
    LogMessage::AddLogToStream(&stream2, LS_VERBOSE);
    LogMessage::AddLogToStream(&stream3, LS_SENSITIVE);
    LogMessage::RemoveLogToStream(&stream1);
    LogMessage::RemoveLogToStream(&stream2);
    LogMessage::RemoveLogToStream(&stream3);
  }

  EXPECT_EQ(sev, LogMessage::GetLogToStream(nullptr));
}

TEST(LogTest, WallClockStartTime) {
  uint32_t time = LogMessage::WallClockStartTime();
  // Expect the time to be in a sensible range, e.g. > 2012-01-01.
  EXPECT_GT(time, 1325376000u);
}

TEST(LogTest, CheckExtraErrorField) {
  LogMessageForTesting log_msg("some/path/myfile.cc", 100, LS_WARNING,
                               ERRCTX_ERRNO, 0xD);
  ASSERT_FALSE(log_msg.is_noop());
  log_msg.stream() << "This gets added at dtor time";

  const std::string& extra = log_msg.get_extra();
  const size_t length_to_check = arraysize("[0x12345678]") - 1;
  ASSERT_GE(extra.length(), length_to_check);
  EXPECT_EQ(std::string("[0x0000000D]"), extra.substr(0, length_to_check));
}

TEST(LogTest, CheckFilePathParsed) {
  LogMessageForTesting log_msg("some/path/myfile.cc", 100, LS_INFO);
  ASSERT_FALSE(log_msg.is_noop());
  log_msg.stream() << "<- Does this look right?";

  const std::string stream = log_msg.GetPrintStream();
#if defined(WEBRTC_ANDROID)
  const char* tag = log_msg.get_tag();
  EXPECT_NE(nullptr, strstr(tag, "myfile.cc"));
  EXPECT_NE(std::string::npos, stream.find("100"));
#else
  EXPECT_NE(std::string::npos, stream.find("(myfile.cc:100)"));
#endif
}

#if defined(WEBRTC_ANDROID)
TEST(LogTest, CheckTagAddedToStringInDefaultOnLogMessageAndroid) {
  std::string str;
  LogSinkImpl<StringStream> stream(&str);
  LogMessage::AddLogToStream(&stream, LS_INFO);
  EXPECT_EQ(LS_INFO, LogMessage::GetLogToStream(&stream));

  RTC_LOG_TAG(LS_INFO, "my_tag") << "INFO";
  EXPECT_NE(std::string::npos, str.find("INFO"));
  EXPECT_NE(std::string::npos, str.find("my_tag"));
}
#endif

TEST(LogTest, CheckNoopLogEntry) {
  if (LogMessage::GetLogToDebug() <= LS_SENSITIVE) {
    printf("CheckNoopLogEntry: skipping. Global severity is being overridden.");
    return;
  }

  // Logging at LS_SENSITIVE severity, is by default turned off, so this should
  // be treated as a noop message.
  LogMessageForTesting log_msg("some/path/myfile.cc", 100, LS_SENSITIVE);
  log_msg.stream() << "Should be logged to nowhere.";
  EXPECT_TRUE(log_msg.is_noop());
  const std::string stream = log_msg.GetPrintStream();
  EXPECT_TRUE(stream.empty());
}

// Test the time required to write 1000 80-character logs to a string.
TEST(LogTest, Perf) {
  std::string str;
  LogSinkImpl<StringStream> stream(&str);
  LogMessage::AddLogToStream(&stream, LS_SENSITIVE);

  const std::string message(80, 'X');
  {
    // Just to be sure that we're not measuring the performance of logging
    // noop log messages.
    LogMessageForTesting sanity_check_msg(__FILE__, __LINE__, LS_SENSITIVE);
    ASSERT_FALSE(sanity_check_msg.is_noop());
  }

  // We now know how many bytes the logging framework will tag onto every msg.
  const size_t logging_overhead = str.size();
  // Reset the stream to 0 size.
  str.clear();
  str.reserve(120000);
  static const int kRepetitions = 1000;

  int64_t start = TimeMillis(), finish;
  for (int i = 0; i < kRepetitions; ++i) {
    LogMessageForTesting(__FILE__, __LINE__, LS_SENSITIVE).stream() << message;
  }
  finish = TimeMillis();

  LogMessage::RemoveLogToStream(&stream);
  stream.Close();

  EXPECT_EQ(str.size(), (message.size() + logging_overhead) * kRepetitions);
  RTC_LOG(LS_INFO) << "Total log time: " << TimeDiff(finish, start) << " ms "
                   << " total bytes logged: " << str.size();
}

}  // namespace rtc
