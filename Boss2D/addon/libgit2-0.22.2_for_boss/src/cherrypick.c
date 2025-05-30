/*
* Copyright (C) the libgit2 contributors. All rights reserved.
*
* This file is part of libgit2, distributed under the GNU GPL v2 with
* a Linking Exception. For full terms see the included COPYING file.
*/

#include BOSS_LIBGIT2_U_common_h //original-code:"common.h"
#include BOSS_LIBGIT2_U_repository_h //original-code:"repository.h"
#include "filebuf.h"
#include "merge.h"
#include BOSS_LIBGIT2_U_vector_h //original-code:"vector.h"

#include BOSS_LIBGIT2_U_git2__types_h //original-code:"git2/types.h"
#include BOSS_LIBGIT2_U_git2__merge_h //original-code:"git2/merge.h"
#include BOSS_LIBGIT2_U_git2__cherrypick_h //original-code:"git2/cherrypick.h"
#include BOSS_LIBGIT2_U_git2__commit_h //original-code:"git2/commit.h"
#include BOSS_LIBGIT2_U_git2__sys__commit_h //original-code:"git2/sys/commit.h"

#define GIT_CHERRYPICK_FILE_MODE		0666

static int write_cherrypick_head(
	git_repository *repo,
	const char *commit_oidstr)
{
	git_filebuf file = GIT_FILEBUF_INIT;
	git_buf file_path = GIT_BUF_INIT;
	int error = 0;

	if ((error = git_buf_joinpath(&file_path, repo->path_repository, GIT_CHERRYPICK_HEAD_FILE)) >= 0 &&
		(error = git_filebuf_open(&file, file_path.ptr, GIT_FILEBUF_FORCE, GIT_CHERRYPICK_FILE_MODE)) >= 0 &&
		(error = git_filebuf_printf(&file, "%s\n", commit_oidstr)) >= 0)
		error = git_filebuf_commit(&file);

	if (error < 0)
		git_filebuf_cleanup(&file);

	git_buf_free(&file_path);

	return error;
}

static int write_merge_msg_cherrypick( //oz
	git_repository *repo,
	const char *commit_msg)
{
	git_filebuf file = GIT_FILEBUF_INIT;
	git_buf file_path = GIT_BUF_INIT;
	int error = 0;

	if ((error = git_buf_joinpath(&file_path, repo->path_repository, GIT_MERGE_MSG_FILE)) < 0 ||
		(error = git_filebuf_open(&file, file_path.ptr, GIT_FILEBUF_FORCE, GIT_CHERRYPICK_FILE_MODE)) < 0 ||
		(error = git_filebuf_printf(&file, "%s", commit_msg)) < 0)
		goto cleanup;

	error = git_filebuf_commit(&file);

cleanup:
	if (error < 0)
		git_filebuf_cleanup(&file);

	git_buf_free(&file_path);

	return error;
}

static int cherrypick_normalize_opts(
	git_repository *repo,
	git_cherrypick_options *opts,
	const git_cherrypick_options *given,
	const char *their_label)
{
	int error = 0;
	unsigned int default_checkout_strategy = GIT_CHECKOUT_SAFE_CREATE |
		GIT_CHECKOUT_ALLOW_CONFLICTS;

	GIT_UNUSED(repo);

	if (given != NULL)
		memcpy(opts, given, sizeof(git_cherrypick_options));
	else {
		git_cherrypick_options default_opts = GIT_CHERRYPICK_OPTIONS_INIT;
		memcpy(opts, &default_opts, sizeof(git_cherrypick_options));
	}

	if (!opts->checkout_opts.checkout_strategy)
		opts->checkout_opts.checkout_strategy = default_checkout_strategy;

	if (!opts->checkout_opts.our_label)
		opts->checkout_opts.our_label = "HEAD";

	if (!opts->checkout_opts.their_label)
		opts->checkout_opts.their_label = their_label;

	return error;
}

static int cherrypick_state_cleanup(git_repository *repo)
{
	const char *state_files[] = { GIT_CHERRYPICK_HEAD_FILE, GIT_MERGE_MSG_FILE };

	return git_repository__cleanup_files(repo, state_files, ARRAY_SIZE(state_files));
}

static int cherrypick_seterr(git_commit *commit, const char *fmt)
{
	char commit_oidstr[GIT_OID_HEXSZ + 1];

	giterr_set(GITERR_CHERRYPICK, fmt,
		git_oid_tostr(commit_oidstr, GIT_OID_HEXSZ + 1, git_commit_id(commit)));

	return -1;
}

