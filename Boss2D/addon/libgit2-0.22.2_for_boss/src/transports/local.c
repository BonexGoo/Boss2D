/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#include BOSS_LIBGIT2_U_common_h //original-code:"common.h"
#include BOSS_LIBGIT2_U_git2__types_h //original-code:"git2/types.h"
#include BOSS_LIBGIT2_U_git2__net_h //original-code:"git2/net.h"
#include BOSS_LIBGIT2_U_git2__repository_h //original-code:"git2/repository.h"
#include BOSS_LIBGIT2_U_git2__object_h //original-code:"git2/object.h"
#include BOSS_LIBGIT2_U_git2__tag_h //original-code:"git2/tag.h"
#include BOSS_LIBGIT2_U_git2__transport_h //original-code:"git2/transport.h"
#include BOSS_LIBGIT2_U_git2__revwalk_h //original-code:"git2/revwalk.h"
#include BOSS_LIBGIT2_U_git2__odb_backend_h //original-code:"git2/odb_backend.h"
#include BOSS_LIBGIT2_U_git2__pack_h //original-code:"git2/pack.h"
#include BOSS_LIBGIT2_U_git2__commit_h //original-code:"git2/commit.h"
#include BOSS_LIBGIT2_U_git2__revparse_h //original-code:"git2/revparse.h"
#include BOSS_LIBGIT2_U_git2__push_h //original-code:"git2/push.h"
#include BOSS_LIBGIT2_U_pack_M_objects_h //original-code:"pack-objects.h"
#include BOSS_LIBGIT2_U_refs_h //original-code:"refs.h"
#include BOSS_LIBGIT2_U_posix_h //original-code:"posix.h"
#include BOSS_LIBGIT2_U_path_h //original-code:"path.h"
#include BOSS_LIBGIT2_U_buffer_h //original-code:"buffer.h"
#include BOSS_LIBGIT2_U_repository_h //original-code:"repository.h"
#include BOSS_LIBGIT2_U_odb_h //original-code:"odb.h"
#include BOSS_LIBGIT2_U_push_h //original-code:"push.h"
#include BOSS_LIBGIT2_U_remote_h //original-code:"remote.h"

typedef struct {
	git_transport parent;
	git_remote *owner;
	char *url;
	int direction;
	int flags;
	git_atomic cancelled;
	git_repository *repo;
	git_transport_message_cb progress_cb;
	git_transport_message_cb error_cb;
	void *message_cb_payload;
	git_vector refs;
	unsigned connected : 1,
		have_refs : 1;
} transport_local;

static void free_head(git_remote_head *head)
{
	git__free(head->name);
	git__free(head->symref_target);
	git__free(head);
}

static void free_heads(git_vector *heads)
{
	git_remote_head *head;
	size_t i;

	git_vector_foreach(heads, i, head)
		free_head(head);

	git_vector_free(heads);
}

static int add_ref(transport_local *t, const char *name)
{
	const char peeled[] = "^{}";
	git_reference *ref, *resolved;
	git_remote_head *head;
	git_oid obj_id;
	git_object *obj = NULL, *target = NULL;
	git_buf buf = GIT_BUF_INIT;
	int error;

	if ((error = git_reference_lookup(&ref, t->repo, name)) < 0)
		return error;

	error = git_reference_resolve(&resolved, ref);
	if (error < 0) {
		git_reference_free(ref);
		if (!strcmp(name, GIT_HEAD_FILE) && error == GIT_ENOTFOUND) {
			/* This is actually okay.  Empty repos often have a HEAD that
			 * points to a nonexistent "refs/heads/master". */
			giterr_clear();
			return 0;
		}
		return error;
	}

	git_oid_cpy(&obj_id, git_reference_target(resolved));
	git_reference_free(resolved);

	head = git__calloc(1, sizeof(git_remote_head));
	GITERR_CHECK_ALLOC(head);

	head->name = git__strdup(name);
	GITERR_CHECK_ALLOC(head->name);

	git_oid_cpy(&head->oid, &obj_id);

	if (git_reference_type(ref) == GIT_REF_SYMBOLIC) {
		head->symref_target = git__strdup(git_reference_symbolic_target(ref));
		GITERR_CHECK_ALLOC(head->symref_target);
	}
	git_reference_free(ref);

	if ((error = git_vector_insert(&t->refs, head)) < 0) {
		free_head(head);
		return error;
	}

	/* If it's not a tag, we don't need to try to peel it */
	if (git__prefixcmp(name, GIT_REFS_TAGS_DIR))
		return 0;

	if ((error = git_object_lookup(&obj, t->repo, &head->oid, GIT_OBJ_ANY)) < 0)
		return error;

	head = NULL;

	/* If it's not an annotated tag, or if we're mocking
	 * git-receive-pack, just get out */
	if (git_object_type(obj) != GIT_OBJ_TAG ||
		t->direction != GIT_DIRECTION_FETCH) {
		git_object_free(obj);
		return 0;
	}

	/* And if it's a tag, peel it, and add it to the list */
	head = git__calloc(1, sizeof(git_remote_head));
	GITERR_CHECK_ALLOC(head);

	if (git_buf_join(&buf, 0, name, peeled) < 0) {
		free_head(head);
		return -1;
	}
	head->name = git_buf_detach(&buf);

	if (!(error = git_tag_peel(&target, (git_tag *)obj))) {
		git_oid_cpy(&head->oid, git_object_id(target));

		if ((error = git_vector_insert(&t->refs, head)) < 0) {
			free_head(head);
		}
	}

	git_object_free(obj);
	git_object_free(target);

	return error;
}

