#ifndef HEADER_CURL_HTTP2_H
#define HEADER_CURL_HTTP2_H
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

#ifdef USE_NGHTTP2
#include BOSS_CURL_U_http_h //original-code:"http.h"

/* value for MAX_CONCURRENT_STREAMS we use until we get an updated setting
   from the peer */
#define DEFAULT_MAX_CONCURRENT_STREAMS 13

/*
 * Store nghttp2 version info in this buffer, Prefix with a space.  Return
 * total length written.
 */
int Curl_http2_ver(char *p, size_t len);

const char *Curl_http2_strerror(uint32_t err);

CURLcode Curl_http2_init(struct connectdata *conn);
void Curl_http2_init_state(struct UrlState *state);
void Curl_http2_init_userset(struct UserDefined *set);
CURLcode Curl_http2_send_request(struct connectdata *conn);
CURLcode Curl_http2_request_upgrade(Curl_send_buffer *req,
                                    struct connectdata *conn);
CURLcode Curl_http2_setup(struct connectdata *conn);
CURLcode Curl_http2_switched(struct connectdata *conn,
                             const char *data, size_t nread);
/* called from Curl_http_setup_conn */
void Curl_http2_setup_conn(struct connectdata *conn);
void Curl_http2_setup_req(struct Curl_easy *data);
void Curl_http2_done(struct connectdata *conn, bool premature);
CURLcode Curl_http2_done_sending(struct connectdata *conn);
#else /* USE_NGHTTP2 */
#define Curl_http2_init(x) CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_send_request(x) CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_request_upgrade(x,y) CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_setup(x) CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_switched(x,y,z) CURLE_UNSUPPORTED_PROTOCOL
#define Curl_http2_setup_conn(x)
#define Curl_http2_setup_req(x)
#define Curl_http2_init_state(x)
#define Curl_http2_init_userset(x)
#define Curl_http2_done(x,y)
#define Curl_http2_done_sending(x)
#endif

#endif /* HEADER_CURL_HTTP2_H */

