/*
 * Copyright (C) the libgit2 contributors. All rights reserved.
 *
 * This file is part of libgit2, distributed under the GNU GPL v2 with
 * a Linking Exception. For full terms see the included COPYING file.
 */
#ifndef INCLUDE_message_h__
#define INCLUDE_message_h__

#include BOSS_LIBGIT2_U_git2__message_h //original-code:"git2/message.h"
#include BOSS_LIBGIT2_U_buffer_h //original-code:"buffer.h"

int git_message__prettify(git_buf *message_out, const char *message, int strip_comments);

#endif /* INCLUDE_message_h__ */