static int store_refs(transport_local *t)
{
	size_t i;
	git_remote_head *head;
	git_strarray ref_names = {0};

	assert(t);

	if (git_reference_list(&ref_names, t->repo) < 0)
		goto on_error;

	/* Clear all heads we might have fetched in a previous connect */
	git_vector_foreach(&t->refs, i, head) {
		git__free(head->name);
		git__free(head);
	}

	/* Clear the vector so we can reuse it */
	git_vector_clear(&t->refs);

	/* Sort the references first */
	git__tsort((void **)ref_names.strings, ref_names.count, &git__strcmp_cb);

	/* Add HEAD iff direction is fetch */
	if (t->direction == GIT_DIRECTION_FETCH && add_ref(t, GIT_HEAD_FILE) < 0)
		goto on_error;

	for (i = 0; i < ref_names.count; ++i) {
		if (add_ref(t, ref_names.strings[i]) < 0)
			goto on_error;
	}

	t->have_refs = 1;
	git_strarray_free(&ref_names);
	return 0;

on_error:
	git_vector_free(&t->refs);
	git_strarray_free(&ref_names);
	return -1;
}

/*
 * Try to open the url as a git directory. The direction doesn't
 * matter in this case because we're calculating the heads ourselves.
 */
static int local_connect(
	git_transport *transport,
	const char *url,
	git_cred_acquire_cb cred_acquire_cb,
	void *cred_acquire_payload,
	int direction, int flags)
{
	git_repository *repo;
	int error;
	transport_local *t = (transport_local *) transport;
	const char *path;
	git_buf buf = GIT_BUF_INIT;

	GIT_UNUSED(cred_acquire_cb);
	GIT_UNUSED(cred_acquire_payload);

	if (t->connected)
		return 0;

	free_heads(&t->refs);

	t->url = git__strdup(url);
	GITERR_CHECK_ALLOC(t->url);
	t->direction = direction;
	t->flags = flags;

	/* 'url' may be a url or path; convert to a path */
	if ((error = git_path_from_url_or_path(&buf, url)) < 0) {
		git_buf_free(&buf);
		return error;
	}
	path = git_buf_cstr(&buf);

	error = git_repository_open(&repo, path);

	git_buf_free(&buf);

	if (error < 0)
		return -1;

	t->repo = repo;

	if (store_refs(t) < 0)
		return -1;

	t->connected = 1;

	return 0;
}

static int local_ls(const git_remote_head ***out, size_t *size, git_transport *transport)
{
	transport_local *t = (transport_local *)transport;

	if (!t->have_refs) {
		giterr_set(GITERR_NET, "The transport has not yet loaded the refs");
		return -1;
	}

	*out = (const git_remote_head **)t->refs.contents;
	*size = t->refs.length;

	return 0;
}

static int local_negotiate_fetch(
	git_transport *transport,
	git_repository *repo,
	const git_remote_head * const *refs,
	size_t count)
{
	transport_local *t = (transport_local*)transport;
	git_remote_head *rhead;
	unsigned int i;

	GIT_UNUSED(refs);
	GIT_UNUSED(count);

	/* Fill in the loids */
	git_vector_foreach(&t->refs, i, rhead) {
		git_object *obj;

		int error = git_revparse_single(&obj, repo, rhead->name);
		if (!error)
			git_oid_cpy(&rhead->loid, git_object_id(obj));
		else if (error != GIT_ENOTFOUND)
			return error;
		else
			giterr_clear();
		git_object_free(obj);
	}

	return 0;
}

