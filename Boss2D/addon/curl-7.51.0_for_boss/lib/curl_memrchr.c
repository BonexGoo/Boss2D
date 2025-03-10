/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2016, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at https://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include BOSS_CURL_U_curl_setup_h //original-code:"curl_setup.h"

#include BOSS_CURL_V_curl__curl_h //original-code:<curl/curl.h>

#include BOSS_CURL_U_curl_memrchr_h //original-code:"curl_memrchr.h"
#include BOSS_CURL_U_curl_memory_h //original-code:"curl_memory.h"

/* The last #include file should be: */
#include BOSS_CURL_U_memdebug_h //original-code:"memdebug.h"

#ifndef HAVE_MEMRCHR

/*
 * Curl_memrchr()
 *
 * Our memrchr() function clone for systems which lack this function. The
 * memrchr() function is like the memchr() function, except that it searches
 * backwards from the end of the n bytes pointed to by s instead of forward
 * from the beginning.
 */

void *
Curl_memrchr(const void *s, int c, size_t n)
{
  const unsigned char *p = s;
  const unsigned char *q = s;

  p += n - 1;

  while(p >= q) {
    if(*p == (unsigned char)c)
      return (void *)p;
    p--;
  }

  return NULL;
}

#endif /* HAVE_MEMRCHR */
