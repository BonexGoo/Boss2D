/*
 * Copyright 2017 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <stdlib.h>              /* size_t */
#include BOSS_OPENSSL_V_openssl__bn_h //original-code:<openssl/bn.h>
#include BOSS_OPENSSL_V_openssl__bio_h //original-code:<openssl/bio.h>
#include "../testutil.h"

int subtest_level(void);
int openssl_error_cb(const char *str, size_t len, void *u);
const BIO_METHOD *BIO_f_tap(void);

void test_fail_message_prefix(const char *prefix, const char *file,
                              int line, const char *type,
                              const char *left, const char *right,
                              const char *op);

void test_fail_string_message(const char *prefix, const char *file,
                              int line, const char *type,
                              const char *left, const char *right,
                              const char *op, const char *m1, size_t l1,
                              const char *m2, size_t l2);

void test_fail_bignum_message(const char *prefix, const char *file,
                              int line, const char *type,
                              const char *left, const char *right,
                              const char *op,
                              const BIGNUM *bn1, const BIGNUM *bn2);
void test_fail_bignum_mono_message(const char *prefix, const char *file,
                                   int line, const char *type,
                                   const char *left, const char *right,
                                   const char *op, const BIGNUM *bn);

void test_fail_memory_message(const char *prefix, const char *file,
                              int line, const char *type,
                              const char *left, const char *right,
                              const char *op,
                              const unsigned char *m1, size_t l1,
                              const unsigned char *m2, size_t l2);

void setup_test_framework(void);
__owur int pulldown_test_framework(int ret);

__owur int run_tests(const char *test_prog_name);
void set_test_title(const char *title);