static int local_push_copy_object(
	git_odb *local_odb,
	git_odb *remote_odb,
	git_pobject *obj)
{
	int error = 0;
	git_odb_object *odb_obj = NULL;
	git_odb_stream *odb_stream;
	size_t odb_obj_size;
	git_otype odb_obj_type;
	git_oid remote_odb_obj_oid;

	/* Object already exists in the remote ODB; do nothing and return 0*/
	if (git_odb_exists(remote_odb, &obj->id))
		return 0;

	if ((error = git_odb_read(&odb_obj, local_odb, &obj->id)) < 0)
		return error;

	odb_obj_size = git_odb_object_size(odb_obj);
	odb_obj_type = git_odb_object_type(odb_obj);

	if ((error = git_odb_open_wstream(&odb_stream, remote_odb,
		odb_obj_size, odb_obj_type)) < 0)
		goto on_error;

	if (git_odb_stream_write(odb_stream, (char *)git_odb_object_data(odb_obj),
		odb_obj_size) < 0 ||
		git_odb_stream_finalize_write(&remote_odb_obj_oid, odb_stream) < 0) {
		error = -1;
	} else if (git_oid__cmp(&obj->id, &remote_odb_obj_oid) != 0) {
		giterr_set(GITERR_ODB, "Error when writing object to remote odb "
			"during local push operation. Remote odb object oid does not "
			"match local oid.");
		error = -1;
	}

	git_odb_stream_free(odb_stream);

on_error:
	git_odb_object_free(odb_obj);
	return error;
}

static int local_push_update_remote_ref(
	git_repository *remote_repo,
	const char *lref,
	const char *rref,
	git_oid *loid,
	git_oid *roid)
{
	int error;
	git_reference *remote_ref = NULL;

	/* check for lhs, if it's empty it means to delete */
	if (lref[0] != '\0') {
		/* Create or update a ref */
		error = git_reference_create(NULL, remote_repo, rref, loid,
					     !git_oid_iszero(roid), NULL, NULL);
	} else {
		/* Delete a ref */
		if ((error = git_reference_lookup(&remote_ref, remote_repo, rref)) < 0) {
			if (error == GIT_ENOTFOUND)
				error = 0;
			return error;
		}

		error = git_reference_delete(remote_ref);
		git_reference_free(remote_ref);
	}

	return error;
}

static int local_push(
	git_transport *transport,
	git_push *push)
{
	transport_local *t = (transport_local *)transport;
	git_odb *remote_odb = NULL;
	git_odb *local_odb = NULL;
	git_repository *remote_repo = NULL;
	push_spec *spec;
	char *url = NULL;
	const char *path;
	git_buf buf = GIT_BUF_INIT;
	int error;
	unsigned int i;
	size_t j;

	/* 'push->remote->url' may be a url or path; convert to a path */
	if ((error = git_path_from_url_or_path(&buf, push->remote->url)) < 0) {
		git_buf_free(&buf);
		return error;
	}
	path = git_buf_cstr(&buf);

	error = git_repository_open(&remote_repo, path);

	git_buf_free(&buf);

	if (error < 0)
		return error;

	/* We don't currently support pushing locally to non-bare repos. Proper
	   non-bare repo push support would require checking configs to see if
	   we should override the default 'don't let this happen' behavior */
	if (!remote_repo->is_bare) {
		error = GIT_EBAREREPO;
		giterr_set(GITERR_INVALID, "Local push doesn't (yet) support pushing to non-bare repos.");
		goto on_error;
	}

	if ((error = git_repository_odb__weakptr(&remote_odb, remote_repo)) < 0 ||
		(error = git_repository_odb__weakptr(&local_odb, push->repo)) < 0)
		goto on_error;

	for (i = 0; i < push->pb->nr_objects; i++) {
		if ((error = local_push_copy_object(local_odb, remote_odb,
			&push->pb->object_list[i])) < 0)
			goto on_error;
	}

	push->unpack_ok = 1;

	git_vector_foreach(&push->specs, j, spec) {
		push_status *status;
		const git_error *last;
		char *ref = spec->refspec.dst;

		status = git__calloc(sizeof(push_status), 1);
		if (!status)
			goto on_error;

		status->ref = git__strdup(ref);
		if (!status->ref) {
			git_push_status_free(status);
			goto on_error;
		}

		error = local_push_update_remote_ref(remote_repo, spec->refspec.src, spec->refspec.dst,
			&spec->loid, &spec->roid);

		switch (error) {
			case GIT_OK:
				break;
			case GIT_EINVALIDSPEC:
				status->msg = git__strdup("funny refname");
				break;
			case GIT_ENOTFOUND:
				status->msg = git__strdup("Remote branch not found to delete");
				break;
			default:
				last = giterr_last();

				if (last && last->message)
					status->msg = git__strdup(last->message);
				else
					status->msg = git__strdup("Unspecified error encountered");
				break;
		}

		/* failed to allocate memory for a status message */
		if (error < 0 && !status->msg) {
			git_push_status_free(status);
			goto on_error;
		}

		/* failed to insert the ref update status */
		if ((error = git_vector_insert(&push->status, status)) < 0) {
			git_push_status_free(status);
			goto on_error;
		}
	}

	if (push->specs.length) {
		int flags = t->flags;
		url = git__strdup(t->url);

		if (!url || t->parent.close(&t->parent) < 0 ||
			t->parent.connect(&t->parent, url,
			push->remote->callbacks.credentials, NULL, GIT_DIRECTION_PUSH, flags))
			goto on_error;
	}

	error = 0;

on_error:
	git_repository_free(remote_repo);
	git__free(url);

	return error;
}

