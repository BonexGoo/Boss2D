/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#ifndef INCLUDE_refspec_h__
#define INCLUDE_refspec_h__

#include BOSS_LIBGIT2_U_git2__refspec_h //original-code:"git2/refspec.h"
#include BOSS_LIBGIT2_U_buffer_h //original-code:"buffer.h"
#include BOSS_LIBGIT2_U_vector_h //original-code:"vector.h"

struct git_refspec {
	char *string;
	char *src;
	char *dst;
	unsigned int force :1,
		push : 1,
		pattern :1,
		matching :1;
};

#define GIT_REFSPEC_TAGS "refs/tags/*:refs/tags/*"

int git_refspec__parse(
	struct git_refspec *refspec,
	const char *str,
	bool is_fetch);

void git_refspec__free(git_refspec *refspec);

int git_refspec__serialize(git_buf *out, const git_refspec *refspec);

/**
 * Determines if a refspec is a wildcard refspec.
 *
 * @param spec the refspec
 * @return 1 if the refspec is a wildcard, 0 otherwise
 */
int git_refspec_is_wildcard(const git_refspec *spec);

/**
 * DWIM `spec` with `refs` existing on the remote, append the dwim'ed
 * result in `out`.
 */
int git_refspec__dwim_one(git_vector *out, git_refspec *spec, git_vector *refs);

#endif
