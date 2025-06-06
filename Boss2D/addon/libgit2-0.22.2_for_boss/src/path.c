/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#include BOSS_LIBGIT2_U_common_h //original-code:"common.h"
#include BOSS_LIBGIT2_U_path_h //original-code:"path.h"
#include BOSS_LIBGIT2_U_posix_h //original-code:"posix.h"
#include BOSS_LIBGIT2_U_repository_h //original-code:"repository.h"
#ifdef GIT_WIN32
#include "win32/posix.h"
#include "win32/w32_util.h"
#else
#include <dirent.h>
#endif
#include <stdio.h>
#include <ctype.h>

#define LOOKS_LIKE_DRIVE_PREFIX(S) (git__isalpha((S)[0]) && (S)[1] == ':')

#ifdef GIT_WIN32
static bool looks_like_network_computer_name(const char *path, int pos)
{
	if (pos < 3)
		return false;

	if (path[0] != '/' || path[1] != '/')
		return false;

	while (pos-- > 2) {
		if (path[pos] == '/')
			return false;
	}

	return true;
}
#endif

/*
 * Based on the Android implementation, BSD licensed.
 * http://android.git.kernel.org/
 *
 * Copyright (C) 2008 The Android Open Source Project
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in
 *   the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * AS IS AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
int git_path_basename_r(git_buf *buffer, const char *path)
{
	const char *endp, *startp;
	int len, result;

	/* Empty or NULL string gets treated as "." */
	if (path == NULL || *path == '\0') {
		startp = ".";
		len		= 1;
		goto Exit;
	}

	/* Strip trailing slashes */
	endp = path + strlen(path) - 1;
	while (endp > path && *endp == '/')
		endp--;

	/* All slashes becomes "/" */
	if (endp == path && *endp == '/') {
		startp = "/";
		len	= 1;
		goto Exit;
	}

	/* Find the start of the base */
	startp = endp;
	while (startp > path && *(startp - 1) != '/')
		startp--;

	/* Cast is safe because max path < max int */
	len = (int)(endp - startp + 1);

Exit:
	result = len;

	if (buffer != NULL && git_buf_set(buffer, startp, len) < 0)
		return -1;

	return result;
}

/*
 * Based on the Android implementation, BSD licensed.
 * Check http://android.git.kernel.org/
 */
int git_path_dirname_r(git_buf *buffer, const char *path)
{
	const char *endp;
	int result, len;

	/* Empty or NULL string gets treated as "." */
	if (path == NULL || *path == '\0') {
		path = ".";
		len = 1;
		goto Exit;
	}

	/* Strip trailing slashes */
	endp = path + strlen(path) - 1;
	while (endp > path && *endp == '/')
		endp--;

	/* Find the start of the dir */
	while (endp > path && *endp != '/')
		endp--;

	/* Either the dir is "/" or there are no slashes */
	if (endp == path) {
		path = (*endp == '/') ? "/" : ".";
		len = 1;
		goto Exit;
	}

	do {
		endp--;
	} while (endp > path && *endp == '/');

	/* Cast is safe because max path < max int */
	len = (int)(endp - path + 1);

#ifdef GIT_WIN32
	/* Mimic unix behavior where '/.git' returns '/': 'C:/.git' will return
		'C:/' here */

	if (len == 2 && LOOKS_LIKE_DRIVE_PREFIX(path)) {
		len = 3;
		goto Exit;
	}

	/* Similarly checks if we're dealing with a network computer name
		'//computername/.git' will return '//computername/' */

	if (looks_like_network_computer_name(path, len)) {
		len++;
		goto Exit;
	}

#endif

Exit:
	result = len;

	if (buffer != NULL && git_buf_set(buffer, path, len) < 0)
		return -1;

	return result;
}


char *git_path_dirname(const char *path)
{
	git_buf buf = GIT_BUF_INIT;
	char *dirname;

	git_path_dirname_r(&buf, path);
	dirname = git_buf_detach(&buf);
	git_buf_free(&buf); /* avoid memleak if error occurs */

	return dirname;
}

char *git_path_basename(const char *path)
{
	git_buf buf = GIT_BUF_INIT;
	char *basename;

	git_path_basename_r(&buf, path);
	basename = git_buf_detach(&buf);
	git_buf_free(&buf); /* avoid memleak if error occurs */

	return basename;
}

size_t git_path_basename_offset(git_buf *buffer)
{
	ssize_t slash;

	if (!buffer || buffer->size <= 0)
		return 0;

	slash = git_buf_rfind_next(buffer, '/');

	if (slash >= 0 && buffer->ptr[slash] == '/')
		return (size_t)(slash + 1);

	return 0;
}

const char *git_path_topdir(const char *path)
{
	size_t len;
	ssize_t i;

	assert(path);
	len = strlen(path);

	if (!len || path[len - 1] != '/')
		return NULL;

	for (i = (ssize_t)len - 2; i >= 0; --i)
		if (path[i] == '/')
			break;

	return &path[i + 1];
}