typedef struct foreach_data {
	git_transfer_progress *stats;
	git_transfer_progress_cb progress_cb;
	void *progress_payload;
	git_odb_writepack *writepack;
} foreach_data;

static int foreach_cb_local(void *buf, size_t len, void *payload) //oz
{
	foreach_data *data = (foreach_data*)payload;

	data->stats->received_bytes += len;
	return data->writepack->append(data->writepack, buf, len, data->stats);
}

static const char *counting_objects_fmt = "Counting objects %d\r";

static int local_download_pack(
		git_transport *transport,
		git_repository *repo,
		git_transfer_progress *stats,
		git_transfer_progress_cb progress_cb,
		void *progress_payload)
{
	transport_local *t = (transport_local*)transport;
	git_revwalk *walk = NULL;
	git_remote_head *rhead;
	unsigned int i;
	int error = -1;
	git_oid oid;
	git_packbuilder *pack = NULL;
	git_odb_writepack *writepack = NULL;
	git_odb *odb = NULL;
	git_buf progress_info = GIT_BUF_INIT;

	if ((error = git_revwalk_new(&walk, t->repo)) < 0)
		goto cleanup;
	git_revwalk_sorting(walk, GIT_SORT_TIME);

	if ((error = git_packbuilder_new(&pack, t->repo)) < 0)
		goto cleanup;

	stats->total_objects = 0;
	stats->indexed_objects = 0;
	stats->received_objects = 0;
	stats->received_bytes = 0;

	git_vector_foreach(&t->refs, i, rhead) {
		git_object *obj;
		if ((error = git_object_lookup(&obj, t->repo, &rhead->oid, GIT_OBJ_ANY)) < 0)
			goto cleanup;

		if (git_object_type(obj) == GIT_OBJ_COMMIT) {
			/* Revwalker includes only wanted commits */
			error = git_revwalk_push(walk, &rhead->oid);
			if (!git_oid_iszero(&rhead->loid))
				error = git_revwalk_hide(walk, &rhead->loid);
		} else {
			/* Tag or some other wanted object. Add it on its own */
			error = git_packbuilder_insert(pack, &rhead->oid, rhead->name);
		}
		git_object_free(obj);
	}

	if ((error = git_buf_printf(&progress_info, counting_objects_fmt, git_packbuilder_object_count(pack))) < 0)
		goto cleanup;

	if (t->progress_cb &&
	    (error = t->progress_cb(git_buf_cstr(&progress_info), git_buf_len(&progress_info), t->message_cb_payload)) < 0)
		goto cleanup;

	/* Walk the objects, building a packfile */
	if ((error = git_repository_odb__weakptr(&odb, repo)) < 0)
		goto cleanup;

	while ((error = git_revwalk_next(&oid, walk)) == 0) {
		git_commit *commit;

		/* Skip commits we already have */
		if (git_odb_exists(odb, &oid)) continue;

		if (!git_object_lookup((git_object**)&commit, t->repo, &oid, GIT_OBJ_COMMIT)) {
			const git_oid *tree_oid = git_commit_tree_id(commit);

			/* Add the commit and its tree */
			if ((error = git_packbuilder_insert(pack, &oid, NULL)) < 0 ||
				 (error = git_packbuilder_insert_tree(pack, tree_oid)) < 0) {
				git_commit_free(commit);
				goto cleanup;
			}

			git_commit_free(commit);

			git_buf_clear(&progress_info);
			if ((error = git_buf_printf(&progress_info, counting_objects_fmt, git_packbuilder_object_count(pack))) < 0)
				goto cleanup;

			if (t->progress_cb &&
			    (error = t->progress_cb(git_buf_cstr(&progress_info), git_buf_len(&progress_info), t->message_cb_payload)) < 0)
				goto cleanup;

		}
	}

	/* One last one with the newline */
	git_buf_clear(&progress_info);
	git_buf_printf(&progress_info, counting_objects_fmt, git_packbuilder_object_count(pack));
	if ((error = git_buf_putc(&progress_info, '\n')) < 0)
		goto cleanup;

	if (t->progress_cb &&
	    (error = t->progress_cb(git_buf_cstr(&progress_info), git_buf_len(&progress_info), t->message_cb_payload)) < 0)
		goto cleanup;

	if ((error = git_odb_write_pack(&writepack, odb, progress_cb, progress_payload)) != 0)
		goto cleanup;

	/* Write the data to the ODB */
	{
		foreach_data data = {0};
		data.stats = stats;
		data.progress_cb = progress_cb;
		data.progress_payload = progress_payload;
		data.writepack = writepack;

		if ((error = git_packbuilder_foreach(pack, foreach_cb_local, &data)) != 0) //oz
			goto cleanup;
	}
	error = writepack->commit(writepack, stats);

cleanup:
	if (writepack) writepack->free(writepack);
	git_buf_free(&progress_info);
	git_packbuilder_free(pack);
	git_revwalk_free(walk);
	return error;
}

