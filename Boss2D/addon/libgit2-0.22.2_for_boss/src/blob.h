/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#ifndef INCLUDE_blob_h__
#define INCLUDE_blob_h__

#include BOSS_LIBGIT2_U_git2__blob_h //original-code:"git2/blob.h"
#include BOSS_LIBGIT2_U_repository_h //original-code:"repository.h"
#include BOSS_LIBGIT2_U_odb_h //original-code:"odb.h"
#include "fileops.h"

struct git_blob {
	git_object object;
	git_odb_object *odb_object;
};

void git_blob__free(void *blob);
int git_blob__parse(void *blob, git_odb_object *obj);
int git_blob__getbuf(git_buf *buffer, git_blob *blob);

extern int git_blob__create_from_paths(
	git_oid *out_oid,
	struct stat *out_st,
	git_repository *repo,
	const char *full_path,
	const char *hint_path,
	mode_t hint_mode,
	bool apply_filters);

#endif
