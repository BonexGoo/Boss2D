/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */

#include BOSS_LIBGIT2_U_common_h //original-code:"common.h"
#include BOSS_LIBGIT2_U_repository_h //original-code:"repository.h"
#include "strmap.h"
#include "refdb.h"
#include "pool.h"
#include "reflog.h"
#include "signature.h"

#include BOSS_LIBGIT2_U_git2__signature_h //original-code:"git2/signature.h"
#include BOSS_LIBGIT2_U_git2__sys__refs_h //original-code:"git2/sys/refs.h"
#include BOSS_LIBGIT2_U_git2__sys__refdb_backend_h //original-code:"git2/sys/refdb_backend.h"

GIT__USE_STRMAP;

typedef struct {
	const char *name;
	void *payload;

	git_ref_t ref_type;
	union {
		git_oid id;
		char *symbolic;
	} target;
	git_reflog *reflog;

	const char *message;
	git_signature *sig;

	unsigned int committed :1,
		remove :1;
} transaction_node;

struct git_transaction {
	git_repository *repo;
	git_refdb *db;

	git_strmap *locks;
	git_pool pool;
};

int git_transaction_new(git_transaction **out, git_repository *repo)
{
	int error;
	git_pool pool;
	git_transaction *tx = NULL;

	assert(out && repo);

	if ((error = git_pool_init(&pool, 1, 0)) < 0)
		return error;

	tx = git_pool_mallocz(&pool, sizeof(git_transaction));
	if (!tx) {
		error = -1;
		goto on_error;
	}

	if ((error = git_strmap_alloc(&tx->locks)) < 0) {
		error = -1;
		goto on_error;
	}

	if ((error = git_repository_refdb(&tx->db, repo)) < 0)
		goto on_error;

	memcpy(&tx->pool, &pool, sizeof(git_pool));
	tx->repo = repo;
	*out = tx;
	return 0;

on_error:
	git_pool_clear(&pool);
	return error;
}

int git_transaction_lock_ref(git_transaction *tx, const char *refname)
{
	int error;
	transaction_node *node;

	assert(tx && refname);

	node = git_pool_mallocz(&tx->pool, sizeof(transaction_node));
	GITERR_CHECK_ALLOC(node);

	node->name = git_pool_strdup(&tx->pool, refname);
	GITERR_CHECK_ALLOC(node->name);

	if ((error = git_refdb_lock(&node->payload, tx->db, refname)) < 0)
		return error;

	git_strmap_insert(tx->locks, node->name, node, error);
	if (error < 0) 
		goto cleanup;

	return 0;

cleanup:
	git_refdb_unlock(tx->db, node->payload, false, false, NULL, NULL, NULL);

	return error;
}

static int find_locked(transaction_node **out, git_transaction *tx, const char *refname)
{
	git_strmap_iter pos;
	transaction_node *node;

	pos = git_strmap_lookup_index(tx->locks, refname);
	if (!git_strmap_valid_index(tx->locks, pos)) {
		giterr_set(GITERR_REFERENCE, "the specified reference is not locked");
		return GIT_ENOTFOUND;
	}

	node = git_strmap_value_at(tx->locks, pos);

	*out = node;
	return 0;
}

static int copy_common(transaction_node *node, git_transaction *tx, const git_signature *sig, const char *msg)
{
	if (sig && git_signature__pdup(&node->sig, sig, &tx->pool) < 0)
		return -1;

	if (!node->sig) {
		git_signature *tmp;
		int error;

		if (git_reference__log_signature(&tmp, tx->repo) < 0)
			return -1;

		/* make sure the sig we use is in our pool */
		error = git_signature__pdup(&node->sig, tmp, &tx->pool);
		git_signature_free(tmp);
		if (error < 0)
			return error;
	}

	if (msg) {
		node->message = git_pool_strdup(&tx->pool, msg);
		GITERR_CHECK_ALLOC(node->message);
	}

	return 0;
}

int git_transaction_set_target(git_transaction *tx, const char *refname, const git_oid *target, const git_signature *sig, const char *msg)
{
	int error;
	transaction_node *node;

	assert(tx && refname && target);

	if ((error = find_locked(&node, tx, refname)) < 0)
		return error;

	if ((error = copy_common(node, tx, sig, msg)) < 0)
		return error;

	git_oid_cpy(&node->target.id, target);
	node->ref_type = GIT_REF_OID;

	return 0;
}

int git_transaction_set_symbolic_target(git_transaction *tx, const char *refname, const char *target, const git_signature *sig, const char *msg)
{
	int error;
	transaction_node *node;

	assert(tx && refname && target);

	if ((error = find_locked(&node, tx, refname)) < 0)
		return error;

	if ((error = copy_common(node, tx, sig, msg)) < 0)
		return error;

	node->target.symbolic = git_pool_strdup(&tx->pool, target);
	GITERR_CHECK_ALLOC(node->target.symbolic);
	node->ref_type = GIT_REF_SYMBOLIC;

	return 0;
}

