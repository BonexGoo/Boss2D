/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */

#include <assert.h>

#include BOSS_LIBGIT2_U_git2__clone_h //original-code:"git2/clone.h"
#include BOSS_LIBGIT2_U_git2__remote_h //original-code:"git2/remote.h"
#include BOSS_LIBGIT2_U_git2__revparse_h //original-code:"git2/revparse.h"
#include BOSS_LIBGIT2_U_git2__branch_h //original-code:"git2/branch.h"
#include BOSS_LIBGIT2_U_git2__config_h //original-code:"git2/config.h"
#include BOSS_LIBGIT2_U_git2__checkout_h //original-code:"git2/checkout.h"
#include BOSS_LIBGIT2_U_git2__commit_h //original-code:"git2/commit.h"
#include BOSS_LIBGIT2_U_git2__tree_h //original-code:"git2/tree.h"

#include BOSS_LIBGIT2_U_common_h //original-code:"common.h"
#include BOSS_LIBGIT2_U_remote_h //original-code:"remote.h"
#include "fileops.h"
#include BOSS_LIBGIT2_U_refs_h //original-code:"refs.h"
#include BOSS_LIBGIT2_U_path_h //original-code:"path.h"
#include BOSS_LIBGIT2_U_repository_h //original-code:"repository.h"
#include BOSS_LIBGIT2_U_odb_h //original-code:"odb.h"

static int clone_local_into(git_repository *repo, git_remote *remote, const git_checkout_options *co_opts, const char *branch, int link, const git_signature *signature);

static int create_branch(
	git_reference **branch,
	git_repository *repo,
	const git_oid *target,
	const char *name,
	const git_signature *signature,
	const char *log_message)
{
	git_commit *head_obj = NULL;
	git_reference *branch_ref = NULL;
	int error;

	/* Find the target commit */
	if ((error = git_commit_lookup(&head_obj, repo, target)) < 0)
		return error;

	/* Create the new branch */
	error = git_branch_create(&branch_ref, repo, name, head_obj, 0, signature, log_message);

	git_commit_free(head_obj);

	if (!error)
		*branch = branch_ref;
	else
		git_reference_free(branch_ref);

	return error;
}

static int setup_tracking_config(
	git_repository *repo,
	const char *branch_name,
	const char *remote_name,
	const char *merge_target)
{
	git_config *cfg;
	git_buf remote_key = GIT_BUF_INIT, merge_key = GIT_BUF_INIT;
	int error = -1;

	if (git_repository_config__weakptr(&cfg, repo) < 0)
		return -1;

	if (git_buf_printf(&remote_key, "branch.%s.remote", branch_name) < 0)
		goto cleanup;

	if (git_buf_printf(&merge_key, "branch.%s.merge", branch_name) < 0)
		goto cleanup;

	if (git_config_set_string(cfg, git_buf_cstr(&remote_key), remote_name) < 0)
		goto cleanup;

	if (git_config_set_string(cfg, git_buf_cstr(&merge_key), merge_target) < 0)
		goto cleanup;

	error = 0;

cleanup:
	git_buf_free(&remote_key);
	git_buf_free(&merge_key);
	return error;
}

static int create_tracking_branch(
	git_reference **branch,
	git_repository *repo,
	const git_oid *target,
	const char *branch_name,
	const git_signature *signature,
	const char *log_message)
{
	int error;

	if ((error = create_branch(branch, repo, target, branch_name, signature, log_message)) < 0)
		return error;

	return setup_tracking_config(
		repo,
		branch_name,
		GIT_REMOTE_ORIGIN,
		git_reference_name(*branch));
}

static int update_head_to_new_branch(
	git_repository *repo,
	const git_oid *target,
	const char *name,
	const git_signature *signature,
	const char *reflog_message)
{
	git_reference *tracking_branch = NULL;
	int error;

	if (!git__prefixcmp(name, GIT_REFS_HEADS_DIR))
		name += strlen(GIT_REFS_HEADS_DIR);

	error = create_tracking_branch(&tracking_branch, repo, target, name,
			signature, reflog_message);

	if (!error)
		error = git_repository_set_head(
			repo, git_reference_name(tracking_branch),
			signature, reflog_message);

	git_reference_free(tracking_branch);

	/* if it already existed, then the user's refspec created it for us, ignore it' */
	if (error == GIT_EEXISTS)
		error = 0;

	return error;
}

