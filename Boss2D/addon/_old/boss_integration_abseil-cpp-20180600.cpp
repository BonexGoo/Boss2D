#include <boss.h>
#if BOSS_NEED_ADDON_WEBRTC

#include "boss_integration_abseil-cpp-20180600.h"

#include <addon/abseil-cpp-20180600_for_boss/absl/base/dynamic_annotations.cc>

#include <addon/abseil-cpp-20180600_for_boss/absl/base/internal/cycleclock.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/base/internal/low_level_alloc.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/base/internal/raw_logging.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/base/internal/spinlock.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/base/internal/spinlock_wait.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/base/internal/sysinfo.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/base/internal/thread_identity.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/base/internal/throw_delegate.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/base/internal/unscaledcycleclock.cc>

#include <addon/abseil-cpp-20180600_for_boss/absl/debugging/failure_signal_handler.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/debugging/leak_check.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/debugging/leak_check_disable.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/debugging/stacktrace.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/debugging/symbolize.cc>

#include <addon/abseil-cpp-20180600_for_boss/absl/debugging/internal/address_is_readable.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/debugging/internal/demangle.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/debugging/internal/elf_mem_image.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/debugging/internal/examine_stack.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/debugging/internal/stack_consumption.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/debugging/internal/vdso_support.cc>

#include <addon/abseil-cpp-20180600_for_boss/absl/numeric/int128.cc>

#include <addon/abseil-cpp-20180600_for_boss/absl/strings/ascii.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/strings/charconv.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/strings/escaping.cc>

#define CaseEqual CaseEqual_match_BOSS
#include <addon/abseil-cpp-20180600_for_boss/absl/strings/match.cc>
#undef CaseEqual

#define kAsciiToInt kAsciiToInt_numbers_BOSS
#define CaseEqual CaseEqual_numbers_BOSS
#include <addon/abseil-cpp-20180600_for_boss/absl/strings/numbers.cc>
#undef kAsciiToInt
#undef CaseEqual

#include <addon/abseil-cpp-20180600_for_boss/absl/strings/str_cat.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/strings/str_replace.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/strings/str_split.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/strings/string_view.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/strings/substitute.cc>

#include <addon/abseil-cpp-20180600_for_boss/absl/strings/internal/charconv_bigint.cc>

#define kAsciiToInt kAsciiToInt_charconv_parse_BOSS
#include <addon/abseil-cpp-20180600_for_boss/absl/strings/internal/charconv_parse.cc>
#undef kAsciiToInt

#include <addon/abseil-cpp-20180600_for_boss/absl/strings/internal/memutil.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/strings/internal/ostringstream.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/strings/internal/utf8.cc>

#define IsZero IsZero_barrier_BOSS
#include <addon/abseil-cpp-20180600_for_boss/absl/synchronization/barrier.cc>
#undef IsZero

#define IsZero IsZero_blocking_counter_BOSS
#include <addon/abseil-cpp-20180600_for_boss/absl/synchronization/blocking_counter.cc>
#undef IsZero

#include <addon/abseil-cpp-20180600_for_boss/absl/synchronization/mutex.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/synchronization/notification.cc>

#include <addon/abseil-cpp-20180600_for_boss/absl/synchronization/internal/create_thread_identity.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/synchronization/internal/graphcycles.cc>
//#include <addon/abseil-cpp-20180600_for_boss/absl/synchronization/internal/mutex_nonprod.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/synchronization/internal/per_thread_sem.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/synchronization/internal/waiter.cc>

#include <addon/abseil-cpp-20180600_for_boss/absl/time/clock.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/time/duration.cc>

#define unix_epoch unix_epoch_format_BOSS
#include <addon/abseil-cpp-20180600_for_boss/absl/time/format.cc>
#undef unix_epoch

#define unix_epoch unix_epoch_time_BOSS
#include <addon/abseil-cpp-20180600_for_boss/absl/time/time.cc>
#undef unix_epoch

#include <addon/abseil-cpp-20180600_for_boss/absl/time/internal/cctz/src/civil_time_detail.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/time/internal/cctz/src/time_zone_fixed.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/time/internal/cctz/src/time_zone_format.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/time/internal/cctz/src/time_zone_if.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/time/internal/cctz/src/time_zone_impl.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/time/internal/cctz/src/time_zone_info.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/time/internal/cctz/src/time_zone_libc.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/time/internal/cctz/src/time_zone_lookup.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/time/internal/cctz/src/time_zone_posix.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/time/internal/cctz/src/zone_info_source.cc>

#include <addon/abseil-cpp-20180600_for_boss/absl/types/bad_any_cast.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/types/bad_optional_access.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/types/bad_variant_access.cc>
#include <addon/abseil-cpp-20180600_for_boss/absl/types/optional.cc>

#endif
