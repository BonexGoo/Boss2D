/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#ifndef INCLUDE_submodule_h__
#define INCLUDE_submodule_h__

#include BOSS_LIBGIT2_U_git2__submodule_h //original-code:"git2/submodule.h"
#include BOSS_LIBGIT2_U_git2__repository_h //original-code:"git2/repository.h"
#include "fileops.h"

/* Notes:
 *
 * Submodule information can be in four places: the index, the config files
 * (both .git/config and .gitmodules), the HEAD tree, and the working
 * directory.
 *
 * In the index:
 * - submodule is found by path
 * - may be missing, present, or of the wrong type
 * - will have an oid if present
 *
 * In the HEAD tree:
 * - submodule is found by path
 * - may be missing, present, or of the wrong type
 * - will have an oid if present
 *
 * In the config files:
 * - submodule is found by submodule "name" which is usually the path
 * - may be missing or present
 * - will have a name, path, url, and other properties
 *
 * In the working directory:
 * - submodule is found by path
 * - may be missing, an empty directory, a checked out directory,
 *   or of the wrong type
 * - if checked out, will have a HEAD oid
 * - if checked out, will have git history that can be used to compare oids
 * - if checked out, may have modified files and/or untracked files
 */

/**
 * Description of submodule
 *
 * This record describes a submodule found in a repository.  There should be
 * an entry for every submodule found in the HEAD and index, and for every
 * submodule described in .gitmodules.  The fields are as follows:
 *
 * - `rc` tracks the refcount of how many hash table entries in the
 *   git_submodule_cache there are for this submodule.  It only comes into
 *   play if the name and path of the submodule differ.
 *
 * - `name` is the name of the submodule from .gitmodules.
 * - `path` is the path to the submodule from the repo root.  It is almost
 *    always the same as `name`.
 * - `url` is the url for the submodule.
 * - `update` is a git_submodule_update_t value - see gitmodules(5) update.
 * - `update_default` is the update value from the config
 * - `ignore` is a git_submodule_ignore_t value - see gitmodules(5) ignore.
 * - `ignore_default` is the ignore value from the config
 * - `fetch_recurse` is a git_submodule_recurse_t value - see gitmodules(5)
 *    fetchRecurseSubmodules.
 * - `fetch_recurse_default` is the recurse value from the config
 *
 * - `repo` is the parent repository that contains this submodule.
 * - `flags` after for internal use, tracking where this submodule has been
 *   found (head, index, config, workdir) and known status info, etc.
 * - `head_oid` is the SHA1 for the submodule path in the repo HEAD.
 * - `index_oid` is the SHA1 for the submodule recorded in the index.
 * - `wd_oid` is the SHA1 for the HEAD of the checked out submodule.
 *
 * If the submodule has been added to .gitmodules but not yet git added,
 * then the `index_oid` will be zero but still marked valid.  If the
 * submodule has been deleted, but the delete has not been committed yet,
 * then the `index_oid` will be set, but the `url` will be NULL.
 */
struct git_submodule {
	git_refcount rc;

	/* information from config */
	char *name;
	char *path; /* important: may just point to "name" string */
	char *url;
	char *branch;
	git_submodule_update_t update;
	git_submodule_update_t update_default;
	git_submodule_ignore_t ignore;
	git_submodule_ignore_t ignore_default;
	git_submodule_recurse_t fetch_recurse;
	git_submodule_recurse_t fetch_recurse_default;

	/* internal information */
	git_repository *repo;
	uint32_t flags;
	git_oid head_oid;
	git_oid index_oid;
	git_oid wd_oid;
};

/**
 * The git_submodule_cache stores known submodules along with timestamps,
 * etc. about when they were loaded
 */
typedef struct {
	git_repository *repo;
	git_strmap *submodules;
	git_mutex  lock;

	/* cache invalidation data */
	git_oid head_id;
	git_futils_filestamp index_stamp;
	git_buf gitmodules_path;
	git_futils_filestamp gitmodules_stamp;
	git_futils_filestamp config_stamp;
} git_submodule_cache;

/* Force revalidation of submodule data cache (alloc as needed) */
extern int git_submodule_cache_refresh(git_repository *repo);

/* Release all submodules */
extern void git_submodule_cache_free(git_repository *repo);

/* Additional flags on top of public GIT_SUBMODULE_STATUS values */
enum {
	GIT_SUBMODULE_STATUS__WD_SCANNED          = (1u << 20),
	GIT_SUBMODULE_STATUS__HEAD_OID_VALID      = (1u << 21),
	GIT_SUBMODULE_STATUS__INDEX_OID_VALID     = (1u << 22),
	GIT_SUBMODULE_STATUS__WD_OID_VALID        = (1u << 23),
	GIT_SUBMODULE_STATUS__HEAD_NOT_SUBMODULE  = (1u << 24),
	GIT_SUBMODULE_STATUS__INDEX_NOT_SUBMODULE = (1u << 25),
	GIT_SUBMODULE_STATUS__WD_NOT_SUBMODULE    = (1u << 26),
	GIT_SUBMODULE_STATUS__INDEX_MULTIPLE_ENTRIES = (1u << 27),
};

#define GIT_SUBMODULE_STATUS__CLEAR_INTERNAL(S) \
	((S) & ~(0xFFFFFFFFu << 20))

/* Internal submodule check does not attempt to refresh cached data */
extern bool git_submodule__is_submodule(git_repository *repo, const char *name);

/* Internal lookup does not attempt to refresh cached data */
extern int git_submodule__lookup(
	git_submodule **out, git_repository *repo, const char *path);

/* Internal status fn returns status and optionally the various OIDs */
extern int git_submodule__status(
	unsigned int *out_status,
	git_oid *out_head_id,
	git_oid *out_index_id,
	git_oid *out_wd_id,
	git_submodule *sm,
	git_submodule_ignore_t ign);

/* Open submodule repository as bare repo for quick HEAD check, etc. */
extern int git_submodule_open_bare(
	git_repository **repo,
	git_submodule *submodule);

extern int git_submodule_parse_ignore(
	git_submodule_ignore_t *out, const char *value);
extern int git_submodule_parse_update(
	git_submodule_update_t *out, const char *value);

extern const char *git_submodule_ignore_to_str(git_submodule_ignore_t);
extern const char *git_submodule_update_to_str(git_submodule_update_t);
extern const char *git_submodule_recurse_to_str(git_submodule_recurse_t);

#endif