static int update_head_to_remote(
		git_repository *repo,
		git_remote *remote,
		const git_signature *signature,
		const char *reflog_message)
{
	int error = 0;
	size_t refs_len;
	git_refspec *refspec;
	const git_remote_head *remote_head, **refs;
	const git_oid *remote_head_id;
	git_buf remote_master_name = GIT_BUF_INIT;
	git_buf branch = GIT_BUF_INIT;

	if ((error = git_remote_ls(&refs, &refs_len, remote)) < 0)
		return error;

	/* We cloned an empty repository or one with an unborn HEAD */
	if (refs_len == 0 || strcmp(refs[0]->name, GIT_HEAD_FILE))
		return setup_tracking_config(
			repo, "master", GIT_REMOTE_ORIGIN, GIT_REFS_HEADS_MASTER_FILE);

	/* We know we have HEAD, let's see where it points */
	remote_head = refs[0];
	assert(remote_head);

	remote_head_id = &remote_head->oid;

	error = git_remote_default_branch(&branch, remote);
	if (error == GIT_ENOTFOUND) {
		error = git_repository_set_head_detached(
			repo, remote_head_id, signature, reflog_message);
		goto cleanup;
	}

	refspec = git_remote__matching_refspec(remote, git_buf_cstr(&branch));

	if (refspec == NULL) {
		giterr_set(GITERR_NET, "the remote's default branch does not fit the refspec configuration");
		error = GIT_EINVALIDSPEC;
		goto cleanup;
	}

	/* Determine the remote tracking reference name from the local master */
	if ((error = git_refspec_transform(
		&remote_master_name,
		refspec,
		git_buf_cstr(&branch))) < 0)
		goto cleanup;

	error = update_head_to_new_branch(
		repo,
		remote_head_id,
		git_buf_cstr(&branch),
		signature, reflog_message);

cleanup:
	git_buf_free(&remote_master_name);
	git_buf_free(&branch);

	return error;
}

static int update_head_to_branch(
		git_repository *repo,
		const char *remote_name,
		const char *branch,
		const git_signature *signature,
		const char *reflog_message)
{
	int retcode;
	git_buf remote_branch_name = GIT_BUF_INIT;
	git_reference* remote_ref = NULL;

	assert(remote_name && branch);

	if ((retcode = git_buf_printf(&remote_branch_name, GIT_REFS_REMOTES_DIR "%s/%s",
		remote_name, branch)) < 0 )
		goto cleanup;

	if ((retcode = git_reference_lookup(&remote_ref, repo, git_buf_cstr(&remote_branch_name))) < 0)
		goto cleanup;

	retcode = update_head_to_new_branch(repo, git_reference_target(remote_ref), branch,
			signature, reflog_message);

cleanup:
	git_reference_free(remote_ref);
	git_buf_free(&remote_branch_name);
	return retcode;
}

static int default_repository_create(git_repository **out, const char *path, int bare, void *payload)
{
	GIT_UNUSED(payload);

	return git_repository_init(out, path, bare);
}

static int default_remote_create(
		git_remote **out,
		git_repository *repo,
		const char *name,
		const char *url,
		void *payload)
{
	int error;
	git_remote_callbacks *callbacks = payload;

	if ((error = git_remote_create(out, repo, name, url)) < 0)
		return error;

	return git_remote_set_callbacks(*out, callbacks);
}

/*
 * submodules?
 */

static int create_and_configure_origin(
		git_remote **out,
		git_repository *repo,
		const char *url,
		const git_clone_options *options)
{
	int error;
	git_remote *origin = NULL;
	char buf[GIT_PATH_MAX];
	git_remote_create_cb remote_create = options->remote_cb;
	void *payload = options->remote_cb_payload;

	/* If the path exists and is a dir, the url should be the absolute path */
	if (git_path_root(url) < 0 && git_path_exists(url) && git_path_isdir(url)) {
		if (p_realpath(url, buf) == NULL)
			return -1;

		url = buf;
	}

	if (!remote_create) {
		remote_create = default_remote_create;
		payload = (void *)&options->remote_callbacks;
	}

	if ((error = remote_create(&origin, repo, "origin", url, payload)) < 0)
		goto on_error;

	if ((error = git_remote_save(origin)) < 0)
		goto on_error;

	*out = origin;
	return 0;

on_error:
	git_remote_free(origin);
	return error;
}

