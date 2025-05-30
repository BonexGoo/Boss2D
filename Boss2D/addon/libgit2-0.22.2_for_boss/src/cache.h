/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#ifndef INCLUDE_cache_h__
#define INCLUDE_cache_h__

#include BOSS_LIBGIT2_U_git2__common_h //original-code:"git2/common.h"
#include BOSS_LIBGIT2_U_git2__oid_h //original-code:"git2/oid.h"
#include BOSS_LIBGIT2_U_git2__odb_h //original-code:"git2/odb.h"

#include "thread-utils.h"
#include "oidmap.h"

enum {
	GIT_CACHE_STORE_ANY = 0,
	GIT_CACHE_STORE_RAW = 1,
	GIT_CACHE_STORE_PARSED = 2
};

typedef struct {
	git_oid    oid;
	int16_t    type;  /* git_otype value */
	uint16_t   flags; /* GIT_CACHE_STORE value */
	size_t     size;
	git_atomic refcount;
} git_cached_obj;

typedef struct {
	git_oidmap *map;
	git_rwlock  lock;
	ssize_t     used_memory;
} git_cache;

extern bool git_cache__enabled;
extern ssize_t git_cache__max_storage;
extern git_atomic_ssize git_cache__current_storage;

int git_cache_set_max_object_size(git_otype type, size_t size);

int git_cache_init(git_cache *cache);
void git_cache_free(git_cache *cache);
void git_cache_clear(git_cache *cache);

void *git_cache_store_raw(git_cache *cache, git_odb_object *entry);
void *git_cache_store_parsed(git_cache *cache, git_object *entry);

git_odb_object *git_cache_get_raw(git_cache *cache, const git_oid *oid);
git_object *git_cache_get_parsed(git_cache *cache, const git_oid *oid);
void *git_cache_get_any(git_cache *cache, const git_oid *oid);

GIT_INLINE(size_t) git_cache_size(git_cache *cache)
{
	return (size_t)kh_size(cache->map);
}

GIT_INLINE(void) git_cached_obj_incref(void *_obj)
{
	git_cached_obj *obj = _obj;
	git_atomic_inc(&obj->refcount);
}

void git_cached_obj_decref(void *_obj);

#endif