int git_cherrypick_commit(
	git_index **out,
	git_repository *repo,
	git_commit *cherrypick_commit,
	git_commit *our_commit,
	unsigned int mainline,
	const git_merge_options *merge_opts)
{
	git_commit *parent_commit = NULL;
	git_tree *parent_tree = NULL, *our_tree = NULL, *cherrypick_tree = NULL;
	int parent = 0, error = 0;

	assert(out && repo && cherrypick_commit && our_commit);

	if (git_commit_parentcount(cherrypick_commit) > 1) {
		if (!mainline)
			return cherrypick_seterr(cherrypick_commit,
				"Mainline branch is not specified but %s is a merge commit");

		parent = mainline;
	} else {
		if (mainline)
			return cherrypick_seterr(cherrypick_commit,
				"Mainline branch specified but %s is not a merge commit");

		parent = git_commit_parentcount(cherrypick_commit);
	}

	if (parent &&
		((error = git_commit_parent(&parent_commit, cherrypick_commit, (parent - 1))) < 0 ||
		(error = git_commit_tree(&parent_tree, parent_commit)) < 0))
		goto done;

	if ((error = git_commit_tree(&cherrypick_tree, cherrypick_commit)) < 0 ||
		(error = git_commit_tree(&our_tree, our_commit)) < 0)
		goto done;

	error = git_merge_trees(out, repo, parent_tree, our_tree, cherrypick_tree, merge_opts);

done:
	git_tree_free(parent_tree);
	git_tree_free(our_tree);
	git_tree_free(cherrypick_tree);
	git_commit_free(parent_commit);

	return error;
}

int git_cherrypick(
	git_repository *repo,
	git_commit *commit,
	const git_cherrypick_options *given_opts)
{
	git_cherrypick_options opts;
	git_reference *our_ref = NULL;
	git_commit *our_commit = NULL;
	char commit_oidstr[GIT_OID_HEXSZ + 1];
	const char *commit_msg, *commit_summary;
	git_buf their_label = GIT_BUF_INIT;
	git_index *index_new = NULL;
	int error = 0;

	assert(repo && commit);

	GITERR_CHECK_VERSION(given_opts, GIT_CHERRYPICK_OPTIONS_VERSION, "git_cherrypick_options");

	if ((error = git_repository__ensure_not_bare(repo, "cherry-pick")) < 0)
		return error;

	if ((commit_msg = git_commit_message(commit)) == NULL ||
		(commit_summary = git_commit_summary(commit)) == NULL) {
		error = -1;
		goto on_error;
	}

	git_oid_nfmt(commit_oidstr, sizeof(commit_oidstr), git_commit_id(commit));

	if ((error = write_merge_msg_cherrypick(repo, commit_msg)) < 0 || //oz
		(error = git_buf_printf(&their_label, "%.7s... %s", commit_oidstr, commit_summary)) < 0 ||
		(error = cherrypick_normalize_opts(repo, &opts, given_opts, git_buf_cstr(&their_label))) < 0 ||
		(error = write_cherrypick_head(repo, commit_oidstr)) < 0 ||
		(error = git_repository_head(&our_ref, repo)) < 0 ||
		(error = git_reference_peel((git_object **)&our_commit, our_ref, GIT_OBJ_COMMIT)) < 0 ||
		(error = git_cherrypick_commit(&index_new, repo, commit, our_commit, opts.mainline, &opts.merge_opts)) < 0 ||
		(error = git_merge__check_result(repo, index_new)) < 0 ||
		(error = git_merge__append_conflicts_to_merge_msg(repo, index_new)) < 0 ||
		(error = git_checkout_index(repo, index_new, &opts.checkout_opts)) < 0)
		goto on_error;
	goto done;

on_error:
	cherrypick_state_cleanup(repo);

done:
	git_index_free(index_new);
	git_commit_free(our_commit);
	git_reference_free(our_ref);
	git_buf_free(&their_label);

	return error;
}

int git_cherrypick_init_options(
	git_cherrypick_options *opts, unsigned int version)
{
	GIT_INIT_STRUCTURE_FROM_TEMPLATE(
		opts, version, git_cherrypick_options, GIT_CHERRYPICK_OPTIONS_INIT);
	return 0;
}