static bool should_checkout(
	git_repository *repo,
	bool is_bare,
	const git_checkout_options *opts)
{
	if (is_bare)
		return false;

	if (!opts)
		return false;

	if (opts->checkout_strategy == GIT_CHECKOUT_NONE)
		return false;

	return !git_repository_head_unborn(repo);
}

static int checkout_branch(git_repository *repo, git_remote *remote, const git_checkout_options *co_opts, const char *branch, const git_signature *signature, const char *reflog_message)
{
	int error;

	if (branch)
		error = update_head_to_branch(repo, git_remote_name(remote), branch,
				signature, reflog_message);
	/* Point HEAD to the same ref as the remote's head */
	else
		error = update_head_to_remote(repo, remote, signature, reflog_message);

	if (!error && should_checkout(repo, git_repository_is_bare(repo), co_opts))
		error = git_checkout_head(repo, co_opts);

	return error;
}

static int clone_into(git_repository *repo, git_remote *_remote, const git_checkout_options *co_opts, const char *branch, const git_signature *signature)
{
	int error;
	git_buf reflog_message = GIT_BUF_INIT;
	git_remote *remote;
	const git_remote_callbacks *callbacks;

	assert(repo && _remote);

	if (!git_repository_is_empty(repo)) {
		giterr_set(GITERR_INVALID, "the repository is not empty");
		return -1;
	}

	if ((error = git_remote_dup(&remote, _remote)) < 0)
		return error;

	callbacks = git_remote_get_callbacks(_remote);
	if (!giterr__check_version(callbacks, 1, "git_remote_callbacks") &&
	    (error = git_remote_set_callbacks(remote, callbacks)) < 0)
		goto cleanup;

	if ((error = git_remote_add_fetch(remote, "refs/tags/*:refs/tags/*")) < 0)
		goto cleanup;

	git_remote_set_update_fetchhead(remote, 0);
	git_buf_printf(&reflog_message, "clone: from %s", git_remote_url(remote));

	if ((error = git_remote_fetch(remote, NULL, signature, git_buf_cstr(&reflog_message))) != 0)
		goto cleanup;

	error = checkout_branch(repo, remote, co_opts, branch, signature, git_buf_cstr(&reflog_message));

cleanup:
	git_remote_free(remote);
	git_buf_free(&reflog_message);

	return error;
}

int git_clone__should_clone_local(const char *url_or_path, git_clone_local_t local)
{
	git_buf fromurl = GIT_BUF_INIT;
	const char *path = url_or_path;
	bool is_url, is_local;

	if (local == GIT_CLONE_NO_LOCAL)
		return 0;

	if ((is_url = git_path_is_local_file_url(url_or_path)) != 0) {
		if (git_path_fromurl(&fromurl, url_or_path) < 0) {
			is_local = -1;
			goto done;
		}

		path = fromurl.ptr;
	}

	is_local = (!is_url || local != GIT_CLONE_LOCAL_AUTO) &&
		git_path_isdir(path);

done:
	git_buf_free(&fromurl);
	return is_local;
}

int git_clone(
	git_repository **out,
	const char *url,
	const char *local_path,
	const git_clone_options *_options)
{
	int error = 0;
	git_repository *repo = NULL;
	git_remote *origin;
	git_clone_options options = GIT_CLONE_OPTIONS_INIT;
	uint32_t rmdir_flags = GIT_RMDIR_REMOVE_FILES;
	git_repository_create_cb repository_cb;

	assert(out && url && local_path);

	if (_options)
		memcpy(&options, _options, sizeof(git_clone_options));

	GITERR_CHECK_VERSION(&options, GIT_CLONE_OPTIONS_VERSION, "git_clone_options");

	/* Only clone to a new directory or an empty directory */
	if (git_path_exists(local_path) && !git_path_is_empty_dir(local_path)) {
		giterr_set(GITERR_INVALID,
			"'%s' exists and is not an empty directory", local_path);
		return GIT_EEXISTS;
	}

	/* Only remove the root directory on failure if we create it */
	if (git_path_exists(local_path))
		rmdir_flags |= GIT_RMDIR_SKIP_ROOT;

	if (options.repository_cb)
		repository_cb = options.repository_cb;
	else
		repository_cb = default_repository_create;

	if ((error = repository_cb(&repo, local_path, options.bare, options.repository_cb_payload)) < 0)
		return error;

	if (!(error = create_and_configure_origin(&origin, repo, url, &options))) {
		int clone_local = git_clone__should_clone_local(url, options.local);
		int link = options.local != GIT_CLONE_LOCAL_NO_LINKS;

		if (clone_local == 1)
			error = clone_local_into(
				repo, origin, &options.checkout_opts,
				options.checkout_branch, link, options.signature);
		else if (clone_local == 0)
			error = clone_into(
				repo, origin, &options.checkout_opts,
				options.checkout_branch, options.signature);
		else
			error = -1;

		git_remote_free(origin);
	}

	if (error != 0) {
		git_error_state last_error = {0};
		giterr_capture(&last_error, error);

		git_repository_free(repo);
		repo = NULL;

		(void)git_futils_rmdir_r(local_path, NULL, rmdir_flags);

		giterr_restore(&last_error);
	}

	*out = repo;
	return error;
}