int git_path_root(const char *path)
{
	int offset = 0;

	/* Does the root of the path look like a windows drive ? */
	if (LOOKS_LIKE_DRIVE_PREFIX(path))
		offset += 2;

#ifdef GIT_WIN32
	/* Are we dealing with a windows network path? */
	else if ((path[0] == '/' && path[1] == '/' && path[2] != '/') ||
		(path[0] == '\\' && path[1] == '\\' && path[2] != '\\'))
	{
		offset += 2;

		/* Skip the computer name segment */
		while (path[offset] && path[offset] != '/' && path[offset] != '\\')
			offset++;
	}
#endif

	if (path[offset] == '/' || path[offset] == '\\')
		return offset;

	return -1;	/* Not a real error - signals that path is not rooted */
}

int git_path_join_unrooted(
	git_buf *path_out, const char *path, const char *base, ssize_t *root_at)
{
	ssize_t root;

	assert(path && path_out);

	root = (ssize_t)git_path_root(path);

	if (base != NULL && root < 0) {
		if (git_buf_joinpath(path_out, base, path) < 0)
			return -1;

		root = (ssize_t)strlen(base);
	} else {
		if (git_buf_sets(path_out, path) < 0)
			return -1;

		if (root < 0)
			root = 0;
		else if (base)
			git_path_equal_or_prefixed(base, path, &root);
	}

	if (root_at)
		*root_at = root;

	return 0;
}

int git_path_prettify(git_buf *path_out, const char *path, const char *base)
{
	char buf[GIT_PATH_MAX];

	assert(path && path_out);

	/* construct path if needed */
	if (base != NULL && git_path_root(path) < 0) {
		if (git_buf_joinpath(path_out, base, path) < 0)
			return -1;
		path = path_out->ptr;
	}

	if (p_realpath(path, buf) == NULL) {
		/* giterr_set resets the errno when dealing with a GITERR_OS kind of error */
		int error = (errno == ENOENT || errno == ENOTDIR) ? GIT_ENOTFOUND : -1;
		giterr_set(GITERR_OS, "Failed to resolve path '%s'", path);

		git_buf_clear(path_out);

		return error;
	}

	return git_buf_sets(path_out, buf);
}

int git_path_prettify_dir(git_buf *path_out, const char *path, const char *base)
{
	int error = git_path_prettify(path_out, path, base);
	return (error < 0) ? error : git_path_to_dir(path_out);
}

int git_path_to_dir(git_buf *path)
{
	if (path->asize > 0 &&
		git_buf_len(path) > 0 &&
		path->ptr[git_buf_len(path) - 1] != '/')
		git_buf_putc(path, '/');

	return git_buf_oom(path) ? -1 : 0;
}

void git_path_string_to_dir(char* path, size_t size)
{
	size_t end = strlen(path);

	if (end && path[end - 1] != '/' && end < size) {
		path[end] = '/';
		path[end + 1] = '\0';
	}
}

int git__percent_decode(git_buf *decoded_out, const char *input)
{
	int len, hi, lo, i;
	assert(decoded_out && input);

	len = (int)strlen(input);
	git_buf_clear(decoded_out);

	for(i = 0; i < len; i++)
	{
		char c = input[i];

		if (c != '%')
			goto append;

		if (i >= len - 2)
			goto append;

		hi = git__fromhex(input[i + 1]);
		lo = git__fromhex(input[i + 2]);

		if (hi < 0 || lo < 0)
			goto append;

		c = (char)(hi << 4 | lo);
		i += 2;

append:
		if (git_buf_putc(decoded_out, c) < 0)
			return -1;
	}

	return 0;
}

static int error_invalid_local_file_uri(const char *uri)
{
	giterr_set(GITERR_CONFIG, "'%s' is not a valid local file URI", uri);
	return -1;
}

static int local_file_url_prefixlen(const char *file_url)
{
	int len = -1;

	if (git__prefixcmp(file_url, "file://") == 0) {
		if (file_url[7] == '/')
			len = 8;
		else if (git__prefixcmp(file_url + 7, "localhost/") == 0)
			len = 17;
	}

	return len;
}

bool git_path_is_local_file_url(const char *file_url)
{
	return (local_file_url_prefixlen(file_url) > 0);
}

int git_path_fromurl(git_buf *local_path_out, const char *file_url)
{
	int offset;

	assert(local_path_out && file_url);

	if ((offset = local_file_url_prefixlen(file_url)) < 0 ||
		file_url[offset] == '\0' || file_url[offset] == '/')
		return error_invalid_local_file_uri(file_url);

#ifndef GIT_WIN32
	offset--;	/* A *nix absolute path starts with a forward slash */
#endif

	git_buf_clear(local_path_out);
	return git__percent_decode(local_path_out, file_url + offset);
}

