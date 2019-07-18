/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#ifndef INCLUDE_zstream_h__
#define INCLUDE_zstream_h__

#include BOSS_LIBGIT2_V_zlib_h //original-code:<zlib.h>

#include BOSS_LIBGIT2_U_common_h //original-code:"common.h"
#include BOSS_LIBGIT2_U_buffer_h //original-code:"buffer.h"

typedef struct {
	z_stream z;
	const char *in;
	size_t in_len;
	int zerr;
} git_zstream;

#define GIT_ZSTREAM_INIT {{0}}

int git_zstream_init(git_zstream *zstream);
void git_zstream_free(git_zstream *zstream);

int git_zstream_set_input(git_zstream *zstream, const void *in, size_t in_len);

size_t git_zstream_suggest_output_len(git_zstream *zstream);

int git_zstream_get_output(void *out, size_t *out_len, git_zstream *zstream);

bool git_zstream_done(git_zstream *zstream);

void git_zstream_reset(git_zstream *zstream);

int git_zstream_deflatebuf(git_buf *out, const void *in, size_t in_len);

#endif /* INCLUDE_zstream_h__ */
