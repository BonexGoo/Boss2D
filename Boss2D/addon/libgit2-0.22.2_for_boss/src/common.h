/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#ifndef INCLUDE_common_h__
#define INCLUDE_common_h__

#include BOSS_LIBGIT2_U_git2__common_h //original-code:"git2/common.h"
#include "cc-compat.h"

/** Declare a function as always inlined. */
#if defined(_MSC_VER)
# define GIT_INLINE(type) static __inline type
#else
# define GIT_INLINE(type) static inline type
#endif

#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#ifdef GIT_WIN32

# include BOSS_FAKEWIN_V_io_h //original-code:<io.h>
# include BOSS_FAKEWIN_V_direct_h //original-code:<direct.h>
# include BOSS_FAKEWIN_V_winsock2_h //original-code:<winsock2.h>
# include BOSS_FAKEWIN_V_windows_h //original-code:<windows.h>
# include BOSS_FAKEWIN_V_ws2tcpip_h //original-code:<ws2tcpip.h>
# include BOSS_LIBGIT2_U_win32__msvc_M_compat_h //original-code:"win32/msvc-compat.h"
# include BOSS_LIBGIT2_U_win32__mingw_M_compat_h //original-code:"win32/mingw-compat.h"
# include "win32/error.h"
# include "win32/version.h"
# ifdef GIT_THREADS
#	include BOSS_LIBGIT2_U_win32__pthread_h //original-code:"win32/pthread.h"
# endif

#else

# include <unistd.h>
# include <strings.h>
# ifdef GIT_THREADS
#	include <pthread.h>
#	include <sched.h>
# endif
#define GIT_STDLIB_CALL

# include <arpa/inet.h>

#endif

#include BOSS_LIBGIT2_U_git2__types_h //original-code:"git2/types.h"
#include BOSS_LIBGIT2_U_git2__errors_h //original-code:"git2/errors.h"
#include "thread-utils.h"
#include "bswap.h"

#include BOSS_LIBGIT2_V_regex_h //original-code:<regex.h>

/**
 * Check a pointer allocation result, returning -1 if it failed.
 */
#define GITERR_CHECK_ALLOC(ptr) if (ptr == NULL) { return -1; }

/**
 * Check a return value and propagate result if non-zero.
 */
#define GITERR_CHECK_ERROR(code) \
	do { int _err = (code); if (_err) return _err; } while (0)

/**
 * Set the error message for this thread, formatting as needed.
 */
void giterr_set(int error_class, const char *string, ...);

/**
 * Set the error message for a regex failure, using the internal regex
 * error code lookup and return a libgit error code.
 */
int giterr_set_regex(const regex_t *regex, int error_code);

/**
 * Set error message for user callback if needed.
 *
 * If the error code in non-zero and no error message is set, this
 * sets a generic error message.
 *
 * @return This always returns the `error_code` parameter.
 */
GIT_INLINE(int) giterr_set_after_callback_function(
	int error_code, const char *action)
{
	if (error_code) {
		const git_error *e = giterr_last();
		if (!e || !e->message)
			giterr_set(e ? e->klass : GITERR_CALLBACK,
				"%s callback returned %d", action, error_code);
	}
	return error_code;
}

#ifdef GIT_WIN32
#define giterr_set_after_callback(code) \
	giterr_set_after_callback_function((code), __FUNCTION__)
#else
#define giterr_set_after_callback(code) \
	giterr_set_after_callback_function((code), __func__)
#endif

/**
 * Gets the system error code for this thread.
 */
int giterr_system_last(void);

/**
 * Sets the system error code for this thread.
 */
void giterr_system_set(int code);

/**
 * Structure to preserve libgit2 error state
 */
typedef struct {
	int       error_code;
	git_error error_msg;
} git_error_state;

/**
 * Capture current error state to restore later, returning error code.
 * If `error_code` is zero, this does nothing and returns zero.
 */
int giterr_capture(git_error_state *state, int error_code);

/**
 * Restore error state to a previous value, returning saved error code.
 */
int giterr_restore(git_error_state *state);

/**
 * Check a versioned structure for validity
 */
GIT_INLINE(int) giterr__check_version(const void *structure, unsigned int expected_max, const char *name)
{
	unsigned int actual;

	if (!structure)
		return 0;

	actual = *(const unsigned int*)structure;
	if (actual > 0 && actual <= expected_max)
		return 0;

	giterr_set(GITERR_INVALID, "Invalid version %d on %s", actual, name);
	return -1;
}
#define GITERR_CHECK_VERSION(S,V,N) if (giterr__check_version(S,V,N) < 0) return -1

/**
 * Initialize a structure with a version.
 */
GIT_INLINE(void) git__init_structure(void *structure, size_t len, unsigned int version)
{
	memset(structure, 0, len);
	*((int*)structure) = version;
}
#define GIT_INIT_STRUCTURE(S,V) git__init_structure(S, sizeof(*S), V)

#define GIT_INIT_STRUCTURE_FROM_TEMPLATE(PTR,VERSION,TYPE,TPL) do { \
	TYPE _tmpl = TPL; \
	GITERR_CHECK_VERSION(&(VERSION), _tmpl.version, #TYPE);	\
	memcpy((PTR), &_tmpl, sizeof(_tmpl)); } while (0)

/* NOTE: other giterr functions are in the public errors.h header file */

#include BOSS_LIBGIT2_U_util_h //original-code:"util.h"

#endif /* INCLUDE_common_h__ */