int git_path_walk_up(
	git_buf *path,
	const char *ceiling,
	int (*cb)(void *data, const char *),
	void *data)
{
	int error = 0;
	git_buf iter;
	ssize_t stop = 0, scan;
	char oldc = '\0';

	assert(path && cb);

	if (ceiling != NULL) {
		if (git__prefixcmp(path->ptr, ceiling) == 0)
			stop = (ssize_t)strlen(ceiling);
		else
			stop = git_buf_len(path);
	}
	scan = git_buf_len(path);

	/* empty path: yield only once */
	if (!scan) {
		error = cb(data, "");
		if (error)
			giterr_set_after_callback(error);
		return error;
	}

	iter.ptr = path->ptr;
	iter.size = git_buf_len(path);
	iter.asize = path->asize;

	while (scan >= stop) {
		error = cb(data, iter.ptr);
		iter.ptr[scan] = oldc;

		if (error) {
			giterr_set_after_callback(error);
			break;
		}

		scan = git_buf_rfind_next(&iter, '/');
		if (scan >= 0) {
			scan++;
			oldc = iter.ptr[scan];
			iter.size = scan;
			iter.ptr[scan] = '\0';
		}
	}

	if (scan >= 0)
		iter.ptr[scan] = oldc;

	/* relative path: yield for the last component */
	if (!error && stop == 0 && iter.ptr[0] != '/') {
		error = cb(data, "");
		if (error)
			giterr_set_after_callback(error);
	}

	return error;
}

bool git_path_exists(const char *path)
{
	assert(path);
	return p_access(path, F_OK) == 0;
}

bool git_path_isdir(const char *path)
{
	struct stat st;
	if (p_stat(path, &st) < 0)
		return false;

	return S_ISDIR(st.st_mode) != 0;
}

bool git_path_isfile(const char *path)
{
	struct stat st;

	assert(path);
	if (p_stat(path, &st) < 0)
		return false;

	return S_ISREG(st.st_mode) != 0;
}

#ifdef GIT_WIN32

bool git_path_is_empty_dir(const char *path)
{
	git_win32_path filter_w;
	bool empty = false;

	if (git_win32__findfirstfile_filter(filter_w, path)) {
		WIN32_FIND_DATAW findData;
		HANDLE hFind = FindFirstFileW(filter_w, &findData);

		/* FindFirstFile will fail if there are no children to the given
		 * path, which can happen if the given path is a file (and obviously
		 * has no children) or if the given path is an empty mount point.
		 * (Most directories have at least directory entries '.' and '..',
		 * but ridiculously another volume mounted in another drive letter's
		 * path space do not, and thus have nothing to enumerate.)  If
		 * FindFirstFile fails, check if this is a directory-like thing
		 * (a mount point).
		 */
		if (hFind == INVALID_HANDLE_VALUE)
			return git_path_isdir(path);

		/* If the find handle was created successfully, then it's a directory */
		empty = true;

		do {
			/* Allow the enumeration to return . and .. and still be considered
			 * empty. In the special case of drive roots (i.e. C:\) where . and
			 * .. do not occur, we can still consider the path to be an empty
			 * directory if there's nothing there. */
			if (!git_path_is_dot_or_dotdotW(findData.cFileName)) {
				empty = false;
				break;
			}
		} while (FindNextFileW(hFind, &findData));

		FindClose(hFind);
	}

	return empty;
}

#else

static int path_found_entry(void *payload, git_buf *path)
{
	GIT_UNUSED(payload);
	return !git_path_is_dot_or_dotdot(path->ptr);
}

bool git_path_is_empty_dir(const char *path)
{
	int error;
	git_buf dir = GIT_BUF_INIT;

	if (!git_path_isdir(path))
		return false;

	if ((error = git_buf_sets(&dir, path)) != 0)
		giterr_clear();
	else
		error = git_path_direach(&dir, 0, path_found_entry, NULL);

	git_buf_free(&dir);

	return !error;
}

#endif

int git_path_set_error(int errno_value, const char *path, const char *action)
{
	switch (errno_value) {
	case ENOENT:
	case ENOTDIR:
		giterr_set(GITERR_OS, "Could not find '%s' to %s", path, action);
		return GIT_ENOTFOUND;

	case EINVAL:
	case ENAMETOOLONG:
		giterr_set(GITERR_OS, "Invalid path for filesystem '%s'", path);
		return GIT_EINVALIDSPEC;

	case EEXIST:
		giterr_set(GITERR_OS, "Failed %s - '%s' already exists", action, path);
		return GIT_EEXISTS;

	default:
		giterr_set(GITERR_OS, "Could not %s '%s'", action, path);
		return -1;
	}
}

int git_path_lstat(const char *path, struct stat *st)
{
	if (p_lstat(path, st) == 0)
		return 0;

	return git_path_set_error(errno, path, "stat");
}

static bool _check_dir_contents(
	git_buf *dir,
	const char *sub,
	bool (*predicate)(const char *))
{
	bool result;
	size_t dir_size = git_buf_len(dir);
	size_t sub_size = strlen(sub);

	/* leave base valid even if we could not make space for subdir */
	if (git_buf_try_grow(dir, dir_size + sub_size + 2, false, false) < 0)
		return false;

	/* save excursion */
	git_buf_joinpath(dir, dir->ptr, sub);

	result = predicate(dir->ptr);

	/* restore path */
	git_buf_truncate(dir, dir_size);
	return result;
}

bool git_path_contains(git_buf *dir, const char *item)
{
	return _check_dir_contents(dir, item, &git_path_exists);
}

bool git_path_contains_dir(git_buf *base, const char *subdir)
{
	return _check_dir_contents(base, subdir, &git_path_isdir);
}

