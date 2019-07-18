// Copyright 2009 The RE2 Authors.  All Rights Reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

#include BOSS_OPENALPR_U_re2__util__util_h //original-code:"re2/util/util.h"
#ifndef _WIN32
#include BOSS_OPENALPR_U_re2__util__valgrind_h //original-code:"re2/util/valgrind.h"
#endif

namespace re2 {

#ifndef __has_feature
#define __has_feature(x) 0
#endif

int RunningOnValgrind() {
#if __has_feature(memory_sanitizer)
	return true;
#elif defined(RUNNING_ON_VALGRIND)
	return RUNNING_ON_VALGRIND;
#else
	return 0;
#endif
}

}  // namespace re2