int git_clone_init_options(git_clone_options *opts, unsigned int version)
{
	GIT_INIT_STRUCTURE_FROM_TEMPLATE(
		opts, version, git_clone_options, GIT_CLONE_OPTIONS_INIT);
	return 0;
}

static const char *repository_base(git_repository *repo)
{
	if (git_repository_is_bare(repo))
		return git_repository_path(repo);

	return git_repository_workdir(repo);
}

static bool can_link(const char *src, const char *dst, int link)
{
#ifdef GIT_WIN32
	GIT_UNUSED(src);
	GIT_UNUSED(dst);
	GIT_UNUSED(link);
	return false;
#else

	struct stat st_src, st_dst;

	if (!link)
		return false;

	if (p_stat(src, &st_src) < 0)
		return false;

	if (p_stat(dst, &st_dst) < 0)
		return false;

	return st_src.st_dev == st_dst.st_dev;
#endif
}

static int clone_local_into(git_repository *repo, git_remote *remote, const git_checkout_options *co_opts, const char *branch, int link, const git_signature *signature)
{
	int error, flags;
	git_repository *src;
	git_buf src_odb = GIT_BUF_INIT, dst_odb = GIT_BUF_INIT, src_path = GIT_BUF_INIT;
	git_buf reflog_message = GIT_BUF_INIT;

	assert(repo && remote);

	if (!git_repository_is_empty(repo)) {
		giterr_set(GITERR_INVALID, "the repository is not empty");
		return -1;
	}

	/*
	 * Let's figure out what path we should use for the source
	 * repo, if it's not rooted, the path should be relative to
	 * the repository's worktree/gitdir.
	 */
	if ((error = git_path_from_url_or_path(&src_path, git_remote_url(remote))) < 0)
		return error;

	/* Copy .git/objects/ from the source to the target */
	if ((error = git_repository_open(&src, git_buf_cstr(&src_path))) < 0) {
		git_buf_free(&src_path);
		return error;
	}

	git_buf_joinpath(&src_odb, git_repository_path(src), GIT_OBJECTS_DIR);
	git_buf_joinpath(&dst_odb, git_repository_path(repo), GIT_OBJECTS_DIR);
	if (git_buf_oom(&src_odb) || git_buf_oom(&dst_odb)) {
		error = -1;
		goto cleanup;
	}

	flags = 0;
	if (can_link(git_repository_path(src), git_repository_path(repo), link))
		flags |= GIT_CPDIR_LINK_FILES;

	if ((error = git_futils_cp_r(git_buf_cstr(&src_odb), git_buf_cstr(&dst_odb),
				     flags, GIT_OBJECT_DIR_MODE)) < 0)
		goto cleanup;

	git_buf_printf(&reflog_message, "clone: from %s", git_remote_url(remote));

	if ((error = git_remote_fetch(remote, NULL, signature, git_buf_cstr(&reflog_message))) != 0)
		goto cleanup;

	error = checkout_branch(repo, remote, co_opts, branch, signature, git_buf_cstr(&reflog_message));

cleanup:
	git_buf_free(&reflog_message);
	git_buf_free(&src_path);
	git_buf_free(&src_odb);
	git_buf_free(&dst_odb);
	git_repository_free(src);
	return error;
}