bool git_path_contains_file(git_buf *base, const char *file)
{
	return _check_dir_contents(base, file, &git_path_isfile);
}

int git_path_find_dir(git_buf *dir, const char *path, const char *base)
{
	int error = git_path_join_unrooted(dir, path, base, NULL);

	if (!error) {
		char buf[GIT_PATH_MAX];
		if (p_realpath(dir->ptr, buf) != NULL)
			error = git_buf_sets(dir, buf);
	}

	/* call dirname if this is not a directory */
	if (!error) /* && git_path_isdir(dir->ptr) == false) */
		error = (git_path_dirname_r(dir, dir->ptr) < 0) ? -1 : 0;

	if (!error)
		error = git_path_to_dir(dir);

	return error;
}

int git_path_resolve_relative(git_buf *path, size_t ceiling)
{
	char *base, *to, *from, *next;
	size_t len;

	if (!path || git_buf_oom(path))
		return -1;

	if (ceiling > path->size)
		ceiling = path->size;

	/* recognize drive prefixes, etc. that should not be backed over */
	if (ceiling == 0)
		ceiling = git_path_root(path->ptr) + 1;

	/* recognize URL prefixes that should not be backed over */
	if (ceiling == 0) {
		for (next = path->ptr; *next && git__isalpha(*next); ++next);
		if (next[0] == ':' && next[1] == '/' && next[2] == '/')
			ceiling = (next + 3) - path->ptr;
	}

	base = to = from = path->ptr + ceiling;

	while (*from) {
		for (next = from; *next && *next != '/'; ++next);

		len = next - from;

		if (len == 1 && from[0] == '.')
			/* do nothing with singleton dot */;

		else if (len == 2 && from[0] == '.' && from[1] == '.') {
			/* error out if trying to up one from a hard base */
			if (to == base && ceiling != 0) {
				giterr_set(GITERR_INVALID,
					"Cannot strip root component off url");
				return -1;
			}

			/* no more path segments to strip,
			 * use '../' as a new base path */
			if (to == base) {
				if (*next == '/')
					len++;

				if (to != from)
					memmove(to, from, len);

				to += len;
				/* this is now the base, can't back up from a
				 * relative prefix */
				base = to;
			} else {
				/* back up a path segment */
				while (to > base && to[-1] == '/') to--;
				while (to > base && to[-1] != '/') to--;
			}
		} else {
			if (*next == '/' && *from != '/')
				len++;

			if (to != from)
				memmove(to, from, len);

			to += len;
		}

		from += len;

		while (*from == '/') from++;
	}

	*to = '\0';

	path->size = to - path->ptr;

	return 0;
}

int git_path_apply_relative(git_buf *target, const char *relpath)
{
	git_buf_joinpath(target, git_buf_cstr(target), relpath);
	return git_path_resolve_relative(target, 0);
}

int git_path_cmp(
	const char *name1, size_t len1, int isdir1,
	const char *name2, size_t len2, int isdir2,
	int (*compare)(const char *, const char *, size_t))
{
	unsigned char c1, c2;
	size_t len = len1 < len2 ? len1 : len2;
	int cmp;

	cmp = compare(name1, name2, len);
	if (cmp)
		return cmp;

	c1 = name1[len];
	c2 = name2[len];

	if (c1 == '\0' && isdir1)
		c1 = '/';

	if (c2 == '\0' && isdir2)
		c2 = '/';

	return (c1 < c2) ? -1 : (c1 > c2) ? 1 : 0;
}

int git_path_make_relative(git_buf *path, const char *parent)
{
	const char *p, *q, *p_dirsep, *q_dirsep;
	size_t plen = path->size, newlen, depth = 1, i, offset;

	for (p_dirsep = p = path->ptr, q_dirsep = q = parent; *p && *q; p++, q++) {
		if (*p == '/' && *q == '/') {
			p_dirsep = p;
			q_dirsep = q;
		}
		else if (*p != *q)
			break;
	}

	/* need at least 1 common path segment */
	if ((p_dirsep == path->ptr || q_dirsep == parent) &&
		(*p_dirsep != '/' || *q_dirsep != '/')) {
		giterr_set(GITERR_INVALID,
			"%s is not a parent of %s", parent, path->ptr);
		return GIT_ENOTFOUND;
	}

	if (*p == '/' && !*q)
		p++;
	else if (!*p && *q == '/')
		q++;
	else if (!*p && !*q)
		return git_buf_clear(path), 0;
	else {
		p = p_dirsep + 1;
		q = q_dirsep + 1;
	}

	plen -= (p - path->ptr);

	if (!*q)
		return git_buf_set(path, p, plen);

	for (; (q = strchr(q, '/')) && *(q + 1); q++)
		depth++;

	newlen = (depth * 3) + plen;

	/* save the offset as we might realllocate the pointer */
	offset = p - path->ptr;
	if (git_buf_try_grow(path, newlen + 1, 1, 0) < 0)
		return -1;
	p = path->ptr + offset;

	memmove(path->ptr + (depth * 3), p, plen + 1);

	for (i = 0; i < depth; i++)
		memcpy(path->ptr + (i * 3), "../", 3);

	path->size = newlen;
	return 0;
}