static int local_set_callbacks(
	git_transport *transport,
	git_transport_message_cb progress_cb,
	git_transport_message_cb error_cb,
	git_transport_certificate_check_cb certificate_check_cb,
	void *message_cb_payload)
{
	transport_local *t = (transport_local *)transport;

	GIT_UNUSED(certificate_check_cb);

	t->progress_cb = progress_cb;
	t->error_cb = error_cb;
	t->message_cb_payload = message_cb_payload;

	return 0;
}

static int local_is_connected(git_transport *transport)
{
	transport_local *t = (transport_local *)transport;

	return t->connected;
}

static int local_read_flags(git_transport *transport, int *flags)
{
	transport_local *t = (transport_local *)transport;

	*flags = t->flags;

	return 0;
}

static void local_cancel(git_transport *transport)
{
	transport_local *t = (transport_local *)transport;

	git_atomic_set(&t->cancelled, 1);
}

static int local_close(git_transport *transport)
{
	transport_local *t = (transport_local *)transport;

	t->connected = 0;

	if (t->repo) {
		git_repository_free(t->repo);
		t->repo = NULL;
	}

	if (t->url) {
		git__free(t->url);
		t->url = NULL;
	}

	return 0;
}

static void local_free(git_transport *transport)
{
	transport_local *t = (transport_local *)transport;

	free_heads(&t->refs);

	/* Close the transport, if it's still open. */
	local_close(transport);

	/* Free the transport */
	git__free(t);
}

/**************
 * Public API *
 **************/

int git_transport_local(git_transport **out, git_remote *owner, void *param)
{
	int error;
	transport_local *t;

	GIT_UNUSED(param);

	t = git__calloc(1, sizeof(transport_local));
	GITERR_CHECK_ALLOC(t);

	t->parent.version = GIT_TRANSPORT_VERSION;
	t->parent.set_callbacks = local_set_callbacks;
	t->parent.connect = local_connect;
	t->parent.negotiate_fetch = local_negotiate_fetch;
	t->parent.download_pack = local_download_pack;
	t->parent.push = local_push;
	t->parent.close = local_close;
	t->parent.free = local_free;
	t->parent.ls = local_ls;
	t->parent.is_connected = local_is_connected;
	t->parent.read_flags = local_read_flags;
	t->parent.cancel = local_cancel;

	if ((error = git_vector_init(&t->refs, 0, NULL)) < 0) {
		git__free(t);
		return error;
	}

	t->owner = owner;

	*out = (git_transport *) t;

	return 0;
}