int git_transaction_remove(git_transaction *tx, const char *refname)
{
	int error;
	transaction_node *node;

	if ((error = find_locked(&node, tx, refname)) < 0)
		return error;

	node->remove = true;
	node->ref_type = GIT_REF_OID; /* the id will be ignored */

	return 0;
}

static int dup_reflog(git_reflog **out, const git_reflog *in, git_pool *pool)
{
	git_reflog *reflog;
	git_reflog_entry *entries;
	size_t len, i;

	reflog = git_pool_mallocz(pool, sizeof(git_reflog));
	GITERR_CHECK_ALLOC(reflog);

	reflog->ref_name = git_pool_strdup(pool, in->ref_name);
	GITERR_CHECK_ALLOC(reflog->ref_name);

	len = in->entries.length;
	reflog->entries.length = len;
	reflog->entries.contents = git_pool_mallocz(pool, len * sizeof(void *));
	GITERR_CHECK_ALLOC(reflog->entries.contents);

	entries = git_pool_mallocz(pool, len * sizeof(git_reflog_entry));
	GITERR_CHECK_ALLOC(entries);

	for (i = 0; i < len; i++) {
		const git_reflog_entry *src;
		git_reflog_entry *tgt;

		tgt = &entries[i];
		reflog->entries.contents[i] = tgt;

		src = git_vector_get(&in->entries, i);
		git_oid_cpy(&tgt->oid_old, &src->oid_old);
		git_oid_cpy(&tgt->oid_cur, &src->oid_cur);

		tgt->msg = git_pool_strdup(pool, src->msg);
		GITERR_CHECK_ALLOC(tgt->msg);

		if (git_signature__pdup(&tgt->committer, src->committer, pool) < 0)
			return -1;
	}


	*out = reflog;
	return 0;
}

int git_transaction_set_reflog(git_transaction *tx, const char *refname, const git_reflog *reflog)
{
	int error;
	transaction_node *node;

	assert(tx && refname && reflog);

	if ((error = find_locked(&node, tx, refname)) < 0)
		return error;

	if ((error = dup_reflog(&node->reflog, reflog, &tx->pool)) < 0)
		return error;

	return 0;
}

static int update_target(git_refdb *db, transaction_node *node)
{
	git_reference *ref;
	int error, update_reflog;

	if (node->ref_type == GIT_REF_OID) {
		ref = git_reference__alloc(node->name, &node->target.id, NULL);
	} else if (node->ref_type == GIT_REF_SYMBOLIC) {
		ref = git_reference__alloc_symbolic(node->name, node->target.symbolic);
	} else {
		abort();
	}

	GITERR_CHECK_ALLOC(ref);
	update_reflog = node->reflog == NULL;

	if (node->remove) {
		error =  git_refdb_unlock(db, node->payload, 2, false, ref, NULL, NULL);
	} else if (node->ref_type == GIT_REF_OID) {
		error = git_refdb_unlock(db, node->payload, true, update_reflog, ref, node->sig, node->message);
	} else if (node->ref_type == GIT_REF_SYMBOLIC) {
		error = git_refdb_unlock(db, node->payload, true, update_reflog, ref, node->sig, node->message);
	} else {
		abort();
	}

	git_reference_free(ref);
	node->committed = true;

	return error;
}

int git_transaction_commit(git_transaction *tx)
{
	transaction_node *node;
	git_strmap_iter pos;
	int error = 0;

	assert(tx);

	for (pos = kh_begin(tx->locks); pos < kh_end(tx->locks); pos++) {
		if (!git_strmap_has_data(tx->locks, pos))
			continue;

		node = git_strmap_value_at(tx->locks, pos);
		if (node->reflog) {
			if ((error = tx->db->backend->reflog_write(tx->db->backend, node->reflog)) < 0)
				return error;
		}

		if (node->ref_type != GIT_REF_INVALID) {
			if ((error = update_target(tx->db, node)) < 0)
				return error;
		}
	}

	return 0;
}

void git_transaction_free(git_transaction *tx)
{
	transaction_node *node;
	git_pool pool;
	git_strmap_iter pos;

	assert(tx);

	/* start by unlocking the ones we've left hanging, if any */
	for (pos = kh_begin(tx->locks); pos < kh_end(tx->locks); pos++) {
		if (!git_strmap_has_data(tx->locks, pos))
			continue;

		node = git_strmap_value_at(tx->locks, pos);
		if (node->committed)
			continue;

		git_refdb_unlock(tx->db, node->payload, false, false, NULL, NULL, NULL);
	}

	git_refdb_free(tx->db);
	git_strmap_free(tx->locks);

	/* tx is inside the pool, so we need to extract the data */
	memcpy(&pool, &tx->pool, sizeof(git_pool));
	git_pool_clear(&pool);
}