bool git_path_has_non_ascii(const char *path, size_t pathlen)
{
	const uint8_t *scan = (const uint8_t *)path, *end;

	for (end = scan + pathlen; scan < end; ++scan)
		if (*scan & 0x80)
			return true;

	return false;
}

#ifdef GIT_USE_ICONV

int git_path_iconv_init_precompose(git_path_iconv_t *ic)
{
	git_buf_init(&ic->buf, 0);
	ic->map = iconv_open(GIT_PATH_REPO_ENCODING, GIT_PATH_NATIVE_ENCODING);
	return 0;
}

void git_path_iconv_clear(git_path_iconv_t *ic)
{
	if (ic) {
		if (ic->map != (iconv_t)-1)
			iconv_close(ic->map);
		git_buf_free(&ic->buf);
	}
}

int git_path_iconv(git_path_iconv_t *ic, char **in, size_t *inlen)
{
	char *nfd = *in, *nfc;
	size_t nfdlen = *inlen, nfclen, wantlen = nfdlen, rv;
	int retry = 1;

	if (!ic || ic->map == (iconv_t)-1 ||
		!git_path_has_non_ascii(*in, *inlen))
		return 0;

	git_buf_clear(&ic->buf);

	while (1) {
		if (git_buf_grow(&ic->buf, wantlen + 1) < 0)
			return -1;

		nfc    = ic->buf.ptr   + ic->buf.size;
		nfclen = ic->buf.asize - ic->buf.size;

		rv = iconv(ic->map, &nfd, &nfdlen, &nfc, &nfclen);

		ic->buf.size = (nfc - ic->buf.ptr);

		if (rv != (size_t)-1)
			break;

		/* if we cannot convert the data (probably because iconv thinks
		 * it is not valid UTF-8 source data), then use original data
		 */
		if (errno != E2BIG)
			return 0;

		/* make space for 2x the remaining data to be converted
		 * (with per retry overhead to avoid infinite loops)
		 */
		wantlen = ic->buf.size + max(nfclen, nfdlen) * 2 + (size_t)(retry * 4);

		if (retry++ > 4)
			goto fail;
	}

	ic->buf.ptr[ic->buf.size] = '\0';

	*in    = ic->buf.ptr;
	*inlen = ic->buf.size;

	return 0;

fail:
	giterr_set(GITERR_OS, "Unable to convert unicode path data");
	return -1;
}

static const char *nfc_file = "\xC3\x85\x73\x74\x72\xC3\xB6\x6D.XXXXXX";
static const char *nfd_file = "\x41\xCC\x8A\x73\x74\x72\x6F\xCC\x88\x6D.XXXXXX";

/* Check if the platform is decomposing unicode data for us.  We will
 * emulate core Git and prefer to use precomposed unicode data internally
 * on these platforms, composing the decomposed unicode on the fly.
 *
 * This mainly happens on the Mac where HDFS stores filenames as
 * decomposed unicode.  Even on VFAT and SAMBA file systems, the Mac will
 * return decomposed unicode from readdir() even when the actual
 * filesystem is storing precomposed unicode.
 */
bool git_path_does_fs_decompose_unicode(const char *root)
{
	git_buf path = GIT_BUF_INIT;
	int fd;
	bool found_decomposed = false;
	char tmp[6];

	/* Create a file using a precomposed path and then try to find it
	 * using the decomposed name.  If the lookup fails, then we will mark
	 * that we should precompose unicode for this repository.
	 */
	if (git_buf_joinpath(&path, root, nfc_file) < 0 ||
		(fd = p_mkstemp(path.ptr)) < 0)
		goto done;
	p_close(fd);

	/* record trailing digits generated by mkstemp */
	memcpy(tmp, path.ptr + path.size - sizeof(tmp), sizeof(tmp));

	/* try to look up as NFD path */
	if (git_buf_joinpath(&path, root, nfd_file) < 0)
		goto done;
	memcpy(path.ptr + path.size - sizeof(tmp), tmp, sizeof(tmp));

	found_decomposed = git_path_exists(path.ptr);

	/* remove temporary file (using original precomposed path) */
	if (git_buf_joinpath(&path, root, nfc_file) < 0)
		goto done;
	memcpy(path.ptr + path.size - sizeof(tmp), tmp, sizeof(tmp));

	(void)p_unlink(path.ptr);

done:
	git_buf_free(&path);
	return found_decomposed;
}

#else

bool git_path_does_fs_decompose_unicode(const char *root)
{
	GIT_UNUSED(root);
	return false;
}

#endif

#if defined(__sun) || defined(__GNU__)
typedef char path_dirent_data[sizeof(struct dirent) + FILENAME_MAX + 1];
#else
typedef struct dirent path_dirent_data;
#endif

