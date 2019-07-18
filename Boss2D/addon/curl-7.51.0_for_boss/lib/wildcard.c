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

#include BOSS_CURL_U_wildcard_h //original-code:"wildcard.h"
#include BOSS_CURL_U_llist_h //original-code:"llist.h"
#include BOSS_CURL_U_fileinfo_h //original-code:"fileinfo.h"
/* The last 3 #include files should be in this order */
#include BOSS_CURL_U_curl_printf_h //original-code:"curl_printf.h"
#include BOSS_CURL_U_curl_memory_h //original-code:"curl_memory.h"
#include BOSS_CURL_U_memdebug_h //original-code:"memdebug.h"

CURLcode Curl_wildcard_init(struct WildcardData *wc)
{
  DEBUGASSERT(wc->filelist == NULL);
  /* now allocate only wc->filelist, everything else
     will be allocated if it is needed. */
  wc->filelist = Curl_llist_alloc(Curl_fileinfo_dtor);
  if(!wc->filelist) {;
    return CURLE_OUT_OF_MEMORY;
  }
  return CURLE_OK;
}

void Curl_wildcard_dtor(struct WildcardData *wc)
{
  if(!wc)
    return;

  if(wc->tmp_dtor) {
    wc->tmp_dtor(wc->tmp);
    wc->tmp_dtor = ZERO_NULL;
    wc->tmp = NULL;
  }
  DEBUGASSERT(wc->tmp == NULL);

  if(wc->filelist) {
    Curl_llist_destroy(wc->filelist, NULL);
    wc->filelist = NULL;
  }

  free(wc->path);
  wc->path = NULL;
  free(wc->pattern);
  wc->pattern = NULL;

  wc->customptr = NULL;
  wc->state = CURLWC_INIT;
}
