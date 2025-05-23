/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */

#ifndef INCLUDE_mwindow__
#define INCLUDE_mwindow__

#include BOSS_LIBGIT2_U_map_h //original-code:"map.h"
#include BOSS_LIBGIT2_U_vector_h //original-code:"vector.h"

typedef struct git_mwindow {
	struct git_mwindow *next;
	git_map window_map;
	git_off_t offset;
	size_t last_used;
	size_t inuse_cnt;
} git_mwindow;

typedef struct git_mwindow_file {
	git_mwindow *windows;
	int fd;
	git_off_t size;
} git_mwindow_file;

typedef struct git_mwindow_ctl {
	size_t mapped;
	unsigned int open_windows;
	unsigned int mmap_calls;
	unsigned int peak_open_windows;
	size_t peak_mapped;
	size_t used_ctr;
	git_vector windowfiles;
} git_mwindow_ctl;

int git_mwindow_contains(git_mwindow *win, git_off_t offset);
void git_mwindow_free_all(git_mwindow_file *mwf); /* locks */
void git_mwindow_free_all_locked(git_mwindow_file *mwf); /* run under lock */
unsigned char *git_mwindow_open(git_mwindow_file *mwf, git_mwindow **cursor, git_off_t offset, size_t extra, unsigned int *left);
int git_mwindow_file_register(git_mwindow_file *mwf);
void git_mwindow_file_deregister(git_mwindow_file *mwf);
void git_mwindow_close(git_mwindow **w_cursor);

int git_mwindow_files_init(void);
void git_mwindow_files_free(void);

struct git_pack_file; /* just declaration to avoid cyclical includes */
int git_mwindow_get_pack(struct git_pack_file **out, const char *path);
void git_mwindow_put_pack(struct git_pack_file *pack);

#endif