int git_path_direach(
	git_buf *path,
	uint32_t flags,
	int (*fn)(void *, git_buf *),
	void *arg)
{
	int error = 0;
	ssize_t wd_len;
	DIR *dir;
	path_dirent_data de_data;
	struct dirent *de, *de_buf = (struct dirent *)&de_data;

	GIT_UNUSED(flags);

#ifdef GIT_USE_ICONV
	git_path_iconv_t ic = GIT_PATH_ICONV_INIT;
#endif

	if (git_path_to_dir(path) < 0)
		return -1;

	wd_len = git_buf_len(path);

	if ((dir = opendir(path->ptr)) == NULL) {
		giterr_set(GITERR_OS, "Failed to open directory '%s'", path->ptr);
		if (errno == ENOENT)
        {
			return GIT_ENOTFOUND;
        }

		return -1;
	}

#ifdef GIT_USE_ICONV
	if ((flags & GIT_PATH_DIR_PRECOMPOSE_UNICODE) != 0)
		(void)git_path_iconv_init_precompose(&ic);
#endif

	while (p_readdir_r(dir, de_buf, &de) == 0 && de != NULL) {
		char *de_path = de->d_name;
		size_t de_len = strlen(de_path);

		if (git_path_is_dot_or_dotdot(de_path))
        {
			continue;
        }

#ifdef GIT_USE_ICONV
		if ((error = git_path_iconv(&ic, &de_path, &de_len)) < 0)
			break;
#endif

		if ((error = git_buf_put(path, de_path, de_len)) < 0)
        {
			break;
        }


		error = fn(arg, path);

		git_buf_truncate(path, wd_len); /* restore path */

		if (error != 0) {
			giterr_set_after_callback(error);
			break;
		}
	}

	closedir(dir);

#ifdef GIT_USE_ICONV
	git_path_iconv_clear(&ic);
#endif

	return error;
}

int git_path_dirload(
	const char *path,
	size_t prefix_len,
	size_t alloc_extra,
	unsigned int flags,
	git_vector *contents)
{
	int error, need_slash;
	DIR *dir;
	size_t path_len;
	path_dirent_data de_data;
	struct dirent *de, *de_buf = (struct dirent *)&de_data;

	GIT_UNUSED(flags);

#ifdef GIT_USE_ICONV
	git_path_iconv_t ic = GIT_PATH_ICONV_INIT;
#endif

	assert(path && contents);

	path_len = strlen(path);

	if (!path_len || path_len < prefix_len) {
		giterr_set(GITERR_INVALID, "Invalid directory path '%s'", path);
		return -1;
	}
	if ((dir = opendir(path)) == NULL) {
		giterr_set(GITERR_OS, "Failed to open directory '%s'", path);
		return -1;
	}

#ifdef GIT_USE_ICONV
	if ((flags & GIT_PATH_DIR_PRECOMPOSE_UNICODE) != 0)
		(void)git_path_iconv_init_precompose(&ic);
#endif

	path += prefix_len;
	path_len -= prefix_len;
	need_slash = (path_len > 0 && path[path_len-1] != '/') ? 1 : 0;

	while ((error = p_readdir_r(dir, de_buf, &de)) == 0 && de != NULL) {
		char *entry_path, *de_path = de->d_name;
		size_t alloc_size, de_len = strlen(de_path);

		if (git_path_is_dot_or_dotdot(de_path))
			continue;

#ifdef GIT_USE_ICONV
		if ((error = git_path_iconv(&ic, &de_path, &de_len)) < 0)
			break;
#endif

		alloc_size = path_len + need_slash + de_len + 1 + alloc_extra;
		if ((entry_path = git__calloc(alloc_size, 1)) == NULL) {
			error = -1;
			break;
		}

		if (path_len)
			memcpy(entry_path, path, path_len);
		if (need_slash)
			entry_path[path_len] = '/';
		memcpy(&entry_path[path_len + need_slash], de_path, de_len);

		if ((error = git_vector_insert(contents, entry_path)) < 0) {
			git__free(entry_path);
			break;
		}
	}

	closedir(dir);

#ifdef GIT_USE_ICONV
	git_path_iconv_clear(&ic);
#endif

	if (error != 0)
		giterr_set(GITERR_OS, "Failed to process directory entry in '%s'", path);

	return error;
}

int git_path_with_stat_cmp(const void *a, const void *b)
{
	const git_path_with_stat *psa = a, *psb = b;
	return strcmp(psa->path, psb->path);
}

int git_path_with_stat_cmp_icase(const void *a, const void *b)
{
	const git_path_with_stat *psa = a, *psb = b;
	return strcasecmp(psa->path, psb->path);
}

