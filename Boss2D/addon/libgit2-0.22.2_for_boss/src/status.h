/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#ifndef INCLUDE_status_h__
#define INCLUDE_status_h__

#include "diff.h"
#include BOSS_LIBGIT2_U_git2__status_h //original-code:"git2/status.h"
#include BOSS_LIBGIT2_U_git2__diff_h //original-code:"git2/diff.h"

struct git_status_list {
	git_status_options opts;

	git_diff *head2idx;
	git_diff *idx2wd;

	git_vector paired;
};

#endif
