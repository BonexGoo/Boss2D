/*
 *  Copyright 2016 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include BOSS_WEBRTC_U_rtc_base__task_queue_win_h //original-code:"rtc_base/task_queue_win.h"

// clang-format off
// clang formating would change include order.

// Include winsock2.h before including <windows.h> to maintain consistency with
// win32.h. To include win32.h directly, it must be broken out into its own
// build target.
#include <winsock2.h>
#include <windows.h>
#include <sal.h>       // Must come after windows headers.
#include <mmsystem.h>  // Must come after windows headers.
// clang-format on
#include <string.h>

#include <algorithm>
#include <memory>
#include <queue>
#include <utility>

#include BOSS_ABSEILCPP_U_absl__strings__string_view_h //original-code:"absl/strings/string_view.h"
#include BOSS_ABSEILCPP_U_absl__types__optional_h //original-code:"absl/types/optional.h"
#include BOSS_WEBRTC_U_api__task_queue__queued_task_h //original-code:"api/task_queue/queued_task.h"
#include BOSS_WEBRTC_U_api__task_queue__task_queue_base_h //original-code:"api/task_queue/task_queue_base.h"
#include BOSS_WEBRTC_U_rtc_base__arraysize_h //original-code:"rtc_base/arraysize.h"
#include BOSS_WEBRTC_U_rtc_base__checks_h //original-code:"rtc_base/checks.h"
#include BOSS_WEBRTC_U_rtc_base__constructor_magic_h //original-code:"rtc_base/constructor_magic.h"
#include BOSS_WEBRTC_U_rtc_base__event_h //original-code:"rtc_base/event.h"
#include BOSS_WEBRTC_U_rtc_base__logging_h //original-code:"rtc_base/logging.h"
#include BOSS_WEBRTC_U_rtc_base__numerics__safe_conversions_h //original-code:"rtc_base/numerics/safe_conversions.h"
#include BOSS_WEBRTC_U_rtc_base__platform_thread_h //original-code:"rtc_base/platform_thread.h"
#include BOSS_WEBRTC_U_rtc_base__synchronization__mutex_h //original-code:"rtc_base/synchronization/mutex.h"
#include BOSS_WEBRTC_U_rtc_base__time_utils_h //original-code:"rtc_base/time_utils.h"

namespace webrtc {
namespace {
#define WM_RUN_TASK WM_USER + 1
#define WM_QUEUE_DELAYED_TASK WM_USER + 2

void CALLBACK InitializeQueueThread(ULONG_PTR param) {
  MSG msg;
  ::PeekMessage(&msg, nullptr, WM_USER, WM_USER, PM_NOREMOVE);
  rtc::Event* data = reinterpret_cast<rtc::Event*>(param);
  data->Set();
}

rtc::ThreadPriority TaskQueuePriorityToThreadPriority(
    TaskQueueFactory::Priority priority) {
  switch (priority) {
    case TaskQueueFactory::Priority::HIGH:
      return rtc::ThreadPriority::kRealtime;
    case TaskQueueFactory::Priority::LOW:
      return rtc::ThreadPriority::kLow;
    case TaskQueueFactory::Priority::NORMAL:
      return rtc::ThreadPriority::kNormal;
  }
}

int64_t GetTick() {
  static const UINT kPeriod = 1;
  bool high_res = (timeBeginPeriod(kPeriod) == TIMERR_NOERROR);
  int64_t ret = rtc::TimeMillis();
  if (high_res)
    timeEndPeriod(kPeriod);
  return ret;
}

class DelayedTaskInfo {
 public:
  // Default ctor needed to support priority_queue::pop().
  DelayedTaskInfo() {}
  DelayedTaskInfo(uint32_t milliseconds, std::unique_ptr<QueuedTask> task)
      : due_time_(GetTick() + milliseconds), task_(std::move(task)) {}
  DelayedTaskInfo(DelayedTaskInfo&&) = default;

  // Implement for priority_queue.
  bool operator>(const DelayedTaskInfo& other) const {
    return due_time_ > other.due_time_;
  }

  // Required by priority_queue::pop().
  DelayedTaskInfo& operator=(DelayedTaskInfo&& other) = default;

  // See below for why this method is const.
  void Run() const {
    RTC_DCHECK(due_time_);
    task_->Run() ? task_.reset() : static_cast<void>(task_.release());
  }

  int64_t due_time() const { return due_time_; }

 private:
  int64_t due_time_ = 0;  // Absolute timestamp in milliseconds.

  // |task| needs to be mutable because std::priority_queue::top() returns
  // a const reference and a key in an ordered queue must not be changed.
  // There are two basic workarounds, one using const_cast, which would also
  // make the key (|due_time|), non-const and the other is to make the non-key
  // (|task|), mutable.
  // Because of this, the |task| variable is made private and can only be
  // mutated by calling the |Run()| method.
  mutable std::unique_ptr<QueuedTask> task_;
};

class MultimediaTimer {
 public:
  // Note: We create an event that requires manual reset.
  MultimediaTimer() : event_(::CreateEvent(nullptr, true, false, nullptr)) {}

  ~MultimediaTimer() {
    Cancel();
    ::CloseHandle(event_);
  }

  bool StartOneShotTimer(UINT delay_ms) {
    RTC_DCHECK_EQ(0, timer_id_);
    RTC_DCHECK(event_ != nullptr);
    timer_id_ =
        ::timeSetEvent(delay_ms, 0, reinterpret_cast<LPTIMECALLBACK>(event_), 0,
                       TIME_ONESHOT | TIME_CALLBACK_EVENT_SET);
    return timer_id_ != 0;
  }

  void Cancel() {
    if (timer_id_) {
      ::timeKillEvent(timer_id_);
      timer_id_ = 0;
    }
    // Now that timer is killed and not able to set the event, reset the event.
    // Doing it in opposite order is racy because event may be set between
    // event was reset and timer is killed leaving MultimediaTimer in surprising
    // state where both event is set and timer is canceled.
    ::ResetEvent(event_);
  }

  HANDLE* event_for_wait() { return &event_; }

 private:
  HANDLE event_ = nullptr;
  MMRESULT timer_id_ = 0;

  RTC_DISALLOW_COPY_AND_ASSIGN(MultimediaTimer);
};

class TaskQueueWin : public TaskQueueBase {
 public:
  TaskQueueWin(absl::string_view queue_name, rtc::ThreadPriority priority);
  ~TaskQueueWin() override = default;

  void Delete() override;
  void PostTask(std::unique_ptr<QueuedTask> task) override;
  void PostDelayedTask(std::unique_ptr<QueuedTask> task,
                       uint32_t milliseconds) override;

  void RunPendingTasks();

 private:
  void RunThreadMain();
  bool ProcessQueuedMessages();
  void RunDueTasks();
  void ScheduleNextTimer();
  void CancelTimers();

  // Since priority_queue<> by defult orders items in terms of
  // largest->smallest, using std::less<>, and we want smallest->largest,
  // we would like to use std::greater<> here. Alas it's only available in
  // C++14 and later, so we roll our own compare template that that relies on
  // operator<().
  template <typename T>
  struct greater {
    bool operator()(const T& l, const T& r) { return l > r; }
  };

  MultimediaTimer timer_;
  std::priority_queue<DelayedTaskInfo,
                      std::vector<DelayedTaskInfo>,
                      greater<DelayedTaskInfo>>
      timer_tasks_;
  UINT_PTR timer_id_ = 0;
  rtc::PlatformThread thread_;
  Mutex pending_lock_;
  std::queue<std::unique_ptr<QueuedTask>> pending_
      RTC_GUARDED_BY(pending_lock_);
  HANDLE in_queue_;
};

TaskQueueWin::TaskQueueWin(absl::string_view queue_name,
                           rtc::ThreadPriority priority)
    : in_queue_(::CreateEvent(nullptr, true, false, nullptr)) {
  RTC_DCHECK(in_queue_);
  thread_ = rtc::PlatformThread::SpawnJoinable(
      [this] { RunThreadMain(); }, queue_name,
      rtc::ThreadAttributes().SetPriority(priority));

  rtc::Event event(false, false);
  RTC_CHECK(thread_.QueueAPC(&InitializeQueueThread,
                             reinterpret_cast<ULONG_PTR>(&event)));
  event.Wait(rtc::Event::kForever);
}

void TaskQueueWin::Delete() {
  RTC_DCHECK(!IsCurrent());
  RTC_CHECK(thread_.GetHandle() != absl::nullopt);
  while (
      !::PostThreadMessage(GetThreadId(*thread_.GetHandle()), WM_QUIT, 0, 0)) {
    RTC_CHECK_EQ(ERROR_NOT_ENOUGH_QUOTA, ::GetLastError());
    Sleep(1);
  }
  thread_.Finalize();
  ::CloseHandle(in_queue_);
  delete this;
}

void TaskQueueWin::PostTask(std::unique_ptr<QueuedTask> task) {
  MutexLock lock(&pending_lock_);
  pending_.push(std::move(task));
  ::SetEvent(in_queue_);
}

void TaskQueueWin::PostDelayedTask(std::unique_ptr<QueuedTask> task,
                                   uint32_t milliseconds) {
  if (!milliseconds) {
    PostTask(std::move(task));
    return;
  }

  // TODO(tommi): Avoid this allocation.  It is currently here since
  // the timestamp stored in the task info object, is a 64bit timestamp
  // and WPARAM is 32bits in 32bit builds.  Otherwise, we could pass the
  // task pointer and timestamp as LPARAM and WPARAM.
  auto* task_info = new DelayedTaskInfo(milliseconds, std::move(task));
  RTC_CHECK(thread_.GetHandle() != absl::nullopt);
  if (!::PostThreadMessage(GetThreadId(*thread_.GetHandle()),
                           WM_QUEUE_DELAYED_TASK, 0,
                           reinterpret_cast<LPARAM>(task_info))) {
    delete task_info;
  }
}

void TaskQueueWin::RunPendingTasks() {
  while (true) {
    std::unique_ptr<QueuedTask> task;
    {
      MutexLock lock(&pending_lock_);
      if (pending_.empty())
        break;
      task = std::move(pending_.front());
      pending_.pop();
    }

    if (!task->Run())
      task.release();
  }
}

void TaskQueueWin::RunThreadMain() {
  CurrentTaskQueueSetter set_current(this);
  HANDLE handles[2] = {*timer_.event_for_wait(), in_queue_};
  while (true) {
    // Make sure we do an alertable wait as that's required to allow APCs to run
    // (e.g. required for InitializeQueueThread and stopping the thread in
    // PlatformThread).
    DWORD result = ::MsgWaitForMultipleObjectsEx(
        arraysize(handles), handles, INFINITE, QS_ALLEVENTS, MWMO_ALERTABLE);
    RTC_CHECK_NE(WAIT_FAILED, result);
    if (result == (WAIT_OBJECT_0 + 2)) {
      // There are messages in the message queue that need to be handled.
      if (!ProcessQueuedMessages())
        break;
    }

    if (result == WAIT_OBJECT_0 ||
        (!timer_tasks_.empty() &&
         ::WaitForSingleObject(*timer_.event_for_wait(), 0) == WAIT_OBJECT_0)) {
      // The multimedia timer was signaled.
      timer_.Cancel();
      RunDueTasks();
      ScheduleNextTimer();
    }

    if (result == (WAIT_OBJECT_0 + 1)) {
      ::ResetEvent(in_queue_);
      RunPendingTasks();
    }
  }
}

bool TaskQueueWin::ProcessQueuedMessages() {
  MSG msg = {};
  // To protect against overly busy message queues, we limit the time
  // we process tasks to a few milliseconds. If we don't do that, there's
  // a chance that timer tasks won't ever run.
  static const int kMaxTaskProcessingTimeMs = 500;
  auto start = GetTick();
  while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) &&
         msg.message != WM_QUIT) {
    if (!msg.hwnd) {
      switch (msg.message) {
        // TODO(tommi): Stop using this way of queueing tasks.
        case WM_RUN_TASK: {
          QueuedTask* task = reinterpret_cast<QueuedTask*>(msg.lParam);
          if (task->Run())
            delete task;
          break;
        }
        case WM_QUEUE_DELAYED_TASK: {
          std::unique_ptr<DelayedTaskInfo> info(
              reinterpret_cast<DelayedTaskInfo*>(msg.lParam));
          bool need_to_schedule_timers =
              timer_tasks_.empty() ||
              timer_tasks_.top().due_time() > info->due_time();
          timer_tasks_.emplace(std::move(*info.get()));
          if (need_to_schedule_timers) {
            CancelTimers();
            ScheduleNextTimer();
          }
          break;
        }
        case WM_TIMER: {
          RTC_DCHECK_EQ(timer_id_, msg.wParam);
          ::KillTimer(nullptr, msg.wParam);
          timer_id_ = 0;
          RunDueTasks();
          ScheduleNextTimer();
          break;
        }
        default:
          RTC_NOTREACHED();
          break;
      }
    } else {
      ::TranslateMessage(&msg);
      ::DispatchMessage(&msg);
    }

    if (GetTick() > start + kMaxTaskProcessingTimeMs)
      break;
  }
  return msg.message != WM_QUIT;
}

void TaskQueueWin::RunDueTasks() {
  RTC_DCHECK(!timer_tasks_.empty());
  auto now = GetTick();
  do {
    const auto& top = timer_tasks_.top();
    if (top.due_time() > now)
      break;
    top.Run();
    timer_tasks_.pop();
  } while (!timer_tasks_.empty());
}

void TaskQueueWin::ScheduleNextTimer() {
  RTC_DCHECK_EQ(timer_id_, 0);
  if (timer_tasks_.empty())
    return;

  const auto& next_task = timer_tasks_.top();
  int64_t delay_ms = std::max(0ll, next_task.due_time() - GetTick());
  uint32_t milliseconds = rtc::dchecked_cast<uint32_t>(delay_ms);
  if (!timer_.StartOneShotTimer(milliseconds))
    timer_id_ = ::SetTimer(nullptr, 0, milliseconds, nullptr);
}

void TaskQueueWin::CancelTimers() {
  timer_.Cancel();
  if (timer_id_) {
    ::KillTimer(nullptr, timer_id_);
    timer_id_ = 0;
  }
}

class TaskQueueWinFactory : public TaskQueueFactory {
 public:
  std::unique_ptr<TaskQueueBase, TaskQueueDeleter> CreateTaskQueue(
      absl::string_view name,
      Priority priority) const override {
    return std::unique_ptr<TaskQueueBase, TaskQueueDeleter>(
        new TaskQueueWin(name, TaskQueuePriorityToThreadPriority(priority)));
  }
};

}  // namespace

std::unique_ptr<TaskQueueFactory> CreateTaskQueueWinFactory() {
  return std::make_unique<TaskQueueWinFactory>();
}

}  // namespace webrtc