int git_path_dirload_with_stat(
	const char *path,
	size_t prefix_len,
	unsigned int flags,
	const char *start_stat,
	const char *end_stat,
	git_vector *contents)
{
	int error;
	unsigned int i;
	git_path_with_stat *ps;
	git_buf full = GIT_BUF_INIT;
	int (*strncomp)(const char *a, const char *b, size_t sz);
	size_t start_len = start_stat ? strlen(start_stat) : 0;
	size_t end_len = end_stat ? strlen(end_stat) : 0, cmp_len;

	if (git_buf_set(&full, path, prefix_len) < 0)
		return -1;

	error = git_path_dirload(
		path, prefix_len, sizeof(git_path_with_stat) + 1, flags, contents);
	if (error < 0) {
		git_buf_free(&full);
		return error;
	}

	strncomp = (flags & GIT_PATH_DIR_IGNORE_CASE) != 0 ?
		git__strncasecmp : git__strncmp;

	/* stat struct at start of git_path_with_stat, so shift path text */
	git_vector_foreach(contents, i, ps) {
		size_t path_len = strlen((char *)ps);
		memmove(ps->path, ps, path_len + 1);
		ps->path_len = path_len;
	}

	git_vector_foreach(contents, i, ps) {
		/* skip if before start_stat or after end_stat */
		cmp_len = min(start_len, ps->path_len);
		if (cmp_len && strncomp(ps->path, start_stat, cmp_len) < 0)
			continue;
		cmp_len = min(end_len, ps->path_len);
		if (cmp_len && strncomp(ps->path, end_stat, cmp_len) > 0)
			continue;

		git_buf_truncate(&full, prefix_len);

		if ((error = git_buf_joinpath(&full, full.ptr, ps->path)) < 0 ||
			(error = git_path_lstat(full.ptr, &ps->st)) < 0) {

			if (error == GIT_ENOTFOUND) {
				/* file was removed between readdir and lstat */
				char *entry_path = git_vector_get(contents, i);
				git_vector_remove(contents, i--);
				git__free(entry_path);
			} else {
				/* Treat the file as unreadable if we get any other error */
				memset(&ps->st, 0, sizeof(ps->st));
				ps->st.st_mode = GIT_FILEMODE_UNREADABLE;
			}

			giterr_clear();
			error = 0;
			continue;
		}

		if (S_ISDIR(ps->st.st_mode)) {
			ps->path[ps->path_len++] = '/';
			ps->path[ps->path_len] = '\0';
		}
		else if (!S_ISREG(ps->st.st_mode) && !S_ISLNK(ps->st.st_mode)) {
			char *entry_path = git_vector_get(contents, i);
			git_vector_remove(contents, i--);
			git__free(entry_path);
		}
	}

	/* sort now that directory suffix is added */
	git_vector_sort(contents);

	git_buf_free(&full);

	return error;
}

int git_path_from_url_or_path(git_buf *local_path_out, const char *url_or_path)
{
	if (git_path_is_local_file_url(url_or_path))
		return git_path_fromurl(local_path_out, url_or_path);
	else
		return git_buf_sets(local_path_out, url_or_path);
}

/* Reject paths like AUX or COM1, or those versions that end in a dot or
 * colon.  ("AUX." or "AUX:")
 */
GIT_INLINE(bool) verify_dospath(
	const char *component,
	size_t len,
	const char dospath[3],
	bool trailing_num)
{
	size_t last = trailing_num ? 4 : 3;

	if (len < last || git__strncasecmp(component, dospath, 3) != 0)
		return true;

	if (trailing_num && (component[3] < '1' || component[3] > '9'))
		return true;

	return (len > last &&
		component[last] != '.' &&
		component[last] != ':');
}

static int32_t next_hfs_char(const char **in, size_t *len)
{
	while (*len) {
		int32_t codepoint;
		int cp_len = git__utf8_iterate((const uint8_t *)(*in), (int)(*len), &codepoint);
		if (cp_len < 0)
			return -1;

		(*in) += cp_len;
		(*len) -= cp_len;

		/* these code points are ignored completely */
		switch (codepoint) {
		case 0x200c: /* ZERO WIDTH NON-JOINER */
		case 0x200d: /* ZERO WIDTH JOINER */
		case 0x200e: /* LEFT-TO-RIGHT MARK */
		case 0x200f: /* RIGHT-TO-LEFT MARK */
		case 0x202a: /* LEFT-TO-RIGHT EMBEDDING */
		case 0x202b: /* RIGHT-TO-LEFT EMBEDDING */
		case 0x202c: /* POP DIRECTIONAL FORMATTING */
		case 0x202d: /* LEFT-TO-RIGHT OVERRIDE */
		case 0x202e: /* RIGHT-TO-LEFT OVERRIDE */
		case 0x206a: /* INHIBIT SYMMETRIC SWAPPING */
		case 0x206b: /* ACTIVATE SYMMETRIC SWAPPING */
		case 0x206c: /* INHIBIT ARABIC FORM SHAPING */
		case 0x206d: /* ACTIVATE ARABIC FORM SHAPING */
		case 0x206e: /* NATIONAL DIGIT SHAPES */
		case 0x206f: /* NOMINAL DIGIT SHAPES */
		case 0xfeff: /* ZERO WIDTH NO-BREAK SPACE */
			continue;
		}

		/* fold into lowercase -- this will only fold characters in
		 * the ASCII range, which is perfectly fine, because the
		 * git folder name can only be composed of ascii characters
		 */
		return tolower(codepoint);
	}
	return 0; /* NULL byte -- end of string */
}

static bool verify_dotgit_hfs(const char *path, size_t len)
{
	if (DOT_GIT_MODULE_B) //oz
		return true;

	return false;
}

