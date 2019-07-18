#include <assert.h>
#include <errno.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <stdarg.h>

#include <sys/types.h>
#include <sys/stat.h>

#include BOSS_LIBGIT2_V_regex_h //original-code:<regex.h>

#include BOSS_FAKEWIN_V_io_h //original-code:<io.h>
#include BOSS_FAKEWIN_V_direct_h //original-code:<direct.h>
#ifdef GIT_THREADS
 #include BOSS_LIBGIT2_U_win32__pthread_h //original-code:"win32/pthread.h"
#endif

#include BOSS_LIBGIT2_U_git2_h //original-code:"git2.h"
#include BOSS_LIBGIT2_U_common_h //original-code:"common.h"
