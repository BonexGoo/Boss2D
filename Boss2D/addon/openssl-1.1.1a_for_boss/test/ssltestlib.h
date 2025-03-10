/*
 * Copyright 2016-2018 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#ifndef HEADER_SSLTESTLIB_H
# define HEADER_SSLTESTLIB_H

# include BOSS_OPENSSL_V_openssl__ssl_h //original-code:<openssl/ssl.h>

int create_ssl_ctx_pair(const SSL_METHOD *sm, const SSL_METHOD *cm,
                        int min_proto_version, int max_proto_version,
                        SSL_CTX **sctx, SSL_CTX **cctx, char *certfile,
                        char *privkeyfile);
int create_ssl_objects(SSL_CTX *serverctx, SSL_CTX *clientctx, SSL **sssl,
                       SSL **cssl, BIO *s_to_c_fbio, BIO *c_to_s_fbio);
int create_bare_ssl_connection(SSL *serverssl, SSL *clientssl, int want);
int create_ssl_connection(SSL *serverssl, SSL *clientssl, int want);
void shutdown_ssl_connection(SSL *serverssl, SSL *clientssl);

/* Note: Not thread safe! */
const BIO_METHOD *bio_f_tls_dump_filter(void);
void bio_f_tls_dump_filter_free(void);

const BIO_METHOD *bio_s_mempacket_test(void);
void bio_s_mempacket_test_free(void);

/* Packet types - value 0 is reserved */
#define INJECT_PACKET                   1
#define INJECT_PACKET_IGNORE_REC_SEQ    2

/*
 * Mempacket BIO ctrls. We make them large enough to not clash with standard BIO
 * ctrl codes.
 */
#define MEMPACKET_CTRL_SET_DROP_EPOCH       (1 << 15)
#define MEMPACKET_CTRL_SET_DROP_REC         (2 << 15)
#define MEMPACKET_CTRL_GET_DROP_REC         (3 << 15)
#define MEMPACKET_CTRL_SET_DUPLICATE_REC    (4 << 15)

int mempacket_test_inject(BIO *bio, const char *in, int inl, int pktnum,
                          int type);

typedef struct mempacket_st MEMPACKET;

DEFINE_STACK_OF(MEMPACKET)

#endif /* HEADER_SSLTESTLIB_H */