GIT_INLINE(bool) verify_dotgit_ntfs(git_repository *repo, const char *path, size_t len)
{
	const char *shortname = NULL;
	size_t i, start, shortname_len = 0;

	/* See if the repo has a custom shortname (not "GIT~1") */
	if (repo &&
		(shortname = git_repository__8dot3_name(repo)) &&
		shortname != git_repository__8dot3_default)
		shortname_len = strlen(shortname);

	if (len >= DOT_GIT_LEN && strncasecmp(path, DOT_GIT, DOT_GIT_LEN) == 0) //oz
		start = DOT_GIT_LEN; //oz
	else if (len >= git_repository__8dot3_default_len &&
		strncasecmp(path, git_repository__8dot3_default, git_repository__8dot3_default_len) == 0)
		start = git_repository__8dot3_default_len;
	else if (shortname_len && len >= shortname_len &&
		strncasecmp(path, shortname, shortname_len) == 0)
		start = shortname_len;
	else
		return true;

	/* Reject paths beginning with ".git\" */
	if (path[start] == '\\')
		return false;

	for (i = start; i < len; i++) {
		if (path[i] != ' ' && path[i] != '.')
			return true;
	}

	return false;
}

GIT_INLINE(bool) verify_char(unsigned char c, unsigned int flags)
{
	if ((flags & GIT_PATH_REJECT_BACKSLASH) && c == '\\')
		return false;

	if ((flags & GIT_PATH_REJECT_SLASH) && c == '/')
		return false;

	if (flags & GIT_PATH_REJECT_NT_CHARS) {
		if (c < 32)
			return false;

		switch (c) {
		case '<':
		case '>':
		case ':':
		case '"':
		case '|':
		case '?':
		case '*':
			return false;
		}
	}

	return true;
}

/*
 * We fundamentally don't like some paths when dealing with user-inputted
 * strings (in checkout or ref names): we don't want dot or dot-dot
 * anywhere, we want to avoid writing weird paths on Windows that can't
 * be handled by tools that use the non-\\?\ APIs, we don't want slashes
 * or double slashes at the end of paths that can make them ambiguous.
 *
 * For checkout, we don't want to recurse into ".git" either.
 */
static bool verify_component(
	git_repository *repo,
	const char *component,
	size_t len,
	unsigned int flags)
{
	if (len == 0)
		return false;

	if ((flags & GIT_PATH_REJECT_TRAVERSAL) &&
		len == 1 && component[0] == '.')
		return false;

	if ((flags & GIT_PATH_REJECT_TRAVERSAL) &&
		len == 2 && component[0] == '.' && component[1] == '.')
		return false;

	if ((flags & GIT_PATH_REJECT_TRAILING_DOT) && component[len-1] == '.')
		return false;

	if ((flags & GIT_PATH_REJECT_TRAILING_SPACE) && component[len-1] == ' ')
		return false;

	if ((flags & GIT_PATH_REJECT_TRAILING_COLON) && component[len-1] == ':')
		return false;

	if (flags & GIT_PATH_REJECT_DOS_PATHS) {
		if (!verify_dospath(component, len, "CON", false) ||
			!verify_dospath(component, len, "PRN", false) ||
			!verify_dospath(component, len, "AUX", false) ||
			!verify_dospath(component, len, "NUL", false) ||
			!verify_dospath(component, len, "COM", true)  ||
			!verify_dospath(component, len, "LPT", true))
			return false;
	}

	if (flags & GIT_PATH_REJECT_DOT_GIT_HFS &&
		!verify_dotgit_hfs(component, len))
		return false;

	if (flags & GIT_PATH_REJECT_DOT_GIT_NTFS &&
		!verify_dotgit_ntfs(repo, component, len))
		return false;

	if ((flags & GIT_PATH_REJECT_DOT_GIT_HFS) == 0 &&
		(flags & GIT_PATH_REJECT_DOT_GIT_NTFS) == 0 &&
		(flags & GIT_PATH_REJECT_DOT_GIT) &&
		len == DOT_GIT_LEN && DOT_GIT_MODULE_C) //oz
		return false;

	return true;
}

GIT_INLINE(unsigned int) dotgit_flags(
	git_repository *repo,
	unsigned int flags)
{
	int protectHFS = 0, protectNTFS = 0;

#ifdef __APPLE__
	protectHFS = 1;
#endif

#ifdef GIT_WIN32
	protectNTFS = 1;
#endif

	if (repo && !protectHFS)
		git_repository__cvar(&protectHFS, repo, GIT_CVAR_PROTECTHFS);
	if (protectHFS)
		flags |= GIT_PATH_REJECT_DOT_GIT_HFS;

	if (repo && !protectNTFS)
		git_repository__cvar(&protectNTFS, repo, GIT_CVAR_PROTECTNTFS);
	if (protectNTFS)
		flags |= GIT_PATH_REJECT_DOT_GIT_NTFS;

	return flags;
}

bool git_path_isvalid(
	git_repository *repo,
	const char *path,
	unsigned int flags)
{
	const char *start, *c;

	/* Upgrade the ".git" checks based on platform */
	if ((flags & GIT_PATH_REJECT_DOT_GIT))
		flags = dotgit_flags(repo, flags);

	for (start = c = path; *c; c++) {
		if (!verify_char(*c, flags))
			return false;

		if (*c == '/') {
			if (!verify_component(repo, start, (c - start), flags))
				return false;

			start = c+1;
		}
	}

	return verify_component(repo, start, (c - start), flags);
}
